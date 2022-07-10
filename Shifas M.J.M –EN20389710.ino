#include <WiFi.h>
#include <ThingSpeak.h>

#define CHANNEL_ID 1768150

const char* CHANNELAPIKEY =  "YLP59LB5B57CFLWE";      // reading ID key

int motor_Relay_pin = 14;
unsigned int dataFieldWatermeasure = 1;

const char* ssid = "Wifi Name";             // add your wifi name here
const char* password =  "password";         // add your wifi password here


float waterLevelmeasure = 2.25E-02;
WiFiClient client;

void motor_operation(){
  if(waterLevelmeasure < 30){                /// threshold value of minimum water level
    digitalWrite(motor_Relay_pin, HIGH);
  }else{
    digitalWrite(motor_Relay_pin, LOW);
  }
}

void data_Refer(){
  // connection configuration
  if ((WiFi.status() == WL_CONNECTED)){
    waterLevelmeasure = ThingSpeak.readFloatField( CHANNEL_ID, dataFieldWatermeasure, CHANNELAPIKEY );
    Serial.print("Water Level reading= ");
    Serial.print(waterLevelmeasure);
    Serial.println(" ");
    delay(15000);
  }
  else{
    Serial.println("Connection Problem occured");
  }
}

QueueHandle_t xQueue;

void setup() {
  pinMode(motor_Relay_pin, OUTPUT);
  
  // WiFi setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("_ .");
  }
  Serial.println( "Connected" );
  
  // cloud config
  ThingSpeak.begin(client);
  delay(1000);

  xQueue = xQueueCreate (8, sizeof(int));
      if(xQueue !=NULL){
        xTaskCreatePinnedToCore(data_Refer, "datareading", 1024,NULL,2,NULL,0);
        xTaskCreatePinnedToCore(motor_operation, "motoroperation", 1024,NULL,1,NULL,0);
        vTaskStartScheduler();
      }else{
        Serial.println("Queue isn't created");
      }
}

void loop() {
  
}
