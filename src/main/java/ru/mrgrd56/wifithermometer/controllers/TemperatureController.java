package ru.mrgrd56.wifithermometer.controllers;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import ru.mrgrd56.wifithermometer.dto.TemperatureSnapshotDto;
import ru.mrgrd56.wifithermometer.services.MicrocontrollerService;

@RestController
@RequestMapping("temperature")
public class TemperatureController {
    private final MicrocontrollerService microcontrollerService;

    public TemperatureController(MicrocontrollerService microcontrollerService) {
        this.microcontrollerService = microcontrollerService;
    }

    @GetMapping("current")
    public TemperatureSnapshotDto getCurrentTemperature() {
        return new TemperatureSnapshotDto(microcontrollerService.getTemperature());
    }
}
