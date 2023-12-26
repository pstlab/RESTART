/**
 * main.js
 *
 * Bootstraps Vuetify and other plugins then mounts the App`
 */

// Plugins
import { registerPlugins } from '@/plugins';
import { useAppStore } from '@/store/app';
import { SensorType, FloatParameter, Sensor } from '@/sensor';

// Components
import App from './App.vue';

// Composables
import { createApp } from 'vue';

const app = createApp(App);

registerPlugins(app);

app.mount('#app');

const store = useAppStore();

store.sensor_types.set("1", new SensorType("1", "Environment", "Environment sensor", new Map([["temperature", new FloatParameter("temperature", -50, 50)], ["humidity", new FloatParameter("humidity", 0, 100)], ["pressure", new FloatParameter("pressure", 0, 100)]])));

store.sensors.set("1", new Sensor("1", "Living room", store.sensor_types.get("1"), "Living room sensor", [0, 0]));

store.sensors.get("1").setValues([{temperature: 20, humidity: 50, pressure: 1000}, {temperature: 21, humidity: 51, pressure: 1001}, {temperature: 22, humidity: 52, pressure: 1002}], [new Date(2024, 0, 1, 0, 0, 0), new Date(2024, 0, 1, 0, 0, 1), new Date(2024, 0, 1, 0, 0, 2)]);