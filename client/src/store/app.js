// Utilities
import { defineStore } from 'pinia'

export const useAppStore = defineStore('app', {
  state: () => ({
    sensor_types: new Map(),
    sensors: new Map(),
    solvers: new Map(),
  })
})
