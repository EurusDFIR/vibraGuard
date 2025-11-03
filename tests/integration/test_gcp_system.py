#!/usr/bin/env python3
"""
GCP System Health Check
Test toàn bộ hệ thống trên Google Cloud Platform
"""

import requests
import json
import sys
import time

GCP_IP = "34.87.133.103"
BACKEND_URL = f"http://{GCP_IP}:8080"

def print_section(title):
    print("\n" + "=" * 70)
    print(f"  {title}")
    print("=" * 70)

def test_backend_api():
    """Test Backend REST API"""
    print_section("1. TESTING BACKEND API")
    
    try:
        # Test control status
        response = requests.get(f"{BACKEND_URL}/api/control/status", timeout=5)
        print(f"✅ Backend API is alive!")
        print(f"   Status Code: {response.status_code}")
        data = response.json()
        print(f"   Devices Count: {data.get('devicesCount', 'N/A')}")
        print(f"   System Status: {data.get('systemStatus', 'N/A')}")
        return True
    except requests.exceptions.ConnectionError:
        print(f"❌ Cannot connect to backend at {BACKEND_URL}")
        return False
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_database_via_api():
    """Test Database through Backend API"""
    print_section("2. TESTING DATABASE (via API)")
    
    try:
        response = requests.get(f"{BACKEND_URL}/api/events", timeout=5)
        if response.status_code == 200:
            events = response.json()
            print(f"✅ Database connection OK!")
            print(f"   Total events in database: {len(events)}")
            
            if events:
                latest = events[0]
                print(f"\n   Latest Event:")
                print(f"   - ID: {latest.get('id')}")
                print(f"   - Device: {latest.get('deviceId')}")
                print(f"   - Severity: {latest.get('severity')}")
                print(f"   - Sensor Value: {latest.get('sensorValue')}")
                print(f"   - Timestamp: {latest.get('timestamp')}")
            return True
        else:
            print(f"⚠️  Status code: {response.status_code}")
            return False
    except Exception as e:
        print(f"❌ Database test failed: {e}")
        return False

def test_mqtt_publish():
    """Test MQTT by publishing a message"""
    print_section("3. TESTING MQTT BROKER")
    
    print("📡 Publishing test message to MQTT broker...")
    print(f"   Broker: {GCP_IP}:1883")
    print(f"   Topic: vibra_guard/sensor")
    
    try:
        import paho.mqtt.client as mqtt
        
        test_message = {
            "deviceId": "ESP32_TEST_GCP",
            "timestamp": int(time.time() * 1000),
            "sensorValue": 95,
            "aiTriggered": True,
            "confidence": 0.95,
            "scores": {
                "attack": 95.0,
                "normal": 3.5,
                "noise": 1.5
            }
        }
        
        published = False
        error_msg = None
        
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("✅ Connected to MQTT broker")
            else:
                print(f"❌ Connection failed with code {rc}")
        
        def on_publish(client, userdata, mid):
            nonlocal published
            published = True
            print("✅ Message published successfully")
        
        client = mqtt.Client(client_id="test_gcp_health")
        client.on_connect = on_connect
        client.on_publish = on_publish
        
        print("   Connecting to broker...")
        client.connect(GCP_IP, 1883, 60)
        client.loop_start()
        
        time.sleep(1)
        
        print(f"   Publishing: {json.dumps(test_message, indent=2)}")
        result = client.publish("vibra_guard/sensor", json.dumps(test_message))
        
        time.sleep(2)
        client.loop_stop()
        client.disconnect()
        
        if published:
            print("\n✅ MQTT Test PASSED")
            print("   Message should be processed by backend")
            print("   Check database for new event with device: ESP32_TEST_GCP")
            return True
        else:
            print("⚠️  Message may not have been delivered")
            return False
            
    except ImportError:
        print("⚠️  paho-mqtt not installed. Install with: pip install paho-mqtt")
        print("   Skipping MQTT test...")
        return None
    except Exception as e:
        print(f"❌ MQTT test failed: {e}")
        return False

def test_websocket():
    """Test WebSocket connection"""
    print_section("4. TESTING WEBSOCKET")
    
    print(f"WebSocket endpoint: ws://{GCP_IP}:8080/ws")
    print(f"Topic: /topic/alerts")
    print("⚠️  WebSocket requires browser or websocket client")
    print("   Use test_websocket.html for manual testing")
    return None

def verify_latest_event():
    """Verify the test message was processed"""
    print_section("5. VERIFYING TEST MESSAGE")
    
    print("Waiting 3 seconds for backend to process...")
    time.sleep(3)
    
    try:
        response = requests.get(f"{BACKEND_URL}/api/events", timeout=5)
        if response.status_code == 200:
            events = response.json()
            
            # Look for our test message
            test_event = None
            for event in events:
                if event.get('deviceId') == 'ESP32_TEST_GCP':
                    test_event = event
                    break
            
            if test_event:
                print("✅ Test message processed successfully!")
                print(f"   Event ID: {test_event.get('id')}")
                print(f"   Device: {test_event.get('deviceId')}")
                print(f"   Sensor Value: {test_event.get('sensorValue')}")
                print(f"   Severity: {test_event.get('severity')}")
                return True
            else:
                print("⚠️  Test message not found in database yet")
                print("   This might be normal if backend takes time to process")
                return False
    except Exception as e:
        print(f"❌ Verification failed: {e}")
        return False

def main():
    print("\n" + "=" * 70)
    print("  🚀 VIBRAGUARD GCP SYSTEM HEALTH CHECK")
    print("=" * 70)
    print(f"  GCP Server: {GCP_IP}")
    print(f"  Backend URL: {BACKEND_URL}")
    print("=" * 70)
    
    results = {}
    
    # Run tests
    results['backend'] = test_backend_api()
    results['database'] = test_database_via_api()
    results['mqtt'] = test_mqtt_publish()
    results['websocket'] = test_websocket()
    
    if results['mqtt']:
        results['verification'] = verify_latest_event()
    
    # Summary
    print_section("SUMMARY")
    
    print("\n📊 Test Results:")
    print(f"   Backend API:      {'✅ PASS' if results['backend'] else '❌ FAIL'}")
    print(f"   Database:         {'✅ PASS' if results['database'] else '❌ FAIL'}")
    
    if results['mqtt'] is None:
        print(f"   MQTT Broker:      ⚠️  SKIPPED (install paho-mqtt)")
    else:
        print(f"   MQTT Broker:      {'✅ PASS' if results['mqtt'] else '❌ FAIL'}")
    
    print(f"   WebSocket:        ⚠️  MANUAL TEST REQUIRED")
    
    if 'verification' in results:
        print(f"   Message Flow:     {'✅ PASS' if results['verification'] else '⚠️  PENDING'}")
    
    # Overall status
    critical_tests = [results['backend'], results['database']]
    all_critical_pass = all(critical_tests)
    
    print("\n" + "=" * 70)
    if all_critical_pass:
        print("  ✅ SYSTEM STATUS: OPERATIONAL")
        print("  All critical components are working correctly!")
    else:
        print("  ❌ SYSTEM STATUS: DEGRADED")
        print("  Some critical components are not working!")
    print("=" * 70)
    
    print("\n📝 Next Steps:")
    if results['mqtt'] is None:
        print("   1. Install MQTT client: pip install paho-mqtt")
        print("   2. Re-run this script to test MQTT")
    print("   3. Test WebSocket with browser: tests/websocket/test_websocket.html")
    print("   4. Test Twilio integration (if needed)")
    print("   5. Deploy ESP32 for real hardware testing")
    
    return 0 if all_critical_pass else 1

if __name__ == "__main__":
    sys.exit(main())
