<template>
  <Header />
  <div class="pa-6">
    <h1 class="reservation-title">Réservation</h1>
    <WeeklyCalendar :selectedDate="selectedDate" @select="onSelectDay" />
    <ReservationForm v-if="selectedDate" :dateProp="selectedDate" />
  </div>
</template>

<script setup>
import Header from '@/components/Header.vue'
import ReservationForm from '@/components/ReservationForm.vue'
import WeeklyCalendar from '@/components/WeeklyCalendar.vue'
import { ref } from 'vue'

const selectedDate = ref('')
function onSelectDay(date) {
  selectedDate.value = date
}
</script>

<style scoped>
.reservation-title {
  text-align: center;
  color: #A7C6DB;
}
</style>
