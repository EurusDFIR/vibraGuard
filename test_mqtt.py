#!/usr/bin/env python3
"""
Test MQTT to WebSocket flow for VibraGuard
Sends test data to MQTT broker and checks if it reaches the frontend via WebSocket
"""

import json
import time
from datetime import datetime
import subprocess
import sys

MQTT_HOST = "34.87.133.103"
MQTT_PORT = 1883
MQTT_TOPIC = "vibraguard/sensor_data"
BACKEND_URL = f"http://{MQTT_HOST}:8080"

def create_test_message():
    """Create a test sensor data message"""
    return {
        "deviceId": "ESP32_TEST_001",
        "vibrationType": "ABNORMAL",
        "severity": "HIGH",
        "aiConfidence": 95.5,
        "timestamp": datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ"),
        "ax": 2.5,
        "ay": 1.2,
        "az": 9.8,
        "gx": 0.1,
        "gy": 0.2,
        "gz": 0.05,
        "temperature": 28.5
    }

def send_mqtt_message(message):
    """Send message using mosquitto_pub"""
    try:
        message_json = json.dumps(message)
        
        print(f"📡 Sending to MQTT broker {MQTT_HOST}:{MQTT_PORT}")
        print(f"📦 Topic: {MQTT_TOPIC}")
        print(f"📝 Message:")
        print(json.dumps(message, indent=2))
        print()
        
        # Try mosquitto_pub command
        result = subprocess.run([
            'mosquitto_pub',
            '-h', MQTT_HOST,
            '-p', str(MQTT_PORT),
            '-t', MQTT_TOPIC,
            '-m', message_json
        ], capture_output=True, text=True, timeout=5)
        
        if result.returncode == 0:
            print("✅ Message sent successfully!")
            return True
        else:
            print(f"❌ Error: {result.stderr}")
            return False
            
    except FileNotFoundError:
        print("❌ mosquitto_pub not found!")
        print("\n📥 Installation instructions:")
        print("   Windows: Download from https://mosquitto.org/download/")
        print("   Linux: sudo apt-get install mosquitto-clients")
        print("   Mac: brew install mosquitto")
        print("\n📱 Alternative: Use MQTT Explorer GUI")
        print("   Download: http://mqtt-explorer.com/")
        return False
    except Exception as e:
        print(f"❌ Error sending message: {e}")
        return False

def check_api_events():
    """Check if event was received by backend"""
    try:
        import urllib.request
        
        print("\n🔍 Checking backend API...")
        with urllib.request.urlopen(f"{BACKEND_URL}/api/events") as response:
            data = json.loads(response.read().decode())
            
            if data:
                print(f"✅ Found {len(data)} events in database")
                latest = data[0]
                print("\n📊 Latest event:")
                print(f"   Device: {latest.get('deviceId', 'N/A')}")
                print(f"   Type: {latest.get('vibrationType', 'N/A')}")
                print(f"   Confidence: {latest.get('aiConfidence', 'N/A')}%")
                print(f"   Timestamp: {latest.get('timestamp', 'N/A')}")
                return True
            else:
                print("⚠️  No events found in database yet")
                return False
                
    except Exception as e:
        print(f"❌ Error checking API: {e}")
        return False

def main():
    print("=" * 60)
    print("🧪 VibraGuard MQTT → Backend → WebSocket Test")
    print("=" * 60)
    print()
    
    # Create test message
    message = create_test_message()
    
    # Send to MQTT
    if send_mqtt_message(message):
        print("\n⏳ Waiting 2 seconds for backend to process...")
        time.sleep(2)
        
        # Check if it reached the backend
        check_api_events()
        
        print("\n📋 Next steps:")
        print("   1. Open quick_test.html or test_websocket.html in browser")
        print("   2. Click 'Connect' to subscribe to WebSocket alerts")
        print("   3. Run this script again to send another test message")
        print("   4. You should see the alert appear in the browser!")
        print()
        print("   Or open frontend at: http://localhost:5174")
    else:
        print("\n💡 Alternative testing method:")
        print("   1. Use MQTT Explorer to connect to", MQTT_HOST)
        print("   2. Publish to topic:", MQTT_TOPIC)
        print("   3. Use this JSON:")
        print(json.dumps(message, indent=2))
    
    print("\n✅ Test complete!")

if __name__ == "__main__":
    main()
