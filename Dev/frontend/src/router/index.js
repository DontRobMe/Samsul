import { createRouter, createWebHistory } from 'vue-router'
import Home from '@/views/Home.vue'
import Reservation from '@/views/Reservation.vue'
import Paris from '@/views/Paris.vue'

const routes = [
  { path: '/', name: 'Home', component: Home },
  { path: '/reservation', name: 'Reservation', component: Reservation },
  { path: '/paris', name: 'Paris', component: Paris }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

export default router
