<template>
  <v-app-bar elevation="0" :color="headerColor" class="header-bar">
    <v-btn to="/" variant="text" class="brand" exact>samsul</v-btn>
    <v-btn to="/reservation" variant="text" class="nav-link">Réservation</v-btn>
    <v-btn to="/paris" variant="text" class="nav-link">Paris</v-btn>
    <v-spacer />
    <v-avatar size="38" class="profile">
      <v-btn to="/user">
      <v-icon icon="mdi-account" color="white" />
      </v-btn>
    </v-avatar>
  </v-app-bar>
</template>

<script setup lang="ts">
const headerColor = '#A7C6DB'
</script>

<style scoped>
.header-bar {
  border-bottom: 3px solid #8FAEC4;
}

.brand {
  font-weight: 800;
  font-size: 1.3rem;
  color: #111111 !important;
  text-transform: none;
}

.nav-link {
  text-transform: none;
  font-weight: 600;
  color: #111111 !important;
}

.profile {
  background: #111111;
  cursor: pointer;
  transition: transform 0.12s ease;
  margin-right: 20px;
}

.v-btn::before {
  background-color: transparent;
}

:deep(.v-btn--active) {
  background-color: transparent !important;
  box-shadow: none !important;
}

:deep(.v-btn--active .v-btn__overlay) {
  background-color: transparent !important;
  opacity: 0 !important;
}

:deep(.v-btn::before) {
  background-color: transparent !important;
}


</style>