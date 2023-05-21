package ru.mrgrd56.wifithermometer.controllers;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import ru.mrgrd56.wifithermometer.dto.PresenceDto;
import ru.mrgrd56.wifithermometer.services.MicrocontrollerService;

@RestController
@RequestMapping("detection")
public class DetectionController {
    private final MicrocontrollerService microcontrollerService;

    public DetectionController(MicrocontrollerService microcontrollerService) {
        this.microcontrollerService = microcontrollerService;
    }

    @GetMapping("presence")
    public PresenceDto getPresence() {
        return microcontrollerService.getPresence();
    }
}
