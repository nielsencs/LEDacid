//---------------------------------- Mark Kriegsman's TwinkleFOX ---------------
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

//#define NUM_LEDS      100
//#define LED_TYPE   WS2812
//#define COLOR_ORDER   RGB
//#define DATA_PIN        5
//#define CLK_PIN       4
//#define VOLTS          5
//#define MAX_MA       12000
//
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
// =============================================================================
void setTopStar(){
// =============================================================================
  if (iTopStar==1){
//    ledsA[iStripLength-1] = CRGB::Gold;
//    ledsB[iStripLength-1] = CRGB::Gold;
    ledsA[iStripLength-1] = CRGB::White;
    ledsB[iStripLength-1] = CRGB::White;
//  } else{
//    ledsA[iStripLength-1] = ledsA[iStripLength-2];
//    ledsB[iStripLength-1] = ledsB[iStripLength-2];
  }
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
//  iMode = O_TwinkleFOX; // uncomment to test one mode
  setTopStar();

  switch (iMode){
    case O_TopStar:
      iTopStar=1-iTopStar;
      iMode = iModePrevious;
      break;
    case O_Still:
      EVERY_N_MILLISECONDS(10) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
        for (int c = 0; c< (iStripLength-iTopStar); c++) {
          ledsA[c] = ColorFromPalette(gCurrentPalette, c*(256/(iStripLength-iTopStar)), 128, NOBLEND);
          ledsB[c] = ColorFromPalette(gCurrentPalette, c*(256/(iStripLength-iTopStar)), 128, NOBLEND);
        }
        FastLED.show();
      }
      break;
    case O_SlowCycle:
      EVERY_N_MILLISECONDS(70) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
        byte iHue;
        for(byte i = 0; i < (iStripLength-iTopStar); i++){
          //iHue = iHueMain;
          iHue = iHueMain + i;
          ledsA[i] = ColorFromPalette(gCurrentPalette, iHue);
          ledsB[i] = ColorFromPalette(gCurrentPalette, iHue);
        }
        FastLED.show();
        iHueMain++; // since this is a 'byte' will keep cycling from 0 to 255
      }
      break;
    case O_FadePalette:
      EVERY_N_MILLISECONDS(70) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
        byte iHue;
        for(byte i = 0; i < (iStripLength-iTopStar); i++){
          iHue = iHueMain;
//          iHue = iHueMain + i;
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
      for(int i = (iStripLength-iTopStar); i > 0; i--) {
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
      for(int i = 0; i < (iStripLength-iTopStar)/2; i++) {
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
        ledsA((iStripLength-iTopStar)/2,(iStripLength-iTopStar)-1) = ledsA((iStripLength-iTopStar)/2 - 1 ,0);
        ledsB((iStripLength-iTopStar)/2,(iStripLength-iTopStar)-1) = ledsB((iStripLength-iTopStar)/2 - 1 ,0);

//      }
      }
      break;
    case O_SparkleHard:
      EVERY_N_MILLISECONDS(10) {
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
      }
//      gCurrentPalette = gTargetPalette;

      ledsA.fadeToBlackBy(10);
      ledsA[random8(iStripLength-iTopStar)] = ColorFromPalette(gTargetPalette, random8());
      ledsB.fadeToBlackBy(10);
      ledsB[random8(iStripLength-iTopStar)] = ColorFromPalette(gTargetPalette, random8());
      FastLED.delay(5);
      break;
    case O_SparkleSoft:
//      EVERY_N_MILLISECONDS(10) {
//        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
//      }
      gCurrentPalette = gTargetPalette;

      ledsA.fadeLightBy(10);
      ledsA[random8(iStripLength-iTopStar)] = ColorFromPalette(gTargetPalette, random8());
      ledsB.fadeLightBy(10);
      ledsB[random8(iStripLength-iTopStar)] = ColorFromPalette(gTargetPalette, random8());
      FastLED.delay(5);
      break;
    case O_TwinkleFOXClassic:
      gTargetPalette = FairyLight_p;
      gCurrentPalette = FairyLight_p;
      iMode = O_TwinkleFOX;
      break;
    case O_SlowCycleRainbow:
      gTargetPalette = RainbowColors_p;
      gCurrentPalette = RainbowColors_p;
      iMode = O_SlowCycle;
      break;
    case O_SlowCycleRainbowStripe:
      gTargetPalette = RainbowStripeColors_p;
      gCurrentPalette = RainbowStripeColors_p;
      iMode = O_SlowCycle;
      break;
    case O_SparkleStar:
      gTargetPalette = Stars_p;
      gCurrentPalette = Stars_p;
      iMode = O_SparkleSoft;
      break;
    case O_SparkleWhiteGold:
      gTargetPalette = WhiteGold_p;
      gCurrentPalette = WhiteGold_p;
      iMode = O_SparkleHard;
      break;
    case O_TwinkleFOXHolly:
      gTargetPalette = Holly_p;
      gCurrentPalette = Holly_p;
      iMode = O_TwinkleFOX;
      break;
    case O_Snowing:
      gTargetPalette = Monochrome_p;
      gCurrentPalette = Monochrome_p;
      iMode = O_SlowCycle;
      break;
    case O_SantaHat:
      gTargetPalette = SantaHat_p;
      gCurrentPalette = SantaHat_p;
      iMode = O_Still;
      break;
    case O_SparkleBlorange:
      gTargetPalette = Blorange_p;
      gCurrentPalette = Blorange_p;
      iMode = O_SparkleSoft;
      break;
    case O_Lightning:
      if(bFirstTimeRound){
        FastLED.clear();
      }

      ledstart = random8(iStripLength-iTopStar);           // Determine starting location of flash
      ledlen = random8(iStripLength-iTopStar-ledstart);    // Determine length of flash (not to go beyond iStripLength-iTopStar-1)
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
      for(int i = 0; i < iStripLength-1; i++) {   
        ledsA[i] = CRGB(2, 1, 4);
        ledsB[i] = CRGB(2, 1, 4);
        EVERY_N_MILLISECONDS(10) {
          ledsA[iStripLength-1] = CHSV(0, 0, random8(223,255)); // top star
          ledsB[iStripLength-1] = CHSV(0, 0, random8(223,255)); // top star
    
          ledsA[int(iStripLength-1/3)] = CHSV(0, 0, random8(16,80));
          ledsA[int((iStripLength-1/3)*2)] = CHSV(0, 0, random8(16,80));
          ledsA[int(iStripLength-1/7)] = CHSV(0, 0, random8(16,80));
          ledsA[int(((iStripLength-1)/7)*4)] = CHSV(0, 0, random8(16,80));
          ledsA[int(((iStripLength-1)/7)*5)] = CHSV(0, 0, random8(16,80));
          ledsA[int(((iStripLength-1)/7)*6)] = CHSV(0, 0, random8(16,80));

          ledsB[int((iStripLength-1)/3)] = CHSV(0, 0, random8(16,80));
          ledsB[int(((iStripLength-1)/3)*2)] = CHSV(0, 0, random8(16,80));
          ledsB[int((iStripLength-1)/7)] = CHSV(0, 0, random8(16,80));
          ledsB[int(((iStripLength-1)/7)*4)] = CHSV(0, 0, random8(16,80));
          ledsB[int(((iStripLength-1)/7)*5)] = CHSV(0, 0, random8(16,80));
          ledsB[int(((iStripLength-1)/7)*6)] = CHSV(0, 0, random8(16,80));
          FastLED.show();
        }
      }
      break;
    case O_Police:
      EVERY_N_MILLISECONDS(37) {
        for(byte i = (iStripLength-iTopStar)/3; i < ((iStripLength-iTopStar)/3)*2; i++) {
          ledsA[i] = CRGB(255*iFlipRed, 0, 0);
          ledsB[i] = CRGB(255*iFlipRed, 0, 0);
        }
        iFlipRed = 1 - iFlipRed;
        FastLED.show();
      }
      EVERY_N_MILLISECONDS(330) {
        for(byte i = 0; i < (iStripLength-iTopStar)/2; i++) {
          ledsA[i] = CRGB(0, 0, 255*iFlipBlue);
          ledsA[(iStripLength-iTopStar) - i] = CRGB(0, 0, 255-(255*iFlipBlue));
          ledsB[i] = CRGB(0, 0, 255*iFlipBlue);
          ledsB[(iStripLength-iTopStar) - i] = CRGB(0, 0, 255-(255*iFlipBlue));
        }
        iFlipBlue = 1 - iFlipBlue;
        FastLED.show();
      }
      break;
    case O_Classic:
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
      setTopStar();
      FastLED.show();
      break;
    case O_TwinkleFOXNextPalette:
//------------------------- Mark Kriegsman's TwinkleFOX next palette -----------
//      chooseNextColorPalette(gTargetPalette); //, 99);
//      iMode = O_TwinkleFOX;
//------------------------- Mark Kriegsman's TwinkleFOX next palette -----------
      break;
//------------------------- Palettes -------------------------------------------
    case O_FairyLight_p:
      gTargetPalette = FairyLight_p;
      iMode = iModePrevious;
      break;
    case O_CloudColors_p:
      gTargetPalette = CloudColors_p;
      iMode = iModePrevious;
      break;
    case O_RainbowColors_p:
      gTargetPalette = RainbowColors_p;
      iMode = iModePrevious;
      break;
    case O_Dawn_p:
      gTargetPalette = Dawn_p;
      iMode = iModePrevious;
      break;
    case O_OceanColors_p:
      gTargetPalette = OceanColors_p;
      iMode = iModePrevious;
      break;
    case O_ForestColors_p:
      gTargetPalette = ForestColors_p;
      iMode = iModePrevious;
      break;
    case O_RainbowStripeColors_p:
      gTargetPalette = RainbowStripeColors_p;
      iMode = iModePrevious;
      break;
    case O_PartyColors_p:
      gTargetPalette = PartyColors_p;
      iMode = iModePrevious;
      break;
    case O_WhiteGold_p:
      gTargetPalette = WhiteGold_p;
      iMode = iModePrevious;
      break;
    case O_Fire_p:
      gTargetPalette = gPalFire;
      iMode = iModePrevious;
      break;
    case O_WhitePurple_p:
      gTargetPalette = WhitePurple_p;
      iMode = iModePrevious;
      break;
    case O_Water_p:
      gTargetPalette = gPalWater;
      iMode = iModePrevious;
      break;
    case O_Blorange_p:
      gTargetPalette = Blorange_p;
      iMode = iModePrevious;
      break;
    case O_RuGBY_p:
      gTargetPalette = RuGBY_p;
      iMode = iModePrevious;
      break;
    case O_Holly_p:
      gTargetPalette = Holly_p;
      iMode = iModePrevious;
      break;
    case O_Aquorange_p:
      gTargetPalette = Aquorange_p;
      iMode = iModePrevious;
      break;
    case O_Pastel_p:
      gTargetPalette = Pastel_p;
      iMode = iModePrevious;
      break;
    case O_Stars_p:
      gTargetPalette = Stars_p;
      iMode = iModePrevious;
      break;
    case O_Grurple_p:
      gTargetPalette = Grurple_p;
      iMode = iModePrevious;
      break;
    case O_Ruthies_p:
      gTargetPalette = Ruthies_p;
      iMode = iModePrevious;
      break;
    case O_AquaGray_p:
      gTargetPalette = AquaGray_p;
      iMode = iModePrevious;
      break;
    case O_Monochrome_p:
      gTargetPalette = Monochrome_p;
      iMode = iModePrevious;
      break;
    case O_SantaHat_p:
      gTargetPalette = SantaHat_p;
      iMode = iModePrevious;
      break;
    case O_Black_p:
      gTargetPalette = Black_p;
      iMode = iModePrevious;
      break;
//---------------------------------- All One Colour ----------------------------
    case O_OneColour:
      gTargetPalette = CRGBPalette16(CHSV(iOneColHue, 255, 255));
      iMode = iModePrevious;
      break;
//---------------------------------- All One Colour ----------------------------
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
  static byte heat[(iStripLength-1)];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < (iStripLength-iTopStar); i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / (iStripLength-iTopStar)) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= (iStripLength-iTopStar) - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < (iStripLength-iTopStar); j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      int pixelnumber;

//      CRGB color = ColorFromPalette(gTargetPalette, colorindex);
      CRGB color = ColorFromPalette(gCurrentPalette, colorindex);
      if( gReverseDirection ) {
//        CRGB color = ColorFromPalette( gPalWater, colorindex);
        pixelnumber = ((iStripLength-iTopStar)-1) - j;
        ledsA[pixelnumber] = color;
        ledsB[pixelnumber] = color;
      } else {
//        CRGB color = ColorFromPalette( gPalFire, colorindex);
        pixelnumber = j;
        ledsA[pixelnumber] = color;
        ledsB[pixelnumber] = color;
      }
//      ledsA[pixelnumber] = color;
    }
}

//------------------ All the rest is Mark Kriegsman's TwinkleFOX ---------------
//  This function loops over each pixel, calculates the 
//  adjusted 'clock' that this pixel should use, and calls 
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color or the background color, 
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
  &RuGBY_p,
  //&RetroC9_p,
  &BlueWhite_p,
  &RainbowColors_p,
  &FairyLight_p,
  &RedGreenWhite_p,
  &PartyColors_p,
  &Aquorange_p,
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

