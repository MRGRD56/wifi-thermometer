package ru.mrgrd56.wifithermometer.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import org.apache.commons.lang3.builder.ToStringBuilder;

public class EcoModeDto {
    private Boolean isEco;

    @JsonProperty("isEco")
    public Boolean getEco() {
        return isEco;
    }

    public void setEco(Boolean eco) {
        isEco = eco;
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
                .appendSuper(super.toString())
                .append("isEco", isEco)
                .toString();
    }
}
