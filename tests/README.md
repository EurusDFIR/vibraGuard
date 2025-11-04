# VibraGuard Test Suite

This directory contains test scripts for the VibraGuard system.

## Configuration

All test scripts now support environment variables for configuration. This allows you to:
- Test against different environments (local, staging, production)
- Avoid hardcoding sensitive information like database passwords
- Make tests more portable and maintainable

### Environment Variables

| Variable | Description | Default Value |
|----------|-------------|---------------|
| `MQTT_HOST` | MQTT broker hostname/IP | `34.87.133.103` |
| `MQTT_PORT` | MQTT broker port | `1883` |
| `MQTT_TOPIC` | MQTT topic for sensor data | `vibra_guard/sensor` |
| `BACKEND_URL` | Backend API URL | `http://<MQTT_HOST>:8080` |
| `GCP_IP` | GCP server IP address | `34.87.133.103` |
| `DB_HOST` | PostgreSQL database host | `34.87.133.103` |
| `DB_PORT` | PostgreSQL database port | `5432` |
| `DB_NAME` | PostgreSQL database name | `vibraguard_db` |
| `DB_USER` | PostgreSQL username | `vibraguard_user` |
| `DB_PASSWORD` | PostgreSQL password | `vibraguard_pass` |

### Usage Examples

#### Test against production (default):
```bash
python tests/mqtt/test_mqtt_python.py
```

#### Test against local development:
```bash
export MQTT_HOST=localhost
export BACKEND_URL=http://localhost:8080
python tests/mqtt/test_mqtt_python.py
```

#### Test against custom environment:
```bash
export MQTT_HOST=192.168.1.100
export MQTT_PORT=1883
export BACKEND_URL=http://192.168.1.100:8080
python tests/integration/test_system_complete.py
```

## Test Categories

### MQTT Tests (`mqtt/`)
- `test_mqtt_python.py` - Test MQTT → Backend → Database flow
- `test_mqtt.py` - Legacy MQTT test script

### Backend Tests (`backend/`)
- `test_control_command.py` - Test control commands (ARM/DISARM/PANIC)

### Integration Tests (`integration/`)
- `test_system_complete.py` - Complete system test (API, MQTT, Database)
- `test_backend_health.py` - Backend health check
- `test_db_connection.py` - Database connection test
- `test_gcp_system.py` - GCP system health check
- `test_twilio_trigger.py` - Twilio SMS/Voice alert test
- `test_twilio_on_gcp.py` - Twilio integration on GCP

## Security Best Practices

**Never commit sensitive credentials to the repository!**

For production deployments:
1. Store credentials in environment variables
2. Use `.env` files (excluded from git via `.gitignore`)
3. Use secrets management tools (AWS Secrets Manager, Google Secret Manager, etc.)

Example `.env` file (DO NOT commit this):
```bash
MQTT_HOST=your-production-server.com
DB_PASSWORD=your-secure-password
BACKEND_URL=https://your-domain.com
```

Load environment variables from `.env`:
```bash
# Using python-dotenv
pip install python-dotenv

# In your script:
from dotenv import load_dotenv
load_dotenv()
```
