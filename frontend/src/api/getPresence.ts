import {Presence} from "../types/api";
import axios from "axios";
import {API_HOST} from "../constants.ts";

const getPresence = async (): Promise<Presence> => {
    const response = await axios.get<Presence>(`${API_HOST}/presence`);
    return response.data;
};

export default getPresence;