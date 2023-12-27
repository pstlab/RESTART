<template>
  <v-window-item :value='solver.id' class='fill-height' eager>
    <v-card :title='solver.name' :subtitle='solver.description'>
      <v-tabs v-if='show_tabs' v-model='tab' color='deep-purple-accent-4'>
        <v-tab value='timelines'><v-icon>mdi-chart-timeline</v-icon>Timelines</v-tab>
        <v-tab value='graph'><v-icon>mdi-graph-outline</v-icon>Graph</v-tab>
      </v-tabs>
      <v-window :id='get_solver_id(solver)' v-model='tab' class='fill-height' show-arrows>
        <v-window-item :id='get_timelines_id(solver)' value='timelines'
          :style='{ height: solver.timelines.size * 200 + "px" }' eager />
        <v-window-item :id='get_graph_id(solver)' value='graph' class='fill-height' eager />
      </v-window>
    </v-card>
  </v-window-item>
</template>

<script setup>
import { Solver } from '@/solver';
import { onMounted, onUnmounted } from 'vue';
import Plotly from 'plotly.js-dist-min';

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
const get_timelines_id = (solver) => 'slv-' + solver.id + '-timelines';
const get_graph_id = (solver) => 'slv-' + solver.id + '-graph';

const vals_xs = [];
const vals_ys = new Map();
const y_axes = new Map();
const traces = new Map();
const layout = { xaxis: { title: 'Time', type: 'date' }, showlegend: false };
const colors = new Map();

onMounted(() => {
  Plotly.newPlot(props.solver.id, [], layout);
});
</script>