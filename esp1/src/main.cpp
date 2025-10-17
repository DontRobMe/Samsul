/* ESP32 - Jeu de score stable
   - Reconnexion WiFi automatique (WiFiMulti)
   - Anti-rebond pour capteurs ultrasons
   - Envoi HTTP limité (throttle)
   - pulseIn timeout réduit (moins de blocage)
   - LCD affichage
   - Gardé tes pins existants (trig1=5, echo1=18, buzzer=19)
   -> Remplace apiUrl par l'IP de ton PC/serveur (ex: http://192.168.43.10:4000)
*/

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>        // inclus si tu utilises le RFID; sinon tu peux commenter
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ----------------- Hardware / Pins -----------------
LiquidCrystal lcd(23, 22, 21, 15, 13, 32); // RS, E, D4, D5, D6, D7

const int trig1 = 5, echo1 = 18;
const int trig2 = 4, echo2 = 2; // si instable, pense à déplacer echo2 vers 34..39 (entrée only)
const int buzzerPin = 19;

// ----------------- Game vars -----------------
int score1 = 0;
int score2 = 0;
const int maxScore = 10;
unsigned long startTime = 0;

// ----------------- WiFi / API -----------------
WiFiMulti wifiMulti;
// Remplace par l'IP de ton serveur (sur le même réseau que l'ESP32)
const char* apiUrl = "http://35.180.130.22:443"; // <-- changer !!!
String realtimeEndpoint = String(apiUrl) + "/realtime";
String finalEndpoint = String(apiUrl) + "/final";
// ----------------- Throttle / Debounce -----------------
unsigned long lastSendMillis = 0;
const unsigned long sendThrottleMs = 1000; // envoi max 1 fois par seconde
unsigned long lastTriggerTime1 = 0;
unsigned long lastTriggerTime2 = 0;
const unsigned long sensorDebounceMs = 600; // ignore triggers rapprochés

// ----------------- Melody -----------------
#define NOTE_C4  262
#define NOTE_E4  330
#define NOTE_G4  392
#define NOTE_C5  523
int melodyWin[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
int durWin[] = {8, 8, 8, 4};

// ----------------- Utils -----------------
float readDistance(int trig, int echo)
{
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    // timeout réduit pour éviter blocage long (10 ms -> ~1.7 m). Ajuste si besoin.
    long duration = pulseIn(echo, HIGH, 10000); // 10 000 us = 10 ms
    if (duration == 0) return -1.0;
    return duration * 0.034 / 2.0;
}

void playVictoryMelody(int player)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Joueur ");
    lcd.print(player);
    lcd.print(" gagne!");
    lcd.setCursor(0, 1);
    lcd.print("Score final");
    for (int i = 0; i < 4; i++)
    {
        int duration = 1000 / durWin[i];
        tone(buzzerPin, melodyWin[i], duration);
        delay(duration * 1.3);
        noTone(buzzerPin);
    }
    delay(1200);
    lcd.clear();
}

// send realtime (throttled)
void sendScoreToAPIIfAllowed(int s1, int s2, bool force = false)
{
    unsigned long now = millis();
    if (!force && (now - lastSendMillis) < sendThrottleMs) return;
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(realtimeEndpoint); // ✅ /realtime
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument doc(256);
    doc["event"] = "realtime";
    doc["player1_score"] = s1;
    doc["player2_score"] = s2;
    doc["elapsed_time"] = (millis() - startTime) / 1000.0;
    String body;
    serializeJson(doc, body);

    int code = http.POST(body);
    Serial.printf("POST %s => %d\n", realtimeEndpoint.c_str(), code);
    http.end();
    lastSendMillis = now;
}

void sendFinalResult(int winner, int s1, int s2)
{
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(finalEndpoint); // ✅ /final
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument doc(256);
    doc["event"] = "final";
    doc["winner"] = winner;
    doc["player1_score"] = s1;
    doc["player2_score"] = s2;
    doc["total_time"] = (millis() - startTime) / 1000.0;
    String body;
    serializeJson(doc, body);

    int code = http.POST(body);
    Serial.printf("POST %s => %d\n", finalEndpoint.c_str(), code);
    http.end();
    startTime = millis();
}

// safe trigger (debounce)
bool sensorTriggered(int id)
{
    unsigned long now = millis();
    if (id == 1)
    {
        if (now - lastTriggerTime1 < sensorDebounceMs) return false;
        lastTriggerTime1 = now;
        return true;
    }
    else
    {
        if (now - lastTriggerTime2 < sensorDebounceMs) return false;
        lastTriggerTime2 = now;
        return true;
    }
}

// WiFi connect with timeout (non-blocking style)
void connectWiFiOnce(unsigned long timeoutMs = 15000)
{
    Serial.println("WiFi: starting connect...");
    unsigned long started = millis();
    while (millis() - started < timeoutMs)
    {
        if (wifiMulti.run() == WL_CONNECTED)
        {
            Serial.println("WiFi OK, IP:");
            Serial.println(WiFi.localIP());
            lcd.clear();
            lcd.print("WiFi OK:");
            lcd.setCursor(0, 1);
            lcd.print(WiFi.localIP());
            return;
        }
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nWiFi timeout");
    lcd.clear();
    lcd.print("WiFi echec");
}

void setup()
{
    Serial.begin(115200);
    delay(50);

    // pins
    pinMode(trig1, OUTPUT);
    pinMode(echo1, INPUT);
    pinMode(trig2, OUTPUT);
    pinMode(echo2, INPUT);
    pinMode(buzzerPin, OUTPUT);

    // LCD
    lcd.begin(16, 2);
    lcd.print("Demarrage...");

    // WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); // réduit risque de mise en veille
    // ajoute ton réseau ici (change SSID/PWD si besoin)
    wifiMulti.addAP("isildur", "isildure");
    // wifiMulti.addAP("FallbackSSID", "pwd"); // optionnel

    connectWiFiOnce(15000);

    startTime = millis();
    lcd.clear();
    lcd.print("Pret");
    delay(800);
    lcd.clear();
}

unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 10000; // 10s

void loop()
{
    // périodic WiFi healthcheck
    if (millis() - lastWiFiCheck > wifiCheckInterval)
    {
        lastWiFiCheck = millis();
        if (wifiMulti.run() != WL_CONNECTED)
        {
            Serial.println("WiFi perdu -> reconnexion...");
            lcd.clear();
            lcd.print("Recon WiFi...");
            connectWiFiOnce(10000);
        }
        else
        {
            Serial.printf("WiFi OK (RSSI=%d dBm)\n", WiFi.RSSI());
        }
    }

    // lire distances
    float d1 = readDistance(trig1, echo1);
    float d2 = readDistance(trig2, echo2);

    // Joueur 1 trigger
    if (d1 > 0 && d1 < 3.0)
    {
        if (sensorTriggered(1))
        {
            score1++;
            tone(buzzerPin, 600, 100);
            noTone(buzzerPin);
            Serial.printf("J1 trigger -> %d\n", score1);
            sendScoreToAPIIfAllowed(score1, score2);
        }
    }

    // Joueur 2 trigger
    if (d2 > 0 && d2 < 3.0)
    {
        if (sensorTriggered(2))
        {
            score2++;
            tone(buzzerPin, 800, 100);
            delay(120);
            noTone(buzzerPin);
            Serial.printf("J2 trigger -> %d\n", score2);
            sendScoreToAPIIfAllowed(score1, score2);
        }
    }

    // affichage LCD (rafraîchim. limité)
    static unsigned long lastLCD = 0;
    if (millis() - lastLCD > 300)
    {
        lastLCD = millis();
        lcd.setCursor(0, 0);
        lcd.print("J1:");
        lcd.print(score1);
        lcd.print(" J2:");
        lcd.print(score2);
        lcd.setCursor(0, 1);
        lcd.print("   Go!    ");
    }

    // victoire
    if (score1 >= maxScore)
    {
        sendFinalResult(1, score1, score2);
        playVictoryMelody(1);
        score1 = score2 = 0;
        lcd.clear();
    }
    else if (score2 >= maxScore)
    {
        sendFinalResult(2, score1, score2);
        playVictoryMelody(2);
        score1 = score2 = 0;
        lcd.clear();
    }

    delay(50); // laisse respirer la pile WiFi
}
