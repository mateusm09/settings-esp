#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <SPIFFS.h>

#define DEFAULT_SSID "ALHN-6593"
#define DEFAULT_PASSWORD "Vdb4U5k-XA"

WebServer server;
Preferences preferences;

bool dirty = false;

void wifiRequest() {

    Serial.println("NEW REQUEST");
    Serial.printf("PATH: %s\n", server.uri());

    if(server.method() != HTTP_POST) {
        server.send(400, "text/html", "Invalid method");
        return;
    }

    if(server.hasArg("ssid")) {
        String key = server.uri() + "/ssid";
        preferences.putString(key.c_str(), server.arg("ssid"));
    }

    if(server.hasArg("password")) {
        String key = server.uri() + "/password";
        preferences.putString(key.c_str(), server.arg("password"));
    }

    server.send(200, "text/html", "ok");
}

void saveRequest() {
    if(server.method() != HTTP_POST) {
        server.send(400, "text/html", "Invalid method");
        return;
    }

    if(server.hasArg("save")) {
        server.send(200, "text/html", "ok");
        ESP.restart();
    }
}

void wifiListRequest() {
    if(server.method() != HTTP_GET) {
        server.send(400, "text/html", "Invalid method");
        return;
    }

    Serial.println("NEW WIFI LIST REQUEST");

    int wifiLength = WiFi.scanNetworks();

    String wifis = "[";

    for (uint8_t i = 0; i < wifiLength; i++)
    {
        wifis += "{" "\"ssid\":\"" + WiFi.SSID(i) + "\"," + "\"rssi\":" + String(WiFi.RSSI(i)) + "}";

        if(i != wifiLength - 1) {
            wifis += ",";
        }
    }

    wifis += "]";

    server.send(200, "application/x-www-form-urlencoded", wifis);
}

void servePage() {
    if(server.method() != HTTP_GET) {
        server.send(400, "text/html", "Invalid method");
        return;
    }

    File index = SPIFFS.open("/index.html");
    uint8_t *buf = (uint8_t *)malloc(index.size());

    index.read(buf, index.size());

    server.send(200, "text/html", String((char*)buf));
}

void setup()
{
    Serial.begin(115200);

    SPIFFS.begin();
    preferences.begin("configurations");

    String ssid = preferences.getString("/local/ssid", DEFAULT_SSID);
    String password = preferences.getString("/local/password", DEFAULT_PASSWORD);

    String hubSsid = preferences.getString("/hub/ssid", "teste");
    String hubPassword = preferences.getString("/hub/password", "12345678");

    Serial.println("Preferences:");
    Serial.printf("[LOCAL] SSID: %s PASSWORD: %s\n", ssid, password.c_str());
    Serial.printf("[HUB] SSID: %s PASSWORD: %s\n", hubSsid, hubPassword.c_str());

    WiFi.begin(ssid.c_str(), password.c_str());

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
       Serial.print('.');
       delay(1000);
    }
    Serial.println("Connected!");
    Serial.print("IP ADDRESS: ");
    Serial.println(WiFi.localIP());

    server.on("/local", HTTP_POST, wifiRequest);
    server.on("/hub", HTTP_POST, wifiRequest);
    server.on("/save", HTTP_POST, saveRequest);
    server.on("/wifis", HTTP_GET, wifiListRequest);
    // server.on("/", HTTP_GET, servePage);
    server.serveStatic("/", SPIFFS, "/");

    server.enableCORS();
    server.begin(80);
}

void loop()
{
    server.handleClient();
}