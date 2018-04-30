#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SFE_BMP180.h>
#include "ini.h"
#include "array_lib.h";
LiquidCrystal_I2C lcd(63, 16, 2);
SFE_BMP180 pressure;
#define ALTITUDE 1655.0
#define Relay_Pin 6

void setup() {
  Serial.begin(9600);
  // delay(100);
  // Serial.print("Start");
  // char header[100];
  // sprintf(header,"[ Blattnr:XXX   WkaNr:XXX   Datum:%s   Rep.Stelle:XXX    Startzeit:%s ]",__DATE__,__TIME__);
  // Serial.println(header);
  get_temperature();
  print_status();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(button_1,INPUT);
  pinMode(button_2,INPUT);
  pinMode(led_green,OUTPUT);
  pinMode(led_red,OUTPUT);
  pinMode(Relay_Pin, OUTPUT);
  pressure.begin();
}
void loop() {
  if (self_delay_1()){
    get_temperature_average();
    print_menu();
  }
  convert_time();
  check_buttons();
  op_relay();
}
//-------Temperature-Control---------
int relay(int a){
  if (temp_av < next_temp){
    // prev = millis();
    load_state = true;
    digitalWrite(Relay_Pin,HIGH);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_red, LOW);
  }
  else{
    temp_interval_step = a;
    check_state();
    load_state = false;
    digitalWrite(Relay_Pin,LOW);
    digitalWrite(led_green, LOW);
    digitalWrite(led_red, HIGH);
  }
}
int op_relay(){
  if (state > 0 and state < (max_temp-start_temp)/temp_step){//regeln im Betrieb
    relay(interval_step[1]);// change self_diffMillis_3 interval >>1min
  }
  if (state == 0){
    if (temp_av < next_temp){
      relay(interval_step[0]);// change self_diffMillis_3 interval >>20min
    }else{
      relay(interval_step[3]);//jump to next step
    }
  }
  if (state == (max_temp-start_temp)/temp_step){
    relay(interval_step[2]); // change self_diffMillis_3 interval >>240min
  }
}
int check_state(){
    if (temp_av < max_temp){
      if (temp_av >= next_temp){
        if (start_and_check_countdown()){
          state ++;
          // self_diffMillis_3 = 0;
      }
        if (last_status != state){
           next_temp += temp_step;
           last_status = state;
        }
      }
      // else{
      //   self_diffMillis_3 = 0;
      // }
  //  }else{
  //    relay(interval_step[3]);
   }
}
bool start_and_check_countdown(){
  self_delay_3();
  if (self_diffMillis_3 == ((interval*temp_interval_step)/1000)){
    return (true);
  }else{
    return (false);
  }
}
int self_delay_3(){
  if ((millis()-prev) > interval*(temp_interval_step)) {//*60
      prev = millis();
      self_diffMillis_3 = ((prev - millis())/1000)+1;
      }
    else {
      self_diffMillis_3 = ((millis() - prev)/1000)+1;
    }
}
//-------working-----
char * convert_countdown(){
  int countdown = ((interval*temp_interval_step)/1000)-self_diffMillis_3;
  char output[10];
  int hrs = countdown / 3600;
  countdown = countdown % 3600;
  int mins = countdown / 60;
  countdown = countdown % 60;
  int secs = countdown;
  char str_temp[6];
  dtostrf(next_temp, 4, 1, str_temp);
  if (load_state){
    sprintf(output,"heat to %s       ",str_temp);
  }else{
    sprintf(output,"%02d:%02d:%02d wait",hrs,mins,secs);
  }
  return (output);
}
void convert_time(){
  main_time = millis();
  if ((main_time - prev_time) >= sec_interval){
    prev_time = main_time;
    second ++;
    if (second > 59){
      minute++;
      second = 0;
    }
    if (minute > 59){
      hour++;
      minute = 0;
    }
    if (second%print_interval == 0){
      //print_status();
    }
  }
}
int check_buttons(){
  uint8_t read_1 = digitalRead(button_1);
  uint8_t read_2 = digitalRead(button_2);
  if (read_1 != lastbutState_1){
    lastDebounceTime = millis();
  }
  if (read_1 != lastbutState_1){
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay){
    if (read_1 != butState_1){
      butState_1 = read_1;
      if (butState_1 == HIGH){
        if (line >= last_line){
          line = last_line;
        }else{
          line += 1;
        }
        menu_state = 1;
        lcd.clear();
      }
    }
  }
  lastbutState_2 = read_2;
  if (read_2 != lastbutState_2){
    lastDebounceTime = millis();
  }
  if (read_2 != lastbutState_2){
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay){
    if (read_2 != butState_2){
      butState_2 = read_2;
      if (butState_2 == HIGH){
        if (line <= 0){
          line = 0;
        }else{
          line -= 1;
        }
        menu_state = 0;
        lcd.clear();
      }
    }
  }
  lastbutState_1 = read_1;
  lastbutState_2 = read_2;
}
bool self_delay_1(){
    self_currentMillis_1 = millis();
  if ((unsigned long)(self_currentMillis_1 - self_previousMillis_1) >=self_interval_1) {
      self_previousMillis_1 = self_currentMillis_1;
      return (true);
      }
    else {return(false);}
}
//--------Sensors----------
float get_temperature_average(){
  float val = 0;
  if (step < step_length){
    val = get_temperature();
    temperature += val;
    step ++;
  }else{
    step = 0;
    temp_av = temperature/step_length;
    print_menu();
    temperature = 0;
  }
}
float get_temperature(){
  float val = analogRead(temperature_Pin_1);
  float mv = ( val/1024.0)*5000;
  float res = mv/10;
  return (res);
}
double get_pressure(){
  char state_p;
  state_p = pressure.startPressure(3);
  double T,P,p0,a;
  if (state_p != 0){
    delay(state_p);
    state_p = pressure.getPressure(P,T);
    if (state_p != 0){
      return (P);
    }
  }
}
int print_status(){
  char _log[100];
  char str_temp[6];
  char str_press[10];
  dtostrf(temp_av, 4, 1, str_temp);
  dtostrf(get_pressure(), 4, 1, str_press);
  sprintf(_log,"%d ; %02d:%02d:%02d ; %s ; %s ; hum:___%%",state,hour,minute,second,str_temp,str_press);
  Serial.println (_log);
}
//--------Menu------------
int print_menu(){
  // uint8_t y;
  if (line <= last_line or line >= 0){  //normal Mode between 0-5
    functions[line](0);
    functions[line+1](1);
  }
}
void title_line(int y){
  lcd.setCursor(0,y);
  lcd.print("CpMax TeprOS v2 ");
}
void time_line(int y){
  lcd.setCursor(4,y);
  char _log[100];
  sprintf(_log,"%02d:%02d:%02d",hour,minute,second);
  lcd.print(_log);
}
void press_line(int y){
  lcd.setCursor(0,y) ;
  lcd.print("Press:    ") ;
  lcd.setCursor(10,y);
  lcd.print(get_pressure(),1);
}
void temp_line(int y){
  lcd.setCursor(0,y) ;
  lcd.print("Temp :   ") ;
  lcd.setCursor(10,y);
  lcd.print(temp_av,1);
}
void state_line(int y){
  char _t[100];
  // char str_temp[6];
  // dtostrf(next_temp, 4, 1, str_temp);
  sprintf(_t,"%s",convert_countdown());
  lcd.setCursor(0,y);
  lcd.print(_t);
}
void empty_line(int y){
  lcd.setCursor(0,y);
  lcd.print("Ende");
}
