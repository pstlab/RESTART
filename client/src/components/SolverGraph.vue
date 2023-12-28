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

onMounted(() => {
  const cy = cytoscape({
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

  cy.add({
    group: 'nodes',
    data: { id: 'n0', type: 'flaw' }
  });
  cy.add({
    group: 'nodes',
    data: { id: 'n1', type: 'resolver' }
  });
  cy.add({
    group: 'edges',
    data: { id: 'e0', source: 'n1', target: 'n0' }
  });

  const layout = cy.layout({
    name: 'klay',
    animate: true
  });
  layout.run();
});
</script>