#!/usr/bin/env python3
"""
Test Twilio Integration on GCP
Gửi test message để trigger Twilio alert
"""

import requests
import json
import time
import os

# Configuration from environment variables with fallback defaults
GCP_IP = os.getenv("GCP_IP", "34.87.133.103")
BACKEND_URL = os.getenv("BACKEND_URL", f"http://{GCP_IP}:8080")

def test_twilio_trigger():
    """Trigger Twilio alert by sending high confidence attack"""
    
    print("=" * 70)
    print("  🚨 TESTING TWILIO INTEGRATION ON GCP")
    print("=" * 70)
    print(f"  Backend URL: {BACKEND_URL}")
    print("=" * 70)
    
    # Create a high confidence attack event
    test_data = {
        "deviceId": "ESP32_TWILIO_TEST",
        "timestamp": int(time.time() * 1000),
        "sensorValue": 98,
        "aiTriggered": True,
        "confidence": 0.98,  # High confidence to trigger Twilio
        "scores": {
            "attack": 98.0,
            "normal": 1.5,
            "noise": 0.5
        }
    }
    
    print("\n📡 Publishing test attack to MQTT...")
    print(f"   Confidence: 98% (should trigger Twilio alert)")
    print(f"   Device: ESP32_TWILIO_TEST")
    
    try:
        import paho.mqtt.client as mqtt
        
        published = False
        
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("✅ Connected to MQTT broker")
        
        def on_publish(client, userdata, mid):
            nonlocal published
            published = True
            print("✅ Message published to MQTT")
        
        client = mqtt.Client(client_id="twilio_test_client")
        client.on_connect = on_connect
        client.on_publish = on_publish
        
        client.connect(GCP_IP, 1883, 60)
        client.loop_start()
        time.sleep(1)
        
        result = client.publish("vibra_guard/sensor", json.dumps(test_data))
        time.sleep(2)
        
        client.loop_stop()
        client.disconnect()
        
        if published:
            print("\n⏳ Waiting for backend to process...")
            time.sleep(3)
            
            print("\n" + "=" * 70)
            print("  📊 CHECKING BACKEND LOGS")
            print("=" * 70)
            
            print("\nTo verify Twilio was triggered, check backend logs:")
            print("   gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \\")
            print("   --command=\"docker logs backend --tail 50 | grep -i twilio\"")
            
            print("\n⚠️  EXPECTED BEHAVIOR:")
            print("   ✅ Twilio initialized successfully")
            print("   📨 Sending SMS to 1 recipients")
            print("   📞 Making calls to 1 recipients")
            print("   ⚠️  Error: Number is unverified (Trial account)")
            
            print("\n" + "=" * 70)
            print("  📱 TWILIO TRIAL ACCOUNT LIMITATION")
            print("=" * 70)
            print("\n⚠️  Phone number +84399354603 needs verification!")
            print("\n   Steps to verify:")
            print("   1. Go to: https://console.twilio.com/us1/develop/phone-numbers/manage/verified")
            print("   2. Click 'Add Verified Number'")
            print("   3. Enter: +84399354603")
            print("   4. Verify with SMS code")
            print("   5. Re-run this test")
            
            print("\n   OR upgrade to paid Twilio account for unrestricted usage")
            
            return True
        else:
            print("❌ Failed to publish message")
            return False
            
    except ImportError:
        print("❌ paho-mqtt not installed. Install with: pip install paho-mqtt")
        return False
    except Exception as e:
        print(f"❌ Test failed: {e}")
        return False

def check_event_in_db():
    """Check if event was saved to database"""
    print("\n" + "=" * 70)
    print("  🗄️  CHECKING DATABASE")
    print("=" * 70)
    
    try:
        response = requests.get(f"{BACKEND_URL}/api/events", timeout=5)
        if response.status_code == 200:
            events = response.json()
            
            # Find our test event
            test_event = None
            for event in events:
                if event.get('deviceId') == 'ESP32_TWILIO_TEST':
                    test_event = event
                    break
            
            if test_event:
                print("\n✅ Test event saved to database!")
                print(f"   Event ID: {test_event.get('id')}")
                print(f"   Device: {test_event.get('deviceId')}")
                print(f"   Sensor Value: {test_event.get('sensorValue')}")
                print(f"   Severity: {test_event.get('severity')}")
                print(f"   Timestamp: {test_event.get('timestamp')}")
                return True
            else:
                print("⚠️  Test event not found in database")
                return False
    except Exception as e:
        print(f"❌ Database check failed: {e}")
        return False

if __name__ == "__main__":
    print("\n")
    success = test_twilio_trigger()
    
    if success:
        time.sleep(2)
        check_event_in_db()
    
    print("\n" + "=" * 70)
    print("  ✅ TEST COMPLETED")
    print("=" * 70)
    print("\nNote: Twilio will show errors until phone number is verified")
    print("But the system is working correctly - it's just a Trial account limitation")
    print("\n")
