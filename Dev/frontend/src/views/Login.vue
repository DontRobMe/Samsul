<template>
  <v-card class="login-card">
    <h1 class="login-title">Connexion / Création de compte</h1>
    <div class="login-sections">
      <div class="login-section">
        <h2>Connexion</h2>
        <v-form @submit.prevent="login">
          <v-text-field v-model="loginEmail" label="Email" required></v-text-field>
          <v-text-field v-model="loginPassword" label="Mot de passe" type="password" required></v-text-field>
          <v-btn color="primary" type="submit">Se connecter</v-btn>
        </v-form>
      </div>
      <div class="login-section">
        <h2>Créer un compte</h2>
        <v-form @submit.prevent="register">
          <v-text-field v-model="registerEmail" label="Email" required></v-text-field>
          <v-text-field v-model="registerPassword" label="Mot de passe" type="password" required></v-text-field>
          <v-btn color="primary" type="submit">Créer un compte</v-btn>
        </v-form>
      </div>
    </div>
    <v-alert v-if="message" type="info" dismissible>{{ message }}</v-alert>
  </v-card>
</template>

<script setup>
import { ref } from 'vue';
import { useRouter } from 'vue-router';

const loginEmail = ref('');
const loginPassword = ref('');
const registerEmail = ref('');
const registerPassword = ref('');
const message = ref('');
const router = useRouter();

function login() {
  // Simuler une authentification
  if (loginEmail.value && loginPassword.value) {
    localStorage.setItem('auth', 'true');
    message.value = 'Connexion réussie !';
    setTimeout(() => {
      message.value = '';
      router.push('/');
    }, 2000);
  } else {
    message.value = 'Veuillez remplir tous les champs.';
    setTimeout(() => message.value = '', 2000);
  }
}

function register() {
  // Simuler une inscription
  if (registerEmail.value && registerPassword.value) {
    localStorage.setItem('auth', 'true');
    message.value = 'Compte créé et connecté !';
    setTimeout(() => {
      message.value = '';
      router.push('/');
    }, 2000);
  } else {
    message.value = 'Veuillez remplir tous les champs.';
    setTimeout(() => message.value = '', 2000);
  }
}
</script>

<style scoped>
.login-card {
  max-width: 600px;
  margin: 40px auto;
  padding: 32px 24px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}
.login-title {
  text-align: center;
  color: #A7C6DB;
  margin-bottom: 24px;
}
.login-sections {
  display: flex;
  gap: 32px;
  justify-content: space-between;
}
.login-section {
  flex: 1;
  background: #F7FAFC;
  border-radius: 8px;
  padding: 24px 16px;
  box-shadow: 0 1px 4px rgba(0,0,0,0.05);
}
.login-section h2 {
  text-align: center;
  margin-bottom: 16px;
  color: #A7C6DB;
}
</style>
