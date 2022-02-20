
#include<ArduinoJson.h>
#include<WiFi.h>
#include<HTTPClient.h>
#include <ESP32Servo.h>
#define tckMs portTICK_PERIOD_MS

const char* ssid="Tee";
const char* password = "11111111";
const char* doorfood_url = "https://ecourse.cpe.ku.ac.th/exceed13/api/door/1";

const char* tiger_cross_url = "https://ecourse.cpe.ku.ac.th/exceed13/api/light";
const char* food_success_url = "https://ecourse.cpe.ku.ac.th/exceed13/api/food/success";


/*
const char* tiger_cross_url = "https://teekung.com/exceed/distance.php";
const char* food_success_url = "https://teekung.com/exceed/distance.php";
*/

int roomNum = 1;

int door_pin = 25;
int food_pin = 21;

int ldr_pin[3] = {33,32,34};
int cldr[3] = {0,0,0};
int deb_ldr[3] = {0,0,0};
int th_ldr[3] = {950,1050,1900};
int stat_ldr[3] = {0,0,0};
xTaskHandle ldrTask;
xTaskHandle servoTask;
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

Servo door_servo;
Servo food_servo;

void setup(){
  door_servo.attach(door_pin);
  food_servo.attach(food_pin);
    pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
  for(int i=0;i<3;i++){
    pinMode(ldr_pin[i],INPUT);
  }
    Serial.begin(9600);
    
  delay(4000);
  WiFi.mode(WIFI_STA);
  WiFi_Connect();
  digitalWrite(5,LOW);
  
    xTaskCreatePinnedToCore(ldr_check, "LDRTask", 10240, NULL, 1, &ldrTask, 1); // stable
    xTaskCreatePinnedToCore(servoCheck,"SERVOTask",10240, NULL,1,&servoTask,0);
}


int door = 0;
int food = 0;

void door_trigger(int s){
  if(s == 1){
    door_servo.write(135);
  }else{
    door_servo.write(45);
  }
}


void food_trigger(int s){
  if(s == 1){
    food_servo.write(160);
  }else{
    food_servo.write(65);
  }
}


const int _size = 2*JSON_OBJECT_SIZE(2);
StaticJsonDocument<_size> JSONGet;
char str[50];

void servoCheck(void *param){
  while(1){
    if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
  
      http.begin(doorfood_url);
  
      int httpCode = http.GET();
  
      if(httpCode == HTTP_CODE_OK){
        String payload = http.getString();
        DeserializationError err = deserializeJson(JSONGet, payload);
        if(err){
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(err.c_str());
        }else{
          Serial.println(httpCode);
          Serial.println(payload);

          int jsdoor = (int)JSONGet["door"];
          int jsfood = (int)JSONGet["food"];
          if(door != jsdoor){
            door = jsdoor;
            door_trigger(jsdoor);
            Serial.print("door change to ");
            Serial.println(door);
            //ประตู
          }
          if(food != jsfood){
            food = jsfood;
            food_trigger(jsfood);
             Serial.print("food change to ");
              Serial.println(food);
            //อาหาร
          }
        }
      }else{
        Serial.println(httpCode);
        Serial.println("ERROR on HTTP Request");
      }
    }else{
      WiFi_Connect();
    }
    
      for(int i=0;i<3;i++){
        Serial.print(stat_ldr[i]);
        Serial.print(" ");
      
     }
    vTaskDelay(2000/tckMs);
  }
}

char str2[50];
const int _size2 = 2*JSON_OBJECT_SIZE(2);
StaticJsonDocument<_size2> JSONPost;

void tigerCross(int n){

    if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;

    int httpCode;
      if(n == 0){
        Serial.println("FOOD SUCCESS");
        http.begin(food_success_url);
        
       http.addHeader("Content-Type","application/json");
      JSONPost["room"] = roomNum;
      //JSONPost["light"] = 1;
      serializeJson(JSONPost,str2);
      httpCode = http.PUT(str2);
      
      }else{
        Serial.println("TIGER CROSSING");
        http.begin(tiger_cross_url);
       http.addHeader("Content-Type","application/json");
      JSONPost["room"] = roomNum;

      serializeJson(JSONPost,str2);
      httpCode = http.POST(str2);
      }

  
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


int ldr_status(int pin,int c){
  int aread =analogRead(pin); 

  Serial.print("pin ");
  Serial.print(analogRead(ldr_pin[0]));
  Serial.print(" ");
  Serial.print(analogRead(ldr_pin[1]));
  Serial.print(" ");
   Serial.println(analogRead(ldr_pin[2]));

  if(aread > th_ldr[c]){
    return 1;
  }else{
    return 0;
  }
}


void ldr_check(void* parameter){

  while(1){
    for(int i=0;i<3;i++){
        int c1 = ldr_status(ldr_pin[i],i);


        if(c1 == 1){
        deb_ldr[i]++;
        if(deb_ldr[i] >= 5){
         deb_ldr[i] = 5;
        } 
        }else{
          deb_ldr[i]--;
          if(deb_ldr[i] <= 0){
          deb_ldr[i] = 0;
          }   
        }
    
      //Serial.print("Distance is ");//Print the analog value read via serial port
      //Serial.println(distance);//Print the analog value read via serial port
    
      if(deb_ldr[i] == 5){
     
         if(stat_ldr[i] == 0){
          Serial.print(i);
          Serial.println(" Stepped in");
          tigerCross(i);
        }
        stat_ldr[i] = 1;
        
      }else if(deb_ldr[i] == 0){

    
        stat_ldr[i] = 0;
        
      }

/*
     for(int i=0;i<3;i++){
        Serial.print(stat_ldr[i]);
        Serial.print(" ");
      
     }*/
     //Serial.println("");
     vTaskDelay(25 / portTICK_PERIOD_MS);
      
    }
  }
}


void loop(){

}
