import paho.mqtt.client as mqtt
import json
import time
import os

# Configuration from environment variables with fallback defaults
broker = os.getenv("MQTT_HOST", "34.87.133.103")
port = int(os.getenv("MQTT_PORT", "1883"))
topic = os.getenv("MQTT_TOPIC", "vibra_guard/sensor")

message = {
    'deviceId': 'ESP32_CUA_SO_01',
    'timestamp': int(time.time() * 1000),
    'sensorValue': 98,
    'aiTriggered': True,
    'confidence': 0.982,  # 98.2% confidence - very high!
    'scores': {
        'attack': 98.2,
        'normal': 1.2,
        'noise': 0.6
    }
}

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, 'test_twilio_2')
print(f'Connecting to {broker}:{port}...')
client.connect(broker, port, 60)
print(f'Publishing HIGH confidence attack to {topic}...')
client.publish(topic, json.dumps(message))
print('Message sent with 98.2% confidence!')
print(json.dumps(message, indent=2))
client.disconnect()
