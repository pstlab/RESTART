<template>
  <v-window-item class="fill-height" :value="sensor.id" eager>
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
import { onMounted } from 'vue';

const props = defineProps({
  sensor: {
    type: Sensor,
    required: true
  }
});

onMounted(() => {
  var d = new Date();
  d.setMonth(d.getMonth() - 1);
  d.setHours(0, 0, 0, 0);
  fetch('http://' + location.host + '/sensor/' + props.sensor.id + '?' + new URLSearchParams({ from: d.getTime() / 1000 }), {
    method: 'GET',
    headers: { 'Content-Type': 'application/json' }
  }).then(res => {
    if (res.status === 200) {
      res.json().then(data => {
        const values = [];
        const timestamps = [];
        data.forEach((value) => {
          values.push(value.value);
          timestamps.push(value.timestamp);
        });
        props.sensor.set_values(values, timestamps);
      });
    }
  });
});
</script>