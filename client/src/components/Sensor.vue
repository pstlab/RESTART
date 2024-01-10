<template>
  <v-window-item class="fill-height" :value="sensor.id" eager @group:selected="lazy_load">
    <v-card :title="sensor.name" :subtitle="sensor.description">
      <SensorChart :sensor="sensor" />
      <SensorPublisher :sensor="sensor" />
    </v-card>
  </v-window-item>
</template>

<script setup>
import { Sensor } from '@/sensor';
import SensorChart from './SensorChart.vue';
import SensorPublisher from './SensorPublisher.vue';

const props = defineProps({
  sensor: {
    type: Sensor,
    required: true
  }
});

let loaded = false;

function set_values(from, to = Date.now()) {
  fetch('http://' + location.host + '/sensor/' + props.sensor.id + '?' + new URLSearchParams({ from: from, to: to }), {
    method: 'GET',
    headers: { 'Content-Type': 'application/json' }
  }).then(res => {
    if (res.ok)
      res.json().then(data => {
        const values = [];
        const timestamps = [];
        data.forEach((value) => {
          values.push(value.value);
          timestamps.push(value.timestamp);
        });
        props.sensor.set_values(values, timestamps);
      });
    else
      res.json().then(data => alert(data.message));
  });
}

function lazy_load() {
  if (!loaded) {
    set_values(Date.now() - 1000 * 60 * 60 * 24 * 14);
    loaded = true;
  }
}
</script>