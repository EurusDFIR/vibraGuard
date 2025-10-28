import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
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
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  static const String backendUrl = 'http://localhost:8080';

  List<Map<String, dynamic>> history = [];
  String systemStatus = 'UNKNOWN';

  @override
  void initState() {
    super.initState();
    _fetchHistory();
    _fetchSystemStatus();
  }

  Future<void> _fetchHistory() async {
    try {
      final response = await http.get(Uri.parse('$backendUrl/api/events'));
      if (response.statusCode == 200) {
        final List<dynamic> data = jsonDecode(response.body);
        setState(() {
          history = data.cast<Map<String, dynamic>>();
        });
      }
    } catch (e) {
      print('Error: $e');
    }
  }

  Future<void> _fetchSystemStatus() async {
    try {
      final response = await http.get(
        Uri.parse('$backendUrl/api/control/status'),
      );
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        setState(() {
          systemStatus = data['systemStatus'] ?? 'UNKNOWN';
        });
      }
    } catch (e) {
      print('Error: $e');
    }
  }

  Future<void> _sendArmCommand() async {
    try {
      await http.post(Uri.parse('$backendUrl/api/control/arm'));
      setState(() => systemStatus = 'ARMED');
      _showSnackBar('System armed!');
    } catch (e) {
      _showSnackBar('Failed: $e');
    }
  }

  Future<void> _sendDisarmCommand() async {
    try {
      await http.post(Uri.parse('$backendUrl/api/control/disarm'));
      setState(() => systemStatus = 'DISARMED');
      _showSnackBar('System disarmed!');
    } catch (e) {
      _showSnackBar('Failed: $e');
    }
  }

  Future<void> _sendPanicCommand() async {
    try {
      await http.post(Uri.parse('$backendUrl/api/control/sound_alarm'));
      _showSnackBar('Panic alarm activated!');
    } catch (e) {
      _showSnackBar('Failed: $e');
    }
  }

  void _showSnackBar(String message) {
    ScaffoldMessenger.of(
      context,
    ).showSnackBar(SnackBar(content: Text(message)));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('VibraGuard'),
        actions: [
          Padding(
            padding: const EdgeInsets.all(16.0),
            child: Center(
              child: Text(
                systemStatus == 'ARMED' ? 'ARMED' : 'DISARMED',
                style: TextStyle(fontWeight: FontWeight.bold),
              ),
            ),
          ),
        ],
      ),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          Card(
            child: Padding(
              padding: const EdgeInsets.all(16),
              child: Column(
                children: [
                  Row(
                    children: [
                      Expanded(
                        child: ElevatedButton(
                          onPressed: _sendArmCommand,
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.orange,
                          ),
                          child: const Text('ARM'),
                        ),
                      ),
                      const SizedBox(width: 12),
                      Expanded(
                        child: ElevatedButton(
                          onPressed: _sendDisarmCommand,
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.green,
                          ),
                          child: const Text('DISARM'),
                        ),
                      ),
                    ],
                  ),
                  const SizedBox(height: 12),
                  SizedBox(
                    width: double.infinity,
                    child: ElevatedButton(
                      onPressed: _sendPanicCommand,
                      style: ElevatedButton.styleFrom(
                        backgroundColor: Colors.red,
                      ),
                      child: const Text('PANIC BUTTON'),
                    ),
                  ),
                ],
              ),
            ),
          ),
          const SizedBox(height: 20),
          Card(
            child: Padding(
              padding: const EdgeInsets.all(16),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    'Event History (${history.length})',
                    style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
                  ),
                  const SizedBox(height: 16),
                  history.isEmpty
                      ? const Center(child: Text('No events'))
                      : Column(
                          children: history.map((event) {
                            return ListTile(
                              title: Text(event['deviceName'] ?? 'Unknown'),
                              subtitle: Text('Sensor: ${event['sensorValue']}'),
                              trailing: Text(event['severity'] ?? 'N/A'),
                            );
                          }).toList(),
                        ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}
