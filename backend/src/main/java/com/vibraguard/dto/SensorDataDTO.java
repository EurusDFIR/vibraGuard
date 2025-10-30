package com.vibraguard.dto;

import com.fasterxml.jackson.annotation.JsonAlias;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.Map;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class SensorDataDTO {
    @JsonProperty("deviceId")
    @JsonAlias("device_id") // Accept both camelCase and snake_case
    private String deviceId;

    private Long timestamp;
    private Integer sensorValue;

    // AI-related fields for Edge Impulse integration
    @JsonProperty("aiTriggered")
    @JsonAlias("ai_triggered") // Accept both camelCase and snake_case
    private Boolean aiTriggered;

    private Double confidence;
    private Map<String, Double> scores; // attack, normal, noise scores
}
