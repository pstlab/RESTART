<template>
  <v-window-item class="fill-height" :value="sensor.id" eager>
    <v-card :title="sensor.name" :subtitle="sensor.description">
      <v-container :id="sensor.id" :style="{ height: sensor.type.parameters.size * 200 + 'px' }" />
    </v-card>
  </v-window-item>
</template>

<script setup>
import { Sensor, BooleanParameter, IntegerParameter, FloatParameter, StringParameter, SymbolParameter } from '@/sensor';
import { onMounted } from 'vue';
import Plotly from 'plotly.js-dist-min';
import chroma from 'chroma-js'

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
  let layout = { xaxis: { title: 'Time', type: 'date' }, showlegend: false };
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
      let colors = new Map();
      if (par instanceof BooleanParameter) {
        colors.set('true', '#00ff00');
        colors.set('false', '#ff0000');
      } else if (par instanceof SymbolParameter) {
        const color_scale = chroma.scale(['#ff0000', '#00ff00']).mode('lch').colors(par.symbols.length);
        for (let j = 0; j < par.symbols.length; j++)
          colors.set(par.symbols[j], color_scale[j]);
      }

      for (let j = 0; j < vals_ys.get(par_name).length; j++) {
        if (j > 0)
          traces[traces.length - 1].x[1] = vals_xs[j];
        if (j == 0 || String(vals_ys.get(par_name)[j]) != traces[traces.length - 1].name) {
          let trace = { x: [vals_xs[j], vals_xs[j] + 1], y: [1, 1], name: String(vals_ys.get(par_name)[j]), type: 'scatter', mode: 'lines', line: { width: 30 }, yaxis: 'y' + i };
          if (par instanceof BooleanParameter || par instanceof SymbolParameter)
            trace.line.color = colors.get(String(vals_ys.get(par_name)[j]));
          traces.push(trace);
        }
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