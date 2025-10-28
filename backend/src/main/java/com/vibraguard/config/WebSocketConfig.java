package com.vibraguard.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.messaging.simp.config.MessageBrokerRegistry;
import org.springframework.web.socket.config.annotation.EnableWebSocketMessageBroker;
import org.springframework.web.socket.config.annotation.StompEndpointRegistry;
import org.springframework.web.socket.config.annotation.WebSocketMessageBrokerConfigurer;

/**
 * WebSocket Configuration cho VibraGuard
 * Cho phép frontend (React/Flutter) nhận cảnh báo real-time
 */
@Configuration
@EnableWebSocketMessageBroker
public class WebSocketConfig implements WebSocketMessageBrokerConfigurer {

    /**
     * Cấu hình message broker
     * - /topic: broadcast messages (sử dụng cho alerts)
     * - /app: messages sent from client
     */
    @Override
    public void configureMessageBroker(MessageBrokerRegistry config) {
        config.enableSimpleBroker("/topic");
        config.setApplicationDestinationPrefixes("/app");
    }

    /**
     * Đăng ký endpoint WebSocket
     * - Frontend kết nối tới: ws://<server>:8080/ws
     * - Hỗ trợ SockJS fallback cho các trình duyệt cũ
     * - Cho phép CORS từ mọi origin (có thể giới hạn trong production)
     */
    @Override
    public void registerStompEndpoints(StompEndpointRegistry registry) {
        registry.addEndpoint("/ws")
                .setAllowedOriginPatterns("*")
                .withSockJS();
    }
}
