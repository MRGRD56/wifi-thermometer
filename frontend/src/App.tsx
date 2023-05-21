import { useQuery } from 'react-query';
import getCurrentTemperature from './api/getCurrentTemperature';
import './App.scss';
import {TemperatureSnapshot} from "./types/api.ts";

const TemperatureCard = ({ title, temperature, humidity, time }) => {
    return (
        <div className="card">
            <h2>{title}</h2>
            <div className="temperature">{temperature ?? '-'}</div>
            <div className="humidity">{humidity ?? '-'}</div>
            <div className="time">{time}</div>
        </div>
    )
}

const App = () => {
    const { data: snapshot, refetch } = useQuery<TemperatureSnapshot>('temperature', getCurrentTemperature, { refetchInterval: 5000 });

    return (
        <div className="app">
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
    );
}

export default App;