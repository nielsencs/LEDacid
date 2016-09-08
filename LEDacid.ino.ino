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
const byte BRIGHTNESS = 30;

const byte iStripLength = 100; // max 254 unless vars changed from 'byte' - 255 used to set whole strip

const byte iModeSantaHat = 1;
const byte iModeSlowRainbow = 0;

const char* tSSID = "SKYA7448";
const char* tPass = "BXTYCFUS";
//const char* tSSID = "MiSMK";
//const char* tPass = "3M3n1nSh3ds12";
 
WiFiServer server(80);

boolean bLEDsOn=false;

// This is an array of leds.  One item for each led in your strip.
//CRGB leds[iStripLength];
CRGBArray<iStripLength> leds;

byte iUp = 1;
byte iMode = 0;
byte iHueMain = random(0, 256);

uint32_t iColGold;

const long interval = 2000;
unsigned long iCurMillisecs = 0;
unsigned long iPreMillisecs = 0;
// =============================================================================
void setup() {
// =============================================================================
  FastLED.delay(1000); // sanity check delay - allows reprogramming if accidently blowing power w/leds

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

// = countdown =================================================================
  for (byte i = 5; i > 0; i--){
    leds[i-1] = CRGB::Red;
    FastLED.show();
    FastLED.delay(700);
    leds[i-1] = CRGB::Black;
    FastLED.show();
    FastLED.delay(300);
  }
// = countdown =================================================================
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
}
// =============================================================================
void loop() {
// =============================================================================
  doWeb();
  if (bLEDsOn){
    doLEDs();
  }else{
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
 
  if (request.indexOf("/LED=0") != -1) {
    bLEDsOn=true;
    iMode = 0;
  } 
  if (request.indexOf("/LED=1") != -1) {
    bLEDsOn=true;
    iMode = 1;
  } 
  if (request.indexOf("/LED=2") != -1) {
    bLEDsOn=true;
    iMode = 2;
  } 
  if (request.indexOf("/LED=999") != -1){
    bLEDsOn=false;
    iMode = 0;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
// = Marvin menu ===============================================================
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
  client.println("    float: left; width: 48%;");
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
  client.println("      <li><a href='LED=1'>Rainbow Twinkle</a></li>");
  client.println("      <li><a href='LED=2'>Santa&lsquo;s Hat</a></li>");
  client.println("      <li><a href='LED=999'>Turn &lsquo;em off</a></li>");
  client.println("    </ul>");
  client.println("  </body>");
  client.println("</html>");
// = Marvin menu ===============================================================

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
//  iMode = 1; // comment out to cycle through all modes
*/

  switch (iMode){
    case 0:
      byte iHue;
      for(byte i = 0; i < iStripLength; i++){
        //iHue = iHueMain;
        iHue = iHueMain + i;
        leds[i] = CHSV(iHue, 255, 255);
      }
      FastLED.show();
      iHueMain++; // since this is a 'byte' will keep cycling from 0 to 255
      FastLED.delay(70);
      break;
    case 1:
      rainbowTwinkle();
      break;
    case 2:
      santaHat();
      break;
    case 3:
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
      break;
    case 4:
      whiteOverRainbow(20,75,5);  
      break;
    case 5:
      theaterChase(CRGB(random(0, 256), random(0, 256), random(0, 256)), 140);
      break;
    case 6:
      oneAllOtherCascade( CRGB(0, 255, 0),  CRGB(255, 255, 255));
      oneAllOtherCascade( CRGB(255, 0, 0),  CRGB(128, 255, 0));
      oneAllOtherCascade( CRGB(0, 0, 255),  CRGB(0, 255, 0));
      break;
    case 7:
      rainbowFade2White(3,3,1);
      break;
    case 18:
      sparkle(100, 100);
      break;
    case 19:
      simpleCycle();
      break;
    case 20:
      theaterChaseRainbow(50);
      break;
    case 21:
      rainbow(20);
      break;
    case 22:
      rainbowCycle(20);
      break;
    case 23:
      colourSet(255, Wheel(15));
      fireLight(10000, 0);
      break;
    case 24:
      colorWipe(CRGB(255, 0, 0), 100); // Red
      //colorWipe(CRGB(0, 255, 0), 75); // Green
      //colorWipe(CRGB(0, 0, 255), 50); // Blue
      //colorWipe(CRGB(255, 255, 255), 10); // White
      FastLED.show();
      break;
    case 99:
      test();
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

  for (byte i = 0; i < 255; i++){
    leds[random8(0,iStripLength)] = CHSV(i, 255, 255);
//    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
//    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
//    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
    leds[random8(0,iStripLength)] += CHSV(8, 0, 0);
//    leds[iDot] = CHSV(i, 255, 8);
//    leds[iDot] += CHSV(i, 255, 8);
    FastLED.show();        
  }
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
void santaHat() {
// =============================================================================
  for (byte i = 0; i < iStripLength; i++) { //assumes iStripLength 100
    if (i < 40 || i > 90){
      leds[i] = CRGB::White;
    }else{
      leds[i] = CRGB::Red;
    }
  }
  FastLED.show();
}
// =============================================================================
void fireLight(unsigned long iCount, byte iWheelStart){
// =============================================================================
  for (unsigned long i = 0; i < iCount; i++){
    fireLight2(iWheelStart);
  }
}
// =============================================================================
void fireLight2(byte iWheelStart){
// =============================================================================
  byte iDot = random(0, iStripLength);
  byte iDotWheel = iWheelStart + random(0, 30);

  colourSet(iDot, Wheel(iDotWheel));
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
// =============================================================================
void simpleCycle() {
// =============================================================================
//  byte iR = 1;
//  byte iG = 0;
//  byte iB = 0;
  byte i, j;

  boolean bLoop = true;
  iUp = 1-iUp;

  j=255*iUp;
  do{
    for(i=0; i<=iStripLength; i++) {
      switch (i % 4){
        case 0:
          leds[i] = CRGB(j, 0, 0);
          FastLED.show();
          break;
        case 1:
          leds[i] = CRGB::Orange;
          FastLED.show();
          break;
        case 2:
          leds[i] = CRGB::Green;
          FastLED.show();
          break;
        case 3:
          leds[i] = CRGB::Blue;
          FastLED.show();
          break;
      }
//      FastLED.delay(70);
    }

    if(iUp == 0){
      j++;
      if (j==255){
        bLoop = false;
      }
    }else{
      j--;
      if (j==0){
        bLoop = false;
      }
    }
  } while(bLoop);
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

