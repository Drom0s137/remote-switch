
#include <WiFi.h>
#include <HTTPClient.h>

/*Put your SSID & Password*/
const char* ssid = "sesameLocal";  // Enter SSID here
const char* password = "ultrasonicspeed";  //Enter Password here

//Your IP address or domain name with URL path
const char* serverNameSwitch1on = "http://192.168.0.29/switch1on";
const char* serverNameSwitch1off = "http://192.168.0.29/switch1off";

bool switch1status = LOW;

unsigned long previousMillis = 0;
const long interval = 5000; 

//button pressing
const int LEDPIN = 34;
const int PushButton = 13;

int Push_button_state = 0;
int prev_button_state = 0;
void setup() {
  Serial.begin(115200);
  
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  pinMode(PushButton, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  Push_button_state = digitalRead(PushButton);
  
  if ( Push_button_state == HIGH && prev_button_state == LOW){ 
    prev_button_state = HIGH;
    Serial.println("button pressed");
    if (switch1status == LOW){
      if(WiFi.status()== WL_CONNECTED ){ 
        int switch1code = httpGETRequest(serverNameSwitch1on);
        switch1status = HIGH;
      }else {
        Serial.println("WiFi Disconnected");
      }
    } else {
      if(WiFi.status()== WL_CONNECTED ){ 
        int switch1code = httpGETRequest(serverNameSwitch1off);
        switch1status = LOW;
      }else {
        Serial.println("WiFi Disconnected");
      }
    }
  } else if (Push_button_state == LOW && prev_button_state == HIGH) {
    prev_button_state = LOW;
  }
}

int httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    //payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return httpResponseCode;
}
