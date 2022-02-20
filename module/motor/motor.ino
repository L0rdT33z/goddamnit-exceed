int MotorPin2 = 18;
int MotorPin3 = 19;
void setup() {
  pinMode(MotorPin2, OUTPUT);
  pinMode(MotorPin3, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  digitalWrite(MotorPin2, HIGH); //สั่งงานให้โมดูลขับมอเตอร์จ่ายไฟ ออกขา OUT3
  digitalWrite(MotorPin3, LOW); //สั่งงานให้ขา OUT4  เป็นขารับไฟจากขา OUT3
  Serial.println("Motor Right");
  delay(2000);


}
