<template>
  <v-window-item value="reactive-editor" class="fill-height" eager>
    <v-row>
      <v-col cols="2">
        <v-btn @click="add_action">Add action</v-btn>
      </v-col>
    </v-row>
    <v-row>
      <v-col cols="2">
        <v-list dense v-model:selected="c_element">
          <v-list-subheader inset>Actions</v-list-subheader>
          <v-list-item v-for="action in actions" :key="action.id" :value="action.id" :title="action.name">
            <v-list-item-action>
              <v-btn icon @click="remove_action(action)">
                <v-icon>mdi-delete</v-icon>
              </v-btn>
            </v-list-item-action>
          </v-list-item>
          <v-list-subheader inset>Transitions</v-list-subheader>
          <v-list-item v-for="transition in transitions" :key="transition.id" :value="transition.id"
            :title="transition.name">
            <v-list-item-action>
              <v-btn icon @click="remove_transition(transition)">
                <v-icon>mdi-delete</v-icon>
              </v-btn>
            </v-list-item-action>
          </v-list-item>
        </v-list>
      </v-col>
      <v-col cols="8">
        <div id="reactive-editor-graph" style="height: 800px"></div>
      </v-col>
      <v-col cols="2">
        <v-window v-model="c_element">
          <v-window-item v-for="action in actions" :key="action.id" :value="action.id" :title="action.name">
            <v-row>
              <v-col cols="12">
                <v-text-field label="Name" v-model="action.name" @input="update_action(action)" />
              </v-col>
            </v-row>
            <v-row>
              <v-col cols="12">
                <v-btn @click="start_transition(action)">Add transition</v-btn>
              </v-col>
            </v-row>
          </v-window-item>
          <v-window-item v-for="transition in transitions" :key="transition.id" :value="transition.id"
            :title="transition.name">
            <v-row>
              <v-col cols="12">
                <v-text-field label="Name" v-model="transition.name" @input="update_transition(transition)" />
              </v-col>
            </v-row>
          </v-window-item>
        </v-window>
      </v-col>
    </v-row>
  </v-window-item>
</template>

<script setup>
import { ref, onMounted } from 'vue';
import cytoscape from 'cytoscape';
import dagre from 'cytoscape-dagre';
import popper from 'cytoscape-popper';
import edgehandles from 'cytoscape-edgehandles';
import tippy from 'tippy.js';
import 'tippy.js/themes/light-border.css';

cytoscape.use(dagre);
cytoscape.use(popper);
cytoscape.use(edgehandles);

const actions = ref([]);
const transitions = ref([]);
const element_id = ref(0);
const c_element = ref([]);

let cy;
let eh;
let layout = {
  name: 'dagre',
  rankDir: 'LR',
  fit: false,
  nodeDimensionsIncludeLabels: true
};

onMounted(() => {
  cy = cytoscape({
    container: document.getElementById('reactive-editor-graph'),
    style: [
      {
        selector: 'node',
        style: {
          'label': 'data(label)',
          'text-valign': 'center',
          'text-halign': 'center'
        }
      },
      {
        selector: 'edge',
        style: {
          'label': 'data(label)',
          'curve-style': 'bezier',
          'target-arrow-shape': 'triangle'
        }
      }
    ]
  });
  eh = cy.edgehandles();

  cy.on('ehcomplete', (event, sourceNode, targetNode, addedEles) => {
    cy.remove(addedEles);
    add_transition(sourceNode.data(), targetNode.data());
  });

  cy.on('click', 'node', (event) => {
    c_element.value = actions.value.find((action) => action.id === event.target.id());
  });
});

const add_action = () => {
  const action = { id: element_id.value++, name: 'action-' + element_id.value };
  actions.value.push(action);
  cy.add({ group: 'nodes', data: { id: action.id, label: action.name } });
  cy.layout(layout).run();
};

const update_action = (action) => {
  cy.$id(action.id).data('label', action.name);
  cy.layout(layout).run();
};

const remove_action = (action) => {
  actions.value.splice(actions.value.indexOf(action), 1);
  cy.remove(cy.$id(action.id));
  cy.layout(layout).run();
};

const start_transition = (action) => {
  eh.start(cy.$id(action.id));
};

const add_transition = (from, to) => {
  const transition = { id: element_id.value++, name: 'transition-' + element_id.value, from, to };
  transitions.value.push(transition);
  cy.add({ group: 'edges', data: { id: transition.id, source: from.id, target: to.id, label: transition.name } });
  cy.layout(layout).run();
};

const update_transition = (transition) => {
  cy.$id(transition.id).data('label', transition.name);
  cy.layout(layout).run();
};

const remove_transition = (transition) => {
  transitions.value.splice(transitions.value.indexOf(transition), 1);
  cy.remove(cy.$id(transition.id));
  cy.layout(layout).run();
};
</script>