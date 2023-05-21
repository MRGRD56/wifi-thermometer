package ru.mrgrd56.wifithermometer.services;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.web.client.RestTemplateBuilder;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;
import ru.mrgrd56.wifithermometer.dto.EcoModeDto;
import ru.mrgrd56.wifithermometer.dto.PresenceDto;
import ru.mrgrd56.wifithermometer.dto.TemperatureDataDto;

import java.time.Duration;

@Service
public class MicrocontrollerApiService {
    @Value("${mcu.api.host}")
    private String apiHost;

    @Value("${mcu.api.auth-key}")
    private String apiKey;

    public TemperatureDataDto getTemperature() {
        return restTemplate().getForObject("/temperature", TemperatureDataDto.class);
    }

    public EcoModeDto getEcoMode() {
        return restTemplate().getForObject("/eco", EcoModeDto.class);
    }

    public EcoModeDto setEcoMode(boolean isEco) {
        return restTemplate().postForObject("/eco?isEco=" + isEco, null, EcoModeDto.class);
    }

    public PresenceDto getPresence() {
        return restTemplate().getForObject("/presence", PresenceDto.class);
    }

    private RestTemplate restTemplate() {
        return new RestTemplateBuilder()
                .rootUri(apiHost)
                .defaultHeader("X-Api-Key", apiKey)
                .setConnectTimeout(Duration.ofMillis(2000))
                .setReadTimeout(Duration.ofMillis(2000))
                .build();
    }
}
