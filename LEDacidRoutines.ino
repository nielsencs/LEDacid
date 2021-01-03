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
// =============================================================================
void Sparkle(byte iHard){
// =============================================================================
//      gCurrentPalette = gTargetPalette;

  switch (iHard){
    case 0:
      ledsA.fadeToBlackBy(10);
      ledsB.fadeToBlackBy(10);
      break;
    case 1:
      ledsA.fadeLightBy(14);
      ledsB.fadeLightBy(14);
      break;
//    case 2:
//      ledsA.fadeLightBy(100);
//      ledsB.fadeToBlackBy(1);
//      break;
  }
  ledsA[random8(iStripLength-iTopStar)] = ColorFromPalette(gCurrentPalette, random8());
  ledsB[random8(iStripLength-iTopStar)] = ColorFromPalette(gCurrentPalette, random8());

  FastLED.show();
}
// =============================================================================
void setTopStar(){
// =============================================================================
  if (iTopStar==1){
//    ledsA[iStripLength-1] = CRGB::Gold;
//    ledsB[iStripLength-1] = CRGB::Gold;
    ledsA[iStripLength-1] = CRGB::White;
    ledsB[iStripLength-1] = CRGB::White;
  }
}
// =============================================================================
void classicCycle(){
// =============================================================================
//  byte i;
//  byte j;
  static byte iBright = 255;
  static byte iBulbA = 0;
  static byte iBulbB = 1;

  if (iBright == 0){
    iBulbA+=2;
    if(iBulbA == 4){
      iBulbA = 0;
    }
    iUp=1;
  }

  if (iBright == 255){
    iBulbB+=2;
    if(iBulbB == 5){
      iBulbB = 1;
    }
    iUp=0;
  }

  if(iUp == 1){ // we're brightening!
    iBright++;    
  } else{
    iBright--;
  }
//  Serial.println(iBright);
  for (byte i=0; i < iStripLength; i += 4){
    setRYGB(i + iBulbA, iBright);
    setRYGB(i + iBulbB, 255-iBright);
  }
  FastLED.show();
}
// =============================================================================
void setRYGB(byte iBulb, byte iBright){ // splits the led strips into RYGB
// =============================================================================
  switch (iBulb%4){
    case 0: // Red
      ledsA[iBulb] = CRGB(iBright, 0, 0);
      ledsB[iBulb] = CRGB(iBright, 0, 0);
      break;
    case 1: // Yellow (Gold)
      ledsA[iBulb] = CRGB(iBright, iBright, 0);
      ledsB[iBulb] = CRGB(iBright, iBright, 0);
      break;
    case 2: // Green
      ledsA[iBulb] = CRGB(0, iBright, 0);
      ledsB[iBulb] = CRGB(0, iBright, 0);
      break;
    case 3: // Blue
      ledsA[iBulb] = CRGB(0, 0, iBright);
      ledsB[iBulb] = CRGB(0, 0, iBright);
      break;
  }
}
// =============================================================================
void doDynamicOptions(){
// =============================================================================  
  switch (iMode){
    case O_TopStar:
      iTopStar=1-iTopStar;
      iMode = iModePrevious;
      break;
    case O_CyclePalettes:
      bCyclePalettes=!bCyclePalettes;
      iMode = iModePrevious;
      break;
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
  static uint8_t hue;
  static byte iLED = 0;
//  iMode = O_TwinkleFOX; // uncomment to test one mode
  
  setTopStar();

  switch (iMode){
// ------------------------------------------ Patterns ------------------------------------------
    case O_Still:
      EVERY_N_MILLISECONDS(10) {
        for (byte c = 0; c< (iStripLength-iTopStar); c++) {
          byte fPaletteSlot = int(iPaletteEnd-c*(float(iPaletteEnd)/float(iStripLength-iTopStar)));
          if(bFirstTimeRound){
            Serial.print(c);
            Serial.print(" <-c fPaletteSlot-> ");
            Serial.println(fPaletteSlot);
          }
          ledsA[c] = ColorFromPalette(gCurrentPalette, fPaletteSlot); //, 128, NOBLEND);
          ledsB[c] = ColorFromPalette(gCurrentPalette, fPaletteSlot); //, 128, NOBLEND);
        }
        FastLED.show();
      }
      break;
    case O_FadePalette:
      EVERY_N_MILLISECONDS(70) {
        for(byte i = 0; i < (iStripLength-iTopStar); i++){
          ledsA[i] = ColorFromPalette(gCurrentPalette, iHueMain);
          ledsB[i] = ColorFromPalette(gCurrentPalette, iHueMain);
        }
        FastLED.show();
        iHueMain++; // since this is a 'byte' will keep cycling from 0 to 255
      }
      break;
    case O_SlowCycle:
      EVERY_N_MILLISECONDS(70) {
        for(byte i = 0; i < (iStripLength-iTopStar); i++){
          ledsA[i] = ColorFromPalette(gCurrentPalette, iHueMain - i);
          ledsB[i] = ColorFromPalette(gCurrentPalette, iHueMain - i);
        }
        FastLED.show();
        iHueMain--; // since this is a 'byte' will keep cycling from 0 to 255
      }
      break;
    case O_Cascade:
      EVERY_N_MILLISECONDS(42) {
//        Serial.print(" iLED:");
//        Serial.print(iLED);
        if (iLED == 0){
          iLED = (iStripLength-iTopStar-1);
        }
        ledsA.fadeToBlackBy(7);
        ledsA[iLED] = ColorFromPalette(gCurrentPalette, hue++);
        ledsB.fadeToBlackBy(7);
        ledsB[iLED] = ColorFromPalette(gCurrentPalette, hue++);
        iLED--;
      }
      setTopStar();
      FastLED.show();
      break;
    case O_TwinCascade:
      for(byte i = 0; i < (iStripLength-iTopStar-1)/2; i++) {
        // fade everything out
        ledsA.fadeToBlackBy(20);
        ledsB.fadeToBlackBy(20);

//        ledsA[i] = ColorFromPalette(gTargetPalette, hue++);
//        ledsB[i] = ColorFromPalette(gTargetPalette, hue++);
        ledsA[i] = ColorFromPalette(gCurrentPalette, hue++);
        ledsB[i] = ColorFromPalette(gCurrentPalette, hue++);

        // now, let's first 20 leds to the top 20 leds, 
        ledsA((iStripLength-iTopStar-1)/2,(iStripLength-iTopStar)-1) = ledsA((iStripLength-iTopStar-1)/2 - 1 ,0);
        ledsB((iStripLength-iTopStar-1)/2,(iStripLength-iTopStar)-1) = ledsB((iStripLength-iTopStar-1)/2 - 1 ,0);
      }
      break;
    case O_Sparkle:
      Sparkle(0);
      break;
    case O_Shimmer:
      Sparkle(1);
      break;
    case O_Shuffle:
      Sparkle(2);
      break;
    case O_TwinkleFOX:
      drawTwinkles(ledsA);
      drawTwinkles(ledsB);
      setTopStar();
      FastLED.show();
      break;
    case O_FireWater:
// ---------------------------------- Mark Kriegsman's Fire2012 ----------------------------------
      // Add entropy to random number generator; we use a lot of it.
//       random16_add_entropy( random());
    
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
// --------------------------------- Mark Kriegsman's Fire2012 ----------------------------------
      break;
// ------------------------------------------ Patterns ------------------------------------------
// ------------------------------------------ Specials ------------------------------------------
    case O_Fire:
      gTargetPalette = gPalFire;
      gCurrentPalette = gPalFire;
      gReverseDirection = false;
      iMode = O_FireWater;
      break;
    case O_Water:
      gTargetPalette = gPalWater;
      gCurrentPalette = gPalWater;
      gReverseDirection = true;
      iMode = O_FireWater;
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
    case O_Classic:
      if(bFirstTimeRound){
        FastLED.clear();
      }

      classicCycle();
      break;
    case O_SantaHat:
      gTargetPalette = SantaHat_p;
      gCurrentPalette = SantaHat_p;
      iMode = O_Still;
      break;
    case O_Police:
      EVERY_N_MILLISECONDS(37) {
        for(byte i = (iStripLength-iTopStar-1)/3; i < ((iStripLength-iTopStar-1)/3)*2; i++) {
          ledsA[i] = CRGB(255*iFlipRed, 0, 0);
          ledsB[i] = CRGB(255*iFlipRed, 0, 0);
        }
        iFlipRed = 1 - iFlipRed;
        FastLED.show();
      }
      EVERY_N_MILLISECONDS(330) {
        for(byte i = 0; i < (iStripLength-iTopStar-1)/2; i++) {
          ledsA[i] = CRGB(0, 0, 255*iFlipBlue);
          ledsA[(iStripLength-iTopStar-1) - i] = CRGB(0, 0, 255-(255*iFlipBlue));
          ledsB[i] = CRGB(0, 0, 255*iFlipBlue);
          ledsB[(iStripLength-iTopStar-1) - i] = CRGB(0, 0, 255-(255*iFlipBlue));
        }
        iFlipBlue = 1 - iFlipBlue;
        FastLED.show();
      }
      break;
    case O_PsychOut:
      gTargetPalette = RuGBY_p;
      gCurrentPalette = RuGBY_p;
      iMode = O_FireWater;
      bCyclePalettes=false;
      break;
// ------------------------------------------ Specials ------------------------------------------
// --------------------------------------- Old Favourites ---------------------------------------
    case O_TwinkleFOXClassic:
      gTargetPalette = FairyLight_p;
      gCurrentPalette = FairyLight_p;
      iMode = O_TwinkleFOX;
      bCyclePalettes=false;
      break;
    case O_SlowCycleRainbow:
      gTargetPalette = RainbowColors_p;
      gCurrentPalette = RainbowColors_p;
      iMode = O_SlowCycle;
      bCyclePalettes=false;
      break;
    case O_SlowCycleRainbowStripe:
      gTargetPalette = RainbowStripeColors_p;
      gCurrentPalette = RainbowStripeColors_p;
      iMode = O_SlowCycle;
      bCyclePalettes=false;
      break;
    case O_CascadeRainbow:
      gTargetPalette = RainbowColors_p;
      gCurrentPalette = RainbowColors_p;
      iMode = O_Cascade;
      bCyclePalettes=false;
      break;
    case O_SparkleStar:
      if(bFirstTimeRound){
        FastLED.clear();
      }
      gTargetPalette = Stars_p;
      gCurrentPalette = Stars_p;
      iMode = O_Shimmer;
      bCyclePalettes=false;
      break;
    case O_SparkleWhiteGold:
      if(bFirstTimeRound){
        FastLED.clear();
      }
      gTargetPalette = WhiteGold_p;
      gCurrentPalette = WhiteGold_p;
      iMode = O_Sparkle;
      bCyclePalettes=false;
      break;
    case O_TwinkleFOXHolly:
      gTargetPalette = Holly_p;
      gCurrentPalette = Holly_p;
      iMode = O_TwinkleFOX;
      bCyclePalettes=false;
      break;
    case O_Snowing:
      gTargetPalette = Monochrome_p;
      gCurrentPalette = Monochrome_p;
      iMode = O_SlowCycle;
      bCyclePalettes=false;
      break;
    case O_SparkleBlorange:
      if(bFirstTimeRound){
        FastLED.clear();
      }
      gTargetPalette = Blorange_p;
      gCurrentPalette = Blorange_p;
      iMode = O_Shimmer;
      bCyclePalettes=false;
      break;
// --------------------------------------- Old Favourites ---------------------------------------
// ------------------------------------------ Palettes ------------------------------------------
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
    case O_RGB_p:
      gTargetPalette = RGB_p;
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
    case O_RetroC9_p:
      gTargetPalette = RetroC9_p;
      iMode = iModePrevious;
      break;
    case O_BlueWhite_p:
      gTargetPalette = BlueWhite_p;
      iMode = iModePrevious;
      break;
    case O_RedGreenWhite_p:
      gTargetPalette = RedGreenWhite_p;
      iMode = iModePrevious;
      break;
    case O_Snow_p:
      gTargetPalette = Snow_p;
      iMode = iModePrevious;
      break;
    case O_Ice_p:
      gTargetPalette = Ice_p;
      iMode = iModePrevious;
      break;
    case O_Black_p:
      gTargetPalette = Black_p;
      iMode = iModePrevious;
      break;
    case O_White_p:
      gTargetPalette = White_p;
      iMode = iModePrevious;
      break;
// ------------------------------------------ Palettes ------------------------------------------
// --------------------------------------- All One Colour ---------------------------------------
    case O_OneColour:
      gTargetPalette = CRGBPalette16(CHSV(iOneColHue, 255, 255));
      iMode = iModePrevious;
      break;
// --------------------------------------- All One Colour ---------------------------------------
    default:
      iMode = 0;
      break;
  }
}

// ---------------------------------- Mark Kriegsman's Fire2012 ----------------------------------
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
// ---------------------------------- Mark Kriegsman's Fire2012 ----------------------------------
