#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "SmartCity_Pro";
const char* password = ""; 

WebServer server(80);
Servo myServo;

const int trigTank = 5,  echoTank = 18;
const int trigWaste = 16, echoWaste = 17;
const int trigP1 = 2,  echoP1 = 4;
const int trigP2 = 13, echoP2 = 15;

const int irSensor = 34;
const int servoPin = 19;
const int buzzer = 23;
const int pumpRelay = 22;
const int ledGreen = 26;
const int ledRed = 25;

int parkingThreshold = 10;
int tankFullLevel = 8;
int wasteFullLevel = 8;

long p1_dist = 0, p2_dist = 0, tank_dist = 0, waste_dist = 0;
bool isGateOpen = false;
bool isParkingFull = false;

unsigned long lastSensorRead = 0;
const long sensorInterval = 500;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart City Dashboard</title>
  <style>
    :root {
      --bg-color: #f4f7f6;
      --card-bg: #ffffff;
      --text-main: #333333;
      --accent: #007bff;
      --success: #28a745;
      --danger: #dc3545;
      --warning: #ffc107;
    }
    [data-theme="dark"] {
      --bg-color: #121212;
      --card-bg: #1e1e1e;
      --text-main: #e0e0e0;
      --accent: #3da9fc;
    }
    body {
      font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
      background-color: var(--bg-color);
      color: var(--text-main);
      margin: 0;
      padding: 20px;
      transition: background-color 0.3s, color 0.3s;
    }
    .container {
      max-width: 800px;
      margin: 0 auto;
    }
    .header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 20px;
    }
    .theme-btn {
      background: var(--accent);
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 8px;
      cursor: pointer;
      font-weight: bold;
    }
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
      gap: 20px;
    }
    .card {
      background: var(--card-bg);
      padding: 20px;
      border-radius: 12px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      transition: background-color 0.3s;
    }
    .card h2 { margin-top: 0; font-size: 1.2rem; }
    .status-badge {
      display: inline-block;
      padding: 5px 10px;
      border-radius: 20px;
      font-weight: bold;
      font-size: 0.9rem;
      color: white;
      margin-top: 10px;
    }
    .bg-success { background-color: var(--success); }
    .bg-danger { background-color: var(--danger); }
    .bg-warning { background-color: var(--warning); color: #333; }
    
    .data-row {
      display: flex;
      justify-content: space-between;
      margin: 10px 0;
      padding-bottom: 5px;
      border-bottom: 1px solid rgba(128,128,128,0.2);
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>Smart City Hub</h1>
      <button class="theme-btn" onclick="toggleTheme()">🌓</button>
    </div>

    <div class="grid">
      <div class="card">
        <h2>🚗 Parking Lot & Gate</h2>
        <div class="data-row"><span>Slot 1:</span> <strong id="p1-status">Loading...</strong></div>
        <div class="data-row"><span>Slot 2:</span> <strong id="p2-status">Loading...</strong></div>
        <div class="data-row"><span>Gate Status:</span> <strong id="gate-status">Loading...</strong></div>
        <div id="parking-badge" class="status-badge bg-success">Available</div>
      </div>

      <div class="card">
        <h2>💧 Water Management</h2>
        <div class="data-row"><span>Tank Level (Distance):</span> <strong id="tank-dist">-- cm</strong></div>
        <div class="data-row"><span>Pump Status:</span> <strong id="pump-status">Loading...</strong></div>
        <div id="tank-badge" class="status-badge bg-success">Tank Full</div>
      </div>

      <div class="card">
        <h2>🗑️ Waste Management</h2>
        <div class="data-row"><span>Bin Level (Distance):</span> <strong id="waste-dist">-- cm</strong></div>
        <div id="waste-badge" class="status-badge bg-success">Bin OK</div>
      </div>
    </div>
  </div>

  <script>
    // Theme Toggle Logic
    function toggleTheme() {
      const body = document.body;
      if (body.getAttribute('data-theme') === 'dark') {
        body.removeAttribute('data-theme');
      } else {
        body.setAttribute('data-theme', 'dark');
      }
    }

    // Fetch data from ESP32 every 1 second
    setInterval(() => {
      fetch('/api/data')
        .then(response => response.json())
        .then(data => {
          // Update Parking
          document.getElementById('p1-status').innerText = data.slot1_full ? 'Occupied' : 'Free';
          document.getElementById('p2-status').innerText = data.slot2_full ? 'Occupied' : 'Free';
          document.getElementById('gate-status').innerText = data.gate_open ? 'OPEN' : 'CLOSED';
          
          const parkBadge = document.getElementById('parking-badge');
          if (data.parking_full) {
            parkBadge.innerText = 'PARKING FULL';
            parkBadge.className = 'status-badge bg-danger';
          } else {
            parkBadge.innerText = 'SPACES AVAILABLE';
            parkBadge.className = 'status-badge bg-success';
          }

          // Update Tank
          document.getElementById('tank-dist').innerText = data.tank_dist + ' cm';
          document.getElementById('pump-status').innerText = data.pump_on ? 'ON (Filling)' : 'OFF';
          const tankBadge = document.getElementById('tank-badge');
          if (data.pump_on) {
            tankBadge.innerText = 'FILLING TANK';
            tankBadge.className = 'status-badge bg-warning';
          } else {
            tankBadge.innerText = 'TANK FULL';
            tankBadge.className = 'status-badge bg-success';
          }

          // Update Waste
          document.getElementById('waste-dist').innerText = data.waste_dist + ' cm';
          const wasteBadge = document.getElementById('waste-badge');
          if (data.waste_full) {
            wasteBadge.innerText = 'BIN FULL - PLEASE EMPTY';
            wasteBadge.className = 'status-badge bg-danger';
          } else {
            wasteBadge.innerText = 'BIN OK';
            wasteBadge.className = 'status-badge bg-success';
          }
        })
        .catch(err => console.error('Error fetching data:', err));
    }, 1000); // 1000ms = 1 second
  </script>
</body>
</html>
)rawliteral";

long readDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 30000);
  if (duration == 0) return 999; 
  return duration * 0.034 / 2;
}

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleData() {
  String json = "{";
  json += "\"slot1_full\":" + String((p1_dist < parkingThreshold && p1_dist > 0) ? "true" : "false") + ",";
  json += "\"slot2_full\":" + String((p2_dist < parkingThreshold && p2_dist > 0) ? "true" : "false") + ",";
  json += "\"parking_full\":" + String(isParkingFull ? "true" : "false") + ",";
  json += "\"gate_open\":" + String(isGateOpen ? "true" : "false") + ",";
  json += "\"tank_dist\":" + String(tank_dist) + ",";
  json += "\"pump_on\":" + String((tank_dist > tankFullLevel) ? "true" : "false") + ",";
  json += "\"waste_dist\":" + String(waste_dist) + ",";
  json += "\"waste_full\":" + String((waste_dist < wasteFullLevel && waste_dist > 0) ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pinMode(trigTank, OUTPUT); pinMode(echoTank, INPUT);
  pinMode(trigWaste, OUTPUT); pinMode(echoWaste, INPUT);
  pinMode(trigP1, OUTPUT); pinMode(echoP1, INPUT);
  pinMode(trigP2, OUTPUT); pinMode(echoP2, INPUT);
  pinMode(irSensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(pumpRelay, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(90); 

  Serial.println("\nStarting Access Point...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.on("/api/data", handleData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (millis() - lastSensorRead > sensorInterval) {
    lastSensorRead = millis();

    p1_dist = readDistance(trigP1, echoP1);
    p2_dist = readDistance(trigP2, echoP2);
    tank_dist = readDistance(trigTank, echoTank);
    waste_dist = readDistance(trigWaste, echoWaste);

    bool slot1 = (p1_dist < parkingThreshold && p1_dist > 0);
    bool slot2 = (p2_dist < parkingThreshold && p2_dist > 0);
    isParkingFull = (slot1 && slot2);
    bool carAtGate = (digitalRead(irSensor) == LOW);

    if (isParkingFull) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(ledGreen, LOW);
      myServo.write(90); 
      isGateOpen = false;
    } else {
      if (carAtGate) {
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledRed, LOW);
        myServo.write(0); // Open
        if (!isGateOpen) { tone(buzzer, 1500, 200); } 
        isGateOpen = true;
      } else {
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledRed, LOW); 
        myServo.write(90); 
        isGateOpen = false;
      }
    }

    if (tank_dist > tankFullLevel) {
      digitalWrite(pumpRelay, HIGH);
    } else {
      digitalWrite(pumpRelay, LOW);
    }

    if (waste_dist < wasteFullLevel && waste_dist > 0) {
      digitalWrite(ledRed, HIGH); 
      tone(buzzer, 800, 100);
    }
  }
}