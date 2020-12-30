//======================================================================================
// LEDacid - adding a bit more 'ooooh' to Christmas tree lights
//======================================================================================

#include "FastLED.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/*
To use- set the relevant connections below, then upload the sketch and get the IP address (e.g. 192.168.0.42)
from the serial monitor; browse via same wifi router and away we go!
*/
// -------- for hotspot ----------------------------------------------------------
const char* tSSID = "LEDacid";
//const char* tPass = "thereisnospoon";
// -------- for hotspot ----------------------------------------------------------
/*
Power consumption: If all WS2812s are full white, 60 mA per dot - 100 dots = 6 Amps

Proposed modes:
- standard Christmas tree lights for a bit - white pulse or flash or colour cycle - one colour per dot 
  then white band travels up tree, turns golden and grows down to fill tree
- starlight sparkle
- fading tail of moving dot
- rainbow sparkle
- Police - red blue flashing
- one colour
- upload palette from app/web menu?
- control knobs on 3D printed case?
- Sound effects? (great for police, fire or explosion!)
*/

#define VOLTS              5
#define MAX_MA             12000
#define BRIGHTNESS         255
//#define FRAMES_PER_SECOND  60
const byte FRAMES_PER_SECOND = 60;
#define NUM_STRIPS         2

const byte iStripLength = 100;

//CRGB leds[NUM_STRIPS][iStripLength];
CRGBArray<iStripLength> ledsA;
CRGBArray<iStripLength> ledsB;

WiFiServer server(80);
//======================================================================================
void setup() {
//======================================================================================
  delay(3000); // 3 second delay for recovery
  
  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);

//  FastLED.addLeds<WS2812, 4, RGB>(leds[0], iStripLength).setCorrection(TypicalLEDStrip);
//  FastLED.addLeds<WS2812, 5, RGB>(leds[1], iStripLength).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, 4, RGB>(ledsA, iStripLength).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, 5, RGB>(ledsB, iStripLength).setCorrection(TypicalLEDStrip);

  Serial.begin(115200);
  FastLED.delay(10);
 
  FastLED.setBrightness(BRIGHTNESS);

// -------- countdown ----------------------------------------------------------
//  for (byte i = 5; i > 0; i--){
//    leds[0][i-1] = CRGB::Red;
//    leds[0][iStripLength-i] = CRGB::Red;
//    leds[1][i-1] = CRGB::Green;
//    leds[1][iStripLength-i] = CRGB::Green;
//    FastLED.show();
//    FastLED.delay(700);
//    leds[0][i-1] = CRGB::Black;
//    leds[0][iStripLength-i] = CRGB::Black;
//    leds[1][i-1] = CRGB::Black;
//    leds[1][iStripLength-i] = CRGB::Black;
//    FastLED.show();
//    FastLED.delay(300);
//  }
// -------- countdown ----------------------------------------------------------
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(tSSID);

//---------------------------------- WiFi Access Point -----------------
  //Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
//  WiFi.softAP(tSSID, tPass);
  WiFi.softAP("LEDacid");
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

//---------------------------------- WiFi Access Point -----------------
/* */
//---------------------------------- Connect to WiFi -----------------
//  WiFi.begin(tSSID, tPass);
// 
//  while (WiFi.status() != WL_CONNECTED) {
//    FastLED.delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.println("WiFi connected");
/* */
//---------------------------------- Start the HTTP server -----------------
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  byte iIP = WiFi.localIP().toString().substring(10).toInt();
  Serial.println("/");

  Serial.println(iIP);
//---------------------------------- Start the HTTP server -----------------
}
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

byte iTopStar=0;

byte iLEDstatus=1; // 0 - off, 1 - on, 2 - paused
byte iFlipBlue=0;
byte iFlipRed=0;
byte iOneColHue=88;

bool bFirstTimeRound = true;
byte iUp = 1;
byte iMode; // = O_SlowCycle;
byte iModePrevious; // = O_SlowCycle;
byte iHueMain = random8();
byte iRG = 1;
byte iYB = 1;
//======================================================================================
void loop()
//======================================================================================
{
//  iLEDstatus = 1; // 1=LEDs on 0=LEDs off (1 for testing without the webserver)
  doWeb();
//  Serial.println(iLEDstatus);
  if (iLEDstatus >= 1){
    if (iLEDstatus == 1){
      doLEDs();
    }
    bFirstTimeRound = false;
  }else{
//    leds[0].fadeToBlackBy(14);
//    leds[1].fadeToBlackBy(14);
    ledsA.fadeToBlackBy(14);
    ledsB.fadeToBlackBy(14);
    FastLED.delay(33);
  }

//  // send the 'leds' array out to the actual LED strip
//  FastLED.show();  
//  // insert a delay to keep the framerate modest
//  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//======================================================================================
void nextPattern(){
//======================================================================================
  // add one to the current pattern number, and wrap around at the end
//  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}
//======================================================================================
void rainbow(){
//======================================================================================
//  fill_rainbow( leds[0], (iStripLength-iTopStar), gHue, 7);
//  fill_rainbow( leds[1], (iStripLength-iTopStar), gHue, 7);
}
//======================================================================================
void rainbowWithGlitter(){
//======================================================================================
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
//======================================================================================
void addGlitter( fract8 chanceOfGlitter){
//======================================================================================
  if( random8() < chanceOfGlitter) {
//    leds[0][ random16((iStripLength-iTopStar)) ] += CRGB::White;
  }
}
//======================================================================================
void confetti(){
//======================================================================================
  // random colored speckles that blink in and fade smoothly
//  fadeToBlackBy( leds[0], (iStripLength-iTopStar), 10);
  int pos = random16((iStripLength-iTopStar));
//  leds[0][pos] += CHSV( gHue + random8(64), 200, 255);
}
//======================================================================================
void sinelon(byte iStrip){
//======================================================================================
  // a colored dot sweeping back and forth, with fading trails
//  fadeToBlackBy( leds[iStrip], (iStripLength-iTopStar), 20);
  int pos = beatsin16( 13, 0, (iStripLength-iTopStar)-1 );
//  leds[iStrip][pos] += CHSV( gHue, 255, 192);
}
//======================================================================================
void bpm(){
//======================================================================================
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < (iStripLength-iTopStar); i++) { //9948
//    leds[0][i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    ledsA[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}
//======================================================================================
void juggle(){
//======================================================================================
  // eight colored dots, weaving in and out of sync with each other
//  fadeToBlackBy( leds[0], (iStripLength-iTopStar), 20);
  fadeToBlackBy( ledsA, (iStripLength-iTopStar), 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
//    leds[0][beatsin16( i+7, 0, (iStripLength-iTopStar)-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

