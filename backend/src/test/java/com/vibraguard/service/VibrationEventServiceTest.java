package com.vibraguard.service;

import com.vibraguard.dto.VibrationEventDTO;
import com.vibraguard.model.Device;
import com.vibraguard.repository.DeviceRepository;
import com.vibraguard.repository.VibrationEventRepository;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.util.Optional;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.when;

public class VibrationEventServiceTest {

    @InjectMocks
    private VibrationEventService vibrationEventService;

    @Mock
    private VibrationEventRepository vibrationEventRepository;

    @Mock
    private DeviceRepository deviceRepository;

    @BeforeEach
    public void setup() {
        MockitoAnnotations.openMocks(this);
    }

    @Test
    public void testCreateEvent_shouldSetSeverityToInfo() {
        Device device = new Device();
        device.setDeviceId("test-device");
        when(deviceRepository.findByDeviceId("test-device")).thenReturn(Optional.of(device));
        when(vibrationEventRepository.save(any())).thenAnswer(i -> i.getArguments()[0]);

        VibrationEventDTO result = vibrationEventService.createEvent("test-device", 400);

        assertEquals("INFO", result.getSeverity());
    }

    @Test
    public void testCreateEvent_shouldSetSeverityToWarning() {
        Device device = new Device();
        device.setDeviceId("test-device");
        when(deviceRepository.findByDeviceId("test-device")).thenReturn(Optional.of(device));
        when(vibrationEventRepository.save(any())).thenAnswer(i -> i.getArguments()[0]);

        VibrationEventDTO result = vibrationEventService.createEvent("test-device", 600);

        assertEquals("WARNING", result.getSeverity());
    }

    @Test
    public void testCreateEvent_shouldSetSeverityToCritical() {
        Device device = new Device();
        device.setDeviceId("test-device");
        when(deviceRepository.findByDeviceId("test-device")).thenReturn(Optional.of(device));
        when(vibrationEventRepository.save(any())).thenAnswer(i -> i.getArguments()[0]);

        VibrationEventDTO result = vibrationEventService.createEvent("test-device", 900);

        assertEquals("CRITICAL", result.getSeverity());
    }
}
