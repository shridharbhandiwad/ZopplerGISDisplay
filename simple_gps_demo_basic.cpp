/**
 * Simple GPS UDP Receiver Demo (Basic Version)
 * 
 * This is a simplified console application that demonstrates the GPS UDP
 * reception functionality without external dependencies.
 * 
 * Compile with: g++ -std=c++17 simple_gps_demo_basic.cpp -o simple_gps_demo_basic -pthread
 * Run with: ./simple_gps_demo_basic
 * Test with: python3 gps_test_sender.py --mode static
 */

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>
#include <iomanip>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

// GPS data structure matching the Qt application
#pragma pack(1)
struct GpsData {
    unsigned short msgId;
    int trackId;
    double lat;
    double lon;
    double alt;
    float heading;
    float velocity;
    int identity;
};
#pragma pack()

class SimpleGpsReceiver {
private:
    int socket_fd;
    std::atomic<bool> running;
    std::thread receiver_thread;
    
public:
    SimpleGpsReceiver() : socket_fd(-1), running(false) {}
    
    ~SimpleGpsReceiver() {
        stop();
    }
    
    bool start(int port = 2026) {
        // Create UDP socket
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            return false;
        }
        
        // Set socket options
        int opt = 1;
        setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // Bind to port
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);
        
        if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Error binding to port " << port << std::endl;
            close(socket_fd);
            return false;
        }
        
        std::cout << "GPS UDP Receiver listening on port " << port << std::endl;
        
        // Start receiver thread
        running = true;
        receiver_thread = std::thread(&SimpleGpsReceiver::receiveLoop, this);
        
        return true;
    }
    
    void stop() {
        running = false;
        if (receiver_thread.joinable()) {
            receiver_thread.join();
        }
        if (socket_fd >= 0) {
            close(socket_fd);
            socket_fd = -1;
        }
    }
    
private:
    void receiveLoop() {
        char buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        while (running) {
            // Set socket timeout
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            
            ssize_t bytes_received = recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                                            (struct sockaddr*)&client_addr, &client_len);
            
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                processGpsData(buffer, bytes_received, client_addr);
            }
        }
    }
    
    void processGpsData(const char* data, size_t size, const struct sockaddr_in& sender) {
        std::string sender_ip = inet_ntoa(sender.sin_addr);
        
        // Try to parse as JSON first (simple parsing)
        if (parseSimpleJsonGpsData(data, size)) {
            return;
        }
        
        // Try to parse as binary
        if (size == sizeof(GpsData)) {
            parseBinaryGpsData(data, size);
            return;
        }
        
        std::cout << "Unknown data format from " << sender_ip 
                  << " (size: " << size << ")" << std::endl;
        
        // Show raw data for debugging
        std::cout << "Raw data: ";
        for (size_t i = 0; i < std::min(size, (size_t)50); ++i) {
            if (data[i] >= 32 && data[i] <= 126) {
                std::cout << data[i];
            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
    
    bool parseSimpleJsonGpsData(const char* data, size_t size) {
        std::string json_str(data, size);
        
        // Simple JSON parsing (not robust, but works for our test data)
        if (json_str.find("\"lat\"") == std::string::npos ||
            json_str.find("\"lon\"") == std::string::npos ||
            json_str.find("\"alt\"") == std::string::npos) {
            return false;
        }
        
        try {
            // Extract values using simple string parsing
            int trackId = extractJsonInt(json_str, "trackId", 1);
            double lat = extractJsonDouble(json_str, "lat");
            double lon = extractJsonDouble(json_str, "lon");
            double alt = extractJsonDouble(json_str, "alt");
            float heading = (float)extractJsonDouble(json_str, "heading", 0.0);
            float velocity = (float)extractJsonDouble(json_str, "velocity", 0.0);
            int identity = extractJsonInt(json_str, "identity", 1);
            
            // Validate coordinates
            if (lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
                std::cout << "Invalid GPS coordinates: " << lat << ", " << lon << std::endl;
                return false;
            }
            
            displayGpsData(trackId, lat, lon, alt, heading, velocity, identity, "JSON");
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    double extractJsonDouble(const std::string& json, const std::string& key, double defaultValue = 0.0) {
        std::string search = "\"" + key + "\":";
        size_t pos = json.find(search);
        if (pos == std::string::npos) {
            return defaultValue;
        }
        
        pos += search.length();
        
        // Skip whitespace
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) {
            pos++;
        }
        
        // Find end of number
        size_t end = pos;
        while (end < json.length() && 
               (std::isdigit(json[end]) || json[end] == '.' || json[end] == '-' || json[end] == '+' || json[end] == 'e' || json[end] == 'E')) {
            end++;
        }
        
        if (end > pos) {
            return std::stod(json.substr(pos, end - pos));
        }
        
        return defaultValue;
    }
    
    int extractJsonInt(const std::string& json, const std::string& key, int defaultValue = 0) {
        return (int)extractJsonDouble(json, key, defaultValue);
    }
    
    bool parseBinaryGpsData(const char* data, size_t size) {
        if (size != sizeof(GpsData)) {
            return false;
        }
        
        GpsData gps;
        memcpy(&gps, data, sizeof(GpsData));
        
        // Validate coordinates
        if (gps.lat < -90.0 || gps.lat > 90.0 || gps.lon < -180.0 || gps.lon > 180.0) {
            std::cout << "Invalid GPS coordinates: " << gps.lat << ", " << gps.lon << std::endl;
            return false;
        }
        
        displayGpsData(gps.trackId, gps.lat, gps.lon, gps.alt, 
                      gps.heading, gps.velocity, gps.identity, "Binary");
        return true;
    }
    
    void displayGpsData(int trackId, double lat, double lon, double alt,
                       float heading, float velocity, int identity, const std::string& format) {
        
        std::string identityStr;
        switch (identity) {
            case 0: identityStr = "Default"; break;
            case 1: identityStr = "Unknown"; break;
            case 2: identityStr = "Friend"; break;
            case 3: identityStr = "Hostile"; break;
            default: identityStr = "Invalid"; break;
        }
        
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        std::cout << "\n=== GPS Data Received (" << format << ") === " 
                  << std::put_time(&tm, "%H:%M:%S") << std::endl;
        std::cout << "Track ID:   " << trackId << std::endl;
        std::cout << "Position:   " << std::fixed << std::setprecision(6) 
                  << lat << "°N, " << lon << "°E" << std::endl;
        std::cout << "Altitude:   " << std::fixed << std::setprecision(1) 
                  << alt << " m" << std::endl;
        std::cout << "Heading:    " << std::fixed << std::setprecision(1) 
                  << heading << "°" << std::endl;
        std::cout << "Velocity:   " << std::fixed << std::setprecision(1) 
                  << velocity << " m/s (" << (velocity * 3.6) << " km/h)" << std::endl;
        std::cout << "Identity:   " << identityStr << " (" << identity << ")" << std::endl;
        std::cout << "===============================" << std::endl;
    }
};

// Global receiver instance for signal handling
SimpleGpsReceiver* g_receiver = nullptr;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    if (g_receiver) {
        g_receiver->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    std::cout << "Simple GPS UDP Receiver Demo (Basic Version)" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "This demo receives GPS data via UDP and displays it." << std::endl;
    std::cout << "Default port: 2026" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments
    int port = 2026;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }
    
    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Create and start receiver
    SimpleGpsReceiver receiver;
    g_receiver = &receiver;
    
    if (!receiver.start(port)) {
        std::cerr << "Failed to start GPS receiver" << std::endl;
        return 1;
    }
    
    std::cout << "Waiting for GPS data..." << std::endl;
    std::cout << "Test with: python3 gps_test_sender.py --mode static" << std::endl;
    std::cout << std::endl;
    
    // Keep main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}