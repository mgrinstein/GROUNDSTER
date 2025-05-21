#include <iostream>
#include <cmath>
#include <sstream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

string generateTelemetry()
{
    ostringstream oss;
    oss << R"({)"
        << R"("timestamp": "2025-05-03T14:32:00Z", )"
        << R"("battery": )" << 75.0 + (rand() % 1000) / 100.0 << ", "
        << R"("temperature": )" << 20 + (rand() % 1000) / 100.0 << ", "
        << R"("orientation": {)"
        << R"("pitch": )" << -10 + (rand() % 200) / 10.0 << ", "
        << R"("yaw": )" << -10 + (rand() % 200) / 10.0 << ", "
        << R"("roll": )" << -10 + (rand() % 200) / 10.0
        << R"(})"
        << R"(})";
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
