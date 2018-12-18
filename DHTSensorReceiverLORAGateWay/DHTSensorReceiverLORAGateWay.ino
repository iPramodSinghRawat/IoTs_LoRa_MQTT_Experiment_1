#include "SX1278.h"
#include <SPI.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define LORA_MODE 4
#define LORA_CHANNEL CH_6_BW_125
//#define LORA_ADDRESS 4
#define LORA_ADDRESS 4

//#define LORA_LED 5
#define LORA_DEVICE_STATUS_LED  D4
#define LORA_LED D3

int e;
char my_packet[100];

const char* ssid = "wifi-ssid";
const char* password = "wifi-password";
const char* mqtt_server = "192.168.100.3";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char* msg="";
int value = 0;

String clientId="DHTNodeMCU1";// MQTT Client of this Device
char* message="Hello From DHTLoraSensor1";
//char* topic1 ="publish1";
char* topic2Publish ="DHTSensorData";

void setup()
{
  pinMode(LORA_LED, OUTPUT);
  pinMode(LORA_DEVICE_STATUS_LED, OUTPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // Print a start message
  Serial.println(F("sx1278 module and Arduino: receiving packets"));

  // Power ON the module
  if (sx1278.ON() == 0) {
    Serial.println(F("Setting power ON: SUCCESS "));    
    digitalWrite(LORA_DEVICE_STATUS_LED, HIGH);
  } else {
    Serial.println(F("Setting power ON: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED, LOW);
  }

  // Set transmission mode and print the result
  if (sx1278.setMode(LORA_MODE) == 0) {
    Serial.println(F("Setting Mode: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED, HIGH);
  } else {
    Serial.println(F("Setting Mode: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED, LOW);
  }

  // Set header
  if (sx1278.setHeaderON() == 0) {
    Serial.println(F("Setting Header ON: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED, HIGH);
  } else {
    Serial.println(F("Setting Header ON: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED, LOW);
  }

  // Select frequency channel
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    Serial.println(F("Setting Channel: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED, HIGH);
  } else {
    Serial.println(F("Setting Channel: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED, LOW);
  }

  // Set CRC
  if (sx1278.setCRC_ON() == 0) {
    Serial.println(F("Setting CRC ON: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED, HIGH);
  } else {
    Serial.println(F("Setting CRC ON: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED, LOW);
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
    Serial.println(F("Setting Power: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED, HIGH);
  } else {
    Serial.println(F("Setting Power: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED, LOW);
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
    Serial.println(F("Setting node address: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED, HIGH);
  } else {
    Serial.println(F("Setting node address: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED, LOW);
  }

  // Print a success message
  Serial.println(F("sx1278 configured finished"));
  Serial.println();

  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(void)
{
  // Receive message for 10 seconds
  e = sx1278.receivePacketTimeout(9000);

  Serial.print(F("e = "));
  Serial.println(e);
  
  if (e == 0) {
    digitalWrite(LORA_LED, HIGH);
      
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
    }
    
    Serial.print(F("Message: "));
    Serial.println(my_packet);
    mqttPublishDHTSensorData();
  } else {
    Serial.print(F("Package received ERROR\n"));
  }
  delay(1000);  
  digitalWrite(LORA_LED, LOW);
}

void setup_wifi(){
  delay(1000);
  WiFi.mode(WIFI_OFF);//Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);//This line hides the viewing of ESP as wifi hotspot  
  WiFi.begin(ssid, password);//Connect to your WiFi router 
  Serial.println("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  } 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());//IP address assigned to your ESP
}

void mqttPublishDHTSensorData() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    
    String completeMessage="[{'client':'"+clientId+"'},"+my_packet+",{'value':'"+String(value)+"'}]";
    
    Serial.println("Publish CompleteMessage: "+completeMessage);
    
    client.publish(topic2Publish, completeMessage.c_str());
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  //Serial.print("\n payloadString char *: ");
  Serial.println((char *)payload);
  
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId.c_str())) {    
      Serial.println("connected");
      // Once connected, publish an announcement...
      String completeMessage=clientId+","+message;

      Serial.println("Connected CompleteMessage: " + completeMessage);
      client.publish(topic2Publish, completeMessage.c_str());
      // ... and resubscribe
      //client.subscribe(topic1);///Subscribe to topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
