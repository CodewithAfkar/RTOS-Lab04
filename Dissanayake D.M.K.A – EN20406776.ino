#include <ThingSpeak.h>       // ThingSpeak header file importing
#include <WiFi.h>             // Wifi header importing

#define CHANNEL_ID 1768150        // channel ID

const char* CHANNELAPIKEY =  "YLP59LB5B57CFLWE";      // channel API key

unsigned int dataFieldSolimoisture = 2;     // data field of soil moisture in the cloud

const char* ssid = "SLT-4G-2913";             // SSID of the Wifi
const char* password =  "safihsmj07";         // Wifi Password

int solenoid_pin = 12;              // solenoid pin declaration

float soilMoisture = 2.25E-02;      // float datatype variable to save soilmoisture
WiFiClient client;

QueueHandle_t xQueue;

void cloud_Reading(){       // cloud reading task
  
  // checking the connection
  if ((WiFi.status() == WL_CONNECTED)){
    soilMoisture = ThingSpeak.readFloatField( CHANNEL_ID, dataFieldSolimoisture, CHANNELAPIKEY );
    Serial.println(" ");
    Serial.print("Soil moisture reading= ");
    Serial.print(soilMoisture);
    Serial.println(" ");
    delay(15000);
  }
  else{
    Serial.println("Connection Error!!");
  }
}

void valve_Control(){         // Valve controlling task
  if(soilMoisture < 50){                // checking for threshold value
    digitalWrite(solenoid_pin, HIGH);   // turning ON the Valve
  }else{
    digitalWrite(solenoid_pin, LOW);    // Turning OFF the valve
  }
}

void setup() {
  pinMode(solenoid_pin, OUTPUT);    // pin mode configuration
  
  // cloud setup
  ThingSpeak.begin( client);
  
  // WiFi setup
  WiFi.begin(ssid, password);             // Wifi connection
  while (WiFi.status() != WL_CONNECTED){    // checking for the connectivity
    delay(500);             // delay for 500 mS
    Serial.print(".");
  }
  Serial.println( "Connected to Wifi Network" );
  delay(1000);    // delay for 1 sec
  

  xQueue = xQueueCreate (8, sizeof(int));
      if(xQueue !=NULL){
        xTaskCreatePinnedToCore(cloud_Reading, "cloudreading", 1024,NULL,2,NULL,0);       // Task 1 implementation
        xTaskCreatePinnedToCore(valve_Control, "valvecontrol", 1024,NULL,1,NULL,0);       // Task 2 implementation
        vTaskStartScheduler();
      }else{
        Serial.println("Queue has not been created");     // Print this statement if Queue is not created
      }

}

void loop() {
  
}
