import socket
import threading
import json
from collections import deque

from fastapi import FastAPI, Request
from fastapi.templating import Jinja2Templates
from fastapi.responses import HTMLResponse
import uvicorn

# In-memory storage (last 20 messages)
telemetry_log = deque(maxlen=20)

# FastAPI setup
app = FastAPI()
templates = Jinja2Templates(directory="templates")

@app.get("/", response_class=HTMLResponse)
async def dashboard(request: Request):
    return templates.TemplateResponse("dashboard.html", {
        "request": request,
        "telemetry": list(reversed(telemetry_log))
    })

# Telemetry TCP listener
def start_receiver(host='0.0.0.0', port=9000):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    s.listen(1)
    print(f"[Receiver] Listening on {host}:{port}")
    conn, addr = s.accept()
    print(f"[Receiver] Connection from {addr}")

    buffer = ""
    while True:
        data = conn.recv(1024).decode()
        if not data:
            break
        buffer += data
        while '\n' in buffer:
            line, buffer = buffer.split('\n', 1)
            try:
                parsed = json.loads(line)
                telemetry_log.append(parsed)
                print(f"[Receiver] Logged: {parsed}")
            except json.JSONDecodeError:
                print("[Receiver] Invalid JSON:", line)

# Start receiver and dashboard server in parallel
def start_all():
    threading.Thread(target=start_receiver, daemon=True).start()
    uvicorn.run(app, host="127.0.0.1", port=8000)

if __name__ == "__main__":
    start_all()
