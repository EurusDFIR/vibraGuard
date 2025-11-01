# Contributing to VibraGuard AI

Thank you for your interest in contributing to VibraGuard AI! 🎉

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Commit Message Guidelines](#commit-message-guidelines)

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues. When creating a bug report, include:

- **Clear title and description**
- **Steps to reproduce** the behavior
- **Expected vs actual behavior**
- **Environment details** (OS, hardware, versions)
- **Logs and screenshots** if applicable

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When suggesting:

- **Use a clear title**
- **Provide detailed description** of the enhancement
- **Explain why this would be useful** to most users
- **List similar features** in other projects if applicable

### Code Contributions

1. **Fork** the repository
2. **Create a branch** (`git checkout -b feature/amazing-feature`)
3. **Make your changes**
4. **Test thoroughly**
5. **Commit** with clear messages
6. **Push** to your branch
7. **Open a Pull Request**

## Development Setup

### Prerequisites

- **ESP32:** Arduino IDE 1.8+ or PlatformIO
- **Backend:** Java 17+, Maven 3.6+, Docker
- **Frontend:** Node.js 18+, npm 9+
- **Mobile:** Flutter 3.9+, Android Studio / Xcode
- **Cloud:** Google Cloud SDK (for GCP deployment)

### Local Development

```bash
# Clone the repository
git clone https://github.com/YourUsername/vibraGuard.git
cd vibraGuard

# Backend setup
cd backend
mvn clean install
docker-compose up -d  # Start Mosquitto & PostgreSQL

# Frontend setup
cd ../frontend
npm install
npm run dev

# Mobile setup
cd ../vibraguard_app
flutter pub get
flutter run
```

### Testing

```bash
# Run backend tests
cd backend
mvn test

# Run integration tests
cd ../tests/integration
python test_system_complete.py

# Test MQTT connectivity
cd ../tests/mqtt
python test_mqtt_python.py
```

## Pull Request Process

1. **Update documentation** if you're changing functionality
2. **Add tests** for new features
3. **Ensure all tests pass**
4. **Update the README.md** if needed
5. **Follow the coding standards** below
6. **Request review** from maintainers

### PR Checklist

- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex logic
- [ ] Documentation updated
- [ ] Tests added/updated
- [ ] All tests pass
- [ ] No merge conflicts

## Coding Standards

### Java (Backend)

```java
// Use Spring Boot best practices
@RestController
@RequestMapping("/api/v1/devices")
public class DeviceController {
    // Clear naming, proper annotations
    @GetMapping("/{id}")
    public ResponseEntity<Device> getDevice(@PathVariable Long id) {
        // Implementation
    }
}
```

### JavaScript/React (Frontend)

```javascript
// Use functional components with hooks
const Dashboard = () => {
  const [devices, setDevices] = useState([]);

  useEffect(() => {
    fetchDevices();
  }, []);

  return <div>...</div>;
};
```

### Dart/Flutter (Mobile)

```dart
// Follow Flutter style guide
class DeviceScreen extends StatefulWidget {
  const DeviceScreen({Key? key}) : super(key: key);

  @override
  State<DeviceScreen> createState() => _DeviceScreenState();
}
```

### C++ (ESP32)

```cpp
// Use clear variable names, document hardware pins
const int MPU_SDA = 8;  // GP8
const int MPU_SCL = 9;  // GP9

void setup() {
  // Initialization code
}
```

### General Guidelines

- **Naming:** Use descriptive names (avoid `a`, `b`, `tmp`)
- **Functions:** Single responsibility, max 50 lines
- **Comments:** Explain _why_, not _what_
- **Magic numbers:** Use constants
- **Error handling:** Always handle errors gracefully

## Commit Message Guidelines

We follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <subject>

[optional body]

[optional footer]
```

### Types

- **feat:** New feature
- **fix:** Bug fix
- **docs:** Documentation only
- **style:** Formatting, missing semicolons, etc.
- **refactor:** Code restructuring
- **perf:** Performance improvement
- **test:** Adding tests
- **chore:** Maintenance tasks

### Examples

```
feat(esp32): add auto-reconnect for MQTT
fix(backend): resolve null pointer in event handler
docs(readme): update installation instructions
refactor(frontend): extract Alert component
test(integration): add E2E test for attack detection
```

## Branch Naming

- **feature/\*** - New features (`feature/wifi-manager`)
- **fix/\*** - Bug fixes (`fix/mqtt-reconnect`)
- **docs/\*** - Documentation (`docs/api-guide`)
- **refactor/\*** - Code refactoring (`refactor/event-service`)
- **test/\*** - Testing (`test/integration-suite`)

## Documentation

- Update `README.md` for user-facing changes
- Update `docs/` for detailed documentation
- Add inline comments for complex algorithms
- Document all public APIs

## Testing Requirements

### Unit Tests

- Cover all business logic
- Mock external dependencies
- Aim for >80% coverage

### Integration Tests

- Test end-to-end flows
- Verify MQTT, WebSocket, Database interactions
- Test error scenarios

### Hardware Tests

- Test on actual ESP32 devices
- Verify sensor readings
- Test under various conditions

## Review Process

1. **Automated checks** must pass (CI/CD)
2. **At least one maintainer** must approve
3. **All conversations** must be resolved
4. **Squash commits** before merging

## Questions?

- **Discord:** [Join our server](https://discord.gg/vibraguard)
- **Email:** vibraguard@example.com
- **Issues:** [GitHub Issues](https://github.com/YourUsername/vibraGuard/issues)

---

Thank you for contributing to VibraGuard AI! 🚀
