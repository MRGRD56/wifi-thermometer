package ru.mrgrd56.wifithermometer.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import org.apache.commons.lang3.builder.ToStringBuilder;

public class PresenceDto {
    private Boolean isPresent;
    private Boolean isPresentNow;

    @JsonProperty("isPresent")
    public Boolean getPresent() {
        return isPresent;
    }

    public void setPresent(Boolean present) {
        isPresent = present;
    }

    @JsonProperty("isPresentNow")
    public Boolean getPresentNow() {
        return isPresentNow;
    }

    public void setPresentNow(Boolean presentNow) {
        isPresentNow = presentNow;
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
                .appendSuper(super.toString())
                .append("isPresent", isPresent)
                .append("isPresentNow", isPresentNow)
                .toString();
    }
}
