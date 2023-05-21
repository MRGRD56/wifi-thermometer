package ru.mrgrd56.wifithermometer.services;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

@Service
public class AutoEcoModeService {
    private final Logger log = LoggerFactory.getLogger(this.getClass());

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
        log.info("Auto Eco Mode - setting {}", isEco);
        microcontrollerService.setEcoMode(isEco);
    }
}
