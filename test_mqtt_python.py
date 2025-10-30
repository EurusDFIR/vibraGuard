#!/usr/bin/env python3
"""
Test MQTT to WebSocket flow for VibraGuard using paho-mqtt library
No need for mosquitto_pub command line tool
"""

import json
import time
from datetime import datetime, timezone
import sys

try:
    import paho.mqtt.client as mqtt
    MQTT_AVAILABLE = True
except ImportError:
    MQTT_AVAILABLE = False
    print("⚠️  paho-mqtt not installed")
    print("Installing now...")
    import subprocess
    try:
        subprocess.check_call([sys.executable, "-m", "pip", "install", "paho-mqtt"])
        import paho.mqtt.client as mqtt
        MQTT_AVAILABLE = True
        print("✅ paho-mqtt installed successfully!\n")
    except Exception as e:
        print(f"❌ Failed to install paho-mqtt: {e}")
        print("\nPlease install manually: pip install paho-mqtt")
        sys.exit(1)

MQTT_HOST = "34.87.133.103"
MQTT_PORT = 1883
MQTT_TOPIC = "vibra_guard/sensor"  # Changed to match backend subscription
BACKEND_URL = f"http://{MQTT_HOST}:8080"

def create_test_message(device_id="ESP32_CUA_SO_01"):
    """Create a test sensor data message matching backend DTO"""
    return {
        "deviceId": device_id,
        "timestamp": int(time.time() * 1000),  # Unix timestamp in milliseconds
        "sensorValue": 85,  # Simulated sensor reading
        "aiTriggered": True,
        "confidence": 95.5,
        "scores": {
            "attack": 95.5,
            "normal": 3.2,
            "noise": 1.3
        }
    }

def on_connect(client, userdata, flags, rc):
    """Callback when connected to MQTT broker"""
    if rc == 0:
        print("✅ Connected to MQTT broker!")
    else:
        print(f"❌ Connection failed with code {rc}")

def on_publish(client, userdata, mid):
    """Callback when message is published"""
    print("✅ Message published successfully!")

def send_mqtt_message_python(message):
    """Send message using paho-mqtt Python library"""
    try:
        message_json = json.dumps(message)
        
        print(f"📡 Connecting to MQTT broker {MQTT_HOST}:{MQTT_PORT}")
        print(f"📦 Topic: {MQTT_TOPIC}")
        print(f"📝 Message:")
        print(json.dumps(message, indent=2))
        print()
        
        # Create MQTT client
        client = mqtt.Client(client_id="vibraguard_test_python")
        client.on_connect = on_connect
        client.on_publish = on_publish
        
        # Connect to broker
        print("🔄 Connecting...")
        client.connect(MQTT_HOST, MQTT_PORT, 60)
        client.loop_start()
        
        # Wait for connection
        time.sleep(1)
        
        # Publish message
        print(f"📤 Publishing to {MQTT_TOPIC}...")
        result = client.publish(MQTT_TOPIC, message_json, qos=1)
        
        # Wait for publish to complete
        result.wait_for_publish()
        time.sleep(0.5)
        
        client.loop_stop()
        client.disconnect()
        
        return True
        
    except Exception as e:
        print(f"❌ Error sending message: {e}")
        import traceback
        traceback.print_exc()
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
                latest = data[-1]  # Get the last event (most recent)
                print("\n📊 Latest event:")
                print(f"   Device: {latest.get('deviceId', 'N/A')}")
                print(f"   Type: {latest.get('vibrationType', 'N/A')}")
                print(f"   Confidence: {latest.get('aiConfidence', 'N/A')}%")
                print(f"   Severity: {latest.get('severity', 'N/A')}")
                print(f"   Timestamp: {latest.get('timestamp', 'N/A')}")
                return True
            else:
                print("⚠️  No events found in database yet")
                print("   This might mean:")
                print("   - MQTT message wasn't received by backend")
                print("   - Backend MQTT subscriber isn't running")
                print("   - Database connection issue")
                return False
                
    except Exception as e:
        print(f"❌ Error checking API: {e}")
        return False

def check_backend_status():
    """Check if backend is running"""
    try:
        import urllib.request
        
        print("🔍 Checking backend status...")
        with urllib.request.urlopen(f"{BACKEND_URL}/api/control/status") as response:
            data = json.loads(response.read().decode())
            print(f"✅ Backend is running!")
            print(f"   System Status: {data.get('systemStatus', 'N/A')}")
            print(f"   Devices Count: {data.get('devicesCount', 0)}")
            return True
    except Exception as e:
        print(f"❌ Backend not reachable: {e}")
        return False

def main():
    print("=" * 70)
    print("🧪 VibraGuard MQTT → Backend → WebSocket Test (Python Edition)")
    print("=" * 70)
    print()
    
    # Check backend first
    if not check_backend_status():
        print("\n❌ Cannot continue - backend is not running!")
        return
    
    print()
    
    # Create test message
    message = create_test_message()
    
    # Send to MQTT
    if send_mqtt_message_python(message):
        print("\n⏳ Waiting 3 seconds for backend to process...")
        time.sleep(3)
        
        # Check if it reached the backend
        if check_api_events():
            print("\n🎉 SUCCESS! The flow is working:")
            print("   MQTT → Backend → Database ✅")
            print("\n📋 Now test WebSocket:")
            print("   1. Open quick_test.html or test_websocket.html in browser")
            print("   2. Click 'Connect' button")
            print("   3. Run this script again:")
            print("      python test_mqtt_python.py")
            print("   4. You should see the alert appear in the browser!")
            print()
            print("   Or open React frontend at: http://localhost:5174")
        else:
            print("\n⚠️  Message sent but not found in database")
            print("\n🔧 Troubleshooting steps:")
            print("   1. Check backend logs:")
            print("      gcloud compute ssh vibraguard-server --zone=asia-southeast1-b")
            print("      docker logs vibraguard-prod_backend_1 --tail 50")
            print()
            print("   2. Check MQTT broker:")
            print("      docker logs vibraguard-prod_mosquitto_1 --tail 50")
            print()
            print("   3. Verify MQTT subscriber is running in backend")
    else:
        print("\n❌ Failed to send MQTT message")
        print("\n🔧 Alternative: Use MQTT Explorer GUI")
        print("   Download: http://mqtt-explorer.com/")
        print("   Connect to: 34.87.133.103:1883")
        print("   Publish to: vibraguard/sensor_data")
        print("   Use this JSON:")
        print(json.dumps(message, indent=2))
    
    print("\n✅ Test complete!")

if __name__ == "__main__":
    main()
