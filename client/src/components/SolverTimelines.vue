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

const y_axes = new Map();
const traces = new Map();
const layout = {
  xaxis: { title: 'Time' },
  showlegend: false,
  shapes: [
    {
      type: 'line',
      x0: props.solver.origin,
      y0: 0,
      x1: props.solver.origin,
      y1: props.solver.timelines.size,
      line: {
        color: 'rgb(55, 128, 191)',
        width: 3,
        dash: 'dot'
      }
    }
  ]
};
const colors = new Map();

const timelines_listener = (timelines) => {
  recompute_timelines();
  Plotly.react(get_timelines_id(props.solver), Array.from(traces.values()).flat(), layout);
};

const time_listener = (timestamp) => {
  layout.shapes[0].x0 = timestamp;
  layout.shapes[0].x1 = timestamp;
  layout.datarevision = timestamp;
  Plotly.relayout(get_timelines_id(props.solver), layout);
};

onMounted(() => {
  props.solver.add_timelines_listener(timelines_listener);
  props.solver.add_time_listener(time_listener);

  recompute_timelines();
  Plotly.newPlot(get_timelines_id(props.solver), Array.from(traces.values()).flat(), layout);
});

onUnmounted(() => {
  props.solver.remove_timelines_listener(timelines_listener);
  props.solver.remove_time_listener(time_listener);
});

function recompute_timelines() {
  let i = 1;
  let start_domain = 0;
  y_axes.clear();
  traces.clear();
  let domain_size = 1 / props.solver.timelines.size;
  const domain_separator = 0.05 * domain_size;
  for (const [id, tl] of props.solver.timelines) {
    if (i == 1)
      y_axes.set(id, 'y');
    else
      y_axes.set(id, 'y' + i);
    traces.set(id, []);
    switch (tl.type) {
      case 'Agent':
        tl.values = tl.values.map(atm_id => props.solver.atoms.get(atm_id));
        const ends = [0];
        for (const val of tl.values) {
          if (val.exprs.has('at')) {
            val.from = val.exprs.get('at').value.num / val.exprs.get('at').value.den;
            val.to = val.from + 1;
          } else {
            val.from = val.exprs.get('start').value.num / val.exprs.get('start').value.den;
            val.to = val.exprs.get('end').value.num / val.exprs.get('end').value.den;
          }
          val.y = values_y(val.from, val.from === val.to ? val.from + 0.1 : val.to, ends);
          val.text = props.solver.ag_value_title(val);
          traces.get(id).push({ x: [val.from, val.to], y: [val.y, val.y], name: val.text, type: 'scatter', opacity: 0.7, mode: 'lines', line: { width: 30 }, yaxis: y_axes.get(id) });
        }
        if (i == 1)
          layout['yaxis'] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, showticklabels: false, showgrid: false };
        else
          layout['yaxis' + i] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, showticklabels: false, showgrid: false };
        break;
      case 'StateVariable':
        for (const val of tl.values) {
          val.from = val.from.num / val.from.den;
          val.to = val.to.num / val.to.den;
          val.text = props.solver.sv_value_title(val);
          val.atoms = val.atoms.map(atm_id => props.solver.atoms.get(atm_id));
          traces.get(id).push({ x: [val.from, val.to], y: [i, i], name: val.text, type: 'scatter', opacity: 0.7, mode: 'lines', line: { width: 30 }, yaxis: y_axes.get(id) });
        }
        if (i == 1)
          layout['yaxis'] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, showticklabels: false, showgrid: false };
        else
          layout['yaxis' + i] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, showticklabels: false, showgrid: false };
        break;
      case 'ReusableResource': {
        tl.capacity = tl.capacity.num / tl.capacity.den;
        const vals_xs = [props.solver.origin];
        const vals_ys = [0];
        for (const val of tl.values) {
          val.from = val.from.num / val.from.den;
          val.to = val.to.num / val.to.den;
          val.usage = val.usage.num / val.usage.den;
          val.atoms = val.atoms.map(atm_id => props.solver.atoms.get(atm_id));
          vals_xs.push(val.from);
          vals_ys.push(val.usage);
          vals_xs.push(val.to);
          vals_ys.push(val.usage);
        }
        vals_xs.push(props.solver.horizon);
        vals_ys.push(0);
        traces.get(id).push({ x: vals_xs, y: vals_ys, name: tl.name, type: 'scatter', opacity: 0.7, mode: 'lines', fill: 'tozeroy', yaxis: y_axes.get(id) });
        traces.get(id).push({ x: [props.solver.origin, props.solver.horizon], y: [tl.capacity, tl.capacity], name: 'Capacity', type: 'scatter', opacity: 0.7, mode: 'lines', yaxis: y_axes.get(id) });
        if (i == 1)
          layout['yaxis'] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, range: [0, tl.capacity] };
        else
          layout['yaxis' + i] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, range: [0, tl.capacity] };
        break;
      }
      case 'ConsumableResource': {
        tl.initial_amount = tl.initial_amount.num / tl.initial_amount.den;
        tl.capacity = tl.capacity.num / tl.capacity.den;
        const vals_xs = [props.solver.origin];
        const vals_ys = [tl.initial_amount];
        for (const val of tl.values) {
          val.from = val.from.num / val.from.den;
          val.to = val.to.num / val.to.den;
          val.start = val.start.num / val.start.den;
          val.end = val.end.num / val.end.den;
          val.atoms = val.atoms.map(atm_id => this.atoms.get(atm_id));
          vals_xs.push(val.from);
          vals_ys.push(val.start);
          vals_xs.push(val.to);
          vals_ys.push(val.end);
        }
        vals_xs.push(props.solver.horizon);
        if (tl.values.length > 0)
          vals_ys.push(tl.values[tl.values.length - 1].end);
        else
          vals_ys.push(tl.initial_amount);
        traces.get(id).push({ x: vals_xs, y: vals_ys, name: tl.name, type: 'scatter', opacity: 0.7, mode: 'lines', fill: 'tozeroy', yaxis: y_axes.get(id) });
        traces.get(id).push({ x: [props.solver.origin, props.solver.horizon], y: [tl.capacity, tl.capacity], name: 'Capacity', type: 'scatter', opacity: 0.7, mode: 'lines', yaxis: y_axes.get(id) });
        if (i == 1)
          layout['yaxis'] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, range: [0, tl.capacity] };
        else
          layout['yaxis' + i] = { title: tl.name, domain: [start_domain + domain_separator, start_domain + domain_size - domain_separator], zeroline: false, range: [0, tl.capacity] };
        break;
      }
    }

    start_domain += domain_size;
    i++;
  }
}
</script>

<script>
function values_y(start, end, ends) {
  for (let i = 0; i < ends.length; i++)
    if (ends[i] <= start) {
      ends[i] = end;
      return i;
    }
  ends.push(end);
  return ends.length - 1;
}
</script>
