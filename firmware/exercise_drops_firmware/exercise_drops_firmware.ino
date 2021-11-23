/*
* Title: Exercise Drop Tubes 
*
* Author: Simon Ndungu
*
* Date: 21st Nov 2021
*/
#include "Arduino.h"
#include "PCF8574.h"

/* U5 --> A0,A1,A2 = 0 => address => 0x20 */
/* Set i2c address */ 
// create an object of the PCF8574 
PCF8574 pcf8574_U1(0x20);

#define solenoid1_trig 8
#define solenoid2_trig 7
#define solenoid3_trig 6

#define speed_ctrl_btn 9
#define on_off_ctrl_btn 10

#define bat_level A0

#define speed1_led A1
#define speed2_led A2
#define speed3_led A3
#define status_led 13

#define rf_d0 2
#define rf_d1 3
#define rf_d2 4
#define rf_d3 5

float R1 = 20000;
float R2 = 10000;
float voltage_raw_val = 0.00;
float voltage_out = 0.00;
float battery_voltage = 0.00;
float correction_factor = 0.13;

int random_tube;
int previous_random_tube;
int speed_ctrl_btn_state = 0;
int on_off_ctrl_btn_state = 0;


unsigned long currentMillis=0;
unsigned long previousMillis = 0;
int speed1 = 1000; // value can be changed by buttons
int speed2 = 2000;
int speed3 = 3000; 
int interval; 

bool level1_speed = true;
bool level2_speed = false;
bool level3_speed = false;

int count = 0;



void setup() {
  // put your setup code here, to run once:
	  pcf8574_U1.pinMode(P0, OUTPUT); // solenoid 6
      pcf8574_U1.pinMode(P1, OUTPUT); // solenoid 7
      pcf8574_U1.pinMode(P2, OUTPUT); // solenoid 8
      pcf8574_U1.pinMode(P3, OUTPUT); // solenoid 4
      pcf8574_U1.pinMode(P5, OUTPUT); // solenoid 5
      pcf8574_U1.pinMode(P6, OUTPUT); // solenoid 10
      pcf8574_U1.pinMode(P7, OUTPUT); // RF VT signal

      pinMode(solenoid1_trig, OUTPUT);
      pinMode(solenoid2_trig, OUTPUT);
      pinMode(solenoid3_trig, OUTPUT);

      pinMode(speed1_led, OUTPUT);
      pinMode(speed2_led, OUTPUT);
      pinMode(speed3_led, OUTPUT);
      pinMode(status_led, OUTPUT);

      pinMode(speed_ctrl_btn, INPUT);
      pinMode(on_off_ctrl_btn, INPUT);
      pinMode(bat_level, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
   unsigned long currentMillis = millis();

   execute_speed_change();
   
   generate_random_tube(); // executes the tubes

}

void take_reading(){
  voltage_raw_val = analogRead(bat_level);
  speed_ctrl_btn_state = digitalRead(speed_ctrl_btn);
  on_off_ctrl_btn_state = digitalRead(on_off_ctrl_btn);

  Serial.print("Raw Value");
  Serial.println(voltage_raw_val);
}

void check_battery_voltage(){
  take_reading();
  voltage_out = (voltage_raw_val * 5) / 1024; 
  battery_voltage = voltage_out / (R2/(R1+ R2)) - correction_factor;
  Serial.print("Measured Voltage is: ");
  Serial.print(battery_voltage);
  Serial.println("V");
  }


void speed1_led_on(){
   digitalWrite(speed1_led, HIGH);
}
void speed1_led_off(){
  digitalWrite(speed1_led, LOW);
}
void speed2_led_on(){
  digitalWrite(speed2_led, HIGH);
}
void speed2_led_off(){
  digitalWrite(speed2_led, LOW);
}
void speed3_led_on(){
  digitalWrite(speed3_led, HIGH);
}
void speed3_led_off(){
  digitalWrite(speed3_led, LOW);
}

int generate_random_tube(){
   if ((unsigned long )(currentMillis - previousMillis) >= interval) {
      random_tube = random(11);
      Serial.print("Random Number");
      Serial.println(random_tube);
      previousMillis = currentMillis;
      previous_random_tube = random_tube;
   }

   return random_tube;
   trigger_solenoids();
}

void listen_change_speed(){
   if(speed_ctrl_btn_state == 1){
      count +=1;
   }

   if(count == 1){
   	 level1_speed = true;
     level2_speed = false;
     level3_speed = false;
   }

   if(count == 2){
     level1_speed = false;
     level2_speed = true;
     level3_speed = false;
   }

   if(count == 3){
     level1_speed = false;
     level2_speed = false;
     level3_speed = true;
   }

   if(count > 3){
    count = 1; //resets count to 1 if val > 3(reps speed levels)
   }
   
}

void execute_speed_change(){
  listen_change_speed();
  if(level1_speed){
    speed1_led_on();
    speed2_led_off();
    speed3_led_off();
    interval = speed3; // level 1 is the slowest speed 
  }
    if(level2_speed){
    speed1_led_off();
    speed2_led_on();
    speed3_led_off();
    interval = speed2; // level 2 is the medium speed 
  }
    if(level3_speed){
    speed1_led_off();
    speed2_led_off();
    speed3_led_on();
    interval = speed1; // level 1 is the fastest speed 
  }
}

void trigger_solenoids(){
 int tube = generate_random_tube();
 Serial.print("Random Tube Passed is: ");
 Serial.println(tube);
 if(tube == 1){
   digitalWrite(solenoid1_trig, HIGH);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 2){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, HIGH);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 3){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, HIGH);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 4){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, HIGH);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 5){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, HIGH);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 6){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, HIGH);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 7){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, HIGH);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 8){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, HIGH);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 9){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, HIGH);
   pcf8574_U1.digitalWrite(P6, LOW);
 }
  if(tube == 10){
   digitalWrite(solenoid1_trig, LOW);
   digitalWrite(solenoid2_trig, LOW);
   digitalWrite(solenoid3_trig, LOW);
   pcf8574_U1.digitalWrite(P0, LOW);
   pcf8574_U1.digitalWrite(P1, LOW);
   pcf8574_U1.digitalWrite(P2, LOW);
   pcf8574_U1.digitalWrite(P3, LOW);
   pcf8574_U1.digitalWrite(P4, LOW);
   pcf8574_U1.digitalWrite(P5, LOW);
   pcf8574_U1.digitalWrite(P6, HIGH);
 }
 
}
