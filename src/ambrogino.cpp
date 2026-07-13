/*
  Ambrogino: Wireless irrigation with ESP8266
*/

#include <Arduino.h> //necessary for platformio
 
#include <ESP8266WiFi.h>  //Wifi that won't cause issues with esp8266
#include <PubSubClient.h> //Publisher subscriber + mqtt client
#include <ESPDateTime.h>  //For date
#include <ArduinoOTA.h>   //OTA updates

#include <settings.h>

// Raw TCP log server on port 8266
WiFiServer logServer(8266);
WiFiClient logClient;

// Dual-output logger: writes to both hardware Serial and TCP log client
class DualStream : public Print {
public:
  size_t write(uint8_t c) override {
    Serial.write(c);
    if (logClient && logClient.connected()) logClient.write(c);
    return 1;
  }
  size_t write(const uint8_t *buf, size_t size) override {
    Serial.write(buf, size);
    if (logClient && logClient.connected()) logClient.write(buf, size);
    return size;
  }
} Log;

int status = WL_IDLE_STATUS;

// Forward declaration (required by platformio)

void callback(char* topic, byte* payload, unsigned int length);


WiFiClient espClient;

PubSubClient client;
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_ota() {
  ArduinoOTA.setHostname(ota_hostname);
  ArduinoOTA.setPassword(ota_password);

  ArduinoOTA.onStart([]() {
    Log.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Log.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Log.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Log.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)         Log.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)   Log.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Log.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Log.println("Receive Failed");
    else if (error == OTA_END_ERROR)     Log.println("End Failed");
  });

  ArduinoOTA.begin();
  Log.println("OTA Ready");
  Log.print("OTA IP: ");
  Log.println(WiFi.localIP());
}

void setup_mqtt() {
  client.setClient(espClient);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Log.println("MQTT server: ");
  Log.println(mqtt_server);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Log.print("Message arrived [");
  Log.print(topic);
  Log.print("] ");
  for (int i = 0; i < length; i++) {
    Log.print((char)payload[i]);
  }
  Log.println();
 
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(16, LOW);  
  } 
  else if((char)payload[0] == '2') {
    digitalWrite(5, LOW);
  } 
  else if((char)payload[0] == '3') {
    digitalWrite(4, LOW); 
  } 
  else if((char)payload[0] == '4') {
    digitalWrite(0, LOW);
  } 

  else {
    digitalWrite(16, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(0, HIGH);
  }
 
}

void setupDateTime() {
  // setup this after wifi connected
  // you can use custom timeZone,server and timeout
  // DateTime.setTimeZone("GMT");
  // DateTime.setServer("europe.pool.ntp.org");
  // DateTime.begin(15 * 1000);
  // from
  /** changed from 0.2.x **/
  DateTime.setTimeZone("GMT");
  // this method config ntp and wait for time sync
  // default timeout is 10 seconds
  DateTime.setServer("europe.pool.ntp.org");
  DateTime.begin(/* timeout param */);
  if (!DateTime.isTimeValid()) {
    Log.println("Failed to get time from server.");
  }
}

void setup_wifi() {
  delay(10);
  // Connect to a WiFi network
  Log.println();
  Log.print("Connecting to ");
  Log.println(ssid);
 
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Log.print(".");
  }
 
  randomSeed(micros());
 
  Log.println("");
  Log.println("WiFi connected");
  Log.println("IP address: ");
  Log.println(WiFi.localIP());

  setupDateTime();
}

void reconnect() {
  // Single attempt per call; loop() will retry on the next iteration
  if (client.connected()) return;

  static long lastReconnectAttempt = 0;
  long now = millis();
  if (now - lastReconnectAttempt < 5000) return;
  lastReconnectAttempt = now;

  Log.print("Attempting MQTT connection...");
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
    Log.println("connected");
    client.publish("irrigation_msg", "Ambrogino just reconnected");
    client.subscribe(mqtt_topic);
  } else {
    Log.print("failed, rc=");
    Log.print(client.state());
    Log.println(" try again in 5 seconds");
  }
}

void setup(){

  //init digit pins output
  pinMode(16, OUTPUT); //D0
  pinMode(5, OUTPUT);  //D1
  pinMode(4, OUTPUT);  //D2
  pinMode(0, OUTPUT);  //D3

  //reset all pins to off
  digitalWrite(16, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(0, HIGH);

  //init serial
  Serial.begin(115200);

  //init wifi
  setup_wifi();

  //init mqtt
  setup_mqtt();

  //init OTA
  setup_ota();

  //init log server (port 8266)
  logServer.begin();
  Log.println("Log server ready on port 8266");
}

void loop() {

  ArduinoOTA.handle();

  // Accept new log client if one is waiting
  if (logServer.hasClient()) {
    logClient = logServer.accept();
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  long now = millis();
  if (now - lastMsg > 5000) { //will say it's alive every 5 sec
    lastMsg = now;
    DateTime.getTime();
    char msg[128];
    sprintf(msg, "Ambrogino is live+ota / wifi: %s / ip: %s / topic: %s / ts: %s", ssid, WiFi.localIP().toString().c_str(), mqtt_topic, DateTime.toISOString().c_str());
    Log.print("Publish message: ");
    Log.println(msg);
    client.publish("irrigation_msg", msg);
  }
  
}
