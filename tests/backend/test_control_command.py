#!/usr/bin/env python3
"""
Test sending MQTT control commands to ESP32
Simulates backend sending SOUND_ALARM command
"""

import json
import sys

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("Installing paho-mqtt...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "paho-mqtt"])
    import paho.mqtt.client as mqtt

MQTT_HOST = "34.87.133.103"
MQTT_PORT = 1883

def send_control_command(device_id, command):
    """Send control command to specific ESP32 device"""
    topic = f"vibra_guard/control/{device_id}"
    
    print(f"📡 Sending {command} command to {device_id}")
    print(f"📦 Topic: {topic}")
    
    client = mqtt.Client(client_id="vibraguard_control_test")
    client.connect(MQTT_HOST, MQTT_PORT, 60)
    client.loop_start()
    
    # Publish command
    result = client.publish(topic, command, qos=1)
    result.wait_for_publish()
    
    print(f"✅ Command sent successfully!")
    
    client.loop_stop()
    client.disconnect()

def main():
    print("=" * 60)
    print("🎮 VibraGuard Control Command Test")
    print("=" * 60)
    print()
    
    # Available commands
    commands = {
        "1": ("SOUND_ALARM", "Activate buzzer (Panic button)"),
        "2": ("DISARM", "Stop buzzer and disarm system"),
        "3": ("ARM", "Arm the system"),
    }
    
    print("Available commands:")
    for key, (cmd, desc) in commands.items():
        print(f"  {key}. {cmd} - {desc}")
    print()
    
    choice = input("Select command (1-3): ").strip()
    
    if choice not in commands:
        print("❌ Invalid choice!")
        return
    
    command = commands[choice][0]
    
    # Device IDs
    device_ids = [
        "ESP32_CUA_SO_01",  # Your actual ESP32
        "ESP32_TEST_001"     # Test device from MQTT test
    ]
    
    print("\nAvailable devices:")
    for i, dev_id in enumerate(device_ids, 1):
        print(f"  {i}. {dev_id}")
    print()
    
    dev_choice = input(f"Select device (1-{len(device_ids)}) or 'all': ").strip()
    
    if dev_choice.lower() == 'all':
        for dev_id in device_ids:
            send_control_command(dev_id, command)
            print()
    elif dev_choice.isdigit() and 1 <= int(dev_choice) <= len(device_ids):
        dev_id = device_ids[int(dev_choice) - 1]
        send_control_command(dev_id, command)
    else:
        print("❌ Invalid device choice!")
        return
    
    print("\n✅ Test complete!")
    print("\n📋 What should happen:")
    if command == "SOUND_ALARM":
        print("  - ESP32 buzzer should start beeping")
        print("  - Check ESP32 Serial Monitor for confirmation")
    elif command == "DISARM":
        print("  - ESP32 buzzer should stop")
        print("  - System state: DISARMED")
    elif command == "ARM":
        print("  - System state: ARMED")
        print("  - Ready to detect attacks")

if __name__ == "__main__":
    main()
