<template>
  <v-window-item :value='solver.id' class='fill-height' eager>
    <v-tabs v-if='show_tabs' v-model='tab' color='deep-purple-accent-4'>
      <v-tab value='timelines'><v-icon>mdi-chart-timeline</v-icon>Timelines</v-tab>
      <v-tab value='graph'><v-icon>mdi-graph-outline</v-icon>Graph</v-tab>
    </v-tabs>
    <v-window :id='get_solver_id(solver)' v-model='tab' class='fill-height' show-arrows>
      <SolverTimelines :solver='solver' />
      <SolverGraph :solver='solver' />
    </v-window>
  </v-window-item>
</template>

<script setup>
import { ref } from 'vue';
import { Solver } from '@/solver';
import SolverTimelines from './SolverTimelines.vue';
import SolverGraph from './SolverGraph.vue';

const props = defineProps({
  solver: {
    type: Solver,
    required: true
  },
  show_tabs: {
    type: Boolean,
    required: false,
    default: true,
  }
});

const tab = ref('timelines');

const get_solver_id = (solver) => 'slv-' + solver.id;
</script>