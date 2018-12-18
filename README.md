# IoTs_LoRa_MQTT_Experiment_1
Simple IoTs Solution Using LoRa and MQTT

My LoRa based Small IoTs Solution 

(LoRa, MQTT, MySQL, NodeJs)

Device used

SX1278 LoRa Module x 2 

Arduino Uno

ESP8266 NodeMCU

--------------------------------------------------------------------------------------------------------------------------------------------------

Transmitter (Arduino Uno + SX1278 LoRa Module) Transmitting DHT22 Sensor Data Room Temperature and Humidity.

Receiver (ESP8266NodeMCU + SX1278 LoRa Module) : Receive Sensor Data from Transmitter and Posting(Publishing) it to Web Data Server using MQTT.

Web(Data) Server: A Node JS subscribes to all the topic get data from receiver and insert it into MySQL database.

MySQL: Database used to data.

Node JS: TO perform MQTT Pub-Sub tasks.

Mosquitto: MQTT Broker. Installed in WebServer.

-------------------------------------------------------------------------------------------------------------------------------------------------------
SX1278 Code and Library from 
http://wirelessopensource.com
https://github.com/wirelessopensource/lora_shield_arduino
