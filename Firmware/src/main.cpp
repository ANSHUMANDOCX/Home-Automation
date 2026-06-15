/*
  Relay Controller — ESP32-S3-WROOM-1
  ─────────────────────────────────────
  • 6 relays controlled via web UI + onboard buttons
  • Buttons are active-LOW (10K pull-up to 3V3, 0.1uF debounce cap)
  • Relay outputs are active-HIGH (BC847 NPN driver)
  • Web server on port 80, served from PROGMEM
  • WebSocket for real-time state sync between page and board

  Pin mapping (from schematic):
    RELAY1 → IO15    BUTTON1 → IO38
    RELAY2 → IO8     BUTTON2 → IO33
    RELAY3 → IO7     BUTTON3 → IO21
    RELAY4 → IO6     BUTTON4 → IO20
    RELAY5 → IO5     BUTTON5 → IO19
    RELAY6 → IO4     BUTTON6 → IO18
*/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// ── Credentials ────────────────────────────────────────────────
const char* WIFI_SSID = "SSID";
const char* WIFI_PASS = "PASSWORD";

// ── Pin definitions ─────────────────────────────────────────────
const uint8_t RELAY_PINS[6] = {16, 15,7, 6, 5, 4};
const uint8_t BTN_PINS[6]   = {21, 14, 13, 12, 11, 10};

// ── State ───────────────────────────────────────────────────────
bool relayState[6] = {false};

// Debounce
uint32_t lastDebounce[6]  = {0};
bool     lastBtnRaw[6]    = {true};   // HIGH = unpressed (pull-up)
bool     btnState[6]      = {true};
const uint32_t DEBOUNCE_MS = 50;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ── HTML (stored in flash) ──────────────────────────────────────
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Relay Controller</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;700&family=Inter:wght@400;500;600&display=swap');

  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

  :root {
    --bg:       #0d0f14;
    --surface:  #161a22;
    --border:   #252a35;
    --text:     #c8cdd8;
    --dim:      #5a6070;
    --on:       #00e5a0;
    --on-glow:  rgba(0,229,160,0.18);
    --off:      #2a2f3a;
    --accent:   #4f8eff;
    --mono:     'JetBrains Mono', monospace;
    --sans:     'Inter', sans-serif;
  }

  body {
    background: var(--bg);
    color: var(--text);
    font-family: var(--sans);
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 2rem 1rem 3rem;
  }

  header {
    width: 100%;
    max-width: 640px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 2.5rem;
  }

  .logo {
    font-family: var(--mono);
    font-size: 0.75rem;
    letter-spacing: 0.15em;
    color: var(--dim);
    text-transform: uppercase;
  }

  .logo span { color: var(--on); }

  #status-dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    background: var(--dim);
    display: inline-block;
    margin-right: 6px;
    transition: background 0.3s;
  }
  #status-dot.connected { background: var(--on); box-shadow: 0 0 6px var(--on); }

  #status-label {
    font-family: var(--mono);
    font-size: 0.7rem;
    color: var(--dim);
    letter-spacing: 0.1em;
  }

  .grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 1rem;
    width: 100%;
    max-width: 640px;
  }

  @media (min-width: 500px) {
    .grid { grid-template-columns: repeat(3, 1fr); }
  }

  .relay-card {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 12px;
    padding: 1.25rem 1rem 1rem;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 0.85rem;
    cursor: pointer;
    transition: border-color 0.2s, box-shadow 0.2s, transform 0.1s;
    user-select: none;
    -webkit-tap-highlight-color: transparent;
  }

  .relay-card:active { transform: scale(0.97); }

  .relay-card.on {
    border-color: var(--on);
    box-shadow: 0 0 0 1px var(--on), inset 0 0 32px var(--on-glow);
  }

  .relay-num {
    font-family: var(--mono);
    font-size: 0.65rem;
    letter-spacing: 0.15em;
    color: var(--dim);
    text-transform: uppercase;
  }

  .relay-icon {
    width: 48px; height: 48px;
    position: relative;
  }

  /* SVG coil relay icon — drawn inline per card via JS */
  .relay-icon svg { width: 100%; height: 100%; }

  .relay-label {
    font-size: 0.8rem;
    font-weight: 600;
    letter-spacing: 0.05em;
  }

  .relay-badge {
    font-family: var(--mono);
    font-size: 0.65rem;
    letter-spacing: 0.12em;
    padding: 2px 8px;
    border-radius: 4px;
    background: var(--off);
    color: var(--dim);
    transition: background 0.2s, color 0.2s;
  }

  .relay-card.on .relay-badge {
    background: var(--on);
    color: #000;
  }

  .controls {
    margin-top: 1.75rem;
    display: flex;
    gap: 0.75rem;
    width: 100%;
    max-width: 640px;
  }

  .btn {
    flex: 1;
    padding: 0.65rem;
    border-radius: 8px;
    border: 1px solid var(--border);
    background: var(--surface);
    color: var(--text);
    font-family: var(--mono);
    font-size: 0.72rem;
    letter-spacing: 0.1em;
    cursor: pointer;
    transition: border-color 0.2s, color 0.2s;
  }

  .btn:hover { border-color: var(--accent); color: var(--accent); }

  .io-row {
    margin-top: 1rem;
    width: 100%;
    max-width: 640px;
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 0.75rem 1rem;
    font-family: var(--mono);
    font-size: 0.68rem;
    color: var(--dim);
    letter-spacing: 0.05em;
    display: flex;
    justify-content: space-between;
    flex-wrap: wrap;
    gap: 0.4rem;
  }

  .io-row span { color: var(--text); }
</style>
</head>
<body>

<header>
  <div class="logo">ESP32-S3 &nbsp;/&nbsp; <span>Relays</span></div>
  <div>
    <span id="status-dot"></span>
    <span id="status-label">CONNECTING</span>
  </div>
</header>

<div class="grid" id="grid"></div>

<div class="controls">
  <button class="btn" onclick="allOn()">ALL ON</button>
  <button class="btn" onclick="allOff()">ALL OFF</button>
</div>

<div class="io-row" id="io-row">
  Loading pin map…
</div>

<script>
const RELAY_PINS = [15, 8, 7, 6, 5, 4];
const BTN_PINS   = [38, 33, 21, 20, 19, 18];
const N = 6;
let states = Array(N).fill(false);
let ws;

// ── Build cards ──────────────────────────────────────────────
const grid = document.getElementById('grid');
for (let i = 0; i < N; i++) {
  const card = document.createElement('div');
  card.className = 'relay-card';
  card.id = `r${i}`;
  card.innerHTML = `
    <div class="relay-num">RLY ${i+1}</div>
    <div class="relay-icon">
      <svg viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
        <!-- coil -->
        <rect x="8" y="20" width="32" height="8" rx="2"
              fill="none" stroke="currentColor" stroke-width="1.5"/>
        <line x1="16" y1="20" x2="16" y2="28" stroke="currentColor" stroke-width="1.2"/>
        <line x1="22" y1="20" x2="22" y2="28" stroke="currentColor" stroke-width="1.2"/>
        <line x1="28" y1="20" x2="28" y2="28" stroke="currentColor" stroke-width="1.2"/>
        <!-- armature -->
        <line id="arm${i}" x1="4" y1="15" x2="24" y2="15"
              stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
        <!-- contact -->
        <circle cx="36" cy="11" r="2.5" fill="none"
                stroke="currentColor" stroke-width="1.5"/>
        <circle cx="36" cy="18" r="2.5" fill="none"
                stroke="currentColor" stroke-width="1.5"/>
        <!-- leads -->
        <line x1="4" y1="24" x2="4" y2="15" stroke="currentColor" stroke-width="1.5"/>
        <line x1="36" y1="8" x2="36" y2="8.5" stroke="currentColor" stroke-width="1.5"/>
        <line x1="36" y1="20.5" x2="36" y2="40" stroke="currentColor" stroke-width="1.5"/>
      </svg>
    </div>
    <div class="relay-label">Relay ${i+1}</div>
    <div class="relay-badge" id="b${i}">OFF</div>`;
  card.addEventListener('click', () => toggle(i));
  grid.appendChild(card);
}

// IO pin map row
document.getElementById('io-row').innerHTML =
  RELAY_PINS.map((p,i)=>`RLY${i+1}:<span>IO${p}</span>`).join(' &nbsp; ') +
  '&nbsp;&nbsp;|&nbsp;&nbsp;' +
  BTN_PINS.map((p,i)=>`BTN${i+1}:<span>IO${p}</span>`).join(' &nbsp; ');

// ── WebSocket ────────────────────────────────────────────────
function connect() {
  ws = new WebSocket(`ws://${location.hostname}/ws`);

  ws.onopen = () => {
    document.getElementById('status-dot').className = 'connected';
    document.getElementById('status-label').textContent = 'LIVE';
  };

  ws.onclose = () => {
    document.getElementById('status-dot').className = '';
    document.getElementById('status-label').textContent = 'RECONNECTING…';
    setTimeout(connect, 2000);
  };

  ws.onmessage = e => {
    const msg = JSON.parse(e.data);
    if (msg.type === 'state') {
      msg.relays.forEach((v, i) => setUI(i, v));
    }
  };
}

function send(obj) {
  if (ws && ws.readyState === WebSocket.OPEN)
    ws.send(JSON.stringify(obj));
}

// ── UI helpers ───────────────────────────────────────────────
function setUI(i, on) {
  states[i] = on;
  const card = document.getElementById(`r${i}`);
  const badge = document.getElementById(`b${i}`);
  card.classList.toggle('on', on);
  badge.textContent = on ? 'ON' : 'OFF';
}

function toggle(i) {
  send({ type: 'toggle', relay: i });
}

function allOn()  { send({ type: 'all', value: true  }); }
function allOff() { send({ type: 'all', value: false }); }

connect();
</script>
</body>
</html>
)rawliteral";


// ── WebSocket broadcast ─────────────────────────────────────────
void broadcastState() {
  StaticJsonDocument<128> doc;
  doc["type"] = "state";
  JsonArray arr = doc.createNestedArray("relays");
  for (int i = 0; i < 6; i++) arr.add(relayState[i]);
  String out;
  serializeJson(doc, out);
  ws.textAll(out);
}

void setRelay(int i, bool on) {
  relayState[i] = on;
  digitalWrite(RELAY_PINS[i], on ? HIGH : LOW);
}

// ── WebSocket event handler ─────────────────────────────────────
void onWsEvent(AsyncWebSocket* s, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    broadcastState();  // send current state to new client
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->opcode == WS_TEXT) {
      data[len] = '\0';
      StaticJsonDocument<128> doc;
      if (deserializeJson(doc, (char*)data)) return;

      const char* msgType = doc["type"];
      if (strcmp(msgType, "toggle") == 0) {
        int idx = doc["relay"];
        if (idx >= 0 && idx < 6) setRelay(idx, !relayState[idx]);
      } else if (strcmp(msgType, "all") == 0) {
        bool v = doc["value"];
        for (int i = 0; i < 6; i++) setRelay(i, v);
      }
      broadcastState();
    }
  }
}

// ── Setup ───────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // Relay outputs
  for (int i = 0; i < 6; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
  }

  // Button inputs (external pull-up on board, no internal needed)
  for (int i = 0; i < 6; i++) {
    pinMode(BTN_PINS[i], INPUT);
    lastBtnRaw[i] = digitalRead(BTN_PINS[i]);
    btnState[i]   = lastBtnRaw[i];
  }
// Static IP config — change to match your network
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);  // optional

WiFi.config(local_IP, gateway, subnet, dns);
WiFi.begin(WIFI_SSID, WIFI_PASS);
  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.printf("\nIP: %s\n", WiFi.localIP().toString().c_str());

  // WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // HTTP
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", INDEX_HTML);
  });

  server.begin();
  Serial.println("Server started");
}

// ── Loop ────────────────────────────────────────────────────────
void loop() {
  uint32_t now = millis();

  for (int i = 0; i < 6; i++) {
    bool raw = digitalRead(BTN_PINS[i]);

    if (raw != lastBtnRaw[i]) {
      lastDebounce[i] = now;
      lastBtnRaw[i] = raw;
    }

    if ((now - lastDebounce[i]) > DEBOUNCE_MS) {
      if (raw != btnState[i]) {
        btnState[i] = raw;
        // Falling edge = button pressed (active LOW)
        if (btnState[i] == LOW) {
          setRelay(i, !relayState[i]);
          broadcastState();
        }
      }
    }
  }

  ws.cleanupClients();
}
