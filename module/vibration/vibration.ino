void setup()
{
Serial.begin(9600); //Open the serial to set the baud rate for 9600bps
}
void loop()
{
int val;
val=analogRead(2); //Connect the analog piezoelectric ceramic vibration sensor to analog interface 0
Serial.print("Vibration is ");
Serial.println(val,DEC);//Print the analog value read via serial port
delay(100);
} 
