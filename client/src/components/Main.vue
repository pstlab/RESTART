<template>
  <v-app-bar>
    <v-app-bar-nav-icon @click="left_drawer = !left_drawer" />
    <v-app-bar-title>Application</v-app-bar-title>
    <v-app-bar-nav-icon @click="right_drawer = !right_drawer" />
  </v-app-bar>

  <v-navigation-drawer v-model="left_drawer" location="left">
    <v-list dense v-model:selected="window_model">
      <v-list-subheader v-if="store.sensors.size > 0" inset>Sensori</v-list-subheader>
      <SensorListItem v-for="[id, sensor] in store.sensors" :key="id" :sensor="sensor" />
      <v-list-subheader v-if="store.solvers.size > 0" inset>Rarionatori</v-list-subheader>
      <SolverListItem v-for="[id, solver] in store.solvers" :key="id" :solver="solver" />
    </v-list>
  </v-navigation-drawer>
  <v-navigation-drawer v-model="right_drawer" location="right">
  </v-navigation-drawer>

  <v-main class="fill-height">
    <v-window v-model="window_model" class="fill-height">
      <Sensor v-for="[id, sensor] in store.sensors" :key="id" :sensor="sensor" />
      <Solver v-for="[id, solver] in store.solvers" :key="id" :solver="solver" />
    </v-window>
  </v-main>
</template>

<script setup>
import { ref } from 'vue';
import { useAppStore } from '@/store/app';
import SensorListItem from '@/components/SensorListItem.vue';
import Sensor from '@/components/Sensor.vue';
import SolverListItem from './SolverListItem.vue';
import Solver from './Solver.vue';

const left_drawer = ref(false);
const right_drawer = ref(false);
const window_model = ref([]);

const store = useAppStore();
</script>