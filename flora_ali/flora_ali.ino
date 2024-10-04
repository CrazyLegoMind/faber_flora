// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#define LED_PIN 9
#define POT_PIN A10
#define MIC_PIN A11
#define WAVE_SPEED 1
#define LEDS_1 3
#define LEDS_2 6
#include <Wire.h>
#include <Adafruit_NeoPixel.h>


#define NUMPIXELS 6  // Popular NeoPixel ring size
Adafruit_NeoPixel wing1(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
unsigned long trigger_time = 0;
bool wave_ended = false;
uint8_t color_trigger[3];


void setup() {
  wing1.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(9600);
  pinMode(MIC_PIN, INPUT);
  pinMode(A10, INPUT);
  pinMode(LEDS_1, OUTPUT);
  pinMode(LEDS_2, OUTPUT);
}

void loop() {
  unsigned long current_time = millis();
  analogWrite(LEDS_1, (current_time%1531)*0.25);
  analogWrite(LEDS_2, (current_time%1531)*0.25);
  int wave_scale = analogRead(A10);
  wave_scale = map(wave_scale,0,1023,5,40);
  int mn = 1024;
  int mx = 0;
  int val = analogRead(MIC_PIN);
  for (int i = 0; i < 50; ++i) {
    val = analogRead(MIC_PIN);
    
    mn = min(mn, val);
    mx = max(mx, val);
  }
  delay(1);
  int delta = mx - mn;
  Serial.print(mn);
  Serial.print(" ");
  Serial.print(mx);
  Serial.print(" ");
  Serial.print(val);
  Serial.print(" ");
  Serial.println(delta);

  delta = constrain(delta * wave_scale, 0, 255);
  int r = 0;
  int g = 0;
  int b = 0;
  if (delta > 60 && wave_ended) {
    wave_ended = false;
    trigger_time = current_time;
    
    if (delta > 180) {
      r = delta;
    } else if (delta > 120) {
      g = delta;
    }else{
      b = delta;
    }
    color_trigger[0] = r;
    color_trigger[1] = g;
    color_trigger[2] = b;
  }

  if (trigger_time > 0) {
    wing1.setPixelColor(0, wing1.Color(color_trigger[0], color_trigger[1], color_trigger[2]));
    wing1.setPixelColor(3, wing1.Color(color_trigger[0], color_trigger[1], color_trigger[2]));
     if (current_time - trigger_time > 100) {
      wing1.setPixelColor(0, wing1.Color(0,0,0));
      wing1.setPixelColor(3, wing1.Color(0,0,0));
     }
  }
  if (current_time - trigger_time > 100) {
    wing1.setPixelColor(1, wing1.Color(color_trigger[0], color_trigger[1], color_trigger[2]));
    wing1.setPixelColor(4, wing1.Color(color_trigger[0], color_trigger[1], color_trigger[2]));
    if (current_time - trigger_time > 200) {
      wing1.setPixelColor(1, wing1.Color(0,0,0));
      wing1.setPixelColor(4, wing1.Color(0,0,0));
     }
  }
  if (current_time - trigger_time > 200) {
    wing1.setPixelColor(2, wing1.Color(color_trigger[0], color_trigger[1], color_trigger[2]));
    wing1.setPixelColor(5, wing1.Color(color_trigger[0], color_trigger[1], color_trigger[2]));
  }
  if (current_time - trigger_time > 300) {
    wing1.clear();
    wave_ended = true;
  }

  wing1.show();
}