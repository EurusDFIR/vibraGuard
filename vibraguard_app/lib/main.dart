import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'dart:async';

void main() {
  runApp(const VibraGuardApp());
}

class VibraGuardApp extends StatelessWidget {
  const VibraGuardApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'VibraGuard AI',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        primarySwatch: Colors.blue,
        brightness: Brightness.light,
        scaffoldBackgroundColor: const Color(0xFFF5F7FA),
        fontFamily: 'Segoe UI',
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

class _HomePageState extends State<HomePage>
    with SingleTickerProviderStateMixin {
  static const String backendUrl = 'http://10.0.2.2:8080';

  List<Map<String, dynamic>> history = [];
  String systemStatus = 'UNKNOWN';
  int devicesCount = 0;
  bool isConnected = false;
  int currentTab = 0;
  Timer? _statusTimer;
  late AnimationController _pulseController;

  // Statistics
  int totalEvents = 0;
  int todayEvents = 0;
  int criticalEvents = 0;
  double avgConfidence = 0.0;

  @override
  void initState() {
    super.initState();
    _pulseController = AnimationController(
      vsync: this,
      duration: const Duration(seconds: 2),
    )..repeat();

    _fetchHistory();
    _fetchSystemStatus();

    _statusTimer = Timer.periodic(const Duration(seconds: 5), (timer) {
      _fetchSystemStatus();
    });
  }

  @override
  void dispose() {
    _statusTimer?.cancel();
    _pulseController.dispose();
    super.dispose();
  }

  Future<void> _fetchHistory() async {
    try {
      final response = await http.get(Uri.parse('$backendUrl/api/events'));
      if (response.statusCode == 200) {
        final List<dynamic> data = jsonDecode(response.body);
        setState(() {
          history = data.cast<Map<String, dynamic>>();
          isConnected = true;
          _calculateStats();
        });
      }
    } catch (e) {
      setState(() => isConnected = false);
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
          devicesCount = data['devicesCount'] ?? 0;
          isConnected = true;
        });
      }
    } catch (e) {
      setState(() => isConnected = false);
      print('Error: $e');
    }
  }

  void _calculateStats() {
    totalEvents = history.length;

    final today = DateTime.now();
    todayEvents = history.where((event) {
      try {
        final eventDate = DateTime.parse(event['eventTimestamp'] ?? '');
        return eventDate.year == today.year &&
            eventDate.month == today.month &&
            eventDate.day == today.day;
      } catch (e) {
        return false;
      }
    }).length;

    criticalEvents = history.where((event) {
      final severity = event['severity'] ?? '';
      return severity == 'CRITICAL' || severity == 'WARNING';
    }).length;

    if (history.isNotEmpty) {
      double totalConf = 0;
      int count = 0;
      for (var event in history) {
        if (event['confidence'] != null) {
          totalConf += (event['confidence'] as num).toDouble();
          count++;
        }
      }
      avgConfidence = count > 0 ? totalConf / count : 0;
    }
  }

  Future<void> _sendArmCommand() async {
    try {
      await http.post(Uri.parse('$backendUrl/api/control/arm'));
      await _fetchSystemStatus();
      _showSnackBar('✅ System ARMED', Colors.red);
    } catch (e) {
      _showSnackBar('❌ Failed: $e', Colors.red);
    }
  }

  Future<void> _sendDisarmCommand() async {
    try {
      await http.post(Uri.parse('$backendUrl/api/control/disarm'));
      await _fetchSystemStatus();
      _showSnackBar('✅ System DISARMED', Colors.green);
    } catch (e) {
      _showSnackBar('❌ Failed: $e', Colors.red);
    }
  }

  Future<void> _sendPanicCommand() async {
    try {
      await http.post(Uri.parse('$backendUrl/api/control/sound_alarm'));
      _showSnackBar('🚨 PANIC ALARM ACTIVATED!', Colors.orange);
    } catch (e) {
      _showSnackBar('❌ Failed: $e', Colors.red);
    }
  }

  void _showSnackBar(String message, Color color) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text(
          message,
          style: const TextStyle(fontWeight: FontWeight.bold),
        ),
        backgroundColor: color,
        behavior: SnackBarBehavior.floating,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
            colors: [Color(0xFF667EEA), Color(0xFF764BA2)],
          ),
        ),
        child: SafeArea(
          child: Column(
            children: [
              _buildHeader(),
              _buildTabBar(),
              Expanded(
                child: Container(
                  margin: const EdgeInsets.all(16),
                  decoration: BoxDecoration(
                    color: Colors.white,
                    borderRadius: BorderRadius.circular(20),
                    boxShadow: [
                      BoxShadow(
                        color: Colors.black.withOpacity(0.1),
                        blurRadius: 20,
                        offset: const Offset(0, 10),
                      ),
                    ],
                  ),
                  child: _buildCurrentTab(),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }

  Widget _buildHeader() {
    return Container(
      padding: const EdgeInsets.all(20),
      child: Column(
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Row(
                    children: const [
                      Text(
                        '🛡️ VibraGuard AI',
                        style: TextStyle(
                          fontSize: 28,
                          fontWeight: FontWeight.bold,
                          color: Colors.white,
                        ),
                      ),
                    ],
                  ),
                  const SizedBox(height: 4),
                  const Text(
                    'Smart Security Monitoring',
                    style: TextStyle(fontSize: 14, color: Colors.white70),
                  ),
                ],
              ),
              Column(
                crossAxisAlignment: CrossAxisAlignment.end,
                children: [
                  _buildConnectionBadge(),
                  const SizedBox(height: 8),
                  _buildStatusBadge(),
                ],
              ),
            ],
          ),
        ],
      ),
    );
  }

  Widget _buildConnectionBadge() {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
      decoration: BoxDecoration(
        color: isConnected
            ? Colors.green.withOpacity(0.9)
            : Colors.red.withOpacity(0.9),
        borderRadius: BorderRadius.circular(20),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          AnimatedBuilder(
            animation: _pulseController,
            builder: (context, child) {
              return Container(
                width: 8,
                height: 8,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: Colors.white,
                  boxShadow: [
                    BoxShadow(
                      color: Colors.white.withOpacity(_pulseController.value),
                      blurRadius: 8,
                      spreadRadius: 2,
                    ),
                  ],
                ),
              );
            },
          ),
          const SizedBox(width: 8),
          Text(
            isConnected ? 'Connected' : 'Disconnected',
            style: const TextStyle(
              color: Colors.white,
              fontWeight: FontWeight.bold,
              fontSize: 12,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildStatusBadge() {
    final isArmed = systemStatus == 'ARMED';
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
      decoration: BoxDecoration(
        color: isArmed ? Colors.red : Colors.green,
        borderRadius: BorderRadius.circular(20),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Text(isArmed ? '🔔' : '🔕', style: const TextStyle(fontSize: 14)),
          const SizedBox(width: 6),
          Text(
            systemStatus,
            style: const TextStyle(
              color: Colors.white,
              fontWeight: FontWeight.bold,
              fontSize: 12,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildTabBar() {
    return Container(
      margin: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
      padding: const EdgeInsets.all(4),
      decoration: BoxDecoration(
        color: Colors.white.withOpacity(0.2),
        borderRadius: BorderRadius.circular(16),
      ),
      child: Row(
        children: [
          _buildTab('📊', 'Dashboard', 0),
          _buildTab('📜', 'Events', 1),
          _buildTab('⚙️', 'Control', 2),
        ],
      ),
    );
  }

  Widget _buildTab(String icon, String label, int index) {
    final isSelected = currentTab == index;
    return Expanded(
      child: GestureDetector(
        onTap: () => setState(() => currentTab = index),
        child: Container(
          padding: const EdgeInsets.symmetric(vertical: 12),
          decoration: BoxDecoration(
            gradient: isSelected
                ? const LinearGradient(
                    colors: [Color(0xFF667EEA), Color(0xFF764BA2)],
                  )
                : null,
            borderRadius: BorderRadius.circular(12),
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(icon, style: const TextStyle(fontSize: 16)),
              const SizedBox(width: 6),
              Text(
                label,
                style: TextStyle(
                  color: isSelected ? Colors.white : Colors.white70,
                  fontWeight: isSelected ? FontWeight.bold : FontWeight.normal,
                  fontSize: 13,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }

  Widget _buildCurrentTab() {
    switch (currentTab) {
      case 0:
        return _buildDashboard();
      case 1:
        return _buildEventsView();
      case 2:
        return _buildControlPanel();
      default:
        return _buildDashboard();
    }
  }

  Widget _buildDashboard() {
    return RefreshIndicator(
      onRefresh: () async {
        await _fetchHistory();
        await _fetchSystemStatus();
      },
      child: ListView(
        padding: const EdgeInsets.all(20),
        children: [
          const Text(
            '📊 Dashboard Overview',
            style: TextStyle(
              fontSize: 24,
              fontWeight: FontWeight.bold,
              color: Color(0xFF2F3542),
            ),
          ),
          const SizedBox(height: 20),
          _buildStatsGrid(),
          const SizedBox(height: 24),
          const Text(
            '🚨 Recent Alerts',
            style: TextStyle(
              fontSize: 20,
              fontWeight: FontWeight.bold,
              color: Color(0xFF2F3542),
            ),
          ),
          const SizedBox(height: 12),
          _buildRecentAlerts(),
        ],
      ),
    );
  }

  Widget _buildStatsGrid() {
    return GridView.count(
      crossAxisCount: 2,
      shrinkWrap: true,
      physics: const NeverScrollableScrollPhysics(),
      mainAxisSpacing: 16,
      crossAxisSpacing: 16,
      childAspectRatio: 1.3,
      children: [
        _buildStatCard(
          '📊',
          'Total Events',
          totalEvents.toString(),
          Colors.blue,
        ),
        _buildStatCard(
          '📅',
          "Today's Events",
          todayEvents.toString(),
          Colors.purple,
        ),
        _buildStatCard(
          '⚠️',
          'Critical Alerts',
          criticalEvents.toString(),
          Colors.red,
        ),
        _buildStatCard(
          '🎯',
          'Avg Confidence',
          '${(avgConfidence * 100).toStringAsFixed(1)}%',
          Colors.green,
        ),
      ],
    );
  }

  Widget _buildStatCard(String icon, String label, String value, Color color) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        gradient: LinearGradient(
          begin: Alignment.topLeft,
          end: Alignment.bottomRight,
          colors: [color.withOpacity(0.7), color],
        ),
        borderRadius: BorderRadius.circular(16),
        boxShadow: [
          BoxShadow(
            color: color.withOpacity(0.3),
            blurRadius: 8,
            offset: const Offset(0, 4),
          ),
        ],
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(icon, style: const TextStyle(fontSize: 32)),
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(
                value,
                style: const TextStyle(
                  fontSize: 28,
                  fontWeight: FontWeight.bold,
                  color: Colors.white,
                ),
              ),
              const SizedBox(height: 4),
              Text(
                label,
                style: const TextStyle(fontSize: 12, color: Colors.white70),
              ),
            ],
          ),
        ],
      ),
    );
  }

  Widget _buildRecentAlerts() {
    final recentEvents = history.take(5).toList();

    if (recentEvents.isEmpty) {
      return Container(
        padding: const EdgeInsets.all(40),
        child: const Center(
          child: Text(
            'No recent alerts',
            style: TextStyle(color: Colors.grey, fontStyle: FontStyle.italic),
          ),
        ),
      );
    }

    return Column(
      children: recentEvents.map((event) => _buildAlertItem(event)).toList(),
    );
  }

  Widget _buildAlertItem(Map<String, dynamic> event) {
    final severity = event['severity'] ?? 'INFO';
    Color alertColor;

    switch (severity) {
      case 'CRITICAL':
        alertColor = Colors.red;
        break;
      case 'WARNING':
        alertColor = Colors.orange;
        break;
      default:
        alertColor = Colors.blue;
    }

    return Container(
      margin: const EdgeInsets.only(bottom: 12),
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        gradient: LinearGradient(
          colors: [alertColor.withOpacity(0.8), alertColor],
        ),
        borderRadius: BorderRadius.circular(12),
        boxShadow: [
          BoxShadow(
            color: alertColor.withOpacity(0.3),
            blurRadius: 8,
            offset: const Offset(0, 4),
          ),
        ],
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                event['deviceName'] ?? event['deviceId'] ?? 'Unknown Device',
                style: const TextStyle(
                  color: Colors.white,
                  fontSize: 16,
                  fontWeight: FontWeight.bold,
                ),
              ),
              Container(
                padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                decoration: BoxDecoration(
                  color: Colors.white.withOpacity(0.3),
                  borderRadius: BorderRadius.circular(8),
                ),
                child: Text(
                  severity,
                  style: const TextStyle(
                    color: Colors.white,
                    fontSize: 12,
                    fontWeight: FontWeight.bold,
                  ),
                ),
              ),
            ],
          ),
          const SizedBox(height: 8),
          if (event['confidence'] != null)
            Text(
              'Confidence: ${((event['confidence'] as num) * 100).toStringAsFixed(1)}%',
              style: const TextStyle(color: Colors.white70, fontSize: 12),
            ),
          if (event['eventTimestamp'] != null)
            Text(
              _formatTimestamp(event['eventTimestamp']),
              style: const TextStyle(color: Colors.white60, fontSize: 11),
            ),
        ],
      ),
    );
  }

  Widget _buildEventsView() {
    return RefreshIndicator(
      onRefresh: _fetchHistory,
      child: Column(
        children: [
          Container(
            padding: const EdgeInsets.all(20),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Text(
                  '📜 Events History (${history.length})',
                  style: const TextStyle(
                    fontSize: 20,
                    fontWeight: FontWeight.bold,
                    color: Color(0xFF2F3542),
                  ),
                ),
                IconButton(
                  icon: const Icon(Icons.refresh, color: Color(0xFF667EEA)),
                  onPressed: _fetchHistory,
                ),
              ],
            ),
          ),
          Expanded(
            child: history.isEmpty
                ? const Center(
                    child: Text(
                      'No events recorded',
                      style: TextStyle(
                        color: Colors.grey,
                        fontStyle: FontStyle.italic,
                      ),
                    ),
                  )
                : ListView.builder(
                    padding: const EdgeInsets.symmetric(horizontal: 20),
                    itemCount: history.length,
                    itemBuilder: (context, index) {
                      final event = history[index];
                      return _buildEventCard(event);
                    },
                  ),
          ),
        ],
      ),
    );
  }

  Widget _buildEventCard(Map<String, dynamic> event) {
    final severity = event['severity'] ?? 'INFO';

    return Container(
      margin: const EdgeInsets.only(bottom: 12),
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: const Color(0xFFF8F9FA),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: Colors.grey.shade200),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Expanded(
                child: Text(
                  event['deviceName'] ?? event['deviceId'] ?? 'Unknown',
                  style: const TextStyle(
                    fontWeight: FontWeight.bold,
                    fontSize: 16,
                  ),
                ),
              ),
              _buildSeverityBadge(severity),
            ],
          ),
          const SizedBox(height: 8),
          if (event['sensorValue'] != null)
            Text(
              'Sensor: ${event['sensorValue']}',
              style: TextStyle(color: Colors.grey.shade600, fontSize: 13),
            ),
          if (event['confidence'] != null)
            Text(
              'Confidence: ${((event['confidence'] as num) * 100).toStringAsFixed(1)}%',
              style: TextStyle(color: Colors.grey.shade600, fontSize: 13),
            ),
          if (event['eventTimestamp'] != null)
            Text(
              _formatTimestamp(event['eventTimestamp']),
              style: TextStyle(color: Colors.grey.shade500, fontSize: 12),
            ),
        ],
      ),
    );
  }

  Widget _buildSeverityBadge(String severity) {
    Color bgColor;
    Color textColor;

    switch (severity) {
      case 'CRITICAL':
        bgColor = Colors.red.shade100;
        textColor = Colors.red.shade900;
        break;
      case 'WARNING':
        bgColor = Colors.orange.shade100;
        textColor = Colors.orange.shade900;
        break;
      default:
        bgColor = Colors.blue.shade100;
        textColor = Colors.blue.shade900;
    }

    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
      decoration: BoxDecoration(
        color: bgColor,
        borderRadius: BorderRadius.circular(12),
      ),
      child: Text(
        severity,
        style: TextStyle(
          color: textColor,
          fontSize: 12,
          fontWeight: FontWeight.bold,
        ),
      ),
    );
  }

  Widget _buildControlPanel() {
    return ListView(
      padding: const EdgeInsets.all(20),
      children: [
        const Text(
          '⚙️ System Control',
          style: TextStyle(
            fontSize: 24,
            fontWeight: FontWeight.bold,
            color: Color(0xFF2F3542),
          ),
        ),
        const SizedBox(height: 20),
        _buildStatusCard(),
        const SizedBox(height: 20),
        _buildControlButtons(),
        const SizedBox(height: 20),
        _buildQuickStats(),
      ],
    );
  }

  Widget _buildStatusCard() {
    final isArmed = systemStatus == 'ARMED';

    return Container(
      padding: const EdgeInsets.all(24),
      decoration: BoxDecoration(
        gradient: LinearGradient(
          colors: isArmed
              ? [Colors.red.shade400, Colors.red.shade600]
              : [Colors.green.shade400, Colors.green.shade600],
        ),
        borderRadius: BorderRadius.circular(16),
        boxShadow: [
          BoxShadow(
            color: (isArmed ? Colors.red : Colors.green).withOpacity(0.3),
            blurRadius: 12,
            offset: const Offset(0, 6),
          ),
        ],
      ),
      child: Column(
        children: [
          Text(isArmed ? '🔔' : '🔕', style: const TextStyle(fontSize: 48)),
          const SizedBox(height: 12),
          Text(
            systemStatus,
            style: const TextStyle(
              fontSize: 28,
              fontWeight: FontWeight.bold,
              color: Colors.white,
            ),
          ),
          const SizedBox(height: 8),
          Text(
            'Connected Devices: $devicesCount',
            style: const TextStyle(fontSize: 16, color: Colors.white70),
          ),
        ],
      ),
    );
  }

  Widget _buildControlButtons() {
    return Column(
      children: [
        _buildGradientButton('🔔 ARM System', const [
          Color(0xFFFF6B6B),
          Color(0xFFEE5A6F),
        ], _sendArmCommand),
        const SizedBox(height: 12),
        _buildGradientButton('🔕 DISARM System', const [
          Color(0xFF2ED573),
          Color(0xFF26D08A),
        ], _sendDisarmCommand),
        const SizedBox(height: 12),
        _buildGradientButton('🚨 PANIC ALARM', const [
          Color(0xFFF093FB),
          Color(0xFFF5576C),
        ], _sendPanicCommand),
      ],
    );
  }

  Widget _buildGradientButton(
    String text,
    List<Color> colors,
    VoidCallback onPressed,
  ) {
    return Container(
      width: double.infinity,
      height: 56,
      decoration: BoxDecoration(
        gradient: LinearGradient(colors: colors),
        borderRadius: BorderRadius.circular(16),
        boxShadow: [
          BoxShadow(
            color: colors[0].withOpacity(0.3),
            blurRadius: 8,
            offset: const Offset(0, 4),
          ),
        ],
      ),
      child: Material(
        color: Colors.transparent,
        child: InkWell(
          onTap: onPressed,
          borderRadius: BorderRadius.circular(16),
          child: Center(
            child: Text(
              text,
              style: const TextStyle(
                color: Colors.white,
                fontSize: 18,
                fontWeight: FontWeight.bold,
              ),
            ),
          ),
        ),
      ),
    );
  }

  Widget _buildQuickStats() {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: const Color(0xFFF8F9FA),
        borderRadius: BorderRadius.circular(16),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text(
            '📊 Quick Stats',
            style: TextStyle(
              fontSize: 18,
              fontWeight: FontWeight.bold,
              color: Color(0xFF2F3542),
            ),
          ),
          const SizedBox(height: 16),
          _buildStatRow('Total Events:', totalEvents.toString()),
          _buildStatRow('Today:', todayEvents.toString()),
          _buildStatRow('Critical:', criticalEvents.toString(), Colors.red),
          _buildStatRow('Devices:', devicesCount.toString()),
        ],
      ),
    );
  }

  Widget _buildStatRow(String label, String value, [Color? valueColor]) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 8),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(
            label,
            style: const TextStyle(fontSize: 15, color: Color(0xFF718093)),
          ),
          Text(
            value,
            style: TextStyle(
              fontSize: 16,
              fontWeight: FontWeight.bold,
              color: valueColor ?? const Color(0xFF2F3542),
            ),
          ),
        ],
      ),
    );
  }

  String _formatTimestamp(String timestamp) {
    try {
      final dt = DateTime.parse(timestamp);
      return '${dt.day.toString().padLeft(2, '0')}/${dt.month.toString().padLeft(2, '0')}/${dt.year} ${dt.hour.toString().padLeft(2, '0')}:${dt.minute.toString().padLeft(2, '0')}:${dt.second.toString().padLeft(2, '0')}';
    } catch (e) {
      return timestamp;
    }
  }
}
