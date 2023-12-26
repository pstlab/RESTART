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
        this.lastValue = new Map();
        this.lastState = new Map();
        this.lastUpdate = new Date();
    }
}