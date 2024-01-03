<template>
  <v-window-item :id='get_graph_id(solver)' value='graph' :style='{ height: 800 + "px" }' eager />
</template>

<script setup>
import { Solver } from '@/solver';
import { onMounted, onUnmounted } from 'vue';
import cytoscape from 'cytoscape';
import dagre from 'cytoscape-dagre';
import popper from 'cytoscape-popper';
import chroma from 'chroma-js';
import tippy from 'tippy.js';
import 'tippy.js/themes/light-border.css';

const scale = chroma.scale(['#0f0', '#f00']).mode('lrgb').domain([0, 15]);

const props = defineProps({
  solver: {
    type: Solver,
    required: true
  }
});

const get_graph_id = (solver) => 'slv-' + solver.id + '-graph';

cytoscape.use(dagre);
cytoscape.use(popper);

let cy;
let layout = {
  name: 'dagre',
  rankDir: 'LR',
  fit: false,
  nodeDimensionsIncludeLabels: true
};

tippy.setDefaultProps({
  arrow: false,
  trigger: 'manual',
  theme: 'light-border',
  placement: 'bottom'
});

const node_listeners = new Map();
const new_node_listener = (node) => {
  const n = cy.add({ group: 'nodes', data: { id: node.id, type: 'phi' in node ? 'flaw' : 'resolver', label: 'phi' in node ? props.solver.flaw_label(node) : props.solver.resolver_label(node), state: node.state, cost: node.cost, color: node.cost < Number.POSITIVE_INFINITY ? scale(node.cost).hex() : '#ccc', stroke: stroke_style(node) } });
  n.tippy = tippy(document.createElement('div'), { getReferenceClientRect: n.popperRef().getBoundingClientRect, content: 'phi' in node ? props.solver.flaw_tooltip(node) : props.solver.resolver_tooltip(node), });
  n.on('mouseover', () => n.tippy.show());
  n.on('mouseout', () => n.tippy.hide());
  cy.layout(layout).run();
  const node_listener = (node) => { cy.$id(node.id).data({ label: 'phi' in node ? props.solver.flaw_label(node) : props.solver.resolver_label(node), state: node.state, cost: node.cost, color: node.cost < Number.POSITIVE_INFINITY ? scale(node.cost).hex() : '#ccc', stroke: stroke_style(node) }); };
  props.solver.add_node_listener(node, node_listener);
  node_listeners.set(node, node_listener);
};

const edge_listeners = new Map();
const new_edge_listener = (edge) => {
  cy.add({ group: 'edges', data: { id: edge.from + '-' + edge.to, source: edge.from, target: edge.to, state: edge.state, stroke: stroke_style(edge) } });
  cy.layout(layout).run();
  const edge_listener = (edge) => { cy.$id(edge.from + '-' + edge.to).data({ state: edge.state, stroke: stroke_style(edge) }); };
  props.solver.add_edge_listener(edge, edge_listener);
  edge_listeners.set(edge, edge_listener);
};

onMounted(() => {
  cy = cytoscape({
    container: document.getElementById(get_graph_id(props.solver)),
    style: [
      {
        selector: 'node[type="flaw"]',
        style: {
          'shape': 'round-rectangle',
          'background-color': 'data(color)',
          'label': 'data(label)',
          'border-width': '1px',
          'border-style': 'data(stroke)',
          'border-color': '#666'
        }
      },
      {
        selector: 'node[type="resolver"]',
        style: {
          'shape': 'ellipse',
          'background-color': 'data(color)',
          'label': 'data(label)',
          'border-width': '1px',
          'border-style': 'data(stroke)',
          'border-color': '#666'
        }
      },
      {
        selector: 'edge',
        style: {
          'curve-style': 'bezier',
          'line-color': '#666',
          'target-arrow-color': '#666',
          'target-arrow-shape': 'triangle',
          'width': '1px',
          'line-style': 'data(stroke)'
        }
      }
    ]
  });

  props.solver.add_new_node_listener(new_node_listener);
  props.solver.add_new_edge_listener(new_edge_listener);

  cy.layout(layout).run();
});

onUnmounted(() => {
  props.solver.remove_new_node_listener(new_node_listener);
  props.solver.remove_new_edge_listener(new_edge_listener);
  for (const [node, node_listener] of node_listeners)
    props.solver.remove_node_listener(node, node_listener);
  for (const [edge, edge_listener] of edge_listeners)
    props.solver.remove_edge_listener(edge, edge_listener);
});
</script>

<script>
function stroke_style(node) {
  switch (node.state) {
    case 0: // False
      return 'dotted';
    case 1: // True
      return 'solid';
    case 2: // Undefined
      return 'dashed';
  }
}
</script>