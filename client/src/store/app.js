// Utilities
import { BooleanParameter, FloatParameter, IntegerParameter, Sensor, SensorType, StringParameter, SymbolParameter } from '@/sensor';
import { Solver } from '@/solver';
import { defineStore } from 'pinia'

export const useAppStore = defineStore('app', {
  state: () => ({
    sensor_types: new Map(),
    sensors: new Map(),
    solvers: new Map(),
  }),
  actions: {
    connect(url = 'ws://' + location.host + '/restart', timeout = 1000) {
      this.socket = new WebSocket(url);
      this.socket.onopen = () => {
        this.socket.send(JSON.stringify({ 'type': 'login' }));
      };
      this.socket.onclose = () => {
        setTimeout(() => { this.connect(url, timeout); }, timeout);
      };
      this.socket.onerror = (error) => {
        console.log('WebSocket Error: ' + error);
        setTimeout(() => { this.connect(url, timeout); }, timeout);
      };
      this.socket.onmessage = (e) => {
        const data = JSON.parse(e.data);
        console.log(data);
        switch (data.type) {
          case 'login':
            break;
          case 'sensor_types':
            this.sensor_types.clear();
            for (const sensor_type of data.sensor_types) {
              const parameters = new Map();
              for (const par of sensor_type.parameters)
                parameters.set(par.name, getParameterType(par));
              this.sensor_types.set(sensor_type.id, new SensorType(sensor_type.id, sensor_type.name, sensor_type.description, parameters));
            }
            break;
          case 'new_sensor_type':
            const parameters = new Map();
            for (const par of data.sensor_type.parameters)
              parameters.set(par.name, getParameterType(par));
            this.sensor_types.set(data.sensor_type.id, new SensorType(data.sensor_type.id, data.sensor_type.name, data.sensor_type.description, parameters));
            break;
          case 'updated_sensor_type':
            const sensor_type = this.sensor_types.get(data.sensor_type.id);
            sensor_type.name = data.sensor_type.name;
            sensor_type.description = data.sensor_type.description;
            sensor_type.parameters.clear();
            for (const par of data.sensor_type.parameters)
              sensor_type.parameters.set(par.name, getParameterType(par));
            break;
          case 'deleted_sensor_type':
            this.sensor_types.delete(data.sensor_type);
            break;
          case 'sensors':
            this.sensors.clear();
            for (let sensor of data.sensors) {
              const s = new Sensor(sensor.id, sensor.name, this.sensor_types.get(sensor.type), sensor.description, sensor.location);
              if (sensor.value)
                s.lastValue = sensor.value;
              if (sensor.state)
                s.lastState = sensor.state;
              if (sensor.value || sensor.state)
                s.lastUpdate = sensor.timestamp;
              this.sensors.set(sensor.id, s);
            }
            break;
          case 'new_sensor':
            this.sensors.set(data.sensor.id, new Sensor(data.sensor.id, data.sensor.name, this.sensor_types.get(data.sensor.type), data.sensor.description, data.sensor.location));
            break;
          case 'updated_sensor':
            const sensor = this.sensors.get(data.sensor.id);
            sensor.name = data.sensor.name;
            sensor.type = this.sensor_types.get(data.sensor.type);
            sensor.description = data.sensor.description;
            sensor.location = data.sensor.location;
            break;
          case 'deleted_sensor':
            this.sensors.delete(data.sensor);
            break;
          case 'sensor_value':
            this.sensors.get(data.sensor).add_value(data.value, data.timestamp * 1000);
            break;
          case 'sensor_state':
            this.sensors.get(data.sensor).addState(data.state, data.timestamp * 1000);
            break;
          case 'solvers':
            this.solvers.clear();
            for (let solver of data.solvers)
              this.solvers.set(solver.id, new Solver(solver.id, solver.name, solver.state));
            break;
          case 'new_solver':
            this.solvers.set(data.solver.id, new Solver(data.solver.id, data.solver.name, data.solver.state));
            break;
          case 'deleted_solver':
            this.solvers.delete(data.solver);
            break;
        }
      };
    }
  }
})

function getParameterType(par) {
  switch (par.type) {
    case 'int':
      return new IntegerParameter(par.name, par.min, par.max);
    case 'float':
      return new FloatParameter(par.name, par.min, par.max);
    case 'string':
      return new StringParameter(par.name);
    case 'bool':
      return new BooleanParameter(par.name);
    case 'symbol':
      return new SymbolParameter(par.name, par.values);
    default:
      throw 'Unknown parameter type: ' + par;
  }
}