<template>
  <v-window-item value="map" class='fill-height' eager>
    <v-container id="map_panel" fluid class="fill-height">
    </v-container>
  </v-window-item>
</template>

<script setup>
import 'leaflet/dist/leaflet.css';
import L from 'leaflet';
import 'leaflet.heat/dist/leaflet-heat.js';
import { onMounted } from 'vue';

let map = null;
let heatmap = L.heatLayer([], { radius: 25 });

setInterval(() => {
  const data = [];
  for (let i = 0; i < 1000; i++) {
    data.push([51.5 + Math.random() / 100, -0.09 + Math.random() / 100, Math.random()]);
  }
  heatmap.setLatLngs(data);
}, 1000);

onMounted(() => {
  map = L.map('map_panel').setView([51.505, -0.09], 13);
  L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', { attribution: '&copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors' }).addTo(map);
  heatmap.addTo(map);
});
</script>