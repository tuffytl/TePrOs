//Time-Management
unsigned long main_time;
unsigned long prev_time;
uint8_t minute;
uint8_t second = 43;
uint8_t hour;
int sec_interval = 1000;
uint8_t state = 0;
uint8_t last_status = state;
// int sys_sec;
// int int_sec;
//Self-Delays
uint8_t self_interval_1=20;
unsigned long self_currentMillis_1 = 0;
unsigned long interval=1000;
unsigned long self_previousMillis_1=0;
unsigned long prev=0;
unsigned long self_currentMillis_3 = 0;
int self_diffMillis_3 = 0;
//Button-Management
uint8_t butState_1 ;
uint8_t butState_2 ;
uint8_t lastbutState_1 = LOW;
uint8_t lastbutState_2 = LOW;
unsigned long lastDebounceTime = 0;
uint8_t debounceDelay = 10;
uint8_t button_1 = 4;
uint8_t button_2 = 5;
uint8_t menu_state = 0;
//LCD-Menu
int line = 3;
uint8_t last_line = 4;
uint8_t print_interval = 60;
//Temperature-Sensorik
float temperature;
float temp_av;
uint8_t temperature_Pin_1 = 1;
float next_temp = 20;
int start_temp = 20;
float temp_step = 5;
float max_temp = 65;
boolean load_state = false;
int temp_interval_step;
int interval_step[] = {20*60,60,4*60*60,5};
//Timer-4-Temperature
uint8_t step = 0;
uint8_t step_length = 5;
//LED
uint8_t led_green = 2;
uint8_t led_red = 3;