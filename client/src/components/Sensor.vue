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
  const y_axes = new Map();
  for (const [par_name, par] of props.sensor.type.parameters)
    vals_ys.set(par_name, []);

  for (let i = 0; i < props.sensor.values.length; i++) {
    vals_xs.push(props.sensor.valueTimestamps[i]);
    for (const [par_name, par] of props.sensor.type.parameters)
      if (props.sensor.values[i].hasOwnProperty(par_name))
        vals_ys.get(par_name).push(props.sensor.values[i][par_name]);
      else if (vals_ys.get(par_name).length > 0)
        vals_ys.get(par_name).push(vals_ys.get(par_name)[vals_ys.get(par_name).length - 1]);
      else
        vals_ys.get(par_name).push(par.default_value);
  }

  const traces = new Map();
  const colors = new Map();
  let layout = { xaxis: { title: 'Time', type: 'date' }, showlegend: false };
  let i = 1;
  let start_domain = 0;
  let domain_size = 1 / props.sensor.type.parameters.size;
  for (const [par_name, par] of props.sensor.type.parameters) {
    if (par instanceof FloatParameter || par instanceof IntegerParameter) {
      if (i == 1) {
        y_axes.set(par_name, 'y');
        traces.set(par_name, [{ x: vals_xs, y: vals_ys.get(par_name), name: par_name, type: 'scatter' }]);
        layout['yaxis'] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, range: [par.min, par.max] };
      }
      else {
        y_axes.set(par_name, 'y' + i);
        traces.set(par_name, [{ x: vals_xs, y: vals_ys.get(par_name), name: par_name, type: 'scatter', yaxis: 'y' + i }]);
        layout['yaxis' + i] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, range: [par.min, par.max] };
      }
    }
    else if (par instanceof BooleanParameter || par instanceof StringParameter || par instanceof SymbolParameter) {
      traces.set(par_name, []);
      let c_colors = new Map();
      if (par instanceof BooleanParameter) {
        c_colors.set('true', '#00ff00');
        c_colors.set('false', '#ff0000');
        colors.set(par_name, c_colors);
      } else if (par instanceof SymbolParameter) {
        const color_scale = chroma.scale(['#ff0000', '#00ff00']).mode('lch').colors(par.symbols.length);
        for (let j = 0; j < par.symbols.length; j++)
          c_colors.set(par.symbols[j], color_scale[j]);
        colors.set(par_name, c_colors);
      }

      if (i == 1) {
        y_axes.set(par_name, 'y');
        layout['yaxis'] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, showticklabels: false, showgrid: false };
      }
      else {
        y_axes.set(par_name, 'y' + i);
        layout['yaxis' + i] = { title: par_name, domain: [start_domain, start_domain + domain_size], zeroline: false, showticklabels: false, showgrid: false };
      }

      for (let j = 0; j < vals_ys.get(par_name).length; j++) {
        if (j > 0)
          traces.get(par_name)[traces.get(par_name).length - 1].x[1] = vals_xs[j];
        if (j == 0 || String(vals_ys.get(par_name)[j]) != traces.get(par_name)[traces.get(par_name).length - 1].name) {
          let trace = { x: [vals_xs[j], vals_xs[j]], y: [1, 1], name: String(vals_ys.get(par_name)[j]), type: 'scatter', opacity: 0.7, mode: 'lines', line: { width: 30 }, yaxis: 'y' + i };
          if (par instanceof BooleanParameter || par instanceof SymbolParameter)
            trace.line.color = c_colors.get(String(vals_ys.get(par_name)[j]));
          traces.get(par_name).push(trace);
        }
      }
    }
    start_domain += domain_size;
    i++;
  }

  props.sensor.addValueListener((value, timestamp) => {
    vals_xs.push(timestamp);
    for (const [par_name, par] of props.sensor.type.parameters) {
      let c_value;
      if (value.hasOwnProperty(par_name))
        c_value = value[par_name];
      else if (vals_ys.get(par_name).length > 0)
        c_value = vals_ys.get(par_name)[vals_ys.get(par_name).length - 1];
      else
        c_value = par.default_value;
      if (par instanceof FloatParameter || par instanceof IntegerParameter)
        traces.get(par_name)[0].y.push(c_value);
      else if (par instanceof BooleanParameter || par instanceof StringParameter || par instanceof SymbolParameter) {
        if (traces.get(par_name).length > 0)
          traces.get(par_name)[traces.get(par_name).length - 1].x[1] = timestamp;
        if (String(c_value) != traces.get(par_name)[traces.get(par_name).length - 1].name) {
          let trace = { x: [timestamp, timestamp], y: [1, 1], name: String(c_value), type: 'scatter', opacity: 0.7, mode: 'lines', line: { width: 30 }, yaxis: y_axes.get(par_name) };
          if (par instanceof BooleanParameter || par instanceof SymbolParameter)
            trace.line.color = colors.get(par_name).get(String(c_value));
          traces.get(par_name).push(trace);
        }
      }
    }
    layout.datarevision = timestamp;
    console.log(Array.from(traces.values()).flat());
    Plotly.react(props.sensor.id, Array.from(traces.values()).flat(), layout);
  });

  Plotly.newPlot(props.sensor.id, Array.from(traces.values()).flat(), layout);
});
</script>