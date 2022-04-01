#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
const char* ssid = "LiterallyOnFire";
const char* password = "askthegatekeeper";

ESP8266WebServer server(80);
 
const int StatusLed = 4;
const int PollingLED = 5;

void setup(void)
{
  Serial.begin(115200);
  
  pinMode(StatusLed, OUTPUT);
  pinMode(PollingLED, OUTPUT);
  digitalWrite(StatusLed,  0);
  digitalWrite(PollingLED, 1);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int ledtoggle = 0;
  while (WiFi.status() != WL_CONNECTED) {
    ledtoggle = !ledtoggle;
    Serial.print("Waiting for ");
    Serial.print(ssid);
    Serial.println("...");
    digitalWrite(StatusLed, ledtoggle);
    delay(500);
  }
   
  digitalWrite(StatusLed, 1);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  server.on("/", handle_root);
  server.on("/ADC", handle_ADC);
  
  server.begin();
  
  Serial.println("HTTP server started");
}

 
void loop(void){
  int ledtoggle = 1;
  digitalWrite(StatusLed, 1);
  server.handleClient();
  
  while (WiFi.status() != WL_CONNECTED) {
    ledtoggle = !ledtoggle;
    Serial.print("Waiting for ");
    Serial.print(ssid);
    Serial.println("...");
    digitalWrite(StatusLed, ledtoggle);
    delay(500);
  } 
} 


void handle_root(){ 
  String html = "";
  String html_1 = "<h1>Thermostat sensor @ ";
  String html_2 = "</h1>Connected to ";
  String html_3 = "<br><br>";
  String html_4 = "<a href='./ADC' target='iframe' style='text-decoration:none'>Poll \"$_SERVER/ADC\"</a>&nbsp;&nbsp;<br><br><iframe style='border:none' name='iframe'></iframe>";

  IPAddress ip = WiFi.localIP();
  char sprintf_buffer[128];
  sprintf(sprintf_buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  html.concat(html_1);
  html.concat(sprintf_buffer);
  html.concat(html_2); 
  html.concat(ssid);
  html.concat(html_3); 
  html.concat(html_4);
  
  server.send(200, "text/html", html);
}


void handle_ADC(){ 
  digitalWrite(PollingLED, 0);
  
  long ADC = 0; 

  for(int i = 0; i < 10; i++){
    ADC += analogRead(A0);
    delay(10);
  }
  
  ADC = ADC / 10;
  
  char sprintf_buffer[20];
  
  sprintf(sprintf_buffer, "{\"ADC0\":\"%d\"}", ADC);
  
  server.send(200, "text/html", sprintf_buffer);

  digitalWrite(PollingLED, 1);
}
