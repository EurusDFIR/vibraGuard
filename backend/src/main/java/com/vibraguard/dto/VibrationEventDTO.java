package com.vibraguard.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class VibrationEventDTO {
    private Long id;
    private String deviceId;
    private String deviceName;
    private String location;
    private java.time.LocalDateTime eventTimestamp;
    private Integer sensorValue;
    private String severity;
    private String notes;
}
