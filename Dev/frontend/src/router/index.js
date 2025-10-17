import { createRouter, createWebHistory } from 'vue-router'
import Home from '@/views/Home.vue'
import Reservation from '@/views/Reservation.vue'
import Paris from '@/views/Paris.vue'
import Login from '@/views/Login.vue'
import User from "@/views/User.vue";

const routes = [
  { path: '/', name: 'Home', component: Home },
  { path: '/reservation', name: 'Reservation', component: Reservation },
  { path: '/paris', name: 'Paris', component: Paris },
    { path: '/user', name: 'User', component: User }
  { path: '/paris', name: 'Paris', component: Paris },
  { path: '/login', name: 'Login', component: Login }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

router.beforeEach((to, from, next) => {
  const isAuthenticated = localStorage.getItem('auth') === 'true';
  if (!isAuthenticated && to.name !== 'Login') {
    next({ name: 'Login' });
  } else {
    next();
  }
});

export default router
