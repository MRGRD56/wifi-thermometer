package ru.mrgrd56.wifithermometer.services;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;
import ru.mrgrd56.wifithermometer.dto.TemperatureDataDto;

import java.util.concurrent.atomic.AtomicReference;

@Service
public class TemperatureService {
    @Value("mcu.api.host")
    private String apiHost;

    private final AtomicReference<TemperatureDataDto> lastTemperature = new AtomicReference<>();

    // TODO add auto updating

    public TemperatureDataDto getTemperature() {
        if (lastTemperature.get() == null) {
            return requestTemperature();
        }

        return lastTemperature.get();
    }

    public TemperatureDataDto requestTemperature() {
        TemperatureDataDto data = new RestTemplate()
                .getForEntity(apiHost + "/temperature", TemperatureDataDto.class)
                .getBody();

        lastTemperature.set(data);
        return data;
    }
}
