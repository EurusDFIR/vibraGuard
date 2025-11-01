#!/usr/bin/env python3
"""
Test PostgreSQL Connection to GCP
Quick script to verify database connectivity before using DBeaver
"""

import psycopg2
from datetime import datetime

# Connection parameters
DB_CONFIG = {
    'host': '34.87.133.103',
    'port': 5432,
    'database': 'vibraguard_db',
    'user': 'vibraguard_user',
    'password': 'vibraguard_pass'
}

def test_connection():
    """Test basic database connection"""
    print("=" * 60)
    print("🔌 Testing PostgreSQL Connection to GCP")
    print("=" * 60)
    print(f"Host: {DB_CONFIG['host']}")
    print(f"Port: {DB_CONFIG['port']}")
    print(f"Database: {DB_CONFIG['database']}")
    print(f"User: {DB_CONFIG['user']}")
    print("-" * 60)
    
    try:
        # Connect to database
        print("\n⏳ Connecting...")
        conn = psycopg2.connect(**DB_CONFIG)
        print("✅ Connection successful!")
        
        # Create cursor
        cursor = conn.cursor()
        
        # Test 1: Check PostgreSQL version
        print("\n📊 PostgreSQL Version:")
        cursor.execute("SELECT version();")
        version = cursor.fetchone()[0]
        print(f"   {version[:50]}...")
        
        # Test 2: List tables
        print("\n📋 Tables in database:")
        cursor.execute("""
            SELECT table_name 
            FROM information_schema.tables 
            WHERE table_schema = 'public'
            ORDER BY table_name;
        """)
        tables = cursor.fetchall()
        for table in tables:
            print(f"   ✓ {table[0]}")
        
        # Test 3: Count devices
        print("\n🖥️  Devices:")
        cursor.execute("SELECT COUNT(*) FROM devices;")
        device_count = cursor.fetchone()[0]
        print(f"   Total: {device_count}")
        
        cursor.execute("SELECT id, device_name, location, is_armed FROM devices;")
        devices = cursor.fetchall()
        for dev in devices:
            armed_status = "🔴 ARMED" if dev[3] else "🟢 DISARMED"
            print(f"   [{dev[0]}] {dev[1]} - {dev[2]} - {armed_status}")
        
        # Test 4: Count events
        print("\n🚨 Vibration Events:")
        cursor.execute("SELECT COUNT(*) FROM vibration_events;")
        event_count = cursor.fetchone()[0]
        print(f"   Total events: {event_count}")
        
        # Test 5: Latest 5 events
        print("\n📅 Latest 5 Events:")
        cursor.execute("""
            SELECT 
                id,
                event_timestamp,
                notes,
                severity
            FROM vibration_events
            ORDER BY event_timestamp DESC
            LIMIT 5;
        """)
        events = cursor.fetchall()
        for event in events:
            timestamp_str = event[1].strftime("%Y-%m-%d %H:%M:%S")
            print(f"   [{event[0]}] {timestamp_str} - {event[3]}")
            print(f"        {event[2]}")
        
        # Test 6: Extract confidence statistics
        print("\n📈 Attack Confidence Statistics:")
        cursor.execute("""
            SELECT
                COUNT(*) AS total_attacks,
                ROUND(AVG(CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC)), 2) AS avg_confidence,
                MAX(CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC)) AS max_confidence,
                MIN(CAST(SUBSTRING(notes FROM 'Confidence: ([0-9.]+)%') AS NUMERIC)) AS min_confidence
            FROM vibration_events
            WHERE notes LIKE '%AI Attack Detected%';
        """)
        stats = cursor.fetchone()
        print(f"   Total attacks: {stats[0]}")
        print(f"   Avg confidence: {stats[1]}%")
        print(f"   Max confidence: {stats[2]}%")
        print(f"   Min confidence: {stats[3]}%")
        
        # Close connection
        cursor.close()
        conn.close()
        
        print("\n" + "=" * 60)
        print("✅ All tests passed! Database is working correctly.")
        print("=" * 60)
        print("\n💡 You can now connect DBeaver with these credentials.")
        
        return True
        
    except psycopg2.OperationalError as e:
        print(f"\n❌ Connection failed!")
        print(f"   Error: {e}")
        print("\n🔧 Troubleshooting:")
        print("   1. Check if firewall port 5432 is open on GCP")
        print("   2. Verify PostgreSQL container is running:")
        print("      gcloud compute ssh vibraguard-server --zone=asia-southeast1-b \\")
        print("        --command='docker ps | grep postgres'")
        print("   3. Check credentials in docker-compose.yml")
        return False
        
    except Exception as e:
        print(f"\n❌ Unexpected error: {e}")
        return False

if __name__ == "__main__":
    import sys
    
    # Check if psycopg2 is installed
    try:
        import psycopg2
    except ImportError:
        print("❌ psycopg2 not installed!")
        print("\n📦 Install it with:")
        print("   pip install psycopg2-binary")
        sys.exit(1)
    
    # Run test
    success = test_connection()
    sys.exit(0 if success else 1)
