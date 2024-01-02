export class Parameter {
    constructor(name, default_value = undefined) { this.name = name; this.default_value = default_value; }
}

export class BooleanParameter extends Parameter {
    constructor(name, default_value = false) { super(name, default_value); }
}

export class IntegerParameter extends Parameter {
    constructor(name, min, max, default_value = min) {
        super(name, default_value);
        this.min = min;
        this.max = max;
    }
}

export class FloatParameter extends Parameter {
    constructor(name, min, max, default_value = min) {
        super(name, default_value);
        this.min = min;
        this.max = max;
    }
}

export class StringParameter extends Parameter {
    constructor(name, default_value = "") { super(name, default_value); }
}

export class SymbolParameter extends Parameter {
    constructor(name, symbols, default_value = symbols ? symbols[0] : "") {
        super(name, default_value);
        this.symbols = symbols;
    }
}

export class SensorType {
    constructor(id, name, description, parameters) {
        this.id = id;
        this.name = name;
        this.description = description;
        this.parameters = parameters;
    }
}

export class Sensor {
    constructor(id, name, type, description = "", location = undefined) {
        this.id = id;
        this.name = name;
        this.type = type;
        this.description = description;
        this.location = location;
        this.lastValue = undefined;
        this.lastState = undefined;
        this.lastUpdate = undefined;
        this.values = [];
        this.valueTimestamps = [];
        this.states = [];
        this.stateTimestamps = [];
        this.values_listeners = new Set();
        this.value_listeners = new Set();
    }

    set_values(values, timestamps) {
        this.values = values;
        this.valueTimestamps = timestamps;
        this.lastValue = values[values.length - 1];
        this.lastUpdate = timestamps[timestamps.length - 1];
        this.values_listeners.forEach(l => l(values, timestamps));
    }

    add_value(value, timestamp = new Date().getTime()) {
        this.lastValue = value;
        this.lastUpdate = timestamp;
        this.values.push(value);
        this.valueTimestamps.push(timestamp);
        this.value_listeners.forEach(l => l(value, timestamp));
    }

    set_states(states, timestamps) {
        this.states = states;
        this.stateTimestamps = timestamps;
        this.lastState = states[states.length - 1];
        this.lastUpdate = timestamps[timestamps.length - 1];
    }

    add_state(state, timestamp) {
        this.lastState = state;
        this.lastUpdate = timestamp;
        this.states.push(state);
        this.stateTimestamps.push(timestamp);
    }

    add_values_listener(callback) { this.values_listeners.add(callback); callback(this.values, this.valueTimestamps); }
    add_value_listener(callback) { this.value_listeners.add(callback); }

    remove_values_listener(callback) { this.values_listeners.delete(callback); }
    remove_value_listener(callback) { this.value_listeners.delete(callback); }
}