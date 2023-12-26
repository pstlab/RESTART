<template>
  <v-window-item class="fill-height" :value="sensor.id" eager>
    <v-card :title="sensor.name" :subtitle="sensor.description">
      <v-card-text class="fill-height" :id="sensor.id" />
    </v-card>
  </v-window-item>
</template>

<script setup>
import { Sensor, IntegerParameter, FloatParameter } from '@/sensor';
import { onMounted } from 'vue';
import Plotly from 'plotly.js-dist-min';

const props = defineProps({
  sensor: {
    type: Sensor,
    required: true
  }
});

onMounted(() => {
  let vals_xs = [];
  const vals_ys = new Map();
  for (const [par_name, par] of props.sensor.type.parameters)
    vals_ys.set(par_name, []);

  for (let i = 0; i < props.sensor.values.length; i++) {
    vals_xs.push(props.sensor.valueTimestamps[i]);
    for (const [par_name, par] of props.sensor.type.parameters)
      vals_ys.get(par_name).push(props.sensor.values[i][par_name]);
  }

  const traces = [];
  let layout = { title: props.sensor.name, xaxis: { title: 'Time' }, showlegend: false };
  let i = 1;
  let start_domain = 0;
  let domain_size = 1 / props.sensor.type.parameters.size;
  for (const [par_name, par] of props.sensor.type.parameters) {
    if (par instanceof FloatParameter || par instanceof IntegerParameter) {
      if (i == 1) {
        traces.push({ x: vals_xs, y: vals_ys.get(par_name), type: 'scatter' });
        layout['yaxis'] = { title: par_name, domain: [start_domain, start_domain + domain_size] };
      }
      else {
        traces.push({ x: vals_xs, y: vals_ys.get(par_name), type: 'scatter', yaxis: 'y' + i });
        layout['yaxis' + i] = { title: par_name, domain: [start_domain, start_domain + domain_size] };
      }
    }
    start_domain += domain_size;
    i++;
  }

  Plotly.newPlot(props.sensor.id, traces, layout);
});
</script>