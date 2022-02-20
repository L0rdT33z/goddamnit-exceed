#include <OneWire.h>
#include <DallasTemperature.h>
#include<WiFi.h>
#include<HTTPClient.h>
#include<ArduinoJson.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
int MotorPin2 = 18;
int MotorPin3 = 19;
int roomNum = 1;
float templimit = 27;

xTaskHandle tempTask;
xTaskHandle tempPostTask;

const char* ssid="Tee";
const char* password = "11111111";
const char* temperature_url = "https://ecourse.cpe.ku.ac.th/exceed13/api/temp";

void setup(void)
{
  // start serial port
  
  pinMode(MotorPin2, OUTPUT);
  pinMode(MotorPin3, OUTPUT);
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
  
  Serial.begin(9600);
        delay(4000);
  WiFi.mode(WIFI_STA);
   WiFi_Connect();
   digitalWrite(5,LOW);
  // Start up the library
  sensors.begin();

  xTaskCreatePinnedToCore(tempCheck, "tempTask", 10240, NULL, 1, &tempTask, 0);
  xTaskCreatePinnedToCore(temp_post, "tempPostTask", 10240, NULL, 1, &tempPostTask, 1); // stable
  //adding temp value
}
 
float temp = -1;

char str[50];
const int _size = 2*JSON_OBJECT_SIZE(2);
StaticJsonDocument<_size> JSONPost;

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



void temp_post(void* parameter){
  while(1){
    if(temp > 0 && temp < 50){
        if(WiFi.status() == WL_CONNECTED){
          HTTPClient http;
      
          http.begin(temperature_url);
          http.addHeader("Content-Type","application/json");
          JSONPost["room"] = roomNum;
          JSONPost["temp"] = temp;
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
       vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

float gettemp(){
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

int tempCheckStatus(){
  float tmp = gettemp();
  if(tmp < 0) return tmp;
    temp = tmp;
    if(tmp > templimit){
      return 1;
    }else{
      return 0;
    }
}

int debounce_temp = 0;
bool tempalert = false;

void MotorRight(){
  digitalWrite(MotorPin2, HIGH); //สั่งงานให้โมดูลขับมอเตอร์จ่ายไฟ ออกขา OUT3
  digitalWrite(MotorPin3, LOW); //สั่งงานให้ขา OUT4  เป็นขารับไฟจากขา OUT3
  return ;
}

void MotorStop(){
  digitalWrite(MotorPin2, LOW); //สั่งงานให้ขา OUT3 หยุดจ่ายไฟ
  digitalWrite(MotorPin3, LOW); //สั่งงานให้ขา OUT4 หยุดจ่ายไฟ
  return ;
}

void tempCheck(void* parameter){
 while(1){
  int c1 = tempCheckStatus();
  
   if(c1 == 1){
    debounce_temp++;
    if(debounce_temp >= 5){
     debounce_temp = 5;
    } 
  }else{
    debounce_temp--;
    if(debounce_temp <= 0){
     debounce_temp = 0;
    }   
  }
  if(temp > 0){
   Serial.print("Temp is ");//Print the analog value read via serial port
    Serial.println(temp);//Print the analog value read via serial port
  }
  if(debounce_temp == 5){
    tempalert = true;
    
  }else if(debounce_temp == 0){
    tempalert = false;
  }

  if(tempalert == true){
    MotorRight();
    Serial.print("Motor started ");
    
  }else{
    //Silent
    MotorStop();
    Serial.print("Motor stopped ");
    
  }
  
  vTaskDelay(25 / portTICK_PERIOD_MS);
  }
   

  
}


void loop(void)
{
  //
 
}
