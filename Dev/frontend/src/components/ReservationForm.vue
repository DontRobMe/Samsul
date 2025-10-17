<template>
  <v-card class="pa-4 card-style" max-width="400">
    <v-card-title>Réserver un créneau</v-card-title>
    <v-card-text>
      <v-form @submit.prevent="submitReservation">
        <v-text-field
          v-model="date"
          label="Date"
          type="date"
          :min="today"
          :disabled="!!dateProp"
          required
        />
        <v-select
          v-model="startTime"
          :items="timeSlots"
          label="Heure de début"
          required
        />
        <v-select
          v-model="endTime"
          :items="timeSlots"
          label="Heure de fin"
          required
        />
        <v-alert v-if="error" type="error" dense>{{ error }}</v-alert>
        <v-btn type="submit" color="primary">Réserver</v-btn>
      </v-form>
      <v-alert v-if="success" type="success" dense>{{ success }}</v-alert>
    </v-card-text>
  </v-card>
</template>

<script setup>
import { ref, computed, watch } from 'vue';

const props = defineProps({
  dateProp: String
});

const date = ref(props.dateProp || '');
const startTime = ref('');
const endTime = ref('');
const error = ref('');
const success = ref('');
let successTimeout = null;

// Met à jour la date si la prop change
watch(() => props.dateProp, (newVal) => {
  if (newVal) date.value = newVal;
});

// Calcul de la date du jour au format YYYY-MM-DD
const today = new Date().toISOString().split('T')[0];

// Créneaux de 15 minutes de 08:00 à 20:00
function generateTimeSlots() {
  const slots = [];
  let hour = 8;
  let minute = 0;
  while (hour < 20 || (hour === 20 && minute === 0)) {
    const h = hour.toString().padStart(2, '0');
    const m = minute.toString().padStart(2, '0');
    slots.push(`${h}:${m}`);
    minute += 15;
    if (minute === 60) {
      minute = 0;
      hour++;
    }
  }
  return slots;
}
const timeSlots = generateTimeSlots();

function timeToMinutes(time) {
  const [h, m] = time.split(':').map(Number);
  return h * 60 + m;
}

function submitReservation() {
  error.value = '';
  success.value = '';
  if (successTimeout) {
    clearTimeout(successTimeout);
    successTimeout = null;
  }
  if (!date.value || !startTime.value || !endTime.value) {
    error.value = 'Veuillez remplir tous les champs.';
    return;
  }
  // Vérification que la date n'est pas antérieure à aujourd'hui
  if (date.value < today) {
    error.value = 'Impossible de réserver à une date antérieure à aujourd\'hui.';
    return;
  }
  const start = timeToMinutes(startTime.value);
  const end = timeToMinutes(endTime.value);
  if (end <= start) {
    error.value = "L'heure de fin doit être après l'heure de début.";
    return;
  }
  if (end - start < 15) {
    error.value = 'La durée minimale est de 15 minutes.';
    return;
  }
  success.value = `Réservation confirmée le ${date.value} de ${startTime.value} à ${endTime.value}.`;
  successTimeout = setTimeout(() => {
    success.value = '';
    successTimeout = null;
  }, 3000);
}
</script>

<style scoped>
.v-card {
  margin: auto;
}

.card-style {
  box-shadow: 0 4px 24px rgba(50, 168, 82, 0.08);
  border-radius: 16px;
  padding: 24px 0;
}
</style>
