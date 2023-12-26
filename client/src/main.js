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

store.sensor_types.set("1", new SensorType("1", "Temperature", "Temperature sensor", new Map([["temperature", new FloatParameter("temperature", -50, 50)]])));
store.sensor_types.set("2", new SensorType("2", "Humidity", "Humidity sensor", new Map([["humidity", new FloatParameter("humidity", 0, 100)]])));

store.sensors.set("3", new Sensor("3", "Temperature sensor", store.sensor_types.get("1"), "Temperature sensor in the living room", [0, 0]));