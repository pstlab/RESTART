<template>
  <v-window-item class="fill-height" :value="sensor.id" eager>
    <v-card :title="sensor.name" :subtitle="sensor.description">
      <v-card-text class="fill-height" :id="sensor.id" />
    </v-card>
  </v-window-item>
</template>

<script setup>
import { Sensor, BooleanParameter, IntegerParameter, FloatParameter, StringParameter, SymbolParameter } from '@/sensor';
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
  let layout = { title: props.sensor.name, xaxis: { title: 'Time', type: 'date' }, barmode: 'stack', showlegend: false };
  let i = 1;
  let start_domain = 0;
  let domain_size = 1 / props.sensor.type.parameters.size;
  for (const [par_name, par] of props.sensor.type.parameters) {
    if (par instanceof FloatParameter || par instanceof IntegerParameter) {
      if (i == 1) {
        traces.push({ x: vals_xs, y: vals_ys.get(par_name), name: par_name, type: 'scatter' });
        layout['yaxis'] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, range: [par.min, par.max] };
      }
      else {
        traces.push({ x: vals_xs, y: vals_ys.get(par_name), name: par_name, type: 'scatter', yaxis: 'y' + i });
        layout['yaxis' + i] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, range: [par.min, par.max] };
      }
    }
    else if (par instanceof BooleanParameter || par instanceof StringParameter || par instanceof SymbolParameter) {
      for (let j = 0; j < vals_ys.get(par_name).length; j++) {
        if (j > 0)
          traces[traces.length - 1].x[1] = vals_xs[j];
        if (j == 0 || String(vals_ys.get(par_name)[j]) != traces[traces.length - 1].name)
          traces.push({ x: [vals_xs[j], vals_xs[j] + 1], y: [1, 1], name: String(vals_ys.get(par_name)[j]), type: 'scatter', mode: 'lines', line: { width: 10 }, yaxis: 'y' + i });
      }

      if (i == 1)
        layout['yaxis'] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, showticklabels: false, showgrid: false };
      else
        layout['yaxis' + i] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, showticklabels: false, showgrid: false };
    }
    start_domain += domain_size;
    i++;
  }

  Plotly.newPlot(props.sensor.id, traces, layout);
});
</script>