#include "DHTesp.h"
#include <cstdlib>
#include <time.h>
#include <WiFi.h>          
#include <PubSubClient.h>  

#define ORG "b7ywuf"                      
#define DEVICE_TYPE "abcd"             
#define DEVICE_ID "12"            
#define TOKEN "12345678" 

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";        
char publishTopic[] ="iot-2/evt/Data/fmt/json";                                          
char authMethod[] = "use-token-auth";                                   
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;          

WiFiClient wifiClient;                                                 
PubSubClient client(server, 1883, wifiClient);    

const int DHT_PIN = 15;

bool is_ventilator_on = false;
bool is_sprayer_on = false;

float temperature  = 0;

int gas_ppm = 0;
int blaze = 0;
int flow = 0;

String blaze_status = "";
String hazard_status = "";
String sprayer_status = "";

DHTesp dhtSensor;


void setup() {
  Serial.begin(99900);

  /**** sensor pin setups ****/
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
 

   wifiConnect();
   mqttConnect();
}

void loop() {

  TempAndHumidity  data = dhtSensor.getTempAndHumidity();

  
  srand(time(0));

  //initial variable activities 
  temperature  = data.temperature;
  gas_ppm = rand()%1000;
  int flamereading = rand()%1024;
  blaze = map(flamereading,0,1024,0,1024);
  int flamerange = map(flamereading,0,1024,0,3);
  int flow = ((rand()%100)>50?1:0);

  //set a blaze status based on how close it is.....
  switch (flamerange) {
  case 2:    // A fire closer than 1.5 feet away.
    blaze_status = "Close Fire";
    break;
  case 1:    // A fire between 1-3 feet away.
    blaze_status = "Distant Fire";
    break;
  case 0:    // No fire detected.
    blaze_status = "No Fire";
    break;
  }

  //toggle the fan according to gas in ppm in the room
  if(gas_ppm > 100){
    is_ventilator_on = true;
  }
  else{
    is_ventilator_on = false;
  }

  //find the hazard status 'cause fake alert may be caused by some mischief activities
  if(temperature < 40 && flamerange ==2){
    hazard_status = "need auditing";
    is_sprayer_on = false;
  }
  else if(temperature < 40 && flamerange ==0){
    hazard_status = "nothing found";
    is_sprayer_on = false;
  }
  else if(temperature > 50 && flamerange == 1){
    is_sprayer_on = true;
    hazard_status = "moderate";
  }
  else if(temperature > 55 && flamerange == 2){
    is_sprayer_on = true;
    hazard_status = "severe";
  }else{
    is_sprayer_on = false;
    hazard_status = "nil";
  }


  //send the sprayer status
  if(is_sprayer_on){
    if(flow){
      sprayer_status = "working";
    }
    else{
      sprayer_status = "not working";
    }
  }
  else if(is_sprayer_on == false){
   sprayer_status = "now it shouldn't";
  }
  else{
    sprayer_status = "something's wrong";
  }

  //Obivously the output.It is like json format 'cause it will help us for future sprints
  String payload = "{\"sensor_values\":{";
  payload+="\"gas_ppm\":";
  payload+=gas_ppm;
  payload+=",";
  payload+="\"temperature\":";
  payload+=(int)temperature;
  payload+=",";
  payload+="\"blaze\":";
  payload+=blaze;
  payload+=",";
  payload+="\"flow\":";
  payload+=flow;
  payload+="},";
  payload+="\"output\":{";
  payload+="\"is_ventilator_on\":"+String((is_ventilator_on)?"true":"false")+",";
  payload+="\"is_sprayer_on\":"+String((is_sprayer_on)?"true":"false")+"";
  payload+="},";
  payload+="\"messages\":{";
  payload+="\"fire_status\":\""+blaze_status+"\",";
  payload+="\"flow_status\":\""+sprayer_status+"\",";
  payload+="\"hazard_status\":\""+hazard_status+"\"";

  payload+="}";
  payload+="}";
  //Serial.println(payload);

  
  if(client.publish(publishTopic, (char*) payload.c_str()))           
  {
    Serial.println("Publish OK");
  }
  else{
    Serial.println("Publish failed");
  }
  delay(1000);
  

  if (!client.loop()) 
  {
    mqttConnect();                              
  }
 

     
}


void wifiConnect()
{
  Serial.print("Connecting to "); 
  Serial.print("Wifi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());
  
}


void mqttConnect() 
{
  if (!client.connected()) 
  {
    Serial.print("Reconnecting MQTT client to "); 
    Serial.println(server);
    while (!client.connect(clientId, authMethod, token))
    {
      Serial.print(".");
      delay(500);
    }
    
    Serial.println();
  }
}
