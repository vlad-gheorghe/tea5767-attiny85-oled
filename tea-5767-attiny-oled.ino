#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include <TEA5767Tiny.h>

#include <EEPROM.h>

 const int analogInPin = PB3;
 int buttonvalue =0;
TEA5767Tiny Radio;
double stored_frequency;
int search_mode = 0;
int search_direction;
int mute;

unsigned long current_time, last_written;

boolean state = false;

double load_frequency() {
  int a = EEPROM.read(0);
  int b = EEPROM.read(1);
  if (a > 0) {
    return a + ((float) b/100);
  }
  return 0;
}

void store_frequency(double f) {
  int a = (int) f;
  int b = (f - a) * 100;
  EEPROM.write(0, a);
  EEPROM.write(1, b); 
}

void setup() {
  Radio.init();
   oled.begin();
  oled.clear();
  oled.on();
   oled.setCursor(60, 0);oled.print("MHz");
    oled.setCursor(0, 10);oled.print("TEA5767-ATTINY85");
   // oled.setCursor(0, 20);oled.print("@ Vlad Gheorghe");
  stored_frequency = load_frequency();
  if (stored_frequency >= 88.5 && stored_frequency <= 108.00) {
    Radio.set_frequency(stored_frequency);
  } else {
    Radio.set_frequency(101.1);  
  }
   
//  led.on();
}

void loop() {
// oled.setFont(FONT8x16);
   //oled.setCursor(60, 0);oled.print("MHz");
  unsigned char buf[5];
  double current_freq;
  
  current_time = millis();
   buttonvalue = analogRead(analogInPin);
  if (Radio.read_status(buf) == 1) {
    current_freq =  floor (Radio.frequency_available (buf) / 100000 + .5) / 10;
    if (current_freq != stored_frequency && current_time - last_written > 10000 && search_mode == 0) {
      stored_frequency = current_freq;
      store_frequency(stored_frequency);
      last_written = current_time;
      oled.setCursor(0, 0);oled.print(String(current_freq));
      // oled.setCursor(0, 10);oled.print("radio");
  
    }
  }
  
  if (search_mode == 1) {
//      led.off();
      if (Radio.process_search (buf, search_direction) == 1) {
          search_mode = 0;
//          led.on();
      }
  }
  
  if (buttonvalue<=0 && buttonvalue<450 ) {
    search_mode = 1;
    search_direction = TEA5767_SEARCH_DIR_UP;
    Radio.search_up(buf);
    delay(300);
  }
  
  if (buttonvalue>=300 && buttonvalue<550) {
    search_mode = 1;
    search_direction = TEA5767_SEARCH_DIR_DOWN;
    Radio.search_down(buf);
    delay(300);
  } 
  
  
  delay(50);
}
