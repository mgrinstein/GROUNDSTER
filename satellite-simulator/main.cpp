#include <iostream>
#include <cmath>
#include <sstream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <random>
#include <iomanip>
#include <ctime>

using namespace std;


std::string generateTelemetry() {
    static std::default_random_engine rng(std::random_device{}());
    static double battery = 100.0;
    static double temp = 20.0;

    static double pitch = 0.0;
    static double yaw = 0.0;
    static double roll = 0.0;

    static std::uniform_real_distribution<double> battery_change(-0.05, 0.0);
    static std::uniform_real_distribution<double> temp_change(-0.2, 0.2);
    static std::uniform_real_distribution<double> orientation_change(-1.0, 1.0);
    
    battery = std::max(0.0, battery + battery_change(rng));
    temp += temp_change(rng);
    
    pitch += orientation_change(rng);
    yaw += orientation_change(rng);
    roll += orientation_change(rng);

    static auto base_time = std::chrono::system_clock::now();
    static int seconds_elapsed = 0;

    auto current_time = base_time + std::chrono::seconds(seconds_elapsed);
    std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_time);
    std::tm *gmt = std::gmtime(&current_time_t);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "{"
        << "\"timestamp\": \"" 
        << std::put_time(gmt, "%FT%TZ") << "\","
        << "\"battery\":" << battery << ","
        << "\"temperature\":" << temp << ","
        << "\"orientation\":{"
            << "\"pitch\":" << pitch << ","
            << "\"yaw\":" << yaw << ","
            << "\"roll\":" << roll
        << "}"
        << "}";

    seconds_elapsed++;  // increment for next call
    return oss.str();
}


int main()
{
    int telemetrySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (telemetrySocket < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }
    
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9000); 
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    
    if (connect(telemetrySocket, (sockaddr*)&server, sizeof(server)) < 0) {
        cerr << "Error connecting socket" << endl;
        return 1;
    }

    auto start = chrono::steady_clock::now();
    auto duration = chrono::seconds(60);
    
    while (chrono::steady_clock::now() - start < duration) {
        string telemetry = generateTelemetry();
        send(telemetrySocket, telemetry.c_str(), telemetry.size(), 0);
        send(telemetrySocket, "\n", 1, 0);
        cout << "Sent telemetry: " << telemetry << endl;
        this_thread::sleep_for(chrono::seconds(2));
    }

    close(telemetrySocket);
    return 0;
}
