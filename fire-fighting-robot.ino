#include "C:\Users\Van Truong\OneDrive\Documents\Arduino\PS2X_lib\examples\PS2X_Example\PS2X_lib.h"  //for v1.6

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        13  //14    
#define PS2_CMD        7  //15
#define PS2_SEL        2  //16
#define PS2_CLK        3  //17

#include <Servo.h>

Servo myservo;

int pos = 0;
boolean fire = false;
boolean is_active = false;
uint32_t tMillis = millis() + 1000;

 // Definde variables for Controller
#define PS2_DAT 13  //14    
#define PS2_CMD 7
#define PS2_SEL 2  //16
#define PS2_CLK 3

// Define sensor for arduino

#define Left 4          // left sensor
#define Right 5         // right sensor
#define Forward 6       //front sensor
#define LM1 8           // left motor
#define LM2 9           // left motor
#define RM1 10          // right motor
#define RM2 11          // right motor
#define pump 12         //water pum  

//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false

PS2X ps2x; // create PS2 Controller Class


int error = 0;
byte type = 0;
byte vibrate = 0;

void setup(){
 
  Serial.begin(9600);

  pinMode(Left, INPUT);
  pinMode(Right, INPUT);
  pinMode(Forward, INPUT);
  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(pump, OUTPUT);

  myservo.attach(A5);
  myservo.write(90);
  
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
   
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
}
}

void put_off_fire() {

    digitalWrite(pump,HIGH);
    delay(500);
 
    for (pos = 50; pos <= 110; pos += 1) { 
    myservo.write(pos); 
    delay(10);  
    }
  
    for (pos = 110; pos >= 50; pos -= 1) { 
    myservo.write(pos); 
    delay(10);
    }
    
    digitalWrite(pump,LOW);
    myservo.write(90); 
    fire=false;
}

void automatic_mode() {
    Serial.println("Auto Running!");  
    if (digitalRead(Left) == HIGH && digitalRead(Right) == HIGH && digitalRead(Forward) == HIGH) 
    {
    delay(500);
    digitalWrite(LM1, HIGH);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, HIGH);
    digitalWrite(RM2, HIGH);
    }
    else if (digitalRead(Forward) == LOW) 
    {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH);
    fire = true;
    
    }
    
    else if (digitalRead(Left) == LOW)
    {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, HIGH);
    }
    
    else if (digitalRead(Right) == LOW) 
    {
    digitalWrite(LM1, LOW);
    digitalWrite(LM2, HIGH);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, LOW);
    }

    delay(400);//change this value to change the distance
   
    while (fire == true)
    {
      put_off_fire();
      Serial.println("Fire Detected.");
    }
}

void loop() {
  myservo.write(90);

  if(error == 1) //skip loop if no controller found
    return;

  if (type == 1) { //DualShock Controller

    digitalWrite(LM1, LOW);
    digitalWrite(LM2, LOW);
    digitalWrite(RM1, LOW);
    digitalWrite(RM2, LOW);
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
        digitalWrite(LM1, LOW);
        digitalWrite(LM2, HIGH);
        digitalWrite(RM1, LOW);
        digitalWrite(RM2, HIGH);
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
        digitalWrite(LM1, LOW);
        digitalWrite(LM2, HIGH);
        digitalWrite(RM1, LOW);
        digitalWrite(RM2, LOW);
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
        digitalWrite(LM1, LOW);
        digitalWrite(LM2, LOW);
        digitalWrite(RM1, LOW);
        digitalWrite(RM2, HIGH);
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
        digitalWrite(LM1, HIGH);
        digitalWrite(LM2, LOW);
        digitalWrite(RM1, HIGH);
        digitalWrite(RM2, LOW);
    }   

    if (ps2x.ButtonPressed(PSB_CIRCLE)) {
      Serial.println("Circle just pressed");
      put_off_fire();
    }   

    if (digitalRead(Forward) == LOW || digitalRead(Left) == LOW || digitalRead(Right) == LOW) {
      automatic_mode();
    }
    
  }

  delay(50);  
}
