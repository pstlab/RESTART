<template>
  <v-list-item :value="sensor.id" :title="sensor.name" :subtitle="sensor.description" prepend-icon="mdi-thermometer"
    three-line />
</template>

<script setup>
import { Sensor } from '@/sensor';

defineProps({
  sensor: {
    type: Sensor,
    required: true
  }
});
</script>