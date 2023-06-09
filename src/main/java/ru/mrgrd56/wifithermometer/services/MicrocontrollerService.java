package ru.mrgrd56.wifithermometer.services;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;
import org.springframework.web.client.HttpClientErrorException;
import ru.mrgrd56.wifithermometer.dto.EcoModeDto;
import ru.mrgrd56.wifithermometer.dto.PresenceDto;
import ru.mrgrd56.wifithermometer.dto.TemperatureDataDto;

import java.util.function.Predicate;
import java.util.function.Supplier;

@Service
public class MicrocontrollerService {
    private final Logger log = LoggerFactory.getLogger(this.getClass());

    private final MicrocontrollerApiService microcontrollerApiService;

    public MicrocontrollerService(MicrocontrollerApiService microcontrollerApiService) {
        this.microcontrollerApiService = microcontrollerApiService;
    }

    public TemperatureDataDto getTemperature() {
        return tryRequest("getTemperature", microcontrollerApiService::getTemperature, this::validateTemperatureData);
    }

    public EcoModeDto getEcoMode() {
        return tryRequest("getEcoMode", microcontrollerApiService::getEcoMode, this::validateEcoMode);
    }

    public EcoModeDto setEcoMode(boolean isEcoMode) {
        return tryRequest("setEcoMode", () -> microcontrollerApiService.setEcoMode(isEcoMode), this::validateEcoMode);
    }

    public PresenceDto getPresence() {
        return tryRequest("getPresence", microcontrollerApiService::getPresence, this::validatePresence);
    }

    private <T> T tryRequest(String name, Supplier<T> requestData, Predicate<T> validateData) {
        try {
            T result = tryRequest(5, requestData, validateData);
            log.info("{} -> {}", name, result);
            return result;
        } catch (Exception e) {
            log.error("{} -> error", name);
            throw e;
        }
    }

    private <T> T tryRequest(int attempts, Supplier<T> requestData, Predicate<T> validateData) {
        T data;
        Exception exception = null;
        try {
            data = requestData.get();
        } catch (HttpClientErrorException e) {
            throw e;
        } catch (Exception e) {
            data = null;
            exception = e;
            log.error("An error occurred while getting data", e);
        }

        if (validateData.test(data)) {
            return data;
        }

        if (attempts > 0) {
            log.warn("Got invalid data, retrying. Data: {}", data);

            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }

            return tryRequest(attempts - 1, requestData, validateData);
        } else {
            if (exception == null) {
                log.error("Unable to get data due to invalid data {}", data);
                throw new RuntimeException("Invalid data gotten");
            } else {
                log.error("Unable to get data due to an exception", exception);
                throw new RuntimeException(exception);
            }
        }
    }

    private boolean validateEcoMode(EcoModeDto ecoMode) {
        return ecoMode != null && ecoMode.getEco() != null;
    }

    private boolean validatePresence(PresenceDto presence) {
        return presence != null && presence.getPresent() != null;
    }

    private boolean validateTemperatureData(TemperatureDataDto data) {
        return data != null
            && data.getOutside() != null
            && data.getInside() != null
            && validateTemperature(data.getOutside().getTemperature())
            && validateHumidity(data.getOutside().getHumidity())
            && validateTemperature(data.getInside().getTemperature());
    }

    private boolean validateTemperature(Float temperature) {
        return temperature != null && temperature != 0.0f;
    }

    private boolean validateHumidity(Float humidity) {
        return humidity != null && humidity != 0.0f && humidity != -6.0f;
    }
}
