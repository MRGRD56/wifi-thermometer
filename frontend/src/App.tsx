import { useQuery } from 'react-query';
import getCurrentTemperature from './api/getCurrentTemperature';
import './App.scss';
import {TemperatureSnapshot} from "./types/api.ts";
import {EcoMode} from "./types/api";
import getEcoMode from "./api/getEcoMode.ts";
import setEcoMode from "./api/setEcoMode.ts";
import {useState} from "react";

const TemperatureCard = ({ title, temperature, humidity, time }) => {
    return (
        <div className="card">
            <h2>{title}</h2>
            <div className="temperature">{temperature ? (temperature.toFixed(2) + ' °C') : '-'}</div>
            <div className="humidity">{humidity ? (humidity.toFixed(2) + ' %') : '-'}</div>
            <div className="time">{time}</div>
        </div>
    )
};

const App = () => {
    const [ecoMode, setEcoModeValue] = useState<EcoMode>();

    const { data: snapshot, isLoading: isTemperatureLoading } = useQuery<TemperatureSnapshot>('temperature', getCurrentTemperature, { refetchInterval: 5000 });
    const { isLoading: isEcoLoading, refetch: fetchEcoMode } =
        useQuery<EcoMode>('eco-mode', getEcoMode, {
            refetchInterval: 1500,
            onSuccess: setEcoModeValue
        });

    const handleEcoModeClick = () => {
        if (ecoMode) {
            setEcoMode(!ecoMode.isEco).then(setEcoModeValue);
        }
    };

    if (isTemperatureLoading || isEcoLoading) {
        return <div className="app"></div>;
    }

    return (
        <div className="app">
            <div className="temperatures">
                <TemperatureCard
                    title="Outside"
                    temperature={snapshot?.data?.outside.temperature}
                    humidity={snapshot?.data?.outside.humidity}
                    time={snapshot?.time}
                />
                <TemperatureCard
                    title="Inside"
                    temperature={snapshot?.data?.inside.temperature}
                    humidity={snapshot?.data?.inside.humidity}
                    time={snapshot?.time}
                />
            </div>

            <button onClick={handleEcoModeClick}>ECO Mode {ecoMode && (ecoMode.isEco ? 'ON' : 'OFF') }</button>
        </div>
    );
}

export default App;