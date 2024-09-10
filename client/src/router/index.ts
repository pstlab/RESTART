import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '../views/HomeView.vue'
import TypeView from '../views/TypeView.vue'
import ItemView from '../views/ItemView.vue'
import SolverView from '../views/SolverView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView
    },
    {
      path: '/type/:id',
      name: 'type',
      component: TypeView
    },
    {
      path: '/item/:id',
      name: 'item',
      component: ItemView
    },
    {
      path: '/solver/:id',
      name: 'solver',
      component: SolverView
    }
  ]
})

export default router
