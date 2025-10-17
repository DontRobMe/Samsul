<script setup lang="ts">
import { ref, onMounted } from 'vue'
import Header from "@/components/Header.vue";

type Reservation = { id: number; time: string; teams: string; score?: string }
type Bet = { id: number; time: string; teams: string; amount: number; result?: string }

const reservations = ref<Reservation[]>([])
const bets = ref<Bet[]>([])
const loading = ref(true)
const error = ref('')

const sampleReservations: Reservation[] = [
  { id: 1, time: new Date(Date.now() + 3600_000).toISOString(), teams: 'Red Rockets vs Blue Comets', score: '—' },
  { id: 2, time: new Date(Date.now() + 86_400_000).toISOString(), teams: 'Tiny Titans vs Mini Meteors', score: '—' }
]

const sampleBets: Bet[] = [
  { id: 1, time: new Date(Date.now() - 86_400_000).toISOString(), teams: 'Red Rockets vs Blue Comets', amount: 10, result: 'Gagné' },
  { id: 2, time: new Date(Date.now() - 2 * 86_400_000).toISOString(), teams: 'Tiny Titans vs Mini Meteors', amount: 5, result: 'Perdu' }
]

const formatDate = (iso?: string) => {
  if (!iso) return '—'
  const d = new Date(iso)
  return d.toLocaleString()
}

async function fetchData() {
  loading.value = true
  const opts = { headers: { Accept: 'application/json' } }

  try {
    const [resR, resB] = await Promise.all([
      fetch('/api/user/reservations', opts),
      fetch('/api/user/bets', opts)
    ])

    const parseJsonOrThrow = async (res: Response) => {
      const ct = res.headers.get('content-type') || ''
      const text = await res.text()
      if (!res.ok) {
        console.error('API error response:', text)
        throw new Error(`Erreur serveur ${res.status}`)
      }
      if (!ct.includes('application/json')) {
        console.error('Réponse non JSON:', { status: res.status, text })
        throw new Error('Réponse inattendue (pas du JSON)')
      }
      try {
        return JSON.parse(text)
      } catch {
        console.error('JSON parse failed:', text)
        throw new Error('Impossible de parser le JSON reçu')
      }
    }

    // L'API peut renvoyer des champs supplémentaires ; on assigne et TypeScript infère
    reservations.value = await parseJsonOrThrow(resR)
    bets.value = await parseJsonOrThrow(resB)
    error.value = ''
  } catch (e: any) {
    console.warn('Fetch failed — utilisation des données en dur', e)
    reservations.value = sampleReservations
    bets.value = sampleBets
    error.value = ''
  } finally {
    loading.value = false
  }
}

onMounted(fetchData)
</script>

<template>
  <Header />
  <div class="user-page">
    <div class="card">
      <h2>Mes réservations</h2>
      <div v-if="loading" class="muted">Chargement...</div>
      <div v-else-if="error" class="error">{{ error }}</div>
      <ul v-else class="list">
        <li v-for="r in reservations" :key="r.id" class="item">
          <div class="left">
            <div class="match">{{ r.teams }}</div>
          </div>
          <div class="right">
            <div class="time">{{ formatDate(r.time) }}</div>
            <div class="meta">Score: {{ r.score ?? '—' }}</div>
          </div>
        </li>
      </ul>
    </div>

    <div class="card">
      <h2>Historique de paris</h2>
      <div v-if="loading" class="muted">Chargement...</div>
      <div v-else-if="error" class="error">{{ error }}</div>
      <ul v-else class="list">
        <li v-for="b in bets" :key="b.id" class="item">
          <div class="left">
            <div class="match">{{ b.teams }}</div>
          </div>
          <div class="right">
            <div class="time">{{ formatDate(b.time) }}</div>
            <div class="meta">Mise: {{ b.amount }} crédits • Résultat: {{ b.result ?? 'En attente' }}</div>
          </div>
        </li>
      </ul>
    </div>
  </div>
</template>

<style>
/* fond blanc cassé pour toute l'application */
html, body, #app {
  background: #f7f3ef;
  min-height: 100%;
  margin: 0;
}
</style>

<style scoped>
.user-page {
  display: flex;
  flex-direction: column;
  gap: 24px;
  align-items: center;
  padding: 40px 20px;
  min-height: calc(100vh - 0px);
  box-sizing: border-box;
}

.card {
  width: 100%;
  max-width: 980px;
  background: #ffffff;
  border-radius: 12px;
  padding: 20px;
  box-shadow: 0 8px 20px rgba(16, 24, 40, 0.06);
  text-align: left;
}

h2 {
  margin: 0 0 12px 0;
  text-align: center;
}

.list {
  list-style: none;
  padding: 0;
  margin: 0;
  display: grid;
  gap: 12px;
}

.item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 14px;
  border-radius: 10px;
  background: #fff8f5;
  gap: 12px;
}

.left { display:flex; flex-direction:column; gap:6px; }
.right { display:flex; flex-direction:column; align-items:flex-end; gap:6px; min-width:160px; }

.match { font-weight: 600; font-size: 1rem; }
.meta { color: #555; font-size: 0.9rem; }
.time { color: #666; font-size: 0.9rem; }

.muted { color: #666; text-align: center; padding: 8px 0; }
.error { color: #b00020; text-align: center; padding: 8px 0; }
</style>
