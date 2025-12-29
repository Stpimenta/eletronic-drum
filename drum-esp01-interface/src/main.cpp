#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "./web/index.h"
#include "struct/pad_state_t.h"

const char *ssid = "ESP01-DRUM";
const char *password = "12345678";

ESP8266WebServer server(80);

// ---------------- UART / ENGINE STATE ----------------

String uartRxBuffer;
String engineDump;

bool receivingEngine = false;
bool engineReady = false;

unsigned long engineRequestAt = 0;
const unsigned long ENGINE_TIMEOUT_MS = 500;

// estado global
String engineJson;
String engineError;
bool getEngineState();

// ---------------- WEB ----------------

void handleRoot()
{
  server.send_P(200, "text/html", PAGE_INDEX);
}

void handleCmd()
{
  String body = server.arg("plain");
  Serial.println(body);
  server.send(200, "text/plain", "OK");
  getEngineState();
}

// ---------------- ENGINE REQUEST ----------------

void requestEngineState()
{
  engineDump = "";
  uartRxBuffer = "";
  receivingEngine = false;
  engineReady = false;

  engineRequestAt = millis();

  Serial.println("GET ENGINE");
}

// ---------------- JSON BUILD ----------------

String buildEngineJson(const String &dump)
{
  String json = "{";
  json += "\"engine_ready\":true,";
  json += "\"pads\":[";

  bool first = true;
  int start = 0;

  while (true)
  {
    int end = dump.indexOf('\n', start);
    if (end < 0) break;

    String line = dump.substring(start, end);
    start = end + 1;

    line.trim();
    if (!line.startsWith("PAD")) continue;

    // PAD 1 snare 100 38 127 600 10000
    int idx = 0;
    String parts[8];

    for (int i = 0; i < 8; i++)
    {
      int space = line.indexOf(' ', idx);
      if (space < 0)
      {
        parts[i] = line.substring(idx);
        break;
      }
      parts[i] = line.substring(idx, space);
      idx = space + 1;
    }

    if (!first) json += ",";
    first = false;

    json += "{";
    json += "\"id\":" + parts[1] + ",";
    json += "\"name\":\"" + parts[2] + "\",";
    json += "\"threshold\":" + parts[3] + ",";
    json += "\"note\":" + parts[4] + ",";
    json += "\"sensitivity\":" + parts[5] + ",";
    json += "\"peak_hold\":" + parts[6] + ",";
    json += "\"retrigger\":" + parts[7];
    json += "}";
  }

  json += "]}";
  return json;
}

// ---------------- ENGINE STATE CORE ----------------

bool getEngineState()
{
  requestEngineState();

  while (!engineReady)
  {
    if (millis() - engineRequestAt > ENGINE_TIMEOUT_MS)
    {
      engineError = "ENGINE TIMEOUT";
      engineJson = "";
      return false;
    }

    while (Serial.available())
    {
      char c = Serial.read();

      if (c == '\n')
      {
        if (uartRxBuffer == "BEGIN")
        {
          receivingEngine = true;
          engineDump = "";
        }
        else if (uartRxBuffer == "END")
        {
          receivingEngine = false;
          engineReady = true;
        }
        else if (receivingEngine)
        {
          engineDump += uartRxBuffer + "\n";
        }

        uartRxBuffer = "";
      }
      else if (c != '\r')
      {
        uartRxBuffer += c;
      }
    }

    yield();
  }

  engineJson = buildEngineJson(engineDump);
  engineError = "";
  return true;
}

// ---------------- /state (HTTP) ----------------

void handleState()
{
  if (!getEngineState())
  {
    server.send(504, "text/plain", engineError);
    return;
  }

  server.send(200, "application/json", engineJson);
}

// ---------------- SETUP / LOOP ----------------

void setup()
{
  Serial.begin(115200);
  delay(200);

  WiFi.mode(WIFI_AP);

  IPAddress ip(192, 168, 0, 1);
  IPAddress gw(192, 168, 0, 1);
  IPAddress sn(255, 255, 255, 0);
  WiFi.softAPConfig(ip, gw, sn);
  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/cmd", HTTP_POST, handleCmd);
  server.on("/state", HTTP_GET, handleState);
  server.begin();
  getEngineState();
  delay(200);
}

void loop()
{
  server.handleClient();
}
