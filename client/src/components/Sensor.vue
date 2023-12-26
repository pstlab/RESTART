<template>
  <v-window-item :value="sensor.id" eager>
    <v-card :title="sensor.name" :subtitle="sensor.description">
      <v-card-text :id="sensor.id" />
    </v-card>
  </v-window-item>
</template>

<script setup>
import { Sensor } from '@/sensor';
import { onMounted } from 'vue';
import Plotly from 'plotly.js-dist-min';

const props = defineProps({
  sensor: {
    type: Sensor,
    required: true
  }
});

onMounted(() => {
  Plotly.newPlot(props.sensor.id, [{
    x: [],
    y: [],
    type: 'scatter'
  }], {
    margin: { t: 0 }
  });
});
</script>