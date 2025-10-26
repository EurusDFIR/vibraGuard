package com.vibraguard.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class SensorDataDTO {
    private String deviceId;
    private Long timestamp;
    private Integer sensorValue;
}
