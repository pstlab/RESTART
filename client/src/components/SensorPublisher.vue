<template>
  <v-container fluid>
    <v-table dense>
      <thead>
        <tr>
          <th class="text-left" width="80%">Parameter name</th>
          <th class="text-left" width="20%">Value</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="[name, par] in sensor.type.parameters" :key="name">
          <td>{{ name }}</td>
          <td>
            <v-text-field v-if="input_type(par) == 'number'" v-model.number="value[name]" :type="input_type(par)"
              :rules="[v => v !== null || 'Value is required', v => par.min <= v || 'Value must be greater than or equal to ' + par.min, v => par.max >= v || 'Value must be less than or equal to ' + par.max]"
              :label="name" required />
            <v-checkbox v-else-if="input_type(par) == 'checkbox'" v-model="value[name]" :label="name" required />
            <v-combobox v-else-if="input_type(par) == 'symbol' && par.symbols && par.symbols.length > 0"
              v-model="value[name]" :label="name" :items="par.symbols" required />
            <v-text-field v-else v-model="value[name]" :type="input_type(par)" :rules="[v => !!v || 'Value is required']"
              :label="name" required />
          </td>
        </tr>
      </tbody>
    </v-table>
    <v-btn block @click="new_sensor_data(sensor.id, value)">Publish</v-btn>
  </v-container>
</template>

<script setup>
import { BooleanParameter, FloatParameter, IntegerParameter, Sensor, StringParameter, SymbolParameter } from '@/sensor';
import { reactive } from 'vue';

const props = defineProps({
  sensor: {
    type: Sensor,
    required: true
  }
});

const value = reactive({});
props.sensor.type.parameters.forEach((parameter) => {
  if (props.sensor.lastValue && props.sensor.lastValue[parameter.name])
    value[parameter.name] = props.sensor.lastValue[parameter.name];
  else
    value[parameter.name] = parameter.default;
});

function new_sensor_data(sensor_id, value) {
  fetch('http://' + location.host + '/sensor/' + sensor_id, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(value)
  }).then(res => {
    if (!res.ok)
      res.json().then(data => alert(data.message));
  })
}
</script>

<script>
function input_type(par) {
  if (par instanceof BooleanParameter)
    return 'checkbox';
  else if (par instanceof IntegerParameter || par instanceof FloatParameter)
    return 'number';
  else if (par instanceof StringParameter)
    return 'text';
  else if (par instanceof SymbolParameter)
    return 'symbol';
  else
    throw new Error(`Unknown parameter type: ${par}`);
}
</script>