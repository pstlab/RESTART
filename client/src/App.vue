<template>
  <v-app>
    <v-app-bar class="bg-grey-darken-3">
      <v-app-bar-title>RESTART: Robot Enhanced Social abilities based on Theory of mind for Acceptance of Robot in
        assistive Treatments</v-app-bar-title>
    </v-app-bar>

    <v-main>
      <v-navigation-drawer expand-on-hover rail location="left">
        <v-list dense v-model:selected="window_model">
          <v-list-item value="chat" title="Chat" prepend-icon="mdi-message-text-outline" />
          <v-list-subheader v-if="solvers.size > 0" inset>Ragionatori</v-list-subheader>
          <SolverListItem v-for="[id, solver] in solvers" :key="id" :solver="solver" />
          <v-list-subheader v-if="sorted_sensors(sensors).size > 0" inset>Sensori</v-list-subheader>
          <SensorListItem v-for="[id, sensor] in sorted_sensors(sensors)" :key="id" :sensor="sensor" />
          <v-divider />
          <v-list-item value="reactive-editor" title="Reactive Editor" prepend-icon="mdi-cog" />
        </v-list>
      </v-navigation-drawer>
      <v-navigation-drawer expand-on-hover rail location="right">
        <v-list dense v-model:selected="window_model">
          <v-list-item v-for="message in messages" :key="message.timestamp" :title="message.text"
            prepend-icon="mdi-message-text-outline" />
        </v-list>
      </v-navigation-drawer>

      <v-window v-model="window_model" class="fill-height">
        <Chat />
        <Sensor v-for="[id, sensor] in sorted_sensors(sensors)" :key="id" :sensor="sensor" />
        <Solver v-for="[id, solver] in solvers" :key="id" :solver="solver" />
        <ReactiveEditor />
      </v-window>
    </v-main>
  </v-app>
</template>

<script setup>
import { ref } from 'vue';
import { useAppStore } from '@/store/app';
import { storeToRefs } from 'pinia';
import SolverListItem from './components/SolverListItem.vue';
import SensorListItem from './components/SensorListItem.vue';
import Sensor from './components/Sensor.vue';
import Solver from './components/Solver.vue';
import Chat from './components/Chat.vue';
import ReactiveEditor from './components/ReactiveEditor.vue';

const window_model = ref(['chat']);

const { sensors, solvers, messages } = storeToRefs(useAppStore());
</script>

<script>
function sorted_sensors(sensors) {
  return new Map([...sensors].filter(([id, sensor]) => !sensor.name.startsWith('part_')).sort((s1, s2) => s1[1].name.localeCompare(s2[1].name)));
}
</script>