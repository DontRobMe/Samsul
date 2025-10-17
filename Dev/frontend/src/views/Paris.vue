<template>
  <Header />
  <section class="bets-page">
    <header class="page-header">
      <h1>Paris</h1>
      <div class="bettor-box">
        <label>
          Bettor ID
          <input v-model.trim="bettorIdInput" @change="saveBettorId" placeholder="ex: 1" />
        </label>
        <button class="btn" @click="createTemporaryBettor" v-if="!bettorId">Créer parieur temporaire</button>
      </div>
    </header>

    <div class="filters">
      <label>
        Table
        <input v-model.trim="filters.table_id" placeholder="ex: T1" />
      </label>
      <label>
        Dès le
        <input type="datetime-local" v-model="filters.from" />
      </label>
      <label>
        Jusqu’au
        <input type="datetime-local" v-model="filters.to" />
      </label>
      <button class="btn" @click="loadMatches" :disabled="loading">Filtrer</button>
    </div>

    <div class="list">
      <div v-if="loading" class="empty">Chargement…</div>
      <div v-else-if="error" class="error">{{ error }}</div>
      <div v-else-if="matches.length === 0" class="empty">Aucun match à venir.</div>

      <article v-for="m in matches" :key="m.game_id" class="match-card">
        <header class="match-head">
          <div class="when">
            <strong>{{ fmtDate(m.game_datetime) }}</strong>
            <small v-if="m.table_id">Table : {{ m.table_id }}</small>
          </div>
          <div class="meta">
            <small>ID: {{ m.game_id }}</small>
          </div>
        </header>

        <div class="teams">
          <div class="team">
            <label>
              <input
                type="radio"
                :name="`winner-${m.game_id}`"
                value="Red"
                v-model="formState[m.game_id].selectedWinner"
                :disabled="placing[m.game_id]"
              />
              <span class="pill red">Red</span>
            </label>
          </div>
          <div class="vs">vs</div>
          <div class="team">
            <label>
              <input
                type="radio"
                :name="`winner-${m.game_id}`"
                value="Blue"
                v-model="formState[m.game_id].selectedWinner"
                :disabled="placing[m.game_id]"
              />
              <span class="pill blue">Blue</span>
            </label>
          </div>
        </div>

        <div class="stake-row">
          <label>Mise (€)
            <input
              type="number"
              min="1"
              step="1"
              v-model.number="formState[m.game_id].stakeEuros"
              :disabled="placing[m.game_id]"
            />
          </label>

          <label>Cote (num/den)
            <input
              class="odds"
              type="number"
              min="1"
              step="1"
              v-model.number="formState[m.game_id].oddsNum"
              :disabled="placing[m.game_id]"
            />
            <span>/</span>
            <input
              class="odds"
              type="number"
              min="1"
              step="1"
              v-model.number="formState[m.game_id].oddsDen"
              :disabled="placing[m.game_id]"
            />
          </label>

          <div class="payout">
            <small>Gain potentiel</small>
            <strong>€ {{ potentialPayout(formState[m.game_id]).toFixed(2) }}</strong>
          </div>

          <button
            class="btn primary"
            @click="placeBet(m)"
            :disabled="!canPlace(m) || placing[m.game_id]"
          >
            {{ placing[m.game_id] ? 'Validation…' : 'Placer le pari' }}
          </button>
        </div>

        <footer class="status-row">
          <span v-if="placedOk[m.game_id]" class="ok">Pari créé ✅</span>
          <span v-else-if="placedErr[m.game_id]" class="err">Erreur : {{ placedErr[m.game_id] }}</span>
        </footer>
      </article>
    </div>

    <div class="pagination" v-if="!loading && matches.length > 0">
      <button class="btn" @click="prevPage" :disabled="page===0 || loading">« Précédent</button>
      <span>Page {{ page+1 }}</span>
      <button class="btn" @click="nextPage" :disabled="loading || matches.length < limit">Suivant »</button>
    </div>
  </section>
</template>

<script setup>
import { reactive, ref, onMounted } from 'vue'
import Header from '@/components/Header.vue'
import mockData from '../data/mockData';

// ⚙️ Config API
const API_BASE = import.meta.env.VITE_API_BASE || 'http://localhost:4000'

const loading = ref(false)
const error = ref('')
const matches = ref([])
const limit = 20
const page = ref(0)

// Filtres
const filters = reactive({
  table_id: '',
  from: '',
  to: ''
})

// Bettor
const bettorId = ref(localStorage.getItem('bettor_id') || '')
const bettorIdInput = ref(bettorId.value)
function saveBettorId() {
  bettorId.value = bettorIdInput.value
  if (bettorId.value) localStorage.setItem('bettor_id', bettorId.value)
}

const formState = reactive({})
const placing   = reactive({})
const placedOk  = reactive({})
const placedErr = reactive({})

function centsFromEuros(e) { return Math.max(0, Math.round(Number(e || 0) * 100)) }

function potentialPayout(fs) {
  const stakeCents = centsFromEuros(fs?.stakeEuros)
  if (!stakeCents) return 0
  const num = Number(fs?.oddsNum || 1)
  const den = Number(fs?.oddsDen || 1)
  return (stakeCents * num / den) / 100
}

function fmtDate(iso) {
  try {
    const d = new Date(iso)
    return d.toLocaleString()
  } catch { return iso }
}

function initFormForMatch(m) {
  if (!formState[m.game_id]) {
    formState[m.game_id] = {
      selectedWinner: 'Red',
      stakeEuros: 5,
      oddsNum: 2,
      oddsDen: 1
    }
  }
}

async function loadMatches() {
  loading.value = true
  error.value = ''
  try {
    const params = new URLSearchParams()
    params.set('limit', String(limit))
    params.set('offset', String(page.value * limit))
    params.set('include_players', '0')

    const nowIso = new Date().toISOString()
    params.set('from', filters.from ? new Date(filters.from).toISOString() : nowIso)
    if (filters.to) params.set('to', new Date(filters.to).toISOString())
    if (filters.table_id) params.set('table_id', filters.table_id)

    const res = await fetch(`${API_BASE}/matches/upcoming?` + params.toString())
    if (!res.ok) throw new Error(`HTTP ${res.status}`)
    const data = await res.json()
    matches.value = Array.isArray(data) ? data : []
    matches.value.forEach(initFormForMatch)
  } catch (e) {
    error.value = e?.message || 'Erreur de chargement'
  } finally {
    loading.value = false
  }
}

function canPlace(m) {
  const fs = formState[m.game_id]
  return !!bettorId.value && fs?.selectedWinner && Number(fs?.stakeEuros) >= 1 && Number(fs?.oddsNum) >= 1 && Number(fs?.oddsDen) >= 1
}

async function placeBet(m) {
  const fs = formState[m.game_id]
  placedOk[m.game_id] = false
  placedErr[m.game_id] = ''

  if (!canPlace(m)) {
    placedErr[m.game_id] = 'Champs incomplets'
    return
  }
  placing[m.game_id] = true

  try {
    const betId = `bet_${m.game_id}_${Date.now()}`
    const body = {
      bet_id: betId,
      bettor_id: Number(bettorId.value) || bettorId.value,
      game_id: m.game_id,
      bet_type_id: 'moneyline',
      stake_cents: centsFromEuros(fs.stakeEuros),
      odds_num: Number(fs.oddsNum),
      odds_den: Number(fs.oddsDen),
      selections: [
        { selection_key: `winner:${fs.selectedWinner}`, odds_num: Number(fs.oddsNum), odds_den: Number(fs.oddsDen) }
      ]
    }

    const res = await fetch(`${API_BASE}/paris`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(body)
    })

    if (!res.ok) {
      const errJson = await safeJson(res)
      throw new Error(errJson?.error || `HTTP ${res.status}`)
    }
    placedOk[m.game_id] = true
  } catch (e) {
    placedErr[m.game_id] = e?.message || 'Erreur inconnue'
  } finally {
    placing[m.game_id] = false
  }
}

async function safeJson(res) {
  try { return await res.json() } catch { return null }
}

async function createTemporaryBettor() {
  try {
    const display = `guest_${Date.now()}`
    const res = await fetch(`${API_BASE}/bettors`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ display_name: display })
    })
    if (!res.ok) throw new Error(`HTTP ${res.status}`)
    const data = await res.json()
    bettorIdInput.value = String(data.bettor_id)
    saveBettorId()
  } catch (e) {
    alert('Impossible de créer un parieur temporaire: ' + (e?.message || 'Erreur'))
  }
}

function nextPage() { page.value += 1; loadMatches() }
function prevPage() { if (page.value > 0) { page.value -= 1; loadMatches() } }

onMounted(loadMatches)
</script>

<style scoped>
.bets-page {
  max-width: 960px;
  margin: 24px auto;
  padding: 12px;
}
.page-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 12px;
}
.page-header h1 { margin: 0; }
.bettor-box {
  display: flex;
  align-items: center;
  gap: 8px;
}
.bettor-box input { width: 160px; }

.filters {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr)) auto;
  gap: 8px;
  align-items: end;
  margin-bottom: 16px;
}
.list { display: grid; gap: 12px; }
.empty, .error { padding: 12px; border-radius: 8px; }
.empty { background: #f5f7fa; color: #444; }
.error { background: #ffe8e8; color: #b00020; }

.match-card {
  border: 1px solid #e6e8eb;
  border-radius: 12px;
  padding: 12px;
  background: #fff;
}
.match-head {
  display: flex;
  align-items: baseline;
  justify-content: space-between;
  margin-bottom: 8px;
}
.when strong { font-size: 1.05rem; margin-right: 8px; }
.meta small { color: #666; }

.teams {
  display: grid;
  grid-template-columns: 1fr auto 1fr;
  align-items: center;
  gap: 8px;
  margin: 8px 0 12px;
}
.team label { display: inline-flex; align-items: center; gap: 8px; }
.pill {
  display: inline-block;
  padding: 4px 10px;
  border-radius: 999px;
  font-weight: 600;
  border: 1px solid #ddd;
}
.pill.red { background: #ffe3e3; }
.pill.blue { background: #e3efff; }
.vs { color: #666; font-weight: 600; }

.stake-row {
  display: grid;
  grid-template-columns: 180px 220px 1fr auto;
  gap: 12px;
  align-items: center;
}
.stake-row label { display: flex; align-items: center; gap: 8px; }
.stake-row input[type="number"], .stake-row input[type="text"] {
  width: 100%;
}
.odds { width: 80px; }
.payout { text-align: right; }
.payout small { display: block; color: #666; }
.payout strong { font-size: 1.1rem; }

.status-row { margin-top: 8px; min-height: 20px; }
.ok { color: #136f2e; font-weight: 600; }
.err { color: #b00020; font-weight: 600; }

.pagination {
  display: flex;
  gap: 12px;
  align-items: center;
  justify-content: center;
  margin-top: 16px;
}

.btn {
  appearance: none; border: 1px solid #ddd; background: #f9fafb;
  padding: 8px 12px; border-radius: 8px; cursor: pointer;
}
.btn:hover { background: #f0f2f5; }
.btn:disabled { opacity: 0.6; cursor: not-allowed; }
.btn.primary { background: #111827; color: white; border-color: #111827; }
.btn.primary:hover { background: #000; }
</style>

