<template>
  <Header />
  <section class="bets-page">
    <header class="page-header">
      <h1>Matches à venir</h1>

      <div class="credits-box">
        <div class="user-name">Thomas Duppi</div>
        <div class="credit-line">
          <strong>Crédits :</strong> <span class="credits">{{ credits }} crédits</span>
        </div>
      </div>
    </header>

    <div class="list">
      <div v-if="loading" class="empty">Chargement…</div>
      <div v-else-if="error" class="error">{{ error }}</div>
      <div v-else-if="visibleMatches.length === 0" class="empty">Aucun match à venir.</div>

      <article v-for="m in visibleMatches" :key="m.game_id" class="match-card">
        <header class="match-head">
          <div class="when">
            <strong>{{ fmtDate(m.game_datetime) }}</strong>
            <small v-if="m.table_id">• Table : {{ m.table_id }}</small>
          </div>
          <div class="meta">
            <span class="state-badge" :class="stateClass(m)">{{ stateLabel(m) }}</span>
          </div>
        </header>

        <div class="teams-line" v-if="m.red_team_name || m.blue_team_name">
          <span class="pill red" v-if="m.red_team_name">{{ m.red_team_name }}</span>
          <span class="vs">vs</span>
          <span class="pill blue" v-if="m.blue_team_name">{{ m.blue_team_name }}</span>
        </div>

        <div class="bet-actions">
          <button
            class="btn-bet"
            :disabled="!canBet(m)"
            :title="!canBet(m) ? 'Pari fermé pour ce match' : 'Parier sur ce match'"
            @click="canBet(m) && openBetModal(m)"
          >
            Parier
          </button>
        </div>
      </article>
    </div>

    <div class="pagination" v-if="!loading && visibleMatches.length > 0">
      <button class="btn" @click="prevPage" :disabled="page===0 || loading">« Précédent</button>
      <span>Page {{ page+1 }}</span>
      <button class="btn" @click="nextPage" :disabled="loading || visibleMatches.length < limit">Suivant »</button>
    </div>
  </section>

  <div v-if="showBetModal" class="modal-backdrop" @click.self="closeBetModal" @keyup.esc="closeBetModal" tabindex="0">
    <div class="modal" role="dialog" aria-modal="true">
      <header class="modal-head">
        <h2>Placer un pari</h2>
        <button class="modal-close" @click="closeBetModal" aria-label="Fermer">✕</button>
      </header>

      <div class="modal-body" v-if="selectedMatch">
        <p class="modal-matchline">
          <strong>{{ teamName(selectedMatch, 'red') }}</strong>
          <span class="vs">vs</span>
          <strong>{{ teamName(selectedMatch, 'blue') }}</strong>
          <small v-if="selectedMatch.table_id">• Table : {{ selectedMatch.table_id }}</small>
        </p>

        <div class="field">
          <label class="label">Vainqueur</label>
          <div class="radio-group">
            <label class="radio">
              <input type="radio" value="red" v-model="betForm.team" />
              <span class="pill red">{{ teamName(selectedMatch, 'red') }}</span>
            </label>
            <label class="radio">
              <input type="radio" value="blue" v-model="betForm.team" />
              <span class="pill blue">{{ teamName(selectedMatch, 'blue') }}</span>
            </label>
          </div>
          <p v-if="errors.team" class="err">{{ errors.team }}</p>
        </div>

        <div class="field">
          <label class="label" for="bet-amount">Montant (pts)</label>
          <input
            id="bet-amount"
            class="input"
            type="number"
            min="1"
            :max="credits"
            v-model.number="betForm.amount"
            placeholder="Ex: 10"
          />
          <p class="hint">Crédits disponibles : {{ credits }} pts</p>
          <p v-if="errors.amount" class="err">{{ errors.amount }}</p>
        </div>
      </div>

      <footer class="modal-foot">
        <button class="btn secondary" @click="closeBetModal">Annuler</button>
        <button class="btn primary" @click="confirmBet">Confirmer le pari</button>
      </footer>
    </div>
  </div>
</template>

<script setup>
import { reactive, ref, onMounted, onUnmounted, computed } from 'vue'
import Header from '@/components/Header.vue'

const API_BASE = import.meta.env.VITE_API_BASE || 'http://localhost:4000'
const USE_MOCK = true
const MATCH_DURATION_MIN = 30
const REFRESH_MS = 30_000

const loading = ref(false)
const error = ref('')
const matches = ref([])
const limit = 20
const page = ref(0)
const credits = ref(120)
const placedBets = ref([])
const filters = reactive({ table_id: '', from: '', to: '' })
const nowTick = ref(Date.now())
let timerId

function fmtDate(iso) {
  try {
    const d = new Date(iso)
    return d.toLocaleString('fr-FR', { dateStyle: 'short', timeStyle: 'short' })
  } catch { return iso }
}

function getState(m) {
  const now = nowTick.value

  if (m.status) {
    const s = String(m.status).toLowerCase()
    if (['finished','done','ended','termine','terminé'].includes(s)) return 'finished'
    if (['live','in_progress','ongoing','en_cours'].includes(s)) return 'in_progress'
  }
  const start = new Date(m.game_datetime).getTime()
  const end = start + MATCH_DURATION_MIN * 60 * 1000
  if (now >= end) return 'finished'
  if (now >= start && now < end) return 'in_progress'
  return 'upcoming'
}
function stateLabel(m) {
  const s = getState(m)
  if (s === 'in_progress') return 'En cours'
  if (s === 'finished') return 'Terminé'
  return 'À venir'
}
function stateClass(m) {
  const s = getState(m)
  return {
    'badge-upcoming': s === 'upcoming',
    'badge-live': s === 'in_progress',
    'badge-finished': s === 'finished'
  }
}
function canBet(m) {
  return getState(m) === 'upcoming'
}

const showBetModal = ref(false)
const selectedMatch = ref(null)
const betForm = reactive({ team: '', amount: 10 })
const errors = reactive({ team: '', amount: '' })

function teamName(match, color) {
  return color === 'red' ? (match.red_team_name || 'Équipe Rouge') : (match.blue_team_name || 'Équipe Bleue')
}
function openBetModal(match) {
  if (!canBet(match)) return
  selectedMatch.value = match
  betForm.team = match.red_team_name ? 'red' : 'blue'
  betForm.amount = Math.min(10, Math.max(1, credits.value))
  errors.team = ''; errors.amount = ''
  showBetModal.value = true
}
function closeBetModal() {
  showBetModal.value = false
  selectedMatch.value = null
}
function validateBet() {
  errors.team = ''
  errors.amount = ''
  if (!betForm.team) errors.team = 'Choisis une équipe.'
  const amt = Number(betForm.amount)
  if (!amt || amt < 1) errors.amount = 'Montant invalide (minimum 1).'
  else if (amt > credits.value) errors.amount = `Montant trop élevé (max ${credits.value}).`
  return !(errors.team || errors.amount)
}
function confirmBet() {
  if (!validateBet()) return
  const amt = Number(betForm.amount)
  credits.value = Math.max(0, credits.value - amt)
  placedBets.value.push({
    game_id: selectedMatch.value.game_id,
    team: betForm.team,
    amount: amt,
    at: new Date().toISOString(),
  })
  alert(`Pari placé : ${teamName(selectedMatch.value, betForm.team)} pour ${amt} pts`)
  closeBetModal()
}

const allMockMatches = ref([])
function buildMockData() {
  const tables = ['T1','T2','T3','T4']
  const teams = [
    ['Les Lynx','Les Aigles'],
    ['Red Rockets','Blue Bears'],
    ['Les Rapides','Les Titans'],
    ['Thunder Squad','Ice Wolves'],
    ['Les Phoenix','Les Dragons']
  ]

  const base = new Date()
  base.setMinutes(0,0,0)

  const arr = []
  let id = 1000
  for (let d = -1; d < 10; d++) {
    for (let h = 10; h <= 22; h += 2) {
      const dt = new Date(base)
      dt.setDate(base.getDate() + d)
      dt.setHours(h)
      const [r, b] = teams[(d + h + teams.length) % teams.length]
      const table = tables[(d + h + tables.length) % tables.length]
      arr.push({
        game_id: String(id++),
        game_datetime: dt.toISOString(),
        table_id: table,
        red_team_name: r,
        blue_team_name: b,
      })
    }
  }
  allMockMatches.value = arr
}

const filteredMock = computed(() => {
  const now = new Date(nowTick.value)
  const defaultFrom = new Date(now.getTime() - MATCH_DURATION_MIN * 60 * 1000).toISOString()
  const fromISO = filters.from ? new Date(filters.from).toISOString() : defaultFrom
  const toISO = filters.to ? new Date(filters.to).toISOString() : null

  return allMockMatches.value.filter(m => {
    if (m.game_datetime < fromISO) return false
    if (toISO && m.game_datetime > toISO) return false
    if (filters.table_id && String(m.table_id).toLowerCase() !== String(filters.table_id).toLowerCase()) return false
    return true
  })
})

const visibleFiltered = computed(() => filteredMock.value.filter(m => getState(m) !== 'finished'))

function slicePage(arr) {
  const start = page.value * limit
  const end = start + limit
  return arr.slice(start, end)
}
const visibleMatches = computed(() => slicePage(visibleFiltered.value))

async function loadMatches() {
  loading.value = true
  error.value = ''
  try {
    if (USE_MOCK) return
    const params = new URLSearchParams()
    params.set('limit', String(limit))
    params.set('offset', String(page.value * limit))
    params.set('include_players', '0')

    // Inclure les matchs "en cours" (from = now - duration)
    const now = new Date(nowTick.value)
    const from = new Date(now.getTime() - MATCH_DURATION_MIN * 60 * 1000)
    const fromIso = filters.from ? new Date(filters.from).toISOString() : from.toISOString()
    params.set('from', fromIso)
    if (filters.to) params.set('to', new Date(filters.to).toISOString())
    if (filters.table_id) params.set('table_id', filters.table_id)

    const res = await fetch(`${API_BASE}/matches/upcoming?` + params.toString())
    if (!res.ok) throw new Error(`HTTP ${res.status}`)
    const data = await res.json()
    matches.value = Array.isArray(data) ? data : []
  } catch (e) {
    error.value = e?.message || 'Erreur de chargement'
  } finally {
    loading.value = false
  }
}

function applyFilters() { page.value = 0; loadMatches() }
function resetFilters() { filters.table_id=''; filters.from=''; filters.to=''; page.value=0; loadMatches() }
function nextPage() { page.value += 1 }
function prevPage() { if (page.value > 0) page.value -= 1 }

onMounted(() => {
  buildMockData()
  loadMatches()
  timerId = setInterval(() => {
    nowTick.value = Date.now()            // force recalcul des états
    if (!USE_MOCK) loadMatches()          // rafraîchit la liste via l’API si besoin
  }, REFRESH_MS)
})
onUnmounted(() => {
  if (timerId) clearInterval(timerId)
})
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

/* 🧍 Crédits utilisateur */
.credits-box {
  background: #f0f4ff;
  border: 1px solid #d2dcff;
  border-radius: 10px;
  padding: 8px 14px;
  display: flex;
  flex-direction: column;
  align-items: flex-end;
  text-align: right;
  font-size: 0.9rem;
  min-width: 180px;
}
.user-name { font-weight: 600; color: #1a237e; }
.credit-line { color: #333; }
.credits { color: #007b00; font-weight: 600; }

/* État */
.state-badge {
  display: inline-block;
  padding: 3px 8px;
  border-radius: 999px;
  font-size: 0.8rem;
  font-weight: 600;
  border: 1px solid transparent;
}
.badge-upcoming { background: #eef7ff; border-color: #cfe6ff; color: #0b5cab; }
.badge-live { background: #fff4e5; border-color: #ffd8a8; color: #b35c00; }
.badge-finished { background: #f1f3f5; border-color: #dee2e6; color: #6c757d; }

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
.meta { display: flex; gap: 8px; align-items: center; }

.teams-line {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-top: 6px;
}
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

.bet-actions { margin-top: 10px; text-align: right; }
.btn-bet {
  background: #2e7d32;
  color: white;
  border: none;
  border-radius: 8px;
  padding: 8px 14px;
  cursor: pointer;
  font-weight: 600;
  transition: background 0.2s ease;
}
.btn-bet:hover { background: #1b5e20; }
.btn-bet:disabled { opacity: 0.6; cursor: not-allowed; filter: grayscale(0.2); }

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

/* Modal */
.modal-backdrop {
  position: fixed; inset: 0;
  background: rgba(0,0,0,0.35);
  display: grid; place-items: center;
  z-index: 1000;
}
.modal {
  background: #fff;
  width: min(560px, calc(100% - 24px));
  border-radius: 14px;
  border: 1px solid #e6e8eb;
  box-shadow: 0 10px 30px rgba(0,0,0,0.15);
  overflow: hidden;
}
.modal-head {
  display: flex; align-items: center; justify-content: space-between;
  padding: 12px 16px; border-bottom: 1px solid #eee;
}
.modal-head h2 { margin: 0; font-size: 1.1rem; }
.modal-close {
  background: transparent; border: none; font-size: 18px; cursor: pointer; line-height: 1;
}
.modal-body { padding: 14px 16px; }
.modal-matchline { display: flex; align-items: center; gap: 8px; margin-bottom: 10px; }
.field { margin-top: 10px; }
.label { display: block; font-weight: 600; margin-bottom: 6px; }
.input {
  width: 100%; padding: 8px 10px; border: 1px solid #ddd; border-radius: 8px;
}
.radio-group { display: flex; gap: 10px; align-items: center; }
.radio { display: flex; gap: 6px; align-items: center; cursor: pointer; }
.err { color: #b00020; font-size: 0.9rem; margin-top: 4px; }
.hint { color: #666; font-size: 0.9rem; margin-top: 4px; }

.modal-foot {
  display: flex; justify-content: flex-end; gap: 8px;
  padding: 12px 16px; border-top: 1px solid #eee;
}
.btn.secondary {
  background: #fff; border: 1px solid #ddd; color: #333;
}
.btn.primary {
  background: #2e7d32; border: 1px solid #2e7d32; color: #fff; font-weight: 600;
}
.btn.primary:hover { filter: brightness(0.95); }
</style>
