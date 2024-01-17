<template>
  <v-window-item class="fill-height" :value="sensor.id" eager @group:selected="lazy_load">
    <v-card :title="sensor.name + ' (' + sensor.type.name + ')'"
      :subtitle="sensor.description + ' (' + sensor.type.description + ')'">
      <v-container>
        <v-row>
          <v-col cols="5">
            <v-menu v-model="from_menu" :close-on-content-click="false" :nudge-right="40" transition="scale-transition">
              <template v-slot:activator="{ props }">
                <v-btn
                  :text="'From: ' + from_date.getDate() + '/' + (from_date.getMonth() + 1) + '/' + from_date.getFullYear()"
                  prepend-icon="mdi-calendar" v-bind="props" />
              </template>
              <v-date-picker v-model="from_date" :max="to_date" />
            </v-menu>
          </v-col>
          <v-col cols="5">
            <v-menu v-model="to_menu" :close-on-content-click="false" :nudge-right="40" transition="scale-transition">
              <template v-slot:activator="{ props }">
                <v-btn :text="'To: ' + to_date.getDate() + '/' + (to_date.getMonth() + 1) + '/' + to_date.getFullYear()"
                  prepend-icon="mdi-calendar" v-bind="props" />
              </template>
              <v-date-picker v-model="to_date" :max="new Date()" :min="from_date" />
            </v-menu>
          </v-col>
          <v-col cols="2">
            <v-btn @click="set_values(from_date.getTime(), to_date.getTime())" color="primary" text>Update</v-btn>
          </v-col>
        </v-row>
      </v-container>
      <SensorChart :sensor="sensor" />
      <SensorPublisher :sensor="sensor" />
    </v-card>
  </v-window-item>
</template>

<script setup>
import { ref } from 'vue';
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

const from_menu = ref(false);
const from_date = ref(new Date(Date.now() - 1000 * 60 * 60 * 24 * 7));
const to_menu = ref(false);
const to_date = ref(new Date());

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