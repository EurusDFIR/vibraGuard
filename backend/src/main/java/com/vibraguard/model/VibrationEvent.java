package com.vibraguard.model;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Entity
@Table(name = "vibration_events")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class VibrationEvent {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @ManyToOne
    @JoinColumn(name = "device_id", nullable = false)
    private Device device;

    @Column(name = "event_timestamp", nullable = false)
    private java.time.LocalDateTime eventTimestamp;

    @Column(name = "sensor_value")
    private Integer sensorValue; // Giá trị từ cảm biến (nếu có)

    private String severity = "NORMAL"; // NORMAL, WARNING, CRITICAL

    @Column(length = 500)
    private String notes; // Ghi chú thêm

    @PrePersist
    protected void onCreate() {
        if (eventTimestamp == null) {
            eventTimestamp = java.time.LocalDateTime.now();
        }
    }
}
