<template>
  <Header />
  <v-container>
    <v-row>
      <v-col cols="12">
        <h2 class="color--text">Matchs en cours</h2>
        <v-row>
          <v-col
              v-for="(match, idx) in matchsEnCours"
              :key="idx"
              cols="12"
              md="6"
          >
            <v-card class="mb-4" outlined>
              <v-card-text>
                <span>
                  <strong>{{ match.equipeA[0] }} et {{ match.equipeA[1] }}</strong>
                  <span :style="{ color: scoreColor(match.scoreA, match.scoreB, true) }">
                    ({{ match.scoreA }})
                  </span>
                  <span class="mx-2 font-weight-black">VS</span>
                  <strong>{{ match.equipeB[0] }} et {{ match.equipeB[1] }}</strong>
                  <span :style="{ color: scoreColor(match.scoreA, match.scoreB, false) }">
                    ({{ match.scoreB }})
                  </span>
                </span>
              </v-card-text>
            </v-card>
          </v-col>
        </v-row>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="12">
        <h2 class="color--text">Matchs à venir</h2>
        <v-row>
          <v-col
              v-for="(match, idx) in matchsAVenir"
              :key="idx"
              cols="12"
              md="6"
          >
            <v-card class="mb-4" outlined>
              <v-card-text>
                <span>
                  <strong>{{ match.equipeA[0] }} et {{ match.equipeA[1] }}</strong>
                  <span class="mx-2 font-weight-black">VS</span>
                  <strong>{{ match.equipeB[0] }} et {{ match.equipeB[1] }}</strong>
                </span>
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
}

const matchs = ref<Match[]>([
  {
    equipeA: ['Alice', 'Bob'],
    equipeB: ['Charlie', 'David'],
    scoreA: 5,
    scoreB: 3,
    enCours: true
  },
  {
    equipeA: ['Eve', 'Frank'],
    equipeB: ['Grace', 'Heidi'],
    scoreA: 0,
    scoreB: 0,
    enCours: false
  },
  {
    equipeA: ['Alice', 'Heidi'],
    equipeB: ['Frank', 'David'],
    scoreA: 0,
    scoreB: 0,
    enCours: false
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
.color--text {
  color: #A7C6DB !important;
}
</style>
