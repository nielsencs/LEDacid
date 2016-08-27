/*
-standard Christmas tree lights for a bit - white pulse or flash or colour cycle - one colour per dot 
  then white band travels up tree, turns golden and grows down to fill tree
-starlight sparkle
*/
#include <Adafruit_NeoPixel.h>

#define PIN 5
#define BRIGHTNESS 20

const byte iStripLength = 100; // max 254 unless vars changed from 'byte' - 255 used to set whole strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(iStripLength, PIN, NEO_RGB + NEO_KHZ800);

byte iUp = 1;
byte iSection = 0;
byte iColWheelPos = 0;

uint32_t iColGold;
uint32_t iColWhite;
uint32_t iColRed;

const long interval = 7000;
unsigned long iCurMillisecs = 0;
unsigned long iPreMillisecs = 0;
// ======================================================================
void setup() {
// ======================================================================
//  Serial.begin(115200);
//  Serial.println("Starting up...");
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  iColGold  = strip.Color(255, 192,  32);
  iColWhite = strip.Color(255, 255, 255);
  iColRed   = strip.Color(255,   0,   0);
}
// ======================================================================
void loop() {
// ======================================================================
  unsigned long iCurMillisecs = millis();

  if (iCurMillisecs - iPreMillisecs >= interval) {
    iPreMillisecs = iCurMillisecs;
    iSection++;
  }
  iSection = 5; // comment out to cycle through all modes

  switch(iSection){
    case 0:
      colorWipe(strip.Color(255, 0, 0), 100); // Red
      colorWipe(strip.Color(0, 255, 0), 75); // Green
      colorWipe(strip.Color(0, 0, 255), 50); // Blue
      colorWipe(strip.Color(255, 255, 255), 10); // White
      break;
    case 1:
      whiteOverRainbow(20,75,5);  
      break;
    case 2:
      theaterChase(strip.Color(random(0, 256), random(0, 256), random(0, 256)), 140);
      break;
    case 3:
      oneAllOtherCascade( strip.Color(0, 255, 0),  strip.Color(255, 255, 255));
      oneAllOtherCascade( strip.Color(255, 0, 0),  strip.Color(128, 255, 0));
      oneAllOtherCascade( strip.Color(0, 0, 255),  strip.Color(0, 255, 0));
      break;
    case 4:
      colourSet(255, Wheel(iColWheelPos));
      iColWheelPos++; // since this is a 'byte' will keep cycling from 0 to 255
      delay(300);
      break;
    case 5:
      rainbowFade2White(3,3,1);
      break;
    case 6:
      santaHat();
      break;
    case 19:
      theaterChaseRainbow(50);
      break;
    case 20:
      rainbow(20);
      break;
    case 21:
      rainbowCycle(20);
      break;
    case 22:
      colourSet(255, Wheel(15));
      fireLight(1000, 0);
      break;
    case 23:
      sparkle(1000);
      break;
    case 24:
      simpleCycle();
      break;
    case 99:
      test();
      break;
    default:
      iSection = 0;
      break;
  }
}
// ======================================================================
void test(){
// ======================================================================
  uint32_t c = strip.Color(255, 255, 255);
  colourSet(255, strip.Color(255, 255, 255));
  delay(1000);
//  for (byte i = 1; i < 101; i++){
//    colourSet(255, int((strip.Color(255, 0, 0)*i)/100));
//    delay(500);
//  }
  do{
    colourSet(255, c);
    c--;
  }while(c>0);

  colourSet(255, 0); // blank all
  delay(1000);
}
// ======================================================================
void colourSet(byte iDot, uint32_t c) { // set specific dot (or if iDot = -1 all dots) to one colour
// ======================================================================
  if (iDot == 255){
    for(byte i = 0; i < iStripLength; i++) {
      strip.setPixelColor(i, c);
    }
  }else{
    strip.setPixelColor(iDot, c);
  }
  strip.show();
}
// ======================================================================
void santaHat() {
// ======================================================================
  for(byte i = 0; i < iStripLength; i++) { //assumes iStripLength 100
    if (i < 40 || i > 90){
      strip.setPixelColor(i, iColWhite);
    }else{
      strip.setPixelColor(i, iColRed);
    }
  }
  strip.show();
}
// ======================================================================
void fireLight(unsigned int iCount, byte iWheelStart){
// ======================================================================
  for (unsigned int i = 0; i < iCount; i++){
    fireLight2(iWheelStart);
  }
}
// ======================================================================
void fireLight2(byte iWheelStart){
// ======================================================================
  byte iDot = random(0, iStripLength);
  byte iDotWheel = iWheelStart + random(0, 30);

  colourSet(iDot, Wheel(iDotWheel));
}
// ======================================================================
void oneAllOtherCascade(uint32_t iColAll, uint32_t iColCascade){
// ======================================================================
  //colourSet(255, iColAll);

  for(byte i=0; i < iStripLength; i++) {
    colourSet(i, iColCascade);
    delay(200);
    colourSet(i, iColAll);
  }
}
// ======================================================================
void simpleCycle() {
// ======================================================================
  byte iR = 1;
  byte iG = 0;
  byte iB = 0;
  byte i, j;

  boolean bLoop = true;
  iUp = 1-iUp;

  for(i=0; i<=iStripLength; i++) {
    j=255*iUp;
    do{
      colourSet(i, strip.Color(j*iG, j*iR, j*iB));

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
      delay(7);
    } while(bLoop);
      delay(7);
  }
      delay(7);
}
// ======================================================================
void sparkle(unsigned int iCount) {
// ======================================================================
  for (unsigned int i = 0; i < iCount; i++){
    sparkle2(iColWhite);
    sparkle2(iColGold);
  }
}
// ======================================================================
void sparkle2(uint32_t iColour) {
// ======================================================================
  byte iDot = random(0, iStripLength);
  uint32_t iCurrColour = strip.getPixelColor(iDot);


//  strip.setBrightness(100);
  colourSet(iDot, iColour);
//  delay(70);
  colourSet(iDot, iCurrColour);
//  colourSet(iDot, 0);
//  colourSet(iDot, Wheel(iColWheelPos));
//  delay(70);
//  strip.setBrightness(BRIGHTNESS);
}
// ======================================================================
void colorWipe(uint32_t c, byte iDelay) { // Fill the dots one after the other with a color
// ======================================================================
  for(byte i=0; i<iStripLength; i++) {
    colourSet(i, c);
    delay(iDelay);
  }
}
// ======================================================================
void rainbowFade2White(byte iDelay, byte rainbowLoops, byte whiteLoops) {
// ======================================================================
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

        strip.setPixelColor( i, strip.Color( redVal, greenVal, blueVal ) );
      }

      //First loop, fade in!
      if(k == 0 && fadeVal < fadeMax-1) {
          fadeVal++;
      }
      //Last loop, fade out!
      else if(k == rainbowLoops - 1 && j > 255 - fadeMax ){
          fadeVal--;
      }
        strip.show();
        delay(iDelay);
    }  
  }

  delay(500);

  for(byte k = 0 ; k < whiteLoops ; k ++){
    for(byte j = 0; j < 256 ; j++){
      for(byte i=0; i < iStripLength; i++) {
        strip.setPixelColor(i, strip.Color(128, 128, 128));
      }
      strip.show();
    }

    delay(2000);
    for(byte j = 255; j >= 0 ; j--){
      for(byte i=0; i < iStripLength; i++) {
        strip.setPixelColor(i, strip.Color(128, 128, 128));
      }
      strip.show();
    }
  }
  delay(500);
}
// ======================================================================
void whiteOverRainbow(byte iDelay, byte whiteSpeed, byte whiteLength ) {
// ======================================================================  
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
          strip.setPixelColor(i, strip.Color(255, 255, 255));
        }
        else{
          strip.setPixelColor(i, Wheel(((i * 256 / iStripLength) + j) & 255));
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
        strip.show();
        delay(iDelay);
    }
  } 
}
// ======================================================================
void rainbowCycle(byte iDelay) { // Slightly different, this makes the rainbow equally distributed throughout
// ======================================================================
  uint16_t i, j;

  for(j=0; j<=256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< iStripLength; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / iStripLength) + j) & 255));
    }
    strip.show();
    delay(iDelay);
  }
}
// ======================================================================
void rainbow(byte iDelay) {
// ======================================================================
  unsigned int i, j;

  for(j=0; j < 256; j++) {
    for(i=0; i < iStripLength; i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(iDelay);
  }
}
// ======================================================================
void theaterChase(uint32_t c, uint8_t wait) { //Theatre-style crawling lights.
// ======================================================================
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
// ======================================================================
void theaterChaseRainbow(uint8_t wait) { //Theatre-style crawling lights with rainbow effect
// ======================================================================
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
// ======================================================================
uint32_t Wheel(byte WheelPos) {
// ======================================================================
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}
// ======================================================================
byte red(uint32_t c) {
// ======================================================================
  return (c >> 8);
}
// ======================================================================
byte green(uint32_t c) {
// ======================================================================
  return (c >> 16);
}
// ======================================================================
byte blue(uint32_t c) {
// ======================================================================
  return (c);
}

