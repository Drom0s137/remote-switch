#include <WiFi.h>
#include <WebServer.h>
#include <Servo.h>
#include <esp_wifi.h>
/*Put your SSID & Password*/
const char* ssid = "sesameLocal";  // Enter SSID here
const char* password = "ultrasonicspeed";  //Enter Password here

void setWiFiPowerSavingMode();

WebServer server(80);
Servo myservo;
int pos = 0;    

uint8_t switch1pin = 15;
bool switch1status = LOW;
bool changed = false;

IPAddress local_IP(192, 168, 0, 29);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

void setup() {
  setCpuFrequencyMhz(40);
  Serial.begin(115200);
  delay(100);
  pinMode(switch1pin, OUTPUT);
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  setWiFiPowerSavingMode();
  server.on("/", handle_OnConnect);
  server.on("/switch1on", handle_switch1on);
  server.on("/switch1off", handle_switch1off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  myservo.attach(25); 
  myservo.write(90);
}
void loop() {
  server.handleClient();
  if(switch1status&&changed)
  {digitalWrite(switch1pin, HIGH);
  myservo.write(140); 
  delay(1000);
  myservo.write(90);
  changed = false;
  }
  else if (!switch1status&&changed)
  {digitalWrite(switch1pin, LOW);
  myservo.write(45);
  delay(1000);
  myservo.write(90);
  changed = false;} 
}

void setWiFiPowerSavingMode(){
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
}
/*
 * response code switch1 : 100 on, 200 off
 */
void handle_OnConnect() {
  switch1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(switch1status)); 
  server.send(100, "text/html", SendHTML(true)); 
}

void handle_switch1on() {
  switch1status = HIGH;
  changed = true;
  Serial.println("GPIO4 Status: ON");
  server.send(100, "text/html", SendHTML(true)); 
}

void handle_switch1off() {
  switch1status = LOW;
  changed = true;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false)); 
}
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t switch1stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Switch Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Eddy's home automation project</h1>\n";
  ptr +="<h3>ESP32 Control</h3>\n";
  
  if(switch1stat)
  {ptr +="<p>Switch1 Status: ON</p><a class=\"button button-off\" href=\"/switch1off\">OFF</a>\n";}
  else
  {ptr +="<p>Switch1 Status: OFF</p><a class=\"button button-on\" href=\"/switch1on\">ON</a>\n";}
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
