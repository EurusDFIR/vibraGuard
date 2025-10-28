package com.vibraguard.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.Map;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class SensorDataDTO {
    private String deviceId;
    private Long timestamp;
    private Integer sensorValue;

    // AI-related fields for Edge Impulse integration
    private Boolean aiTriggered;
    private Double confidence;
    private Map<String, Double> scores; // attack, normal, noise scores
}
