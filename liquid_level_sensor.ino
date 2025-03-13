/***************************************************
* Techeonics
* Liquid Level Sensor-XKC-Y25-T12V Sensor
* @author - Gaurav Kumar (Techeonics) 

<thetecheonics@gmail.com>
Youtube- https://www.youtube.com/c/THETECHEONICS
See <http://www.techeonics.com> for details.

* ****************************************************/


int Liquid_level=0;

void setup() {
  // initialize LCD
  
  
 Serial.begin(9600);
 pinMode(8,INPUT);// sensor pin
}

void loop() {
 
Liquid_level=digitalRead(8);
Serial.print("Liquid_level= ");
Serial.println(Liquid_level,DEC);




delay(500);

}
