import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:stomp_dart_client/stomp_dart_client.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'dart:convert';

void main() {
runApp(const VibraGuardApp());
}

class VibraGuardApp extends StatelessWidget {
const VibraGuardApp({Key? key}) : super(key: key);

@override
Widget build(BuildContext context) {
return MaterialApp(
title: 'VibraGuard Security',
theme: ThemeData(
primarySwatch: Colors.deepPurple,
brightness: Brightness.dark,
scaffoldBackgroundColor: const Color(0xFF1a1a1a),
),
home: const HomePage(),
);
}
}

class HomePage extends StatefulWidget {
const HomePage({Key? key}) : super(key: key);

@override
State<HomePage> createState() => \_HomePageState();
}

class \_HomePageState extends State<HomePage> {
// Backend configuration
static const String backendUrl = 'http://192.168.1.11:8080'; // Thay đổi IP này
static const String wsUrl = 'ws://192.168.1.11:8080/ws';

// State variables
List<Map<String, dynamic>> alerts = [];
List<Map<String, dynamic>> history = [];
String systemStatus = 'UNKNOWN';
bool isConnected = false;
StompClient? stompClient;
FlutterLocalNotificationsPlugin? flutterLocalNotificationsPlugin;

@override
void initState() {
super.initState();
\_initNotifications();
\_connectWebSocket();
\_fetchHistory();
\_fetchSystemStatus();
}

@override
void dispose() {
stompClient?.deactivate();
super.dispose();
}

// Initialize local notifications
void \_initNotifications() async {
flutterLocalNotificationsPlugin = FlutterLocalNotificationsPlugin();

    const AndroidInitializationSettings initializationSettingsAndroid =
        AndroidInitializationSettings('@mipmap/ic_launcher');

    const InitializationSettings initializationSettings =
        InitializationSettings(android: initializationSettingsAndroid);

    await flutterLocalNotificationsPlugin!.initialize(initializationSettings);

}

// Show notification
void \_showNotification(String deviceId) async {
const AndroidNotificationDetails androidPlatformChannelSpecifics =
AndroidNotificationDetails(
'vibraguard_alerts',
'VibraGuard Alerts',
channelDescription: 'Security alerts from VibraGuard system',
importance: Importance.max,
priority: Priority.high,
showWhen: true,
);

    const NotificationDetails platformChannelSpecifics =
        NotificationDetails(android: androidPlatformChannelSpecifics);

    await flutterLocalNotificationsPlugin!.show(
      0,
      '🚨 VibraGuard Alert!',
      'Device $deviceId detected suspicious vibration!',
      platformChannelSpecifics,
    );

}

// Connect to WebSocket
void \_connectWebSocket() {
print('🔌 Connecting to WebSocket: $wsUrl');

    stompClient = StompClient(
      config: StompConfig(
        url: wsUrl,
        onConnect: (StompFrame frame) {
          print('✅ WebSocket connected');
          setState(() => isConnected = true);

          // Subscribe to alerts topic
          stompClient!.subscribe(
            destination: '/topic/alerts',
            callback: (StompFrame frame) {
              if (frame.body != null) {
                final alert = jsonDecode(frame.body!);
                print('🚨 Alert received: $alert');

                setState(() {
                  alerts.insert(0, alert);
                });

                // Show push notification
                _showNotification(alert['deviceId'] ?? 'Unknown');
              }
            },
          );
        },
        onWebSocketError: (dynamic error) {
          print('❌ WebSocket error: $error');
          setState(() => isConnected = false);
        },
        onDisconnect: (StompFrame frame) {
          print('🔌 WebSocket disconnected');
          setState(() => isConnected = false);
        },
      ),
    );

    stompClient!.activate();

}

// Fetch event history
Future<void> \_fetchHistory() async {
try {
final response = await http.get(Uri.parse('$backendUrl/api/events'));
if (response.statusCode == 200) {
final List<dynamic> data = jsonDecode(response.body);
setState(() {
history = data.cast<Map<String, dynamic>>();
});
print('📜 History fetched: ${history.length} events');
}
} catch (e) {
print('❌ Error fetching history: $e');
\_showSnackBar('Failed to fetch history: $e');
}
}

// Fetch system status
Future<void> \_fetchSystemStatus() async {
try {
final response = await http.get(Uri.parse('$backendUrl/api/control/status'));
if (response.statusCode == 200) {
final data = jsonDecode(response.body);
setState(() {
systemStatus = data['systemStatus'] ?? 'UNKNOWN';
});
print('📊 Status fetched: $systemStatus');
}
} catch (e) {
print('❌ Error fetching status: $e');
}
}

// Send ARM command
Future<void> \_sendArmCommand() async {
try {
final response = await http.post(Uri.parse('$backendUrl/api/control/arm'));
if (response.statusCode == 200) {
setState(() => systemStatus = 'ARMED');
\_showSnackBar('✅ System armed successfully!');
}
} catch (e) {
\_showSnackBar('❌ Failed to arm system: $e');
}
}

// Send DISARM command
Future<void> \_sendDisarmCommand() async {
try {
final response = await http.post(Uri.parse('$backendUrl/api/control/disarm'));
if (response.statusCode == 200) {
setState(() => systemStatus = 'DISARMED');
\_showSnackBar('✅ System disarmed successfully!');
}
} catch (e) {
\_showSnackBar('❌ Failed to disarm system: $e');
}
}

void \_showSnackBar(String message) {
ScaffoldMessenger.of(context).showSnackBar(
SnackBar(content: Text(message)),
);
}

@override
Widget build(BuildContext context) {
return Scaffold(
appBar: AppBar(
title: const Text('🛡️ VibraGuard'),
centerTitle: true,
elevation: 4,
actions: [
// Connection status
Padding(
padding: const EdgeInsets.all(8.0),
child: Center(
child: Container(
padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
decoration: BoxDecoration(
color: isConnected ? Colors.green : Colors.red,
borderRadius: BorderRadius.circular(12),
),
child: Text(
isConnected ? '🟢 Connected' : '🔴 Disconnected',
style: const TextStyle(fontSize: 12, fontWeight: FontWeight.bold),
),
),
),
),
// System status
Padding(
padding: const EdgeInsets.all(8.0),
child: Center(
child: Container(
padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
decoration: BoxDecoration(
color: systemStatus == 'ARMED' ? Colors.orange : Colors.grey,
borderRadius: BorderRadius.circular(12),
),
child: Text(
systemStatus == 'ARMED' ? '🔔 ARMED' : '🔕 DISARMED',
style: const TextStyle(fontSize: 12, fontWeight: FontWeight.bold),
),
),
),
),
],
),
body: RefreshIndicator(
onRefresh: () async {
await \_fetchHistory();
await \_fetchSystemStatus();
},
child: ListView(
padding: const EdgeInsets.all(16),
children: [
// Control Panel
\_buildControlPanel(),
const SizedBox(height: 20),

            // Real-time Alerts
            _buildAlertsSection(),
            const SizedBox(height: 20),

            // Event History
            _buildHistorySection(),
          ],
        ),
      ),
    );

}

Widget \_buildControlPanel() {
return Card(
elevation: 4,
child: Padding(
padding: const EdgeInsets.all(16),
child: Column(
crossAxisAlignment: CrossAxisAlignment.start,
children: [
const Text(
'⚙️ Điều khiển',
style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
),
const SizedBox(height: 16),
Row(
children: [
Expanded(
child: ElevatedButton.icon(
onPressed: _sendArmCommand,
icon: const Icon(Icons.notifications_active),
label: const Text('ARM'),
style: ElevatedButton.styleFrom(
backgroundColor: Colors.orange,
padding: const EdgeInsets.symmetric(vertical: 16),
),
),
),
const SizedBox(width: 12),
Expanded(
child: ElevatedButton.icon(
onPressed: _sendDisarmCommand,
icon: const Icon(Icons.notifications_off),
label: const Text('DISARM'),
style: ElevatedButton.styleFrom(
backgroundColor: Colors.green,
padding: const EdgeInsets.symmetric(vertical: 16),
),
),
),
],
),
],
),
),
);
}

Widget \_buildAlertsSection() {
return Card(
elevation: 4,
child: Padding(
padding: const EdgeInsets.all(16),
child: Column(
crossAxisAlignment: CrossAxisAlignment.start,
children: [
Text(
'🚨 Cảnh báo Real-time (${alerts.length})',
              style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
            const SizedBox(height: 16),
            alerts.isEmpty
                ? const Center(
                    child: Padding(
                      padding: EdgeInsets.all(20),
                      child: Text('Không có cảnh báo mới'),
                    ),
                  )
                : Column(
                    children: alerts.take(5).map((alert) {
                      return Card(
                        color: Colors.red[700],
                        margin: const EdgeInsets.only(bottom: 8),
                        child: ListTile(
                          leading: const Icon(Icons.warning, color: Colors.white),
                          title: Text('Device: ${alert['deviceId']}'),
                          subtitle: Text(
                            'Severity: ${alert['severity']}\n'
                            'Sensor: ${alert['sensorValue']}'
                            '${alert['confidence'] != null ? "\nAI: ${(alert['confidence'] \* 100).toStringAsFixed(1)}%" : ""}',
),
trailing: Text(
\_formatTimestamp(alert['timestamp']),
style: const TextStyle(fontSize: 10),
),
),
);
}).toList(),
),
],
),
),
);
}

Widget \_buildHistorySection() {
return Card(
elevation: 4,
child: Padding(
padding: const EdgeInsets.all(16),
child: Column(
crossAxisAlignment: CrossAxisAlignment.start,
children: [
Text(
'📜 Lịch sử Sự kiện (${history.length})',
              style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
            const SizedBox(height: 16),
            history.isEmpty
                ? const Center(
                    child: Padding(
                      padding: EdgeInsets.all(20),
                      child: Text('Chưa có sự kiện nào'),
                    ),
                  )
                : Column(
                    children: history.map((event) {
                      return ListTile(
                        leading: CircleAvatar(
                          backgroundColor: Colors.deepPurple,
                          child: Text('${event['id']}'),
),
title: Text(event['deviceName'] ?? 'Unknown'),
subtitle: Text(
'${\_formatTimestamp(event['eventTimestamp'])}\n'
'Sensor: ${event['sensorValue']}',
),
trailing: Container(
padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
decoration: BoxDecoration(
color: \_getSeverityColor(event['severity']),
borderRadius: BorderRadius.circular(8),
),
child: Text(
event['severity'] ?? 'N/A',
style: const TextStyle(fontSize: 10),
),
),
);
}).toList(),
),
],
),
),
);
}

String \_formatTimestamp(dynamic timestamp) {
try {
if (timestamp is String) {
final dt = DateTime.parse(timestamp);
return '${dt.day}/${dt.month} ${dt.hour}:${dt.minute.toString().padLeft(2, '0')}';
}
return timestamp.toString();
} catch (e) {
return 'N/A';
}
}

Color \_getSeverityColor(String? severity) {
switch (severity?.toUpperCase()) {
case 'CRITICAL':
return Colors.red;
case 'WARNING':
return Colors.orange;
default:
return Colors.blue;
}
}
}
