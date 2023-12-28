/**
 * main.js
 *
 * Bootstraps Vuetify and other plugins then mounts the App`
 */

// Plugins
import { registerPlugins } from '@/plugins';
import { useAppStore } from '@/store/app';
import { SensorType, BooleanParameter, FloatParameter, Sensor } from '@/sensor';

// Components
import App from './App.vue';

// Composables
import { createApp } from 'vue';
import { Solver } from './solver';

const app = createApp(App);

registerPlugins(app);

app.mount('#app');

const store = useAppStore();

store.sensor_types.set("1", new SensorType("1", "Environment", "Environment sensor", new Map([["temperature", new FloatParameter("temperature", -50, 50)], ["humidity", new FloatParameter("humidity", 0, 100)], ["pressure", new FloatParameter("pressure", 0, 100)], ["connected", new BooleanParameter("connected")]])));

store.sensors.set("2", new Sensor("2", "Living room", store.sensor_types.get("1"), "Living room sensor", [0, 0]));

store.sensors.get("2").setValues([
    { temperature: 20, humidity: 50, pressure: 80, connected: true },
    { temperature: 21, humidity: 51, pressure: 81, connected: true },
    { temperature: 22, humidity: 52, pressure: 82, connected: false },
    { temperature: 23, humidity: 53, pressure: 83, connected: true }
], [
    new Date() - 1000 * 60 * 4,
    new Date() - 1000 * 60 * 3,
    new Date() - 1000 * 60 * 2,
    new Date() - 1000 * 60 * 1
]
);

setInterval(() => {
    store.sensors.get("2").addValue({
        temperature: Math.floor(Math.random() * 10) + 20,
        humidity: Math.floor(Math.random() * 10) + 50,
        pressure: Math.floor(Math.random() * 10) + 80,
        connected: Math.random() > 0.5
    });
}, 1000);

store.solvers.set("3", new Solver("3", "Solver", "idle"));

let i = 0;
setInterval(() => {
    store.solvers.get("3").flaw_created({
        id: i++,
        phi: "phi",
        causes: [],
        state: "active",
        cost: { num: 1, den: 2 },
        data: "data"
    });
    store.solvers.get("3").resolver_created({
        id: i++,
        rho: "rho",
        preconditions: [],
        effect: i - 2,
        state: "active",
        intrinsic_cost: { num: 1, den: 2 },
        data: "data"
    });
}, 1000);
