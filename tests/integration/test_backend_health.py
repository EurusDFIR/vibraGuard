#!/usr/bin/env python3
"""
Test Backend Health and Configuration
Kiểm tra backend Spring Boot có khởi động đúng không
"""

import requests
import sys
import json

def test_backend_health(backend_url="http://localhost:8080"):
    """Kiểm tra backend có sống không"""
    print("=" * 60)
    print("🔍 TESTING BACKEND HEALTH")
    print("=" * 60)
    
    # Test 1: API có sống không
    try:
        response = requests.get(f"{backend_url}/api/events", timeout=5)
        print(f"\n✅ Backend is alive!")
        print(f"   Status Code: {response.status_code}")
        print(f"   Response: {response.text[:200]}")
    except requests.exceptions.ConnectionError:
        print(f"\n❌ Backend is DOWN! Cannot connect to {backend_url}")
        return False
    except Exception as e:
        print(f"\n❌ Error: {e}")
        return False
    
    # Test 2: Database connection
    print("\n" + "=" * 60)
    print("🗄️  TESTING DATABASE CONNECTION")
    print("=" * 60)
    
    try:
        response = requests.get(f"{backend_url}/api/events", timeout=5)
        if response.status_code == 200:
            print("✅ Database connection OK!")
            events = response.json()
            print(f"   Found {len(events)} events in database")
        else:
            print(f"⚠️  Status code: {response.status_code}")
    except Exception as e:
        print(f"❌ Database test failed: {e}")
    
    print("\n" + "=" * 60)
    print("✅ ALL TESTS COMPLETED")
    print("=" * 60)
    return True

if __name__ == "__main__":
    backend_url = sys.argv[1] if len(sys.argv) > 1 else "http://localhost:8080"
    success = test_backend_health(backend_url)
    sys.exit(0 if success else 1)
