package ru.mrgrd56.wifithermometer.dto;

import org.apache.commons.lang3.builder.ToStringBuilder;

public class TemperaturePlaceDto {
    private Float temperature;
    private Float humidity;

    public Float getTemperature() {
        return temperature;
    }

    public void setTemperature(Float temperature) {
        this.temperature = temperature;
    }

    public Float getHumidity() {
        return humidity;
    }

    public void setHumidity(Float humidity) {
        this.humidity = humidity;
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
                .appendSuper(super.toString())
                .append("temperature", temperature)
                .append("humidity", humidity)
                .toString();
    }
}
