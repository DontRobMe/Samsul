<template>
  <div class="calendar-container">
    <div
      v-for="(day, idx) in weekDays"
      :key="idx"
      class="calendar-day"
      :class="{ selected: day.date === selectedDate }"
      @click="$emit('select', day.date)"
    >
      <div class="day-label">{{ day.label }}</div>
      <div class="day-date">{{ day.display }}</div>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, watch } from 'vue';

const props = defineProps({
  selectedDate: String
});

const today = new Date();
const currentYear = today.getFullYear();
// Liste simplifiée des jours fériés français (fixes + calcul de Pâques)
function getEasterDate(year) {
  // Algorithme de Meeus/Jones/Butcher
  const f = Math.floor,
    G = year % 19,
    C = f(year / 100),
    H = (C - f(C / 4) - f((8 * C + 13) / 25) + 19 * G + 15) % 30,
    I = H - f(H / 28) * (1 - f(29 / (H + 1)) * f((21 - G) / 11)),
    J = (year + f(year / 4) + I + 2 - C + f(C / 4)) % 7,
    L = I - J,
    month = 3 + f((L + 40) / 44),
    day = L + 28 - 31 * f(month / 4);
  return new Date(year, month - 1, day);
}
const easter = getEasterDate(currentYear);
function formatDate(d) {
  return d.toISOString().split('T')[0];
}
const holidays = [
  `${currentYear}-01-01`, // Jour de l'an
  `${currentYear}-05-01`, // Fête du Travail
  `${currentYear}-05-08`, // Victoire 1945
  `${currentYear}-07-14`, // Fête nationale
  `${currentYear}-08-15`, // Assomption
  `${currentYear}-11-01`, // Toussaint
  `${currentYear}-11-11`, // Armistice
  `${currentYear}-12-25`, // Noël
  formatDate(easter), // Pâques
  formatDate(new Date(easter.getFullYear(), easter.getMonth(), easter.getDate() + 1)), // Lundi de Pâques
  formatDate(new Date(easter.getFullYear(), easter.getMonth(), easter.getDate() + 39)), // Ascension
  formatDate(new Date(easter.getFullYear(), easter.getMonth(), easter.getDate() + 49)), // Pentecôte
  formatDate(new Date(easter.getFullYear(), easter.getMonth(), easter.getDate() + 50)), // Lundi de Pentecôte
];
function isHoliday(dateStr) {
  return holidays.includes(dateStr);
}
function isWeekend(d) {
  const day = d.getDay();
  return day === 0 || day === 6;
}
const weekDays = [];
let i = 0;
while (weekDays.length < 6 && i < 14) { // Limite à 2 semaines pour éviter boucle infinie
  const d = new Date(today);
  d.setDate(today.getDate() + i);
  const dateStr = formatDate(d);
  if (!isWeekend(d) && !isHoliday(dateStr)) {
    const label = d.toLocaleDateString('fr-FR', { weekday: 'short' });
    const display = d.toLocaleDateString('fr-FR', { day: '2-digit', month: '2-digit' });
    weekDays.push({ label, display, date: dateStr });
  }
  i++;
}
</script>

<style scoped>
.calendar-container {
  display: flex;
  justify-content: center;
  gap: 16px;
  margin-bottom: 32px;
}
.calendar-day {
  cursor: pointer;
  padding: 16px 12px;
  border-radius: 10px;
  background: #f5f8fa;
  border: 2px solid transparent;
  min-width: 70px;
  text-align: center;
  transition: border 0.2s, background 0.2s;
}
.calendar-day.selected {
  border: 2px solid #A7C6DB;
  background: #e6f0f7;
}
.calendar-day:hover {
  background: #dbeaf5;
  border: 2px solid #A7C6DB;
}
.day-label {
  font-weight: bold;
  margin-bottom: 4px;
}
.day-date {
  font-size: 1.1em;
}
</style>
