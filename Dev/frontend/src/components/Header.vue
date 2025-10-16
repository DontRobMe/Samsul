<template>
  <v-app-bar elevation="1" color="white">
    <v-btn to="/" variant="text" class="text-h6 font-weight-bold px-0" exact>
      samsul
    </v-btn>

    <v-spacer />
        <v-btn to="/reservation" variant="text">Réservation</v-btn>
        <v-btn to="/paris" variant="text">Paris</v-btn>
    <v-spacer />

    <v-avatar size="36" class="mr-2" color="primary">
      <v-icon icon="mdi-account" />
    </v-avatar>
  </v-app-bar>
</template>