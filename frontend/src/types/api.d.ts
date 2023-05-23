export interface TemperatureSnapshot {
    time: string;
    data: TemperaturePlace;
}

export interface TemperaturePlace {
    outside: TemperatureData;
    inside: TemperatureData;
}

export interface TemperatureData {
    temperature?: number;
    humidity?: number;
}

export interface EcoMode {
    isEco: boolean;
}

export interface Presence {
    isPresent: boolean;
    isPresentNow: boolean;
}