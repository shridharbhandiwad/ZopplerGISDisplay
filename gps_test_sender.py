#!/usr/bin/env python3
"""
GPS Test Data Sender
Sends test GPS data via UDP to the Qt application for testing purposes.
"""

import socket
import json
import time
import math
import argparse
from datetime import datetime

def send_json_gps_data(host, port, track_id, lat, lon, alt, heading=0.0, velocity=0.0, identity=1):
    """Send GPS data as JSON via UDP"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    gps_data = {
        "msgId": 1,
        "trackId": track_id,
        "lat": lat,
        "lon": lon,
        "alt": alt,
        "heading": heading,
        "velocity": velocity,
        "identity": identity
    }
    
    json_data = json.dumps(gps_data)
    sock.sendto(json_data.encode('utf-8'), (host, port))
    sock.close()
    
    print(f"[{datetime.now().strftime('%H:%M:%S')}] Sent GPS data: Track {track_id} at ({lat:.6f}, {lon:.6f}, {alt:.1f}m)")

def simulate_moving_track(host, port, track_id, start_lat, start_lon, start_alt, 
                         heading, velocity_kmh, duration_seconds, update_interval=1.0):
    """Simulate a moving GPS track"""
    
    # Convert velocity from km/h to m/s
    velocity_ms = velocity_kmh / 3.6
    
    # Calculate movement per update
    distance_per_update = velocity_ms * update_interval
    
    # Convert heading to radians (0° = North, 90° = East)
    heading_rad = math.radians(heading)
    
    # Approximate conversion: 1 degree latitude ≈ 111,320 meters
    # 1 degree longitude ≈ 111,320 * cos(latitude) meters
    lat_per_meter = 1.0 / 111320.0
    lon_per_meter = 1.0 / (111320.0 * math.cos(math.radians(start_lat)))
    
    current_lat = start_lat
    current_lon = start_lon
    current_alt = start_alt
    
    start_time = time.time()
    
    print(f"Starting simulation for Track {track_id}")
    print(f"Initial position: ({current_lat:.6f}, {current_lon:.6f}, {current_alt:.1f}m)")
    print(f"Heading: {heading}°, Velocity: {velocity_kmh} km/h")
    print(f"Duration: {duration_seconds}s, Update interval: {update_interval}s")
    print("-" * 60)
    
    while (time.time() - start_time) < duration_seconds:
        # Send current position
        send_json_gps_data(host, port, track_id, current_lat, current_lon, current_alt, 
                          heading, velocity_ms, 2)  # identity=2 (FRIEND)
        
        # Calculate next position
        # North/South movement (latitude)
        lat_delta = distance_per_update * math.cos(heading_rad) * lat_per_meter
        # East/West movement (longitude)
        lon_delta = distance_per_update * math.sin(heading_rad) * lon_per_meter
        
        current_lat += lat_delta
        current_lon += lon_delta
        
        time.sleep(update_interval)
    
    print(f"Simulation completed for Track {track_id}")
    print(f"Final position: ({current_lat:.6f}, {current_lon:.6f}, {current_alt:.1f}m)")

def send_static_test_data(host, port):
    """Send some static test GPS positions"""
    test_positions = [
        {"track_id": 1, "lat": 13.2716, "lon": 77.2946, "alt": 915.0, "heading": 45.0, "velocity": 25.0, "identity": 2},  # Bangalore
        {"track_id": 2, "lat": 13.2800, "lon": 77.3000, "alt": 920.0, "heading": 180.0, "velocity": 15.0, "identity": 3}, # North of Bangalore
        {"track_id": 3, "lat": 13.2600, "lon": 77.2800, "alt": 910.0, "heading": 90.0, "velocity": 30.0, "identity": 1},  # South-West of Bangalore
    ]
    
    print("Sending static test GPS data...")
    for pos in test_positions:
        send_json_gps_data(host, port, **pos)
        time.sleep(0.5)

def main():
    parser = argparse.ArgumentParser(description='GPS Test Data Sender for Qt QGIS Application')
    parser.add_argument('--host', default='127.0.0.1', help='Target host (default: 127.0.0.1)')
    parser.add_argument('--port', type=int, default=2026, help='Target port (default: 2026)')
    parser.add_argument('--mode', choices=['static', 'moving'], default='static', 
                       help='Test mode: static positions or moving simulation')
    
    # Moving simulation parameters
    parser.add_argument('--track-id', type=int, default=100, help='Track ID for moving simulation')
    parser.add_argument('--start-lat', type=float, default=13.2716, help='Starting latitude')
    parser.add_argument('--start-lon', type=float, default=77.2946, help='Starting longitude')
    parser.add_argument('--start-alt', type=float, default=915.0, help='Starting altitude (meters)')
    parser.add_argument('--heading', type=float, default=45.0, help='Heading in degrees (0=North, 90=East)')
    parser.add_argument('--velocity', type=float, default=50.0, help='Velocity in km/h')
    parser.add_argument('--duration', type=int, default=60, help='Simulation duration in seconds')
    parser.add_argument('--interval', type=float, default=1.0, help='Update interval in seconds')
    
    args = parser.parse_args()
    
    print(f"GPS Test Sender - Target: {args.host}:{args.port}")
    print(f"Mode: {args.mode}")
    print("=" * 50)
    
    try:
        if args.mode == 'static':
            send_static_test_data(args.host, args.port)
        else:  # moving
            simulate_moving_track(args.host, args.port, args.track_id, 
                                args.start_lat, args.start_lon, args.start_alt,
                                args.heading, args.velocity, args.duration, args.interval)
    except KeyboardInterrupt:
        print("\nStopped by user")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()