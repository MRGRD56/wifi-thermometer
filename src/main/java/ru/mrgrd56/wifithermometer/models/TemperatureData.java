package ru.mrgrd56.wifithermometer.models;

import org.apache.commons.lang3.builder.ToStringBuilder;
import ru.mrgrd56.wifithermometer.dto.TemperatureDataDto;

import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

public class TemperatureData {
    private final String time;
    private TemperatureDataDto data;

    public TemperatureData(TemperatureDataDto data) {
        this.time = DateTimeFormatter.ISO_OFFSET_DATE_TIME.format(ZonedDateTime.now());
        this.data = data;
    }

    public String getTime() {
        return time;
    }

    public TemperatureDataDto getData() {
        return data;
    }

    public void setData(TemperatureDataDto data) {
        this.data = data;
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
                .appendSuper(super.toString())
                .append("time", time)
                .append("data", data)
                .toString();
    }
}
