package com.vibraguard.service;

import com.vibraguard.model.Device;
import com.vibraguard.repository.DeviceRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Service xử lý các lệnh điều khiển hệ thống
 * Gửi lệnh ARM/DISARM qua MQTT đến các thiết bị ESP32
 */
@Service
@RequiredArgsConstructor
@Slf4j
public class ControlService {

    private final DeviceRepository deviceRepository;

    @Value("${mqtt.broker.url:tcp://localhost:1883}")
    private String mqttBrokerUrl;

    private String systemStatus = "ARMED"; // Mặc định là ARMED

    /**
     * Gửi lệnh ARM đến một thiết bị cụ thể
     */
    public void sendArmCommand(String deviceId) {
        String topic = "vibra_guard/control/" + deviceId;
        sendMqttCommand(topic, "ARM");
        systemStatus = "ARMED";
        log.info("✅ ARM command sent to device: {}", deviceId);
    }

    /**
     * Gửi lệnh DISARM đến một thiết bị cụ thể
     */
    public void sendDisarmCommand(String deviceId) {
        String topic = "vibra_guard/control/" + deviceId;
        sendMqttCommand(topic, "DISARM");
        systemStatus = "DISARMED";
        log.info("✅ DISARM command sent to device: {}", deviceId);
    }

    /**
     * Gửi lệnh ARM đến tất cả thiết bị
     */
    public void sendArmCommandToAll() {
        List<Device> devices = deviceRepository.findAll();
        for (Device device : devices) {
            sendArmCommand(device.getDeviceId());
        }
        log.info("✅ ARM command sent to {} devices", devices.size());
    }

    /**
     * Gửi lệnh DISARM đến tất cả thiết bị
     */
    public void sendDisarmCommandToAll() {
        List<Device> devices = deviceRepository.findAll();
        for (Device device : devices) {
            sendDisarmCommand(device.getDeviceId());
        }
        log.info("✅ DISARM command sent to {} devices", devices.size());
    }

    /**
     * Gửi lệnh SOUND_ALARM (Panic Button) đến một thiết bị cụ thể
     */
    public void sendSoundAlarmCommand(String deviceId) {
        String topic = "vibra_guard/control/" + deviceId;
        sendMqttCommand(topic, "SOUND_ALARM");
        log.info("🚨 SOUND_ALARM command sent to device: {}", deviceId);
    }

    /**
     * Gửi lệnh SOUND_ALARM (Panic Button) đến tất cả thiết bị
     */
    public void sendSoundAlarmCommandToAll() {
        List<Device> devices = deviceRepository.findAll();
        for (Device device : devices) {
            sendSoundAlarmCommand(device.getDeviceId());
        }
        log.info("🚨 SOUND_ALARM command sent to {} devices", devices.size());
    }

    /**
     * Lấy trạng thái hiện tại của hệ thống
     */
    public String getSystemStatus() {
        return systemStatus;
    }

    /**
     * Đếm số lượng thiết bị đang hoạt động
     */
    public int getActiveDevicesCount() {
        return (int) deviceRepository.count();
    }

    /**
     * Gửi lệnh qua MQTT
     */
    private void sendMqttCommand(String topic, String command) {
        try {
            MqttClient client = new MqttClient(mqttBrokerUrl,
                    "VibraGuard-Backend-Control-" + System.currentTimeMillis());
            client.connect();

            MqttMessage message = new MqttMessage(command.getBytes());
            message.setQos(1); // QoS 1: At least once delivery
            message.setRetained(false);

            client.publish(topic, message);
            client.disconnect();

            log.info("📤 MQTT command sent: topic={}, command={}", topic, command);
        } catch (MqttException e) {
            log.error("❌ Failed to send MQTT command: {}", e.getMessage(), e);
            throw new RuntimeException("Failed to send MQTT command", e);
        }
    }
}
