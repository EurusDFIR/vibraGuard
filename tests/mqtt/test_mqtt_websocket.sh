#!/bin/bash

# Test MQTT và WebSocket end-to-end

echo "🧪 Testing VibraGuard MQTT → Backend → WebSocket flow"
echo "=================================================="
echo ""

MQTT_HOST="34.87.133.103"
MQTT_PORT="1883"
TOPIC="vibraguard/sensor_data"

echo "📡 Sending test data to MQTT broker..."
echo "Host: $MQTT_HOST:$MQTT_PORT"
echo "Topic: $TOPIC"
echo ""

# Test message
TEST_DATA='{
  "deviceId": "ESP32_TEST_001",
  "vibrationType": "ABNORMAL",
  "severity": "HIGH",
  "aiConfidence": 95.5,
  "timestamp": "'$(date -u +"%Y-%m-%dT%H:%M:%SZ")'",
  "ax": 2.5,
  "ay": 1.2,
  "az": 9.8,
  "gx": 0.1,
  "gy": 0.2,
  "gz": 0.05,
  "temperature": 28.5
}'

echo "📦 Test data:"
echo "$TEST_DATA" | python -m json.tool
echo ""

# Check if mosquitto_pub is available
if ! command -v mosquitto_pub &> /dev/null; then
    echo "❌ mosquitto_pub not found!"
    echo "Install with: sudo apt-get install mosquitto-clients (Linux)"
    echo "Or download from: https://mosquitto.org/download/"
    echo ""
    echo "Alternative: Use MQTT Explorer GUI"
    echo "Download: http://mqtt-explorer.com/"
    exit 1
fi

echo "🚀 Publishing message..."
mosquitto_pub -h "$MQTT_HOST" -p "$MQTT_PORT" -t "$TOPIC" -m "$TEST_DATA"

if [ $? -eq 0 ]; then
    echo "✅ Message sent successfully!"
    echo ""
    echo "📊 Now check:"
    echo "  1. Backend logs: docker logs vibraguard-prod_backend_1 --tail 20"
    echo "  2. Frontend console: Should receive alert in /topic/alerts"
    echo "  3. Database: SELECT * FROM vibration_events ORDER BY timestamp DESC LIMIT 1;"
else
    echo "❌ Failed to send message"
    exit 1
fi

echo ""
echo "🔍 Fetching latest event from API..."
curl -s "http://$MQTT_HOST:8080/api/events" | python -m json.tool | tail -20

echo ""
echo "✅ Test complete!"
