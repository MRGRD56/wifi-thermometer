import {EcoMode} from "../types/api";
import axios from "axios";
import {API_HOST} from "../constants.ts";

const getEcoMode = async (): Promise<EcoMode> => {
    const response = await axios.get<EcoMode>(`${API_HOST}/eco`);
    return response.data;
};

export default getEcoMode;