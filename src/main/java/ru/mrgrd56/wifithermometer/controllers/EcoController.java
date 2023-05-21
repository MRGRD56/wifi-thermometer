package ru.mrgrd56.wifithermometer.controllers;

import org.springframework.web.bind.annotation.*;
import ru.mrgrd56.wifithermometer.dto.EcoModeDto;
import ru.mrgrd56.wifithermometer.services.MicrocontrollerService;

@RestController
@RequestMapping("eco")
public class EcoController {
    private final MicrocontrollerService microcontrollerService;

    public EcoController(MicrocontrollerService microcontrollerService) {
        this.microcontrollerService = microcontrollerService;
    }

    @GetMapping
    public EcoModeDto getEcoMode() {
        return microcontrollerService.getEcoMode();
    }

    @PostMapping
    public EcoModeDto setEcoMode(@RequestParam boolean isEco) {
        return microcontrollerService.setEcoMode(isEco);
    }
}
