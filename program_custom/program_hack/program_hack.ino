/*
Connections:
 - 3.3V to mic amp +
 - GND to mic amp -
 - Analog pin to microphone output (configurable below)
 - Digital pin to LED data input (configurable below)
 See notes in setup() regarding 5V vs. 3.3V boards - there may be an
 extra connection to make and one line of code to enable or disable.
*/

#include <Adafruit_NeoPixel.h>

#define N_PIXELS  6  // Number of pixels in strand
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define LED_PIN    5  // NeoPixel LED strand is connected to this pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define TOP       (N_PIXELS) // Allow dot to go slightly off scale


byte
  peak      = 0,      // Used for falling dot
  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data
int
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 10,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;
Adafruit_NeoPixel
  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  
  analogReference(EXTERNAL);

  memset(vol, 0, sizeof(vol));
  strip.begin();
 Serial.begin(9600);
}
void loop() {
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;

 

  n   = analogRead(MIC_PIN);
  Serial.print("Mic reading: ");
  Serial.println(n);
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
  Serial.print("Lvl: ");
  Serial.println(lvl);
  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg); // gives number of pixels to color
  Serial.print("height: ");
  Serial.println(height);
  
  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top


  // Color pixels based on rainbow gradient
  // Specify Striker points and response, set tresholds on variable: lvl, (bounces between: 0 --> 484  )
  if(lvl == 0){
    colorStrip(0);
  }
  else if(lvl >=1 && lvl < 500){
    colorStrip(1);
  }  
  else if(lvl >=50 &&  lvl< 700){
    colorStrip(2);
  }
  else if(lvl >=100 && lvl < 800){
    colorStrip(3);
  } 
  else if(lvl >=200 && lvl < 900){
    colorStrip(4);
  } 
  else if(lvl >=350 && lvl < 1024){
    colorStrip(5);
  } 
  else{
    colorStrip(6);
  }  


  
 //strip.show(); // Update strip




  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


void colorStrip(int height){
  for(int i=0; i<N_PIXELS; i++) {
  
    if(i >= height) {
      strip.setPixelColor(i,   0,   0, 0);
    }
    else{ 
      strip.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
    }
    
  }

  strip.show();
}
