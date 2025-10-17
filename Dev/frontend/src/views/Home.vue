<template>
  <Header />
  <v-container class="main-container" fluid>
    <v-row justify="center" align="center">
      <v-col cols="12" md="8">
        <h2 class="color--text text-center mb-6 display-1">Matchs en cours</h2>
        <v-row justify="center" align="center">
          <v-col
              v-for="(match, idx) in matchsEnCours"
              :key="idx"
              cols="12"
              md="6"
          >
            <v-card class="mb-6 card-style" outlined>
              <v-card-text>
                <div class="d-flex flex-column align-center">
                  <span class="text-style">{{ match.heure }}</span>
                  <span>
                    <strong class="text-style">{{ match.equipeA[0] }} et {{ match.equipeA[1] }}</strong>
                  </span>
                  <span class="mx-2 font-weight-black vs-style">
                    <span class="text-style" :style="{ color: scoreColor(match.scoreA, match.scoreB, true) }">
                      ({{ match.scoreA }})
                    </span>VS
                  <span class="text-style" :style="{ color: scoreColor(match.scoreA, match.scoreB, false) }">
                      ({{ match.scoreB }})
                    </span>
                  </span>
                  <span>
                    <strong class="text-style">{{ match.equipeB[0] }} et {{ match.equipeB[1] }}</strong>
                  </span>
                </div>
              </v-card-text>
            </v-card>
          </v-col>
        </v-row>
      </v-col>
    </v-row>
    <v-row justify="center" align="center">
      <v-col cols="12" md="8">
        <h2 class="color--text text-center mb-6 display-1">Matchs à venir</h2>
        <v-row justify="center" align="center">
          <v-col
              v-for="(match, idx) in matchsAVenir"
              :key="idx"
              cols="12"
              md="6"
          >
            <v-card class="mb-6 card-style" outlined>
              <v-card-text>
                <div class="d-flex flex-column align-center">
                  <span class="text-style">{{ match.heure }}</span>
                  <span>
                    <strong class="text-style">{{ match.equipeA[0] }} et {{ match.equipeA[1] }}</strong>
                  </span>
                  <span class="mx-2 font-weight-black vs-style">VS</span>
                  <span>
                    <strong class="text-style">{{ match.equipeB[0] }} et {{ match.equipeB[1] }}</strong>
                  </span>
                </div>
              </v-card-text>
            </v-card>
          </v-col>
        </v-row>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup lang="ts">
import Header from '@/components/Header.vue'
import { ref } from 'vue'

interface Match {
  equipeA: [string, string]
  equipeB: [string, string]
  scoreA: number
  scoreB: number
  enCours: boolean
  heure: string
}

const matchs = ref<Match[]>([
  {
    equipeA: ['Alice', 'Bob'],
    equipeB: ['Charlie', 'David'],
    scoreA: 5,
    scoreB: 3,
    enCours: true,
    heure: '14:00'
  },
  {
    equipeA: ['Eve', 'Frank'],
    equipeB: ['Grace', 'Heidi'],
    scoreA: 0,
    scoreB: 0,
    enCours: false,
    heure: '15:00'
  },
  {
    equipeA: ['Alice', 'Heidi'],
    equipeB: ['Frank', 'David'],
    scoreA: 0,
    scoreB: 0,
    enCours: false,
    heure: '16:30'
  }
])

const matchsEnCours = matchs.value.filter(m => m.enCours)
const joueursEnCours = matchsEnCours.flatMap(m => [...m.equipeA, ...m.equipeB])

const matchsAVenir = matchs.value.filter(m => {
  if (m.enCours) return false
  const joueursMatch = [...m.equipeA, ...m.equipeB]
  return joueursMatch.every(joueur => !joueursEnCours.includes(joueur))
})

function scoreColor(scoreA: number, scoreB: number, isA: boolean) {
  if (scoreA === scoreB) return 'black'
  if ((isA && scoreA > scoreB) || (!isA && scoreB > scoreA)) return '#32a852'
  return 'red'
}
</script>

<style scoped>
.main-container {
  min-height: 100vh;
  padding-top: 40px;
}
.color--text {
  color: #A7C6DB !important;
}
.card-style {
  box-shadow: 0 4px 24px rgba(50, 168, 82, 0.08);
  border-radius: 16px;
  padding: 24px 0;
}
.vs-style {
  font-size: 1.5rem;
  margin: 12px 0;
}
.display-1 {
  font-size: 2.5rem;
  font-weight: bold;
}
.text-style{
  font-size: 1.5rem;
}
</style>
