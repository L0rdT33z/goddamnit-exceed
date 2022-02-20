#include "Ultrasonic.h"
#include<WiFi.h>
#include<HTTPClient.h>
#include<ArduinoJson.h>

int buzzerPin = 14; //buzzerPin
Ultrasonic ultrasonic(19); //ultraSonic

int distanceAll = 30;
int vibrationALL = 300;

xTaskHandle distanceTask;
xTaskHandle wifiTask;
int roomNum = 1;
const char* ssid="Tee";
const char* password = "11111111";
const char* distance_url = "https://ecourse.cpe.ku.ac.th/exceed13/api/Danger_Distance";
const char* vibration_url = "https://ecourse.cpe.ku.ac.th/exceed13/api/vibrate";


void setup()
{

    pinMode (buzzerPin, OUTPUT); // BUZZER
        pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
    Serial.begin(9600);
      delay(4000);
  WiFi.mode(WIFI_STA);
   WiFi_Connect();
digitalWrite(5,LOW);
  Serial.begin(9600);
    xTaskCreatePinnedToCore(distanceCheck, "DistanceTask", 10240, NULL, 1, &distanceTask, 0); // stable
    xTaskCreatePinnedToCore(wifiCheck, "wifiTask", 10240, NULL, 1, &wifiTask, 1); // stable
}


char str[50];
const int _size = 2*JSON_OBJECT_SIZE(2);
StaticJsonDocument<_size> JSONPost;

char str2[50];
const int _size2 = 2*JSON_OBJECT_SIZE(2);
StaticJsonDocument<_size2> JSONPost2;

void WiFi_Connect(){
  WiFi.disconnect();
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
}


void distance_post(int s){
    if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
  
      http.begin(distance_url);
      http.addHeader("Content-Type","application/json");
     JSONPost["room"] = roomNum;
      JSONPost["danger"] = s;  
      serializeJson(JSONPost,str);
      int httpCode = http.PUT(str);
  
      if(httpCode == HTTP_CODE_OK){
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
        
      }else{
        Serial.println(httpCode);
        Serial.println("ERROR on HTTP Request");
      }
    }else{
      WiFi_Connect();
    }
}



void vibrate_post(int s){
    if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
  
      http.begin(vibration_url);
      http.addHeader("Content-Type","application/json");
      JSONPost2["room"] = roomNum;
      JSONPost2["vibrate"] = s;
      serializeJson(JSONPost2,str2);
      int httpCode = http.PUT(str2);
  
      if(httpCode == HTTP_CODE_OK){
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
        
      }else{
        Serial.println(httpCode);
        Serial.println("ERROR on HTTP Request");
      }
    }else{
      WiFi_Connect();
    }
}


int distance;
int vibrateVal;
long ultraSonic(){ // return cm
  
    long RangeInCentimeters;
    RangeInCentimeters = ultrasonic.MeasureInCentimeters(); // two measurements should keep an interval
    distance = RangeInCentimeters;
    return RangeInCentimeters;
}


long ultraSonic_distancecheck(){ // return cm
  
    long cm = ultraSonic(); 
    if(cm <= distanceAll){
        return 1;
    }else{
        return 0;
    }
}


void buzzer_scream(){ // return cm
  digitalWrite (buzzerPin, HIGH);
  return;
}

void buzzer_silent(){ // return cm
  
  digitalWrite (buzzerPin, LOW);
  return;
}


int debounce_ultrasonic = 0;
int debounce_vibration = 0;

int buzzerStatus = 0;


int vibrate(){
  
    int val;
    val=analogRead(32);
   // Serial.print("Ez ");
    //Serial.println(val,DEC);
    vibrateVal = val;
    if(val >= vibrationALL){
      return 1;
    }else{
      return 0;
    }
}


int vibrates = 0;



int oldBuzzer = 0;
int oldVibrate = 0;

void wifiCheck(void* parameter){
  while(1){
    int currentBuzzer = buzzerStatus;
    if(currentBuzzer != oldBuzzer ){
      oldBuzzer = currentBuzzer;
      Serial.println("Distance sent");
    distance_post(currentBuzzer);
    }

 int currentVibrate = vibrates;
    if(currentVibrate != oldVibrate ){
      oldVibrate = currentVibrate;
      Serial.println("Vibrate sent");
     vibrate_post(currentVibrate);
    }


    
    vTaskDelay(50 / portTICK_PERIOD_MS);
    
  }
}

void distanceCheck(void* parameter){
  while(1){

  int c1 = ultraSonic_distancecheck();
  
   if(c1 == 1){
    debounce_ultrasonic++;
    if(debounce_ultrasonic >= 20){
     debounce_ultrasonic = 20;
    } 
  }else{
    debounce_ultrasonic--;
    if(debounce_ultrasonic <= 0){
     debounce_ultrasonic = 0; 
    }   
  }

  //Serial.print("Distance is ");//Print the analog value read via serial port
 // Serial.println(distance);//Print the analog value read via serial port

  if(debounce_ultrasonic == 20){
    
    buzzerStatus = 1;
         
    
  }else if(debounce_ultrasonic == 0){

    buzzerStatus = 0;
     
  }

  if(buzzerStatus == 1){
    //Scream
      buzzer_scream();
  }else{
    //Silent
      buzzer_silent();
  }






  //VibrateCheck

   int c2 = vibrate();


   if(c2 == 1){
    debounce_vibration++;
    if(debounce_vibration >= 2){
     debounce_vibration = 2;
    } 
  }else{
    debounce_vibration--;
    if(debounce_vibration <= 0){
     debounce_vibration = 0;
    }   
  }

 Serial.print("Vibrate is ");//Print the analog value read via serial port
Serial.println(vibrateVal);//Print the analog value read via serial port

  if(debounce_vibration == 2){
    vibrates = 1;
    
  }else if(debounce_vibration == 0){
    vibrates = 0;
  }

  if(vibrates == 1){
    //Scream
    //  Serial.print("Vibrated ");
    //  Serial.println(vibrateVal);
  }else{
    //Silent
  }

  vTaskDelay(25 / portTICK_PERIOD_MS);
  }
   
}





void loop()
{

  

    
}
