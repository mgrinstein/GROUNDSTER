# GROUNDSTER - GROUND STATION TELEMETRY RECEIVER

This project simulates a ground station receiving telemetry data from a satellite.

![Telemetry](media/telemetry.gif)

## Architecture

```mermaid
    graph TD
        A["**Satellite Simulator (C++)** </br> Generates telemetry"] -- TCP (JSON stream - Port 9000) --> B["**Ground Station (Python)**" </br> Receives telemetry </br> Stores recent data </br> Serves dashboard ]
        B -- HTTP (FastAPI - port 8000) --> C[**Web Browser UI** </br> Telemetry Dashboard]
```

## Requirements

### C++ Satellite Simulator
- C++11 or later
- `g++` compiler
- Unix-like system (Linux/macOS)

### Python Ground Station
- Python 3.x
- `fastapi`, `uvicorn`, `jinja2`

## Instructions

### 1. Clone this repository
```bash
git clone https://github.com/mgrinstein/GROUNDSTER.git
cd GROUNDSTER
```

### 2. Compile the Satellite Simulator (Telemetry sender)

```bash
cd satellite-simulator
g++ -std=c++11 -o satellite_simulator main.cpp
```

### 3. Run the Ground Station (Receiver)
In a separate terminal, run:
```bash
cd ground-station-receiver
python3 main.py
```

### 4. Run the Satellite Simulator
Back in the simulator terminal, run:
```bash
./satellite_simulator
```

Telemetry will start being printed on the terminal:
![Terminal](media/terminal.gif)


It will also be received in the web interface dashboard (http://127.0.0.1:8000):

![Dashboard](media/dashboard-screenshot.png)

