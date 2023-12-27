export class Parameter {
    constructor(name) { this.name = name; }
}

export class BooleanParameter extends Parameter {
    constructor(name) { super(name); }
}

export class IntegerParameter extends Parameter {
    constructor(name, min, max) {
        super(name);
        this.min = min;
        this.max = max;
    }
}

export class FloatParameter extends Parameter {
    constructor(name, min, max) {
        super(name);
        this.min = min;
        this.max = max;
    }
}

export class StringParameter extends Parameter {
    constructor(name) { super(name); }
}

export class SymbolParameter extends Parameter {
    constructor(name, symbols) {
        super(name);
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
        this.value_listeners = [];
    }

    setValues(values, timestamps) {
        this.values = values;
        this.valueTimestamps = timestamps;
        this.lastValue = values[values.length - 1];
        this.lastUpdate = timestamps[timestamps.length - 1];
    }

    addValue(value, timestamp = new Date().getTime()) {
        this.lastValue = value;
        this.lastUpdate = timestamp;
        this.values.push(value);
        this.valueTimestamps.push(timestamp);
        this.value_listeners.forEach(l => l(value, timestamp));
    }

    setStates(states, timestamps) {
        this.states = states;
        this.stateTimestamps = timestamps;
        this.lastState = states[states.length - 1];
        this.lastUpdate = timestamps[timestamps.length - 1];
    }

    addState(state, timestamp) {
        this.lastState = state;
        this.lastUpdate = timestamp;
        this.states.push(state);
        this.stateTimestamps.push(timestamp);
    }

    addValueListener(callback) {
        this.value_listeners.push(callback);
    }

    removeValueListener(callback) {
        this.value_listeners = this.value_listeners.filter(l => l !== callback);
    }
}