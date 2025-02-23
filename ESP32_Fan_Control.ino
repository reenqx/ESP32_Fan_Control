#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
 
#define ENA 19  // ขา ENA (เปิด/ปิด)
#define IN1 5   // ขา IN1 ของ L298N
#define IN2 18  // ขา IN2 ของ L298N
 
const char* ssid = "ESP32_Fan_Control";  // ชื่อ Wi-Fi ที่ ESP32 จะสร้าง
const char* password = "12345678"; // รหัสผ่าน Wi-Fi
 
WebServer server(80);
 
bool fanState = false;
 
// ฟังก์ชันควบคุมพัดลม (เปิด/ปิด)
void controlFan() {
    if (fanState) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(ENA, HIGH);  // เปิดพัดลม
    } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(ENA, LOW);   // ปิดพัดลม
    }
}
 
// แสดงหน้าเว็บ UI
void handleRoot() {
    String html = "<!DOCTYPE html>"
                  "<html><head><title>ESP32 Fan Control</title>"
                  "<style>"
                  "body { font-family: Arial, sans-serif; text-align: center; margin: 50px; }"
                  "h2 { color: #333; }"
                  "p { font-size: 18px; font-weight: bold; }"
                  "button { font-size: 16px; padding: 10px 20px; margin: 10px; border: none; border-radius: 5px; cursor: pointer; }"
                  ".on { background-color: green; color: white; }"
                  ".off { background-color: red; color: white; }"
                  "</style></head>"
                  "<body>"
                  "<h2>ESP32 Fan Control</h2>"
                  "<p>Fan Status: <strong id='fanStatus'>Loading...</strong></p>"
                  "<button class='on' onclick=\"controlFan('on')\">Turn ON</button>"
                  "<button class='off' onclick=\"controlFan('off')\">Turn OFF</button>"
                  "<script>"
                  "function controlFan(action) { fetch('/' + action).then(response => location.reload()); }"
                  "function updateFanStatus() { fetch('/status').then(response => response.text()).then(status => { document.getElementById('fanStatus').textContent = status === '1' ? 'ON' : 'OFF'; }); }"
                  "updateFanStatus();"
                  "</script>"
                  "</body></html>";
    server.send(200, "text/html", html);
}
 
// เปิดพัดลม
void handleOn() {
    fanState = true;
    controlFan();
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Fan Turned ON");
}
 
// ปิดพัดลม
void handleOff() {
    fanState = false;
    controlFan();
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Fan Turned OFF");
}
 
// ส่งสถานะพัดลมกลับไปที่ UI
void handleStatus() {
    server.send(200, "text/plain", fanState ? "1" : "0");
}
 
void setup() {
    Serial.begin(115200);
 
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
 
    // ตั้งค่า Static IP สำหรับ ESP32 AP Mode
    IPAddress local_IP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
 
    // สร้าง Wi-Fi Hotspot
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_IP, gateway, subnet);
 
    Serial.println("Wi-Fi AP Created!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
 
    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);
    server.on("/status", handleStatus);
 
    server.begin();
}
 
void loop() {
    server.handleClient();
}