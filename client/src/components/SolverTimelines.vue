<template>
  <v-window-item :id='get_timelines_id(solver)' value='timelines' :style='{ height: solver.timelines.size * 200 + "px" }'
    eager />
</template>

<script setup>
import { Solver } from '@/solver';
import { onMounted, onUnmounted } from 'vue';
import Plotly from 'plotly.js-dist-min';

const props = defineProps({
  solver: {
    type: Solver,
    required: true
  }
});

const get_timelines_id = (solver) => 'slv-' + solver.id + '-timelines';

const vals_xs = [];
const vals_ys = new Map();
const y_axes = new Map();
const traces = new Map();
const layout = { xaxis: { title: 'Time', type: 'date' }, showlegend: false };
const colors = new Map();

onMounted(() => {
  Plotly.newPlot(get_timelines_id(props.solver), [], layout);
});
</script>