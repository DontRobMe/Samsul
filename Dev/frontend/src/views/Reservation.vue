<template>
  <Header />
  <div class="pa-6">
    <h1 class="reservation-title">Réservation</h1>
    <WeeklyCalendar :selectedDate="selectedDate" @select="onSelectDay" />
    <ReservationForm v-if="selectedDate" :dateProp="selectedDate" />
  </div>
</template>

<script setup lang="ts">
import Header from '@/components/Header.vue'
import ReservationForm from '@/components/ReservationForm.vue'
import WeeklyCalendar from '@/components/WeeklyCalendar.vue'
import { ref } from 'vue'

const selectedDate = ref('')
function onSelectDay(date: string) {
  selectedDate.value = date
}

const today = new Date()
function getWeekDates(): string[] {
  // Trouve le lundi de la semaine courante
  const dayOfWeek = today.getDay() === 0 ? 6 : today.getDay() - 1
  const monday = new Date(today)
  monday.setDate(today.getDate() - dayOfWeek)
  return Array.from({ length: 7 }, (_, i) => {
    const d = new Date(monday)
    d.setDate(monday.getDate() + i)
    return d.toISOString().slice(0, 10) // YYYY-MM-DD
  })
}
const weekDates = getWeekDates()

function availableSlots(dayIdx: number) {
  const date = weekDates[dayIdx]
  return Array.from({ length: slotsPerDay }, (_, i) => i)
    .filter(slot => !reservations.value.some(r => r.date === date && r.slot === slot))
    .map(slot => ({ value: slot, label: slotTime(slot) }))
}

async function reserveSlot(dayIdx: number) {
  const slot = selectedSlots.value[dayIdx]
  const date = weekDates[dayIdx]
  if (slot === -1) return
  loading.value = true
  error.value = ''
  success.value = ''
  try {
    const res = await fetch('http://35.180.130.22:443/api/calendar/reserve', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ date, slot })
    })
    if (!res.ok) {
      const data = await res.json()
      throw new Error(data.error || 'Erreur serveur')
    }
    success.value = `Créneau réservé pour ${daysOfWeek[dayIdx]} (${date}) à ${slotTime(slot)}`
    selectedSlots.value[dayIdx] = -1
    await fetchReservations()
  } catch (e: any) {
    error.value = e.message
  } finally {
    loading.value = false
  }
}
</script>

<style scoped>
.reservation-title {
  text-align: center;
  color: #A7C6DB;
}
</style>