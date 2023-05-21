import axios from 'axios'
import {TemperatureSnapshot} from "../types/api.ts";

const API_HOST = import.meta.env.VITE_API_HOST;

const getCurrentTemperature = async () => {
    const response = await axios.get<TemperatureSnapshot>(`${API_HOST}/temperature/current`);
    return response.data;
}

export default getCurrentTemperature;