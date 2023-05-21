package ru.mrgrd56.wifithermometer.services;

import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

@Service
public class AutoEcoModeService {
    private final MicrocontrollerService microcontrollerService;

    public AutoEcoModeService(MicrocontrollerService microcontrollerService) {
        this.microcontrollerService = microcontrollerService;
    }

    @Scheduled(cron = "${auto-eco.enable.cron}")
    public void autoEnableEcoMode() {
        autoSetEcoMode(true);
    }

    @Scheduled(cron = "${auto-eco.disable.cron}")
    public void autoDisableEcoMode() {
        autoSetEcoMode(false);
    }

    private void autoSetEcoMode(boolean isEco) {
        microcontrollerService.setEcoMode(isEco);
    }
}
