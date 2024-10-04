// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#define LED_PIN 9    //pin della led strip
#define POT_PIN A10  //pin potenziometro
#define MIC_PIN A11  //pin sensore microfono
#define LEDS_1 3     //pin led monocolore ala 1
#define LEDS_2 6     //pin led monocolore ala 2

#define WAVE_SPEED 10          //velocitá di base del fade dei led a singolo colore
#define MAX_BOOST 5            //incremento massimo velocitá fade in base al rumore

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 6  // Popular NeoPixel ring size
Adafruit_NeoPixel wing1(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
unsigned long trigger_time = 0;
int led_intensity = 0;
int led_int_dir = 1;

void setup() {
  wing1.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(9600);
  pinMode(MIC_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(LEDS_1, OUTPUT);
  pinMode(LEDS_2, OUTPUT);
}

void loop() {
  unsigned long current_time = millis();

  int wave_scale = analogRead(POT_PIN);
  wave_scale = map(wave_scale, 0, 1023, 5, 40);

  //legge il rumore nel microfono e lo mette in delta
  int mn = 1024;
  int mx = 0;
  int val = analogRead(MIC_PIN);
  for (int i = 0; i < 50; ++i) {
    val = analogRead(MIC_PIN);
    delay(1);
    mn = min(mn, val);
    mx = max(mx, val);
  }
  int delta = mx - mn;
  delta = constrain(delta * wave_scale, 0, 255);
  /*
  Serial.print(mn);
  Serial.print(" ");
  Serial.print(mx);
  Serial.print(" ");
  Serial.print(val);
  Serial.print(" ");
  Serial.println(delta);
  //*/

  //esegue la wave sui pin monocolore
  float speed_boost = map(delta, 0, 255, 1, MAX_BOOST);
  led_intensity = led_intensity + WAVE_SPEED * led_int_dir * speed_boost;
  if (led_intensity > 255 || led_intensity < 0) {
    led_int_dir = led_int_dir * -1;
  }
  led_intensity = constrain(led_intensity, 0, 255);
  analogWrite(LEDS_1, led_intensity);
  analogWrite(LEDS_2, led_intensity);

  //se c'é abbastanza rumore colora i led (led dal piú interno al piu' esterno per ogni ala)
  //poco rumore led = blu bianco bianco
  //medio rumore led = verde verde bianco
  // massimo rumore led = rosso rosso rosso

  //in assenza di rumore
  // led = bianco bianco bianco (wave insieme agli altri led)
  if (delta > 60) {
    trigger_time = current_time;
    for (int led_i = 0; led_i < NUMPIXELS; led_i++) {
      wing1.setPixelColor(led_i, wing1.Color(255, 255, 255));
    }
    if (delta > 180) {
      wing1.setPixelColor(0, wing1.Color(255, 0, 0));
      wing1.setPixelColor(3, wing1.Color(255, 0, 0));
      wing1.setPixelColor(1, wing1.Color(255, 0, 0));
      wing1.setPixelColor(4, wing1.Color(255, 0, 0));
      wing1.setPixelColor(2, wing1.Color(255, 0, 0));
      wing1.setPixelColor(5, wing1.Color(255, 0, 0));
    } else if (delta > 120) {
      wing1.setPixelColor(0, wing1.Color(0, 255, 0));
      wing1.setPixelColor(3, wing1.Color(0, 255, 0));
      wing1.setPixelColor(1, wing1.Color(0, 255, 0));
      wing1.setPixelColor(4, wing1.Color(0, 255, 0));
    } else {
      wing1.setPixelColor(0, wing1.Color(0, 0, 255));
      wing1.setPixelColor(3, wing1.Color(0, 0, 255));
    }
  }
  if (current_time - trigger_time > 100) {
    for (int led_i = 0; led_i < NUMPIXELS; led_i++) {
      wing1.setPixelColor(led_i, wing1.Color(led_intensity, led_intensity, led_intensity));
    }
  }
  wing1.show();
}