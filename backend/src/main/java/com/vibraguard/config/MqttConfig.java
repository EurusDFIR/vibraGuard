package com.vibraguard.config;

import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.core.MessageProducer;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.integration.mqtt.core.MqttPahoClientFactory;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.integration.mqtt.support.DefaultPahoMessageConverter;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;

@Configuration
public class MqttConfig {

    @Value("${mqtt.broker.url}")
    private String brokerUrl;

    @Value("${mqtt.client.id}")
    private String clientId;

    @Value("${mqtt.topic.sensor}")
    private String sensorTopic;

    @Bean
    public MqttPahoClientFactory mqttClientFactory() {
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        MqttConnectOptions options = new MqttConnectOptions();
        options.setServerURIs(new String[] { brokerUrl });
        options.setCleanSession(true);
        options.setAutomaticReconnect(true);
        options.setConnectionTimeout(10);
        factory.setConnectionOptions(options);
        return factory;
    }

    @Bean
    public MessageChannel mqttInputChannel() {
        return new DirectChannel();
    }

    @Bean
    public MessageProducer inbound() {
        System.out.println("===============================================");
        System.out.println("🔧 Configuring MQTT Inbound Adapter");
        System.out.println("📡 Broker URL: " + brokerUrl);
        System.out.println("🆔 Client ID: " + clientId + "-inbound");
        System.out.println("📢 Subscribe Topic: " + sensorTopic);
        System.out.println("===============================================");

        MqttPahoMessageDrivenChannelAdapter adapter = new MqttPahoMessageDrivenChannelAdapter(
                clientId + "-inbound",
                mqttClientFactory(),
                sensorTopic);
        adapter.setCompletionTimeout(5000);
        adapter.setConverter(new DefaultPahoMessageConverter());
        adapter.setQos(1);
        adapter.setOutputChannel(mqttInputChannel());
        return adapter;
    }

    // 🧪 TEMPORARY: Enable debug handler to test MQTT receipt
    @Bean
    @ServiceActivator(inputChannel = "mqttInputChannel")
    public MessageHandler debugHandler() {
        return message -> {
            System.out.println("===============================================");
            System.out.println("🔥 DEBUG HANDLER: MQTT Message Received!");
            System.out.println("📦 Payload: " + message.getPayload());
            System.out.println("📋 Headers: " + message.getHeaders());
            System.out.println("===============================================");
        };
    }
}
