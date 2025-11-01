package com.vibraguard.config;

import com.twilio.Twilio;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;
import jakarta.annotation.PostConstruct;

/**
 * Twilio Configuration
 * Khởi tạo Twilio SDK khi Spring Boot startup
 */
@Configuration
public class TwilioConfig {

    @Value("${twilio.account_sid}")
    private String accountSid;

    @Value("${twilio.auth_token}")
    private String authToken;

    @PostConstruct // Hàm này tự chạy sau khi Spring Boot khởi động
    public void initTwilio() {
        Twilio.init(accountSid, authToken);
        System.out.println("===============================================");
        System.out.println("✅ Twilio initialized successfully");
        System.out.println("📱 Account SID: " + accountSid.substring(0, 10) + "...");
        System.out.println("===============================================");
    }
}
