
#include <Adafruit_NeoPixel.h>

#define N_PIXELS  6  // Number of pixels in strand
#define MIC_PIN   A0  // Microphone is attached to this analog pin 
#define LED_PIN    6  // NeoPixel LED strand is connected to this pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment

int lvl = 50;      // Current "dampened" audio level

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int mic_reading = 0; // declare global mic_reading variable.

void setup() {
  Serial.begin(9600);
  strip.begin(); //start strip
  strip.show(); // update strip
}

void loop() {
  
  mic_reading = analogRead(MIC_PIN);  // Raw reading from mic
  Serial.print("Raw Reading: ");
  Serial.print(mic_reading); // print the analog value
  Serial.print("\n"); // make next print appear on new line

  //mic_reading   = abs(mic_reading - 512 - DC_OFFSET); // Center on zero
  //Serial.print("Centered on zero: ");
  //Serial.print(mic_reading); // print the new value
  //Serial.print("\n"); // make next print appear on new line

  //mic_reading   = (mic_reading <= NOISE) ? 0 : (mic_reading - NOISE); // Remove noise/hum
  //Serial.print("After noise/hum removale: ");
  //Serial.print(mic_reading); // print the analog value
  //Serial.print("\n"); // make next print appear on new line

  // not sure what this does, yet, I will find out!
  //lvl = ((lvl * 7) + mic_reading) ;    // "Dampened" reading (else looks twitchy)
  //Serial.print("Level variable: ");
  //Serial.print(lvl); // print the analog value
  //Serial.print("\n"); // make next print appear on new line

  //Call the Update strip function
  //delay(20);
  striker();
  delay(1000);
  stripClear();
  delay(400);
}

// Helping functions
void eq() {
    // maps the input reading value (min - max) to number of led's
    int antLeds = map(mic_reading, 0, 1024, 0, N_PIXELS); // try and change lvl with mic_reading, to see different results. 
    Serial.print("antLeds: ");
    Serial.print(antLeds); // print the analog value
    Serial.print("\n"); // make next print appear on new line

  
    //set value to update strip with:
    for(int i = 0; i < N_PIXELS; i++){
        //set the pixel active
        if(antLeds > 1 && i <= antLeds ){
                        //led , red, green, blue
          strip.setPixelColor(i, 255, 10, 10);
          strip.show();
        }
        else{
            strip.setPixelColor(i,0);
            strip.show();
          }
       
      }
      strip.show();
  }

void striker(){

    if(mic_reading <= 100){
        colorStrip(1);
    }
    else if(mic_reading >100 && mic_reading <= 400){
      colorStrip(2);  
    }
    else if(mic_reading >400 && mic_reading < 600){
      colorStrip(3);
    }
    else if(mic_reading >600 && mic_reading <= 800){
      colorStrip(4);  
    }
    else if(mic_reading >800 && mic_reading <= 1024){
      colorStrip(5);  
    }
    delay(500);
    
}

void colorStrip(int pixels){
  for(int i = 0; i < pixels; i++){
    strip.setPixelColor(i, 255, 10, 10);
    strip.show();
  }
}

void stripClear(){
    for(int i = 0; i < N_PIXELS; i++){
    strip.setPixelColor(i, 0);
    strip.show();
  }
  }


