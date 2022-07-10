#include <ThingSpeak.h>
#include <WiFi.h>

#define CHANNEL_ID 1792423
const char* CHANNELAPIKEY =  "SO0UYYU422YJW674";

const char* ssid = "Shams Villa";
const char* password =  "6076e7d1";

int water_level_pin = 36;
int water_value ;
int water_level_percent;

int soil_moisture_pin = 34;
int soil_value ;
int soil_moisture_percent;

WiFiClient Client; 

void Water_level( void * pvParameters ){

  for(;;){
    water_value= analogRead(water_level_pin);
    water_level_percent = map(water_value,0,1750,0,100);
    delay(1000);
    Serial.print("Water Level: ");
    Serial.println(water_level_percent);
  }
}

void Soil_moisture( void * pvParameters ){

  for(;;){
    soil_value= analogRead(soil_moisture_pin);
    soil_moisture_percent = map(soil_value,4095,1781,0,100);
    delay(1000);
    Serial.print("Soil Moisture: ");
    Serial.println(soil_moisture_percent);
  }
}

void cloud_update( void * pvParameters ){
  for(;;){
    if ((WiFi.status() == WL_CONNECTED)) //Check the current connection status
  {
    ThingSpeak.setField(3, water_level_percent);
    ThingSpeak.setField(4, soil_moisture_percent);
    ThingSpeak.writeFields(CHANNEL_ID,CHANNELAPIKEY);
    delay(15000);
  }
  else
  {
    Serial.println("Connection lost");
    WiFi_setup();
  }
  }
}

QueueHandle_t xQueue;
   
void setup() {
  Serial.begin(9600);
  ThingSpeak.begin(Client); 
  WiFi_setup();
  delay(1000);

  xQueue = xQueueCreate (8, sizeof(int));
      if(xQueue !=NULL){
        xTaskCreatePinnedToCore(Water_level, "waterlevel", 1024,NULL,1,NULL,0);
        xTaskCreatePinnedToCore(Soil_moisture, "soilmoisture", 1024,NULL,1,NULL,0);
        xTaskCreatePinnedToCore(cloud_update, "cloudupdate", 1024,NULL,2,NULL,1);
        vTaskStartScheduler();
      }else{
        Serial.println("Queue has not been created");
      }
             

}

void loop() {
}

void WiFi_setup(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
