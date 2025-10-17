<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'

type ApiResp = { ok?: boolean; token?: string; user?: any; error?: string }

const router = useRouter()
const mode = ref<'login' | 'register'>('login')
const loading = ref(false)
const error = ref('')

const email = ref('')
const password = ref('')
const passwordConfirm = ref('')

function switchMode(m: 'login' | 'register') {
  mode.value = m
  error.value = ''
}

function saveToken(token: string) {
  try { localStorage.setItem('token', token) } catch {}
}

function reset() {
  email.value = ''
  password.value = ''
  passwordConfirm.value = ''
  error.value = ''
}

async function submit() {
  error.value = ''
  if (!email.value || !password.value) { error.value = 'Email et mot de passe requis'; return }
  if (mode.value === 'register') {
    if (password.value.length < 6) { error.value = 'Mot de passe min 6 caractères'; return }
    if (password.value !== passwordConfirm.value) { error.value = 'Les mots de passe ne correspondent pas'; return }
  }

  loading.value = true
  try {
    const url = mode.value === 'login' ? '/auth/login' : '/auth/register'
    const body: any = { email: email.value, password: password.value }

    const res = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json', Accept: 'application/json' },
      body: JSON.stringify(body)
    })

    const text = await res.text()
    let json: ApiResp
    try { json = text ? JSON.parse(text) : {} as ApiResp } catch (e) { json = { error: 'Réponse invalide du serveur' } }

    if (!res.ok) {
      error.value = json.error || `Erreur ${res.status}`
      return
    }

    if (json.token) {
      saveToken(json.token)
      router.push({ name: 'User' })
    } else {
      error.value = 'Réponse inattendue du serveur'
    }
  } catch (e: any) {
    console.error('Auth submit failed', e)
    error.value = e?.message || 'Erreur réseau'
  } finally {
    loading.value = false
  }
}
</script>

<template>
  <v-app-bar elevation="0" color="#A7C6DB" class="header-bar-auth">
    <v-btn to="/" variant="text" class="brand" exact>samsul</v-btn>
  </v-app-bar>
  <div class="auth-page">
    <div class="card auth-card">
      <div class="card-header">
        <h2>{{ mode === 'login' ? 'Connexion' : 'Inscription' }}</h2>
        <div class="muted">Utilisez votre compte pour accéder à vos réservations</div>
      </div>
      <div class="mode-toggle">
        <v-btn variant="text" :class="{ active: mode === 'login' }" @click="switchMode('login')">Connexion</v-btn>
        <v-btn variant="text" :class="{ active: mode === 'register' }" @click="switchMode('register')">Inscription</v-btn>
      </div>
      <div class="form">
        <v-text-field label="Email" v-model="email" type="email" :disabled="loading" hide-details dense/>
        <v-text-field label="Mot de passe" v-model="password" type="password" :disabled="loading" hide-details dense/>
        <v-text-field v-if="mode === 'register'" label="Confirmer le mot de passe" v-model="passwordConfirm" type="password" :disabled="loading" hide-details dense/>
        <div class="error" v-if="error">{{ error }}</div>
        <div class="actions">
          <v-btn color="#111" elevation="2" @click="submit" :loading="loading">{{ mode === 'login' ? 'Se connecter' : "S'inscrire" }}</v-btn>
          <v-btn variant="text" @click="reset">Annuler</v-btn>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.header-bar-auth {
  border-bottom: 3px solid #8FAEC4;
}
.brand {
  font-weight: 800;
  font-size: 1.3rem;
  color: #111111 !important;
  text-transform: none;
}
.auth-page {
  display: flex;
  justify-content: center;
  align-items: flex-start;
  min-height: calc(100vh - 0px);
  padding: 40px 16px;
  background: #f7f3ef;
}

.card.auth-card {
  width: 100%;
  max-width: 420px;
  background: #ffffff;
  border-radius: 12px;
  padding: 28px 24px 24px 24px;
  box-shadow: 0 8px 20px rgba(16, 24, 40, 0.06);
  text-align: left;
  display: flex;
  flex-direction: column;
  gap: 18px;
}

.card-header {
  text-align: center;
  margin-bottom: 8px;
}
.card-header h2 {
  margin: 0 0 8px 0;
  font-weight: 700;
  font-size: 1.5rem;
}
.muted {
  color: #666;
  font-size: 0.98rem;
}

.mode-toggle {
  display: flex;
  justify-content: center;
  gap: 8px;
  margin-bottom: 10px;
}
.mode-toggle .v-btn {
  text-transform: none;
  font-weight: 600;
  font-size: 1rem;
  color: #555 !important;
}
.mode-toggle .active {
  color: #111 !important;
  border-bottom: 2px solid #A7C6DB;
  border-radius: 0;
}

.form {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.error {
  color: #b00020;
  text-align: center;
  padding: 8px 0 0 0;
  font-size: 1rem;
}

.actions {
  display: flex;
  gap: 12px;
  align-items: center;
  margin-top: 10px;
  justify-content: center;
}
</style>
