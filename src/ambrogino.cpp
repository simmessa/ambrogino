/*
  Ambrogino: Wireless irrigation with ESP8266
*/

#include <Arduino.h> //necessary for platformio
 
#include <ESP8266WiFi.h>  //Wifi that won't cause issues with esp8266
#include <PubSubClient.h> //Publisher subscriber
#include <ESPDateTime.h>  //For date

#include <settings.h>

int status = WL_IDLE_STATUS;

// Forward declaration (required by platformio)

void callback(char* topic, byte* payload, unsigned int length);


WiFiClient espClient;

PubSubClient client;
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_mqtt() {
  client.setClient(espClient);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("MQTT server: ");
  Serial.println(mqtt_server);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
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
    Serial.println("Failed to get time from server.");
  }
}

void setup_wifi() {
  delay(10);
  // Connect to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  randomSeed(micros());
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  setupDateTime();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("irrigation_msg", "hello world");
      // ... and resubscribe
      client.subscribe("irrigation");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
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
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  long now = millis();
  if (now - lastMsg > 2000) { //will say it's alive every 2 sec
    lastMsg = now;
    DateTime.getTime();
    snprintf (msg, 50, "ambrogino is live / %s", DateTime.toISOString().c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("irrigation_msg", msg);
  }
  
}
