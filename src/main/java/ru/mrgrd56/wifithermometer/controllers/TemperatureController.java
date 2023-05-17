package ru.mrgrd56.wifithermometer.controllers;

import jakarta.servlet.http.HttpServletRequest;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;
import java.util.Objects;

@RestController
@RequestMapping("temperature")
public class TemperatureController {
    private final Logger log = LoggerFactory.getLogger(this.getClass());

    private final String authKey;

    public TemperatureController(
            @Value("${auth-key}") String authKey) {
        this.authKey = authKey;
    }

    @PostMapping
    public ResponseEntity<Void> updateTemperature(
            @RequestParam String key,
            @RequestParam double temperature,
            @RequestParam double humidity) {
        if (!StringUtils.isBlank(authKey) && !Objects.equals(key, authKey)) {
            return ResponseEntity.status(HttpStatus.FORBIDDEN).build();
        }

        log.info("NEW_DATA: temperature={}, humidity={}", temperature, humidity);

        return ResponseEntity.ok().build();
    }
}
