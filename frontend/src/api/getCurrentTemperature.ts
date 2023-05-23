import axios from 'axios'
import {TemperatureSnapshot} from "../types/api.ts";
import {API_HOST} from "../constants.ts";

const getCurrentTemperature = async (): Promise<TemperatureSnapshot> => {
    const response = await axios.get<TemperatureSnapshot>(`${API_HOST}/temperature/current`);
    return response.data;
}

export default getCurrentTemperature;