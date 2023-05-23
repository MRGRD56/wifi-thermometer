import {EcoMode} from "../types/api";
import axios from "axios";
import {API_HOST} from "../constants.ts";

const setEcoMode = async (isEco: boolean): Promise<EcoMode> => {
    const response = await axios.post<EcoMode>(`${API_HOST}/eco`, undefined, {
        params: {isEco}
    });
    return response.data;
};

export default setEcoMode;