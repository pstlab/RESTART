<template>
  <v-window-item :id='get_graph_id(solver)' value='graph' :style='{ width: 800 + "px", height: 800 + "px" }' eager />
</template>

<script setup>
import { Solver } from '@/solver';
import { onMounted, onUnmounted } from 'vue';
import cytoscape from 'cytoscape';
import klay from 'cytoscape-klay';

const props = defineProps({
  solver: {
    type: Solver,
    required: true
  }
});

const get_graph_id = (solver) => 'slv-' + solver.id + '-graph';

cytoscape.use(klay);
let cy;
let layout;

const node_listeners = new Map();
const new_node_listener = (node) => {
  cy.add({ group: 'nodes', data: { id: node.id, type: 'phi' in node ? 'flaw' : 'resolver', label: node.label, state: node.state, cost: node.cost } });
  layout.run();
  const node_listener = (node) => { cy.$id(node.id).data({ label: node.label, state: node.state, cost: node.cost }); };
  props.solver.add_node_listener(node, node_listener);
  node_listeners.set(node, node_listener);
};

const edge_listeners = new Map();
const new_edge_listener = (edge) => {
  cy.add({ group: 'edges', data: { id: edge.from + '-' + edge.to, source: edge.from, target: edge.to, state: edge.state } });
  layout.run();
  const edge_listener = (edge) => { cy.$id(edge.from + '-' + edge.to).data({ state: edge.state }); };
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
          'background-color': '#666',
          'label': 'data(id)'
        }
      },
      {
        selector: 'node[type="resolver"]',
        style: {
          'shape': 'ellipse',
          'background-color': '#666',
          'label': 'data(id)'
        }
      },
      {
        selector: 'edge',
        style: {
          'curve-style': 'bezier',
          'line-color': '#ccc',
          'target-arrow-color': '#ccc',
          'target-arrow-shape': 'triangle'
        }
      }
    ]
  });

  props.solver.add_new_node_listener(new_node_listener);
  props.solver.add_new_edge_listener(new_edge_listener);

  layout = cy.layout({
    name: 'klay',
    animate: true,
    klay: {
      direction: 'RIGHT',
    },
  });
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