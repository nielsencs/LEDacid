/*
LEDacid - adding a bit more 'ooooh' to Christmas tree lights

Power consumption: If all WS2812s are full white, 60 mA per dot - 100 dots = 6 Amps

Proposed modes:
-standard Christmas tree lights for a bit - white pulse or flash or colour cycle - one colour per dot 
  then white band travels up tree, turns golden and grows down to fill tree
-starlight sparkle
-fading tail of moving dot
-rainbow sparkle
*/
// ==== FastLED includes =======================================================
#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>
// ==== FastLED includes =======================================================
#include <ESP8266WiFi.h>

#define DATA_PIN 5
const byte BRIGHTNESS = 64;
//const byte BRIGHTNESS = 255;

//---------------------------------- Mark Kriegsman's Fire2012 -----------------
//#define FRAMES_PER_SECOND 60
const byte FRAMES_PER_SECOND = 60;
bool gReverseDirection = false;

//CRGB leds[NUM_LEDS];

// Fire2012 with programmable Color Palette
//
// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function.
//
// Four different static color palettes are provided here, plus one dynamic one.
// 
// The three static ones are: 
//   1. the FastLED built-in HeatColors_p -- this is the default, and it looks
//      pretty much exactly like the original Fire2012.
//
//  To use any of the other palettes below, just "uncomment" the corresponding code.
//
//   2. a gradient from black to red to yellow to white, which is
//      visually similar to the HeatColors_p, and helps to illustrate
//      what the 'heat colors' palette is actually doing,
//   3. a similar gradient, but in blue colors rather than red ones,
//      i.e. from black to blue to aqua to white, which results in
//      an "icy blue" fire effect,
//   4. a simplified three-step gradient, from black to red to white, just to show
//      that these gradients need not have four components; two or
//      three are possible, too, even if they don't look quite as nice for fire.
//
// The dynamic palette shows how you can change the basic 'hue' of the
// color palette every time through the loop, producing "rainbow fire".

CRGBPalette16 gPalFire;
//---------------------------------- Mark Kriegsman's Fire2012 -----------------

const byte iStripLength = 100; // max 254 unless vars changed from 'byte' - 255 used to set whole strip

const char* tSSID = "SKYA7448";
const char* tPass = "BXTYCFUS";
//const char* tSSID = "MiSMK";
//const char* tPass = "3M3n1nSh3ds12";

WiFiServer server(80);

boolean bLEDsOn=false;

//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

//#define NUM_LEDS      100
//#define LED_TYPE   WS2812
//#define COLOR_ORDER   RGB
//#define DATA_PIN        5
//#define CLK_PIN       4
#define VOLTS          5
#define MAX_MA       4000

//  TwinkleFOX: Twinkling 'holiday' lights that fade in and out.
//  Colors are chosen from a palette; a few palettes are provided.
//
//  This December 2015 implementation improves on the December 2014 version
//  in several ways:
//  - smoother fading, compatible with any colors and any palettes
//  - easier control of twinkle speed and twinkle density
//  - supports an optional 'background color'
//  - takes even less RAM: zero RAM overhead per pixel
//  - illustrates a couple of interesting techniques (uh oh...)
//
//  The idea behind this (new) implementation is that there's one
//  basic, repeating pattern that each pixel follows like a waveform:
//  The brightness rises from 0..255 and then falls back down to 0.
//  The brightness at any given point in time can be determined as
//  as a function of time, for example:
//    brightness = sine( time ); // a sine wave of brightness over time
//
//  So the way this implementation works is that every pixel follows
//  the exact same wave function over time.  In this particular case,
//  I chose a sawtooth triangle wave (triwave8) rather than a sine wave,
//  but the idea is the same: brightness = triwave8( time ).  
//  
//  Of course, if all the pixels used the exact same wave form, and 
//  if they all used the exact same 'clock' for their 'time base', all
//  the pixels would brighten and dim at once -- which does not look
//  like twinkling at all.
//
//  So to achieve random-looking twinkling, each pixel is given a 
//  slightly different 'clock' signal.  Some of the clocks run faster, 
//  some run slower, and each 'clock' also has a random offset from zero.
//  The net result is that the 'clocks' for all the pixels are always out 
//  of sync from each other, producing a nice random distribution
//  of twinkles.
//
//  The 'clock speed adjustment' and 'time offset' for each pixel
//  are generated randomly.  One (normal) approach to implementing that
//  would be to randomly generate the clock parameters for each pixel 
//  at startup, and store them in some arrays.  However, that consumes
//  a great deal of precious RAM, and it turns out to be totally
//  unnessary!  If the random number generate is 'seeded' with the
//  same starting value every time, it will generate the same sequence
//  of values every time.  So the clock adjustment parameters for each
//  pixel are 'stored' in a pseudo-random number generator!  The PRNG 
//  is reset, and then the first numbers out of it are the clock 
//  adjustment parameters for the first pixel, the second numbers out
//  of it are the parameters for the second pixel, and so on.
//  In this way, we can 'store' a stable sequence of thousands of
//  random clock adjustment parameters in literally two bytes of RAM.
//
//  There's a little bit of fixed-point math involved in applying the
//  clock speed adjustments, which are expressed in eighths.  Each pixel's
//  clock speed ranges from 8/8ths of the system clock (i.e. 1x) to
//  23/8ths of the system clock (i.e. nearly 3x).
//
//  On a basic Arduino Uno or Leonardo, this code can twinkle 300+ pixels
//  smoothly at over 50 updates per seond.
//
//  -Mark Kriegsman, December 2015

//CRGBArray<NUM_LEDS> leds;

// Overall twinkle speed.
// 0 (VERY slow) to 8 (VERY fast).  
// 4, 5, and 6 are recommended, default is 4.
#define TWINKLE_SPEED 4

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once).  
// Default is 5.
#define TWINKLE_DENSITY 8

// How often to change color palettes.
#define SECONDS_PER_PALETTE  10
// Also: toward the bottom of the file is an array 
// called "ActivePaletteList" which controls which color
// palettes are used; you can add or remove color palettes
// from there freely.

// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
CRGB gBackgroundColor = CRGB::Black; 
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries 
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will 
// fade out slightly 'reddened', similar to how
// incandescent bulbs change color as they dim down.
#define COOL_LIKE_INCANDESCENT 0

bool bFirstTimeRound = true;

CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;
//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------

// This is an array of leds.  One item for each led in your strip.
//CRGB leds[iStripLength];
CRGBArray<iStripLength> leds;

byte iUp = 1;
byte iMode = 0;
byte iHueMain = random(0, 256);
byte iRG = 1;
byte iYB = 1;

//---------------------------------- lightning --------------------------------
//  Original by: Daniel Wilson, 2014
//
//  Modified by: Andrew Tuline 2015
//
//  This modified version creates lightning along various sections of the strip. Looks great inside my poly fill constructed cloud.
//
uint8_t frequency = 50;                                       // controls the interval between strikes
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;

uint8_t ledstart;                                             // Starting location of a flash
uint8_t ledlen;                                               // Length of a flash
//---------------------------------- lightning --------------------------------

const long interval = 2000;
unsigned long iCurMillisecs = 0;
unsigned long iPreMillisecs = 0;
// =============================================================================
void setup() {
// =============================================================================
  FastLED.delay(1000); // sanity check delay - allows reprogramming if accidently blowing power w/leds

//---------------------------------- Mark Kriegsman's Fire2012 -----------------
  // This first palette is the basic 'black body radiation' colors,
  // which run from black to red to bright yellow to white.
  // gPalFire = HeatColors_p;
  
  // These are other ways to set up the color palette for the 'fire'.
  // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
  //   gPalFire = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
     gPalFire = CRGBPalette16( CRGB(2,0,0), CRGB(64,0,0), CRGB::Orange, CRGB::Gold);
  
  // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
  //   gPalFire = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  
  // Third, here's a simpler, three-step gradient, from black to red to white
  //   gPalFire = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------
  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
//  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, iStripLength)
    .setCorrection(TypicalLEDStrip);

  chooseNextColorPalette(gTargetPalette); //, 99);
//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------
// Uncomment one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, iStripLength);
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, iStripLength);
  // FastLED.addLeds<APA104, DATA_PIN>(leds, iStripLength);
  // FastLED.addLeds<WS2811_400, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, iStripLength);

  // FastLED.addLeds<WS2801, RGB>(leds, iStripLength);
  // FastLED.addLeds<SM16716, RGB>(leds, iStripLength);
  // FastLED.addLeds<LPD8806, RGB>(leds, iStripLength);
  // FastLED.addLeds<P9813, RGB>(leds, iStripLength);
  // FastLED.addLeds<APA102, RGB>(leds, iStripLength);
  // FastLED.addLeds<DOTSTAR, RGB>(leds, iStripLength);
  
  // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, iStripLength);
  // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, iStripLength);

  Serial.begin(115200);
  FastLED.delay(10);
 
  FastLED.setBrightness(BRIGHTNESS);

// -------- countdown ----------------------------------------------------------
  for (byte i = 5; i > 0; i--){
    leds[i-1] = CRGB::Red;
    FastLED.show();
    FastLED.delay(700);
    leds[i-1] = CRGB::Black;
    FastLED.show();
    FastLED.delay(300);
  }
// -------- countdown ----------------------------------------------------------
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(tSSID);
 
  WiFi.begin(tSSID, tPass);
 
  while (WiFi.status() != WL_CONNECTED) {
    FastLED.delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

/*      for (byte i = 0; i < iStripLength; i++){
        leds[i] = CHSV(i + random8(0, 256-iStripLength), 255, 255);
        FastLED.show();        
      }*/
}
// =============================================================================
void loop() {
// =============================================================================
  doWeb();
//  bLEDsOn = true; // for testing without the webserver
  if (bLEDsOn){
    doLEDs();
    bFirstTimeRound = false;
  }else{
    //EVERY_N_MILLISECONDS(33) {
    for(int i = 0; i < iStripLength/2; i++) {   
      // fade everything out
      leds.fadeToBlackBy(20);
      FastLED.delay(33);
    }
  }
}
// =============================================================================
void doWeb() {
// =============================================================================
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    FastLED.delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  if (request.indexOf("/LED=999") != -1) {
    bLEDsOn=false;
  }else{
    if (request.indexOf("/LED=") != -1) {
      bLEDsOn=true;
      iMode = request.substring(request.indexOf("/LED=")+5, request.indexOf(" HTTP")).toInt();
      bFirstTimeRound = true;
    }
  }
  // Display Web Page
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
//---------------------------------- Marvin menu -------------------------------
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("  <head>");
  client.println("    <meta charset=\"UTF-8\">");
  client.println("    <title>LEDacid Menu</title>");
  client.println("    <style>");
  client.println("* {margin: 0; padding:0; box-sizing: border-box; font-family: 'Helvetica', Arial; font-weight: bold;}");
  client.println("body { background-color: #EEE; }");
  client.println("#list {");
  client.println("  display: block;");
  client.println("  list-style-type: none;");
  client.println("  padding: 0;");
  client.println("  overflow: hidden;");
  client.println("}");
  client.println("#list li {");
  client.println("  margin: 1%;");
  client.println("  margin-bottom: 10px;");
  client.println("}");
  client.println("@media only screen and (min-width: 650px) {");
  client.println("  #list li {");
//  client.println("    float: left; width: 48%;");
  client.println("    float: left; width: 98%;");
  client.println("  }");
  client.println("}");
  client.println("#list li a {");
  client.println("  background: #FFF;");
  client.println("  color: #D00;");
  client.println("  border-radius: 10px;");
  client.println("  box-shadow: 0 3px 10px #CCC;");
  client.println("  border: solid 1px #CCC;");
  client.println("  display: block;");
  client.println("  padding: 10px;");
  client.println("  text-align: center;");
  client.println("  text-decoration: none;");
  client.println("}");
  client.println("#list li a:hover {");
  client.println("  border: solid 1px #000;");
  client.println("  color: #000;");
  client.println("}");
  client.println("#list li a:active {");
  client.println("  background-color: #FFC;");
  client.println("}   </style>");
  client.println("  </head>");
  client.println("  <body>");
  client.println("    <ul id=\"list\">");
  client.println("      <li><a href='LED=0'>Slow Rainbow</a></li>");
  client.println("      <li><a href='LED=1'>Twin Rainbow Cascade</a></li>");
  client.println("      <li><a href='LED=2'>Santa&lsquo;s Hat</a></li>");
  client.println("      <li><a href='LED=3'>Lightning</a></li>");
  client.println("      <li><a href='LED=4'>Twinkle, Twinkle Little Star</a></li>");
//  client.println("      <li><a href='LED=5'>Sparkle</a></li>");
//  client.println("      <li><a href='LED=6'>option 06</a></li>");
//  client.println("      <li><a href='LED=7'>option 07</a></li>");
//  client.println("      <li><a href='LED=8'>option 08</a></li>");
//  client.println("      <li><a href='LED=9'>All White (subtle twinkle)</a></li>");
//  client.println("      <li><a href='LED=10'>option 10</a></li>");
//  client.println("      <li><a href='LED=11'>option 11</a></li>");
//  client.println("      <li><a href='LED=12'>option 12</a></li>");
//  client.println("      <li><a href='LED=13'>option 13</a></li>");
//  client.println("      <li><a href='LED=14'>option 14</a></li>");
//  client.println("      <li><a href='LED=15'>option 15</a></li>");
//  client.println("      <li><a href='LED=16'>option 16</a></li>");
//  client.println("      <li><a href='LED=17'>Firelight</a></li>");
  client.println("      <li><a href='LED=17'>Fire</a></li>");
//  client.println("      <li><a href='LED=18'>Mark Kriegsman&lsquo;s TwinkleFOX</a></li>");
  client.println("      <li><a href='LED=19'>TwinkleFOX (click again for new colours)</a></li>");
  client.println("      <li><a href='LED=20'>TwinkleFOX fairy</a></li>");
  client.println("      <li><a href='LED=999'>Turn &lsquo;em off</a></li>");
  client.println("    </ul>");
  client.println("  </body>");
  client.println("</html>");
//---------------------------------- Marvin menu -------------------------------

  FastLED.delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
// =============================================================================
void doLEDs(){
// =============================================================================  
/*  unsigned long iCurMillisecs = millis();

  if (iCurMillisecs - iPreMillisecs >= interval) {
    iPreMillisecs = iCurMillisecs;
    iMode++;
  }
*/
//  iMode = 25; // uncomment to test one mode

  switch (iMode){
    case 0:
//---------------------------------- slow rainbow ------------------------------
      byte iHue;
      for(byte i = 0; i < iStripLength; i++){
        //iHue = iHueMain;
        iHue = iHueMain + i;
        leds[i] = CHSV(iHue, 255, 255);
      }
      FastLED.show();
      iHueMain++; // since this is a 'byte' will keep cycling from 0 to 255
      FastLED.delay(70);
//---------------------------------- slow rainbow ------------------------------
      break;
    case 1:
//---------------------------------- twin rainbow cascade ----------------------
      static uint8_t hue;
      for(int i = 0; i < iStripLength/2; i++) {
        // fade everything out
        leds.fadeToBlackBy(40);

        // let's set an led value
        leds[i] = CHSV(hue++,255,255);
    
        // now, let's first 20 leds to the top 20 leds, 
        leds(iStripLength/2,iStripLength-1) = leds(iStripLength/2 - 1 ,0);
        FastLED.delay(33);
      }
//---------------------------------- twin rainbow cascade ----------------------
      break;
    case 2:
//---------------------------------- Santa's Hat -------------------------------
      for (byte i = 0; i < iStripLength; i++) { //assumes iStripLength 100
        if (i < 40 || i > 90){
          leds[i] = CRGB::White;
//          leds[i] = CHSV(0, 0, 64);
        }else{
          leds[i] = CRGB::Red;
//          leds[i] = CHSV(0, 255, 64);
        }
      }
      FastLED.show();
//---------------------------------- Santa's Hat -------------------------------
      break;
    case 3:
//---------------------------------- lightning ---------------------------------
      if(bFirstTimeRound){
        FastLED.clear();
      }

      ledstart = random8(iStripLength);           // Determine starting location of flash
      ledlen = random8(iStripLength-ledstart);    // Determine length of flash (not to go beyond iStripLength-1)
      for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
        if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
        else dimmer = random8(1,3);           // return strokes are brighter than the leader
        fill_solid(leds+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
        FastLED.show();                       // Show a section of LED's
        delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
        fill_solid(leds+ledstart,ledlen,CHSV(255,0,0));   // Clear the section of LED's
        FastLED.show();     
        if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
        delay(50+random8(100));               // shorter delay between strokes  
      } // for()
      delay(random8(frequency)*100);          // delay between strikes
//---------------------------------- lightning ---------------------------------
      break;
    case 4:
//---------------------------------- Twinkle, Twinkle Little Star --------------
      for(int i = 0; i < iStripLength; i++) {   
        leds[i] = CRGB(2, 1, 4);
        FastLED.delay(10);
        //leds[iStripLength-1] = CHSV(0, 0, 255); // top star
        leds[iStripLength-1] = CHSV(0, 0, random8(223,255)); // top star
  
        leds[int(iStripLength/3)] = CHSV(0, 0, random8(16,80));
        leds[int((iStripLength/3)*2)] = CHSV(0, 0, random8(16,80));
        leds[int(iStripLength/7)] = CHSV(0, 0, random8(16,80));
        leds[int((iStripLength/7)*4)] = CHSV(0, 0, random8(16,80));
        leds[int((iStripLength/7)*5)] = CHSV(0, 0, random8(16,80));
        leds[int((iStripLength/7)*6)] = CHSV(0, 0, random8(16,80));
        FastLED.show();
      }
//---------------------------------- Twinkle, Twinkle Little Star --------------
      break;
    case 5:
    //  FastLED.setBrightness(iBrightness);
      for (unsigned int i = 0; i < 100; i++){
        sparkle2(CRGB::White);
        sparkle2(CRGB::Gold);
        //sparkle2(CRGB(255, 192,  32));
        
        //leds[random(0, iStripLength)] = CHSV(0 + random(0, 30), 255, 255);
        //sparkle2(Wheel(random(0, 256))); // rainbow
      }
    //  FastLED.setBrightness(BRIGHTNESS);
      break;
    case 6:
//      whiteOverRainbow(20,75,5);
      for(int i = 0; i < iStripLength; i++) {   
//        leds[i] = CHSV(sin8(i), 255, 255);
        leds[i] = CHSV(i, 255, 255);
        FastLED.show();
      }
      break;
    case 7:
      theaterChase(CRGB(random(0, 256), random(0, 256), random(0, 256)), 140);
      break;
    case 8:
      oneAllOtherCascade( CRGB(0, 255, 0),  CRGB(255, 255, 255));
      oneAllOtherCascade( CRGB(255, 0, 0),  CRGB(128, 255, 0));
      oneAllOtherCascade( CRGB(0, 0, 255),  CRGB(0, 255, 0));
      break;
    case 9:
//---------------------------------- All White (subtle twinkle)-----------------
      for(int i = 0; i < iStripLength; i++) {   
        leds[i] = CRGB(255, 255, 255);
        FastLED.delay(10);
        FastLED.show();
      }
//---------------------------------- All White (subtle twinkle)-----------------
      break;
    case 10:
      RGBoverlaid();
      break;
    case 11:
      sparkle(100, 100);
      break;
    case 12:
//      simpleCycle();
      break;
    case 13:
      theaterChaseRainbow(50);
      break;
    case 14:
      rainbow(20);
      break;
    case 15:
      rainbowCycle(20);
      break;
    case 16:
      colorWipe(CRGB(255, 0, 0), 100); // Red
      //colorWipe(CRGB(0, 255, 0), 75); // Green
      //colorWipe(CRGB(0, 0, 255), 50); // Blue
      //colorWipe(CRGB(255, 255, 255), 10); // White
      FastLED.show();
      break;
/*    case 17:
//---------------------------------- firelight ---------------------------------
//      byte iWheelStart = 0;
      EVERY_N_MILLISECONDS( 10 ) {
        leds[random(0, iStripLength)] = CHSV(0 + random(10, 40), 255, 255);
        FastLED.show();
      }
//---------------------------------- firelight ---------------------------------
      break; */
    case 17:
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
      // Add entropy to random number generator; we use a lot of it.
      // random16_add_entropy( random());
    
      // Fourth, the most sophisticated: this one sets up a new palette every
      // time through the loop, based on a hue that changes every time.
      // The palette is a gradient from black, to a dark color based on the hue,
      // to a light color based on the hue, to white.
    
/*         static uint8_t hue = 0;
         hue++;
         CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
         CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
         gPalFire = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);
*/
    
      Fire2012WithPalette(); // run simulation frame, using palette colors
      
      FastLED.show(); // display this frame
      FastLED.delay(1000 / FRAMES_PER_SECOND);
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
      break;
    case 18:
//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------
//  EVERY_N_SECONDS( SECONDS_PER_PALETTE ) { 
//    chooseNextColorPalette( gTargetPalette ); 
//  }
  
  EVERY_N_MILLISECONDS( 10 ) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 12);
  }

  drawTwinkles( leds);

  FastLED.show();
//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------
      break;
    case 19:
//------------------------- Mark Kriegsman's TwinkleFOX next palette -----------
      chooseNextColorPalette(gTargetPalette); //, 99);
      iMode = 18;
//------------------------- Mark Kriegsman's TwinkleFOX next palette -----------
      break;
    case 20:
//------------------------- Mark Kriegsman's TwinkleFOX fairy palette ----------
//      chooseNextColorPalette(gTargetPalette, 3);
      iMode = 18;
//------------------------- Mark Kriegsman's TwinkleFOX fairy palette ----------
      break;
    default:
      iMode = 0;
      break;
  }
}
// =============================================================================
void rainbowTwinkle(){
// =============================================================================
//  byte iDot = random8(0,iStripLength);

//  for (byte i = 0; i < 255; i++){
//    leds[random8(0,iStripLength)] = CHSV(i, 255, 255);
//    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
//    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
//    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
//    leds[iDot] = CHSV(i, 255, 8);
//    leds[iDot] += CHSV(i, 255, 8);
    FastLED.show();        
//  }
}
// =============================================================================
void test(){
// =============================================================================
  uint32_t c = CRGB(255, 255, 255);
  colourSet(255, CRGB(255, 255, 255));
  FastLED.delay(1000);
//  for (byte i = 1; i < 101; i++){
//    colourSet(255, int((CRGB(255, 0, 0)*i)/100));
//    FastLED.delay(500);
//  }
  do{
    colourSet(255, c);
    c--;
  }while(c>0);

  colourSet(255, 0); // blank all
  FastLED.delay(1000);
}
// =============================================================================
void colourSet(byte iDot, uint32_t c) { // set specific dot (or if iDot = -1 all dots) to one colour
// =============================================================================
  if (iDot == 255){
    for(byte i = 0; i < iStripLength; i++) {
      leds[i] = c;
    }
  }else{
    leds[iDot] = c;
  }
  FastLED.show();
}
// =============================================================================
void oneAllOtherCascade(uint32_t iColAll, uint32_t iColCascade){
// =============================================================================
  //colourSet(255, iColAll);

  for(byte i=0; i < iStripLength; i++) {
    colourSet(i, iColCascade);
    FastLED.delay(200);
    colourSet(i, iColAll);
  }
}
/* / =============================================================================
void simpleCycle() {
// =============================================================================
  int iCounter;

  for(iCounter=1; iCounter<=255; iCounter++) {
    setRYGB(iCounter);
    //FastLED.delay(30);
    }
    iYB = 1 - iYB;
  }
  for(iCounter=254; iCounter>=0; iCounter--) {
    setRYGB(iCounter);
    //FastLED.delay(30);
    }
    iRG = 1 - iRG;
  }
}
// =============================================================================
void setRYGB(int iCounter) {
// =============================================================================
  byte i;

  // set red or green leds to iCounter
  // set yellow or blue leds to 255-iCounter
  for (i=0; i<=iStripLength; i++) {
    switch (i % 4){
      case 0:
        if (iRG == 1){
          leds[i] = CRGB(iCounter, 0, 0);
        }
        break;
      case 1:
        if (iYB == 1){
          leds[i] = CRGB(iCounter, iCounter, 0);
        }
        break;
      case 2:
        if (iRG == 0){
          leds[i] = CRGB(0, 255 - iCounter, 0);
        }
        break;
      case 3:
        if (iYB == 0){
          leds[i] = CRGB(0, 0, iCounter);
        }
        break;
    }
    FastLED.show();
  }
} */
// =============================================================================
void RGBoverlaid() {
// =============================================================================
  static byte iBand=int(iStripLength/3);
  static byte iRstart=0;
  static byte iGstart=iBand;
  static byte iBstart=iBand*2;
    for(byte i=0; i<=iBand; i++) {
      leds[iRstart] -= CRGB::Red;
      leds[iGstart] -= CRGB::Green;
      leds[iBstart] -= CRGB::Blue;
      leds[i+iRstart] += CRGB::Red;
      leds[i+iGstart] += CRGB::Green;
      leds[i+iBstart] += CRGB::Blue;
    }
//    leds[iRstart] += CRGB::Red;
    FastLED.show();
    FastLED.delay(300);
    iRstart += 1;
    iGstart += 2;
    iBstart += 3;
    if (iRstart>=iStripLength){iRstart=0;}
    if (iGstart>=iStripLength){iGstart=0;}
    if (iBstart>=iStripLength){iBstart=0;}
}
// =============================================================================
void sparkle(unsigned int iCount, byte iBrightness) {
// =============================================================================
//  FastLED.setBrightness(iBrightness);
  for (unsigned int i = 0; i < iCount; i++){
    sparkle2(CRGB::White);
    sparkle2(CRGB::Gold);
    //sparkle2(CRGB(255, 192,  32));
    //sparkle2(Wheel(random(0, 256))); // rainbow
  }
//  FastLED.setBrightness(BRIGHTNESS);
}
// =============================================================================
void sparkle2(uint32_t iColour) {
// =============================================================================
  byte iDot = random(0, iStripLength);
  uint32_t iCurrColour = leds[iDot];


  colourSet(iDot, iColour);
  //FastLED.delay(70);
  colourSet(iDot, iCurrColour);
}
// =============================================================================
void colorWipe(uint32_t c, byte iDelay) { // Fill the dots one after the other with a color
// =============================================================================
  for(byte i=0; i<iStripLength; i++) {
    colourSet(i, c);
    FastLED.delay(iDelay);
  }
}
// =============================================================================
void rainbowFade2White(byte iDelay, byte rainbowLoops, byte whiteLoops) {
// =============================================================================
  float fadeMax = 100.0;
  byte fadeVal = 0;
  uint32_t wheelVal;
  byte redVal, greenVal, blueVal;

  for(byte k = 0 ; k < rainbowLoops ; k ++){
    for(byte j=0; j!=255; j++) { // 5 cycles of all colors on wheel
      for(byte i=0; i< iStripLength; i++) {
        wheelVal = Wheel(((i * 256 / iStripLength) + j) & 255);

        redVal = red(wheelVal) * float(fadeVal/fadeMax);
        greenVal = green(wheelVal) * float(fadeVal/fadeMax);
        blueVal = blue(wheelVal) * float(fadeVal/fadeMax);

        leds[i] = CRGB( redVal, greenVal, blueVal);
      }

      //First loop, fade in!
      if(k == 0 && fadeVal < fadeMax-1) {
          fadeVal++;
      }
      //Last loop, fade out!
      else if(k == rainbowLoops - 1 && j > 255 - fadeMax ){
          fadeVal--;
      }
        FastLED.show();
        FastLED.delay(iDelay);
    }  
  }

  FastLED.delay(500);

  for(byte k = 0 ; k < whiteLoops ; k ++){
    for(byte j = 0; j < 256 ; j++){
      for(byte i=0; i < iStripLength; i++) {
        leds[i] = CRGB(128, 128, 128);
      }
      FastLED.show();
    }

    FastLED.delay(2000);
    for(byte j = 255; j >= 0 ; j--){
      for(byte i=0; i < iStripLength; i++) {
        leds[i] = CRGB(128, 128, 128);
      }
      FastLED.show();
    }
  }
  FastLED.delay(500);
}
// =============================================================================
void whiteOverRainbow(byte iDelay, byte whiteSpeed, byte whiteLength ) {
// =============================================================================
  if(whiteLength >= iStripLength) whiteLength = iStripLength - 1;

  byte head = whiteLength - 1;
  byte tail = 0;

  byte loops = 3;
  byte loopNum = 0;

  static unsigned long lastTime = 0;

  while(true){
    for(byte j=0; j!=255; j++) {
      for(byte i=0; i<iStripLength; i++) {
        if((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) ){
          leds[i] = CRGB(255, 255, 255);
        }
        else{
          leds[i] = Wheel(((i * 256 / iStripLength) + j) & 255);
        }        
      }

      if(millis() - lastTime > whiteSpeed) {
        head++;
        tail++;
        if(head == iStripLength){
          loopNum++;
        }
        lastTime = millis();
      }

      if(loopNum == loops) return;
    
      head%=iStripLength;
      tail%=iStripLength;
        FastLED.show();
        FastLED.delay(iDelay);
    }
  } 
}
// =============================================================================
void rainbowCycle(byte iDelay) { // Slightly different, this makes the rainbow equally distributed throughout
// =============================================================================
  uint16_t i, j;

  for(j=0; j<=256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< iStripLength; i++) {
      leds[i] = Wheel(((i * 256 / iStripLength) + j) & 255);
    }
    FastLED.show();
    FastLED.delay(iDelay);
  }
}
// =============================================================================
void rainbow(byte iDelay) {
// =============================================================================
  unsigned int i, j;

  for(j=0; j < 256; j++) {
    for(i=0; i < iStripLength; i++) {
      leds[i] = Wheel((i+j) & 255);
    }
    FastLED.show();
    FastLED.delay(iDelay);
  }
}
// =============================================================================
void theaterChase(uint32_t c, uint8_t iDelay) { //Theatre-style crawling lights.
// =============================================================================
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < iStripLength; i=i+3) {
        leds[i+q] = c;    //turn every third pixel on
      }
      FastLED.show();

      FastLED.delay(iDelay);

      for (uint16_t i=0; i < iStripLength; i=i+3) {
        leds[i+q] = 0;        //turn every third pixel off
      }
    }
  }
}
// =============================================================================
void theaterChaseRainbow(uint8_t iDelay) { //Theatre-style crawling lights with rainbow effect
// =============================================================================
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < iStripLength; i=i+3) {
        leds[i+q] = Wheel((i+j) % 255);    //turn every third pixel on
      }
      FastLED.show();

      FastLED.delay(iDelay);

      for (uint16_t i=0; i < iStripLength; i=i+3) {
        leds[i+q] = 0;        //turn every third pixel off
      }
    }
  }
}
// =============================================================================
uint32_t Wheel(byte WheelPos) {
// =============================================================================
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);

//  CHSV hsv(WheelPos, 255, 255);
//  CRGB rgb;
//  hsv2rgb_rainbow(hsv, rgb);
//  return rgb;
}
// =============================================================================
byte red(uint32_t c) {
// =============================================================================
  return (c >> 8);
}
// =============================================================================
byte green(uint32_t c) {
// =============================================================================
  return (c >> 16);
}
// =============================================================================
byte blue(uint32_t c) {
// =============================================================================
  return (c);
}
// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[iStripLength];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < iStripLength; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / iStripLength) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= iStripLength - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < iStripLength; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPalFire, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (iStripLength-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}
/* void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[iStripLength];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < iStripLength; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / iStripLength) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= iStripLength - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < iStripLength; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (iStripLength-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
} */

//------------------ All the rest is Mark Kriegsman's TwinkleFOX ---------------
//  This function loops over each pixel, calculates the 
//  adjusted 'clock' that this pixel should use, and calls 
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color, 
//  whichever is brighter.
void drawTwinkles( CRGBSet& L)
{
  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;
  
  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
  // the current palette are identical, then a deeply faded version of
  // that color is used for the background color
  CRGB bg;
  if( (AUTO_SELECT_BACKGROUND_COLOR == 1) &&
      (gCurrentPalette[0] == gCurrentPalette[1] )) {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if( bglight > 64) {
      bg.nscale8_video( 16); // very bright, so scale to 1/16th
    } else if( bglight > 16) {
      bg.nscale8_video( 64); // not that bright, so scale to 1/4th
    } else {
      bg.nscale8_video( 86); // dim, scale to 1/3rd.
    }
  } else {
    bg = gBackgroundColor; // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();
  
  for( CRGB& pixel: L) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16= PRNG16; // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle( myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if( deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color, 
      // use the new color.
      pixel = c;
    } else if( deltabright > 0 ) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend( bg, c, deltabright * 8);
    } else { 
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }
  }
}


//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as 
//  input to the brightness wave function.  
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB computeOneTwinkle( uint32_t ms, uint8_t salt)
{
  uint16_t ticks = ms >> (8-TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8( slowcycle16);
  slowcycle16 =  (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);
  
  uint8_t bright = 0;
  if( ((slowcycle8 & 0x0E)/2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8( fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if( bright > 0) {
    c = ColorFromPalette( gCurrentPalette, hue, bright, NOBLEND);
    if( COOL_LIKE_INCANDESCENT == 1 ) {
      coolLikeIncandescent( c, fastcycle8);
    }
  } else {
    c = CRGB::Black;
  }
  return c;
}


// This function is like 'triwave8', which produces a 
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \
//    /     \
//   /         \
//  /             \
//

uint8_t attackDecayWave8( uint8_t i)
{
  if( i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i/2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the 
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent( CRGB& c, uint8_t phase)
{
  if( phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8( c.g, cooling);
  c.b = qsub8( c.b, cooling * 2);
}


// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
{  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
{  Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Red 
};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
{  CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
{  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
{  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
{  0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red    0xB80400
#define C9_Orange 0x902C02
#define C9_Green  0x046002
#define C9_Blue   0x070758
//#define C9_White  0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
{  C9_Red,    C9_Orange, C9_Red,    C9_Orange,
   C9_Orange, C9_Red,    C9_Orange, C9_Red,
   C9_Green,  C9_Green,  C9_Green,  C9_Green,
   C9_Blue,   C9_Blue,   C9_Blue
//   C9_White
};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
{
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};

const TProgmemRGBPalette16 Classic_p FL_PROGMEM =
{
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Blue
};

const TProgmemRGBPalette16 Blorange_p FL_PROGMEM =
{
/*  0xFF1040

255 FF
238 EE
221 DD
204 CC
187 BB
170 AA
153 99
136 88
128 80
119 77
111 6F
102 66
 94 5E
 85 55
 77 4D
 68 44
 60 3C
 51 33
 43 2B
 34 22
 26 1A
 18 11
  9 09
  0 00
*/  
  0xFF8000, 0xEE7711, 0xDD6F22, 0xCC6633,
  0xBB5E44, 0xAA5555, 0x994D66, 0x884477,
  0x773C88, 0x663399, 0x552BAA, 0x4422BB,
  0x331ACC, 0x2211DD, 0x1109EE, 0x0000FF
};

// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16* ActivePaletteList[] = {
  &Classic_p,
  //&RetroC9_p,
  &BlueWhite_p,
  &RainbowColors_p,
  &FairyLight_p,
  &RedGreenWhite_p,
  &PartyColors_p,
  &RedWhite_p,
  &Snow_p,
  &Holly_p,
  &Ice_p,
  &Blorange_p
};


// Advance to the next color palette in the list (above).
void chooseNextColorPalette( CRGBPalette16& pal) //, byte iPal)
{
//  if (iPal ==999){
    const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
    static uint8_t whichPalette = -1; 
    whichPalette = addmod8( whichPalette, 1, numberOfPalettes);
  
    pal = *(ActivePaletteList[whichPalette]);
//  } else{
//    pal = *(ActivePaletteList[iPal]);
//  }
}

