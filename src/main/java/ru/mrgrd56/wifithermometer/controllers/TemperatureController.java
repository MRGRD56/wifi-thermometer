package ru.mrgrd56.wifithermometer.controllers;

import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.annotation.PostConstruct;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import ru.mrgrd56.wifithermometer.util.MapBuilder;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;
import java.util.LinkedHashMap;
import java.util.Objects;

@RestController
@RequestMapping("temperature")
public class TemperatureController {
    private final Logger log = LoggerFactory.getLogger(this.getClass());

    private final ObjectMapper objectMapper = new ObjectMapper();

    private final String authKey;

    public TemperatureController(
            @Value("${auth-key}") String authKey) {
        this.authKey = authKey;
    }

    @PostConstruct
    private void initialize() {
        try {
            Path dataDirectory = Path.of("data");

            if (!Files.exists(dataDirectory)) {
                Files.createDirectory(dataDirectory);
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @PostMapping
    public ResponseEntity<?> updateTemperature(
            @RequestParam String key,
            @RequestParam double temperature,
            @RequestParam double humidity) throws IOException {
        if (!StringUtils.isBlank(authKey) && !Objects.equals(key, authKey)) {
            return ResponseEntity.status(HttpStatus.FORBIDDEN).build();
        }

        if (temperature == 0.0 && (humidity == 0.0 || humidity == -6.0)) {
            log.warn("INVALID_DATA: temperature={}, humidity={}", temperature, humidity);
            return ResponseEntity.badRequest().body("Invalid temperature or humidity");
        }

        log.info("NEW_DATA: temperature={}, humidity={}", temperature, humidity);

        String json = objectMapper.writeValueAsString(
                MapBuilder.fromEntries(LinkedHashMap::new,
                        MapBuilder.entry("time", DateTimeFormatter.ISO_OFFSET_DATE_TIME.format(ZonedDateTime.now())),
                        MapBuilder.entry("data",
                                MapBuilder.fromEntries(LinkedHashMap::new,
                                        MapBuilder.entry("outside", MapBuilder.fromEntries(LinkedHashMap::new,
                                                MapBuilder.entry("temperature", temperature),
                                                MapBuilder.entry("humidity", humidity))
                                        )
                                )
                        )
                )
        ) + "\n";

        Files.writeString(Path.of(".", "data", "temperature.jsonl"),
                json,
                StandardCharsets.UTF_8,
                StandardOpenOption.CREATE,
                StandardOpenOption.APPEND);

        return ResponseEntity.ok().build();
    }
}
