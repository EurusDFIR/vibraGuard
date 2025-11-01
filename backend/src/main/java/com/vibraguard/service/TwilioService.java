package com.vibraguard.service;

import com.twilio.rest.api.v2010.account.Call;
import com.twilio.rest.api.v2010.account.Message;
import com.twilio.type.PhoneNumber;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.net.URI;
import java.util.ArrayList;
import java.util.List;

/**
 * Twilio Service
 * Xử lý gửi SMS và thực hiện cuộc gọi cảnh báo
 */
@Service
@Slf4j
public class TwilioService {

    @Value("${twilio.phone_number}")
    private String twilioPhoneNumber;

    @Value("${my.phone_number}")
    private String myPhoneNumber;

    @Value("${friend.phone_number}")
    private String friendPhoneNumber;

    @Value("${twilio.twiml_bin_url}")
    private String twimlBinUrl;

    /**
     * Gửi SMS cảnh báo tới một số điện thoại
     */
    public void sendSms(String toPhoneNumber, String messageBody) {
        try {
            Message message = Message.creator(
                    new PhoneNumber(toPhoneNumber), // Tới ai
                    new PhoneNumber(twilioPhoneNumber), // Từ ai (Twilio number)
                    messageBody) // Nội dung
                    .create();

            log.info("📤 SMS sent to {}: SID={}", toPhoneNumber, message.getSid());
        } catch (Exception e) {
            log.error("❌ Failed to send SMS to {}: {}", toPhoneNumber, e.getMessage());
        }
    }

    /**
     * Gửi SMS tới tất cả số đã đăng ký
     */
    public void sendSmsToAll(String messageBody) {
        List<String> recipients = getRecipients();
        log.info("📨 Sending SMS to {} recipients", recipients.size());

        for (String recipient : recipients) {
            sendSms(recipient, messageBody);
        }
    }

    /**
     * Thực hiện cuộc gọi tới một số điện thoại
     */
    public void makeCall(String toPhoneNumber) {
        try {
            Call call = Call.creator(
                    new PhoneNumber(toPhoneNumber), // Tới ai
                    new PhoneNumber(twilioPhoneNumber), // Từ ai (Twilio number)
                    new URI(twimlBinUrl)) // Nội dung cuộc gọi (TwiML URL)
                    .create();

            log.info("📞 Making call to {}: SID={}", toPhoneNumber, call.getSid());
        } catch (Exception e) {
            log.error("❌ Failed to make call to {}: {}", toPhoneNumber, e.getMessage());
        }
    }

    /**
     * Thực hiện cuộc gọi tới tất cả số đã đăng ký
     */
    public void makeCallToAll() {
        List<String> recipients = getRecipients();
        log.info("📞 Making calls to {} recipients", recipients.size());

        for (String recipient : recipients) {
            makeCall(recipient);
        }
    }

    /**
     * Gửi cảnh báo đầy đủ (SMS + Call) tới tất cả
     */
    public void sendFullAlert(String deviceId, double confidence) {
        String smsMessage = String.format(
                "🚨 VibraGuard ALERT!\n" +
                        "Device: %s\n" +
                        "AI Confidence: %.1f%%\n" +
                        "ATTACK DETECTED! Check immediately!",
                deviceId, confidence);

        log.info("🚨 Sending full alert (SMS + Call) for device: {}", deviceId);

        // Gửi SMS
        sendSmsToAll(smsMessage);

        // Gọi điện (delay 1 giây để tránh rate limit)
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }

        makeCallToAll();
    }

    /**
     * Lấy danh sách số điện thoại nhận cảnh báo
     */
    private List<String> getRecipients() {
        List<String> recipients = new ArrayList<>();

        if (myPhoneNumber != null && !myPhoneNumber.isEmpty()) {
            recipients.add(myPhoneNumber);
        }

        if (friendPhoneNumber != null && !friendPhoneNumber.isEmpty()) {
            recipients.add(friendPhoneNumber);
        }

        return recipients;
    }
}
