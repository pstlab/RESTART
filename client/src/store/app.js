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
            this.sensors.get(data.sensor).add_value(data.value, new Date(data.timestamp * 1000));
            break;
          case 'sensor_state':
            this.sensors.get(data.sensor).add_state(data.state, new Date(data.timestamp * 1000));
            break;
          case 'solvers':
            this.solvers.clear();
            for (let solver of data.solvers)
              this.solvers.set(solver.id, new Solver(solver.id, solver.name, solver.state));
            break;
          case 'new_solver':
            this.solvers.set(data.id, new Solver(data.id, data.name, data.state));
            break;
          case 'deleted_solver':
            this.solvers.delete(data.id);
            break;
          case 'state_changed':
            this.solvers.get(data.solver_id).state_changed(data);
            break;
          case 'graph':
            this.solvers.get(data.solver_id).graph(data);
            break;
          case 'flaw_created':
            this.solvers.get(data.solver_id).flaw_created(data);
            break;
          case 'flaw_state_changed':
            this.solvers.get(data.solver_id).flaw_state_changed(data);
            break;
          case 'flaw_cost_changed':
            this.solvers.get(data.solver_id).flaw_cost_changed(data);
            break;
          case 'flaw_position_changed':
            this.solvers.get(data.solver_id).flaw_position_changed(data);
            break;
          case 'current_flaw':
            this.solvers.get(data.solver_id).current_flaw_changed(data);
            break;
          case 'resolver_created':
            this.solvers.get(data.solver_id).resolver_created(data);
            break;
          case 'resolver_state_changed':
            this.solvers.get(data.solver_id).resolver_state_changed(data);
            break;
          case 'current_resolver':
            this.solvers.get(data.solver_id).current_resolver_changed(data);
            break;
          case 'causal_link_added':
            this.solvers.get(data.solver_id).causal_link_added(data);
            break;
          case 'executor_state_changed':
            if (this.solvers.has(data.solver_id))
              this.solvers.get(data.solver_id).state = data.state;
            break;
          case 'tick':
            this.solvers.get(data.solver_id).tick(data);
            break;
          case 'starting':
            this.solvers.get(data.solver_id).starting(data);
            break;
          case 'ending':
            this.solvers.get(data.solver_id).ending(data);
            break;
          case 'start':
            this.solvers.get(data.solver_id).start(data);
            break;
          case 'end':
            this.solvers.get(data.solver_id).end(data);
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
      return new SymbolParameter(par.name, par.symbols);
    default:
      throw 'Unknown parameter type: ' + par;
  }
}