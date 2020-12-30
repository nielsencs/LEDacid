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
#define MAX_MA       12000

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

CRGBPalette16 gCurrentPalette = RainbowColors_p;
CRGBPalette16 gTargetPalette = RainbowColors_p;
//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
//#define FRAMES_PER_SECOND 60
//const byte FRAMES_PER_SECOND = 60;
bool gReverseDirection = false;

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
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
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

//---------------------------------- Mark Kriegsman's Fire2012 -----------------
//CRGBPalette16 gPalFire;
//CRGBPalette16 gPalWater;
//  // This first palette is the basic 'black body radiation' colors,
//  // which run from black to red to bright yellow to white.
//  // gPalFire = HeatColors_p;
//  
//  // These are other ways to set up the color palette for the 'fire'.
//  // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
//  //   gPalFire = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
//     gPalFire = CRGBPalette16( CRGB(2,0,0), CRGB(64,0,0), CRGB::Orange, CRGB::Gold);
//  
//  // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
//  //   gPalFire = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
//     gPalWater = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
//  
//  // Third, here's a simpler, three-step gradient, from black to red to white
//  //   gPalFire = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
const TProgmemRGBPalette16 gPalFire = 
{
  CRGB(4,0,0),  CRGB(16,0,0), CRGB(64,0,0), CRGB(128,0,0),
  CRGB::Red,    CRGB::Red,    CRGB::Red,    CRGB::Orange,
  CRGB::Orange, CRGB::Orange, CRGB::Gold,   CRGB::Gold,
  CRGB::Gold,   CRGB::Orange, CRGB::Gold,   CRGB::White
};
const TProgmemRGBPalette16 gPalWater = 
{
  CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black, 
  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
  CRGB::Aqua, CRGB::Aqua, CRGB::Aqua, CRGB::Aqua,  
  CRGB::White, CRGB::White, CRGB::White, CRGB::White
};

#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p =
{  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p =
{  Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Red 
};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p =
{  CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };
//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------

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

/*const TProgmemRGBPalette16 Blorange_p =
{ 
  0xFF8000, 0xEE7711, 0xDD6F22, 0xCC6633,
  0xBB5E44, 0xAA5555, 0x994D66, 0x884477,
  0x773C88, 0x663399, 0x552BAA, 0x4422BB,
  0x331ACC, 0x2211DD, 0x1109EE, 0x0000FF
}; */

const TProgmemRGBPalette16 AquaGray_p =
{  CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray,
   CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray,
   CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray,
   CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray};

const TProgmemRGBPalette16 Grurple_p =
{  CRGB::Green,        CRGB::Green,       CRGB::Green,        CRGB::Purple,
   CRGB::DarkGreen,    CRGB::DarkGreen,   CRGB::Indigo,       CRGB::Green,
   CRGB::Blue,         CRGB::Magenta,     CRGB::Aqua,         CRGB::Orchid,
   CRGB::MidnightBlue, CRGB::DarkGreen,   CRGB::Purple,       CRGB::Plum };

const TProgmemRGBPalette16 Blorange_p =
{  CRGB::DarkOrange,   CRGB::Orange,      CRGB::LightYellow,  CRGB::Orange,
   CRGB::Tan,          CRGB::DarkOrange,  CRGB::Tan,          CRGB::DarkBlue,
   CRGB::PapayaWhip,   CRGB::SlateGray,   CRGB::Aqua,         CRGB::DarkOrange,
   CRGB::DeepSkyBlue,  CRGB::SlateBlue,   CRGB::Blue,         CRGB::Orange };

const TProgmemRGBPalette16 WhiteGold_p =
{  CRGB::White,       CRGB::Orange,       CRGB::White,        CRGB::White, 
   CRGB::White,       CRGB::White,        CRGB::LightYellow,  CRGB::White, 
   CRGB::PapayaWhip,  CRGB::White,        CRGB::White,        CRGB::White, 
   CRGB::White,       CRGB::White,        CRGB::White,        CRGB::LemonChiffon };

const TProgmemRGBPalette16 Classic_p =
{
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue
};

const TProgmemRGBPalette16 Stars_p =
{
  0xFFFFFF, 0x100010, 0x100010, 0x100010,
  0x100010, 0x100010, 0x100010, 0x100010,
  0x100010, 0x100010, 0x100010, 0x100010,
  0x100010, 0x100010, 0x100010, 0x100010
};

const TProgmemRGBPalette16 Pastel_p =
{
  0x401010, 0x404010, 0x404040, 0x104040,
  0x104010, 0x104040, 0x404040, 0x401040,
  0x101040, 0x401040, 0x404040, 0x404010,
  0x100808, 0x081008, 0x081010, 0x000000
};

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
    case O_Still:
      EVERY_N_MILLISECONDS(10) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
        for (int c = 0; c< 255; c++) {
          ledsA[int(c/2.56)] = ColorFromPalette(gCurrentPalette, c, 128, NOBLEND);
          ledsB[int(c/2.56)] = ColorFromPalette(gCurrentPalette, c, 128, NOBLEND);
        }
        FastLED.show();
      }
      break;
    case O_SlowCycle:
      EVERY_N_MILLISECONDS(70) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
        byte iHue;
        for(byte i = 0; i < iStripLength; i++){
          //iHue = iHueMain;
          iHue = iHueMain + i;
          ledsA[i] = ColorFromPalette(gCurrentPalette, iHue);
          ledsB[i] = ColorFromPalette(gCurrentPalette, iHue);
        }
        FastLED.show();
        iHueMain++; // since this is a 'byte' will keep cycling from 0 to 255
      }
      break;
    case O_Cascade:
      EVERY_N_MILLISECONDS(10) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
      }
      static uint8_t hue;
      for(int i = iStripLength; i > 0; i--) {
        ledsA.fadeToBlackBy(8);
        ledsA[i] = ColorFromPalette(gTargetPalette, hue++);
        ledsB.fadeToBlackBy(8);
        ledsB[i] = ColorFromPalette(gTargetPalette, hue++);
        FastLED.delay(33);
      }
      break;
    case O_TwinCascade:
      EVERY_N_MILLISECONDS(10) {
//      EVERY_N_MILLISECONDS(33) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
      }
//      static uint8_t hue;
      for(int i = 0; i < iStripLength/2; i++) {
        // fade everything out
//        ledsA.fadeToBlackBy(40);
        ledsA.fadeToBlackBy(20);
        ledsB.fadeToBlackBy(20);

        // let's set an led value
//        ledsA[i] = CHSV(hue++,255,255);
//        ledsA[i] = ColorFromPalette(gCurrentPalette, hue++);
        ledsA[i] = ColorFromPalette(gTargetPalette, hue++);
        ledsB[i] = ColorFromPalette(gTargetPalette, hue++);

        // now, let's first 20 leds to the top 20 leds, 
        ledsA(iStripLength/2,iStripLength-1) = ledsA(iStripLength/2 - 1 ,0);
        ledsB(iStripLength/2,iStripLength-1) = ledsB(iStripLength/2 - 1 ,0);

//      }
      }
      break;
    case O_Sparkle:
      EVERY_N_MILLISECONDS(10) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
      }
      ledsA.fadeLightBy(10);
      ledsA[random8(iStripLength)] = ColorFromPalette(gTargetPalette, random8());
      ledsB.fadeLightBy(10);
      ledsB[random8(iStripLength)] = ColorFromPalette(gTargetPalette, random8());
      //ledsA[random8(iStripLength)] = ColorFromPalette(gCurrentPalette, random8());
      FastLED.delay(5);
      break;
    case O_SparkleStar:
      gTargetPalette = Stars_p;
      gCurrentPalette = Stars_p;
      iMode = O_Sparkle;
      break;
    case O_SantaHat:
      for (byte i = 0; i < iStripLength; i++) { // if iStripLength > 255 change to int
        if (i < 40 || i > 90){
          ledsA[i] = CRGB::White;
          ledsB[i] = CRGB::White;

          ledsA[i] = CRGB::Red;
          ledsB[i] = CRGB::Red;
        }
      }
      FastLED.show();
      break;
    case O_Lightning:
      if(bFirstTimeRound){
        FastLED.clear();
      }

      ledstart = random8(iStripLength);           // Determine starting location of flash
      ledlen = random8(iStripLength-ledstart);    // Determine length of flash (not to go beyond iStripLength-1)
      for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
        if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
        else dimmer = random8(1,3);           // return strokes are brighter than the leader
        fill_solid(ledsA+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
        fill_solid(ledsB+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
        FastLED.show();                       // Show a section of LED's
        delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
        fill_solid(ledsA+ledstart,ledlen,CHSV(255,0,0));   // Clear the section of LED's
        fill_solid(ledsB+ledstart,ledlen,CHSV(255,0,0));   // Clear the section of LED's
        FastLED.show();     
        if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
        delay(50+random8(100));               // shorter delay between strokes  
      } // for()
      delay(random8(frequency)*100);          // delay between strikes
      break;
    case O_TwinkleStar:
      for(int i = 0; i < iStripLength; i++) {   
        ledsA[i] = CRGB(2, 1, 4);
        ledsB[i] = CRGB(2, 1, 4);
        EVERY_N_MILLISECONDS(10) {
          //ledsA[iStripLength-1] = CHSV(0, 0, 255); // top star
          ledsA[iStripLength-1] = CHSV(0, 0, random8(223,255)); // top star
          ledsB[iStripLength-1] = CHSV(0, 0, random8(223,255)); // top star
    
          ledsA[int(iStripLength/3)] = CHSV(0, 0, random8(16,80));
          ledsA[int((iStripLength/3)*2)] = CHSV(0, 0, random8(16,80));
          ledsA[int(iStripLength/7)] = CHSV(0, 0, random8(16,80));
          ledsA[int((iStripLength/7)*4)] = CHSV(0, 0, random8(16,80));
          ledsA[int((iStripLength/7)*5)] = CHSV(0, 0, random8(16,80));
          ledsA[int((iStripLength/7)*6)] = CHSV(0, 0, random8(16,80));

          ledsB[int(iStripLength/3)] = CHSV(0, 0, random8(16,80));
          ledsB[int((iStripLength/3)*2)] = CHSV(0, 0, random8(16,80));
          ledsB[int(iStripLength/7)] = CHSV(0, 0, random8(16,80));
          ledsB[int((iStripLength/7)*4)] = CHSV(0, 0, random8(16,80));
          ledsB[int((iStripLength/7)*5)] = CHSV(0, 0, random8(16,80));
          ledsB[int((iStripLength/7)*6)] = CHSV(0, 0, random8(16,80));
          FastLED.show();
        }
      }
      break;
    case O_Police:
      EVERY_N_MILLISECONDS(37) {
        for(byte i = iStripLength/3; i < (iStripLength/3)*2; i++) {
          ledsA[i] = CRGB(255*iFlipRed, 0, 0);
          ledsB[i] = CRGB(255*iFlipRed, 0, 0);
        }
        iFlipRed = 1 - iFlipRed;
        FastLED.show();
      }
      EVERY_N_MILLISECONDS(330) {
        for(byte i = 0; i < iStripLength/2; i++) {
          ledsA[i] = CRGB(0, 0, 255*iFlipBlue);
          ledsA[iStripLength - i] = CRGB(0, 0, 255-(255*iFlipBlue));
          ledsB[i] = CRGB(0, 0, 255*iFlipBlue);
          ledsB[iStripLength - i] = CRGB(0, 0, 255-(255*iFlipBlue));
        }
        iFlipBlue = 1 - iFlipBlue;
        FastLED.show();
      }
      break;
    case 7:
//      theaterChase(CRGB(random8(), random8(), random8()), 140);
      break;
    case 8:
//      oneAllOtherCascade( CRGB(0, 255, 0),  CRGB(255, 255, 255));
//      oneAllOtherCascade( CRGB(255, 0, 0),  CRGB(128, 255, 0));
//      oneAllOtherCascade( CRGB(0, 0, 255),  CRGB(0, 255, 0));
      break;
    case 9:
      break;
    case 10:
//      RGBoverlaid();
      break;
    case 11:
      break;
    case 12:
      //classicCycle();
      EVERY_N_MILLISECONDS(100) {
        int iCounter;
      
        for(iCounter=1; iCounter<=255; iCounter++) {
//          setRYGB(iCounter);
          FastLED.delay(30);
          iYB = 1 - iYB;
        }
      }
      break;
    case 13:
//      theaterChaseRainbow(50);
      break;
    case 14:
//      rainbow(20);
      break;
    case O_FireWater:
      EVERY_N_MILLISECONDS(10) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
      }
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
      Fire2012WithPalette(); // run simulation frame, using palette colors

      FastLED.show(); // display this frame
      FastLED.delay(1000 / FRAMES_PER_SECOND);
//---------------------------------- Mark Kriegsman's Fire2012 -----------------
      break;
    case O_Water:
      gTargetPalette = gPalWater;
      gCurrentPalette = gPalWater;
      gReverseDirection = true;
      iMode = O_FireWater;
      break;
    case O_Fire:
      gTargetPalette = gPalFire;
      gCurrentPalette = gPalFire;
      gReverseDirection = false;
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
    
      iMode = O_FireWater;
      break;
    case O_TwinkleFOX:
      EVERY_N_MILLISECONDS(10) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
      }

      drawTwinkles(ledsA);
      drawTwinkles(ledsB);
      FastLED.show();
      break;
    case 19:
//------------------------- Mark Kriegsman's TwinkleFOX next palette -----------
//      chooseNextColorPalette(gTargetPalette); //, 99);
//      iMode = O_TwinkleFOX;
//------------------------- Mark Kriegsman's TwinkleFOX next palette -----------
      break;
    case 20:
//------------------------- Mark Kriegsman's TwinkleFOX fairy palette ----------
      gTargetPalette = FairyLight_p;
      iMode = iModePrevious;
//------------------------- Mark Kriegsman's TwinkleFOX fairy palette ----------
      break;
    case 21:
//---------------------------------- Palette Cloud -----------------------------
      gTargetPalette = CloudColors_p;
      iMode = iModePrevious;
//---------------------------------- Palette Cloud -----------------------------
      break;
    case 22:
//---------------------------------- Palette Rainbow ---------------------------
      gTargetPalette = RainbowColors_p;
      iMode = iModePrevious;
//---------------------------------- Palette Rainbow ---------------------------
      break;
    case 23:
//---------------------------------- Palette LavaColors ------------------------
      gTargetPalette = LavaColors_p;
      iMode = iModePrevious;
//---------------------------------- Palette LavaColors ------------------------
      break;
    case 24:
//---------------------------------- Palette OceanColors -----------------------
      gTargetPalette = OceanColors_p;
      iMode = iModePrevious;
//---------------------------------- Palette OceanColors -----------------------
      break;
    case 25:
//---------------------------------- Palette ForestColors ----------------------
      gTargetPalette = ForestColors_p;
      iMode = iModePrevious;
//---------------------------------- Palette ForestColors ----------------------
      break;
    case 26:
//---------------------------------- Palette RainbowStripeColors ---------------
      gTargetPalette = RainbowStripeColors_p;
      iMode = iModePrevious;
//---------------------------------- Palette RainbowStripeColors ---------------
      break;
    case 27:
//---------------------------------- Palette PartyColors -----------------------
      gTargetPalette = PartyColors_p;
      iMode = iModePrevious;
//---------------------------------- Palette PartyColors -----------------------
      break;
    case 28:
//---------------------------------- Palette WhiteGold -------------------------
      gTargetPalette = WhiteGold_p;
      iMode = iModePrevious;
//---------------------------------- Palette WhiteGold -------------------------
      break;
    case 29:
//---------------------------------- Palette Fire ------------------------------
      gTargetPalette = gPalFire;
      iMode = iModePrevious;
//---------------------------------- Palette Fire ------------------------------
      break;
    case 30:
//---------------------------------- Palette Water -----------------------------
      gTargetPalette = gPalWater;
      iMode = iModePrevious;
//---------------------------------- Palette Water -----------------------------
      break;
    case 31:
//---------------------------------- Palette Blorange --------------------------
      gTargetPalette = Blorange_p;
      iMode = iModePrevious;
//---------------------------------- Palette Blorange --------------------------
      break;
    case 32:
//---------------------------------- Palette Classic ---------------------------
      gTargetPalette = Classic_p;
      iMode = iModePrevious;
//---------------------------------- Palette Classic ---------------------------
      break;
    case 33:
//---------------------------------- Palette Holly -----------------------------
      gTargetPalette = Holly_p;
      iMode = iModePrevious;
//---------------------------------- Palette Holly -----------------------------
      break;
    case 34:
//---------------------------------- Palette RedWhite --------------------------
      gTargetPalette = RedWhite_p;
      iMode = iModePrevious;
//---------------------------------- Palette RedWhite --------------------------
      break;
    case 35:
//---------------------------------- Palette Pastel ----------------------------
      gTargetPalette = Pastel_p;
      iMode = iModePrevious;
//---------------------------------- Palette Pastel ----------------------------
      break;
    case 36:
//---------------------------------- Palette Stars -----------------------------
      gTargetPalette = Stars_p;
      iMode = iModePrevious;
//---------------------------------- Palette Stars -----------------------------
      break;
    case O_Grurple:
      gTargetPalette = Grurple_p;
      iMode = iModePrevious;
      break;
    case O_AquaGray:
      gTargetPalette = AquaGray_p;
      iMode = iModePrevious;
      break;
    case 39:
//---------------------------------- All One Colour ----------------------------
//      gTargetPalette = CHSV(iOneColHue, 255, 255);
      iMode = iModePrevious;
//---------------------------------- All One Colour ----------------------------
      break;
    default:
      iMode = 0;
      break;
  }
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
      int pixelnumber;

//      CRGB color = ColorFromPalette(gTargetPalette, colorindex);
      CRGB color = ColorFromPalette(gCurrentPalette, colorindex);
      if( gReverseDirection ) {
//        CRGB color = ColorFromPalette( gPalWater, colorindex);
        pixelnumber = (iStripLength-1) - j;
        ledsA[pixelnumber] = color;
      } else {
//        CRGB color = ColorFromPalette( gPalFire, colorindex);
        pixelnumber = j;
        ledsA[pixelnumber] = color;
      }
//      ledsA[pixelnumber] = color;
    }
}

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

/*
// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
{  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
{  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

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
*/

