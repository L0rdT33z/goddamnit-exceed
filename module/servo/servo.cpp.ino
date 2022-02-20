#include <ESP32Servo.h>
//PRatu 25
//
Servo door_servo;
Servo food_servo;

int door_pin = 25;
int food_pin = 21;

void setup() {

 door_servo.attach(door_pin);
  food_servo.attach(food_pin);
  door_servo.write(135);
}

void loop() {
 // door_servo.write(45); // สั่งให้ Servo หมุนไปองศาที่ 0 // ปิด
  //door_servo.write(135); // สั่งให้ Servo หมุนไปองศาที่ 0 // gปิด
  //Pratu
  
  //food_servo.write(65); // สั่งให้ Servo หมุนไปองศาที่ 0 // ปิด
  //food_servo.write(160); // สั่งให้ Servo หมุนไปองศาที่ 0 // gปิด
  //ตัวอักษรลงล่าง
}
