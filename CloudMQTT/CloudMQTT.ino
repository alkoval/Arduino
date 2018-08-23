/*
  I'll refactore the code later =)
*/

#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "PubSubClient.h"

char ssid[] = "xxxxx"; // SSID WiFi AP
char pass[] = "xxxxx"; // WiFi password

String mqtt_id = "Arduino101"; // Unique id
const char* mqtt_server = "m11.cloudmqtt.com"; // Instance info - Server
const char* mqtt_user = "xxxxx"; // Instance info - User
const char* mqtt_pass = "xxxxx"; // Instance info - Password
const int mqtt_port = 15644; // Instance info - Port

long lastMsg = 0;
char msg[50];
int value = 0;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiEspClient espClient;
PubSubClient client(espClient);

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  
  printWifiStatus();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  delay(1000);
  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("test", msg);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_id.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("test", "hello world");
      // ... and resubscribe
      client.subscribe("test");
      Serial.print("subscribe...");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    delay(5000);
  }
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
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
