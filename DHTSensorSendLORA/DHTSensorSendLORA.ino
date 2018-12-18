#include "SX1278.h"
#include <SPI.h>
#include <DHT.h>

#define DHTPIN 4 // DHT Sensor Pin
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11 // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)

#define LORA_MODE  4
#define LORA_CHANNEL  CH_6_BW_125
#define LORA_ADDRESS  2
#define LORA_SEND_TO_ADDRESS  4
#define LORA_LED  5
#define LORA_DEVICE_STATUS_LED  6
//#define LORA_LED  D4 // for ESP8266NodeMCU

int e;

int dtSndCount=0;
DHT dht(DHTPIN, DHTTYPE);

void sendDHTSensorData(float,float); // receive array element value

void setup()
{
  pinMode(LORA_LED, OUTPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(LORA_DEVICE_STATUS_LED, OUTPUT);
    
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  //Serial.begin(115200);
    
  // Print a start message
  Serial.println(F("sx1278 module and Arduino: send two packets (One to an addrees and another one in broadcast)"));

  // Power ON the module
  if (sx1278.ON() == 0) {
    Serial.println(F("Setting power ON: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED,1);
  } else {
    Serial.println(F("Setting power ON: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED,0);
  }

  // Set transmission mode and print the result
  if (sx1278.setMode(LORA_MODE) == 0) {
    Serial.println(F("Setting Mode: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED,1);
  } else {
    Serial.println(F("Setting Mode: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED,0);
  }

  // Set header
  if (sx1278.setHeaderON() == 0) {
    Serial.println(F("Setting Header ON: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED,1);
  } else {
    Serial.println(F("Setting Header ON: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED,0);
  }

  // Select frequency channel
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    Serial.println(F("Setting Channel: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED,1);
  } else {
    Serial.println(F("Setting Channel: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED,0);
  }

  // Set CRC
  if (sx1278.setCRC_ON() == 0) {
    Serial.println(F("Setting CRC ON: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED,1);
  } else {
    Serial.println(F("Setting CRC ON: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED,0);
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
    Serial.println(F("Setting Power: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED,1);
  } else {
    Serial.println(F("Setting Power: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED,0);
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
    Serial.println(F("Setting node address: SUCCESS "));
    digitalWrite(LORA_DEVICE_STATUS_LED,1);
  } else {
    Serial.println(F("Setting node address: ERROR "));
    digitalWrite(LORA_DEVICE_STATUS_LED,0);
  }

  // Print a success message
  Serial.println(F("sx1278 configured finished"));
  Serial.println();
  
  dht.begin();
}

void loop(void)
{
  readDHTSensorData();
}

void readDHTSensorData(){
  
  // put your main code here, to run repeatedly:
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float hmdt = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float tmpr = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float fmpr = dht.readTemperature(true);

  //Serial.println("Read hmdt: "+String(hmdt));  
  //Serial.println("Read tmpr: "+String(tmpr));
  //Serial.println("Read fmpr: "+String(fmpr));
  // Check if any reads failed and exit early (to try again).
  if (isnan(hmdt) || isnan(tmpr) || isnan(fmpr)) {
    Serial.println("Failed to read from DHT sensor!");
  }else{
    Serial.println("Read hmdt: "+String(hmdt));  
    Serial.println("Read tmpr: "+String(tmpr));
    Serial.println("Read fmpr: "+String(fmpr));
    //postData = true;
    sendDHTSensorData(hmdt,tmpr);
  }

  delay(4000);
}

void sendDHTSensorData(float hmdt,float tmpr){
  String data2Send="";
  
  //{'data':'dht','count':'count'}
  dtSndCount=dtSndCount+1;
  char cstr[16];
  itoa(dtSndCount, cstr, 10);
  
  //Serial.print("dtSndCount: ");
  //Serial.println(dtSndCount);
   
  char hmdtChar[8]; // Buffer big enough for 7-character float
  dtostrf(hmdt, 6, 2, hmdtChar); // Leave room for too large numbers!
  //Serial.print("hmdtChar: ");
  //Serial.println(hmdtChar);
  
  char tmprChar[8]; // Buffer big enough for 7-character float
  dtostrf(tmpr, 6, 2, tmprChar); // Leave room for too large numbers!
  //Serial.print("tmprChar: ");
  //Serial.println(tmprChar);

  /*d=data
   * c=count
   * h=humidity
   * t=temperature
  */
  data2Send=data2Send+"{'d':'dht','c':'"+String(dtSndCount)+"','h':'"+hmdtChar+"','t':'"+tmprChar+"'}";

  data2Send.replace(" ", "");
  char data2SendChar[data2Send.length()];
  data2Send.toCharArray(data2SendChar, data2Send.length()+1);
   
  //data2Send=ss2;
  
  Serial.print("data2SendChar: ");
  Serial.println(data2SendChar);
  
  // Send message1 and print the result
  e = sx1278.sendPacketTimeout(LORA_SEND_TO_ADDRESS, data2SendChar);
  Serial.print(F("Packet sent, state "));
  Serial.println(e, DEC);

  if (e == 0) {
      digitalWrite(LORA_LED, HIGH);
      delay(500);
      digitalWrite(LORA_LED, LOW);
  }else{
    dtSndCount=dtSndCount-1;
  } 
   
}
