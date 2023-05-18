package ru.mrgrd56.wifithermometer.dto;

import org.apache.commons.lang3.builder.ToStringBuilder;

public class TemperatureDataDto {
    private TemperaturePlaceDto outside;
    private TemperaturePlaceDto inside;

    public TemperaturePlaceDto getOutside() {
        return outside;
    }

    public void setOutside(TemperaturePlaceDto outside) {
        this.outside = outside;
    }

    public TemperaturePlaceDto getInside() {
        return inside;
    }

    public void setInside(TemperaturePlaceDto inside) {
        this.inside = inside;
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
                .appendSuper(super.toString())
                .append("outside", outside)
                .append("inside", inside)
                .toString();
    }
}
