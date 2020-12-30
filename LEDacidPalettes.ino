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
//const TProgmemRGBPalette16 gPalFire = 
//{
//  CRGB(64,0,0), CRGB(128,0,0), CRGB::Red,    CRGB::Red,
//  CRGB::Orange, CRGB::Orange,  CRGB::Red,    CRGB::Orange,
//  CRGB::Orange, CRGB::Orange,  CRGB::Gold,   CRGB::Gold,
//  CRGB::Gold,   CRGB::Orange,  CRGB::Gold,   CRGB::Yellow
//};

#define CARLS_ORANGE   0xFF7700
#define CARLS_DK_BL    0x070014

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

const TProgmemRGBPalette16 gPalFire = 
{
  CRGB::Black,  CRGB::Black,   CRGB::Red,      CARLS_ORANGE,
  CARLS_ORANGE, CRGB::Orange,  CRGB::Orange,   CRGB::Orange,
  CRGB::Orange, CRGB::Orange,  CRGB::Orange,   CRGB::Orange,
  CRGB::Orange, CRGB::Orange,  CARLS_ORANGE,   CRGB::Red
};
//  CRGB::Black,  CARLS_DK_BL,   CARLS_DK_BL,    CARLS_DK_BL
const TProgmemRGBPalette16 gPalWater = 
{
  CRGB::Black,  CRGB::Blue,   CRGB::Blue, CRGB::Aqua, 
  CRGB::Blue,   CRGB::Aqua,   CRGB::Blue, CRGB::Blue,
  CRGB::Aqua,   CRGB::Aqua,   CRGB::Aqua, CRGB::Aqua,  
  CRGB::White,  CRGB::White,  CRGB::Aqua, CRGB::White
};

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
{  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly (dark) green palette with red berries.
//#define Holly_Green 0x00580c
#define Holly_Green 0x004009
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p =
{  Holly_Red,   Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green  
};

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
const TProgmemRGBPalette16 FairyLight_p =
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
#define C9_White  0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
{  C9_Red,    C9_Orange, C9_Red,    C9_Orange,
   C9_Orange, C9_Red,    C9_Orange, C9_Red,
   C9_Green,  C9_Green,  C9_Green,  C9_Green,
   C9_Blue,   C9_Blue,   C9_Blue,
   C9_White
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

/*const TProgmemRGBPalette16 Blorange_p =
{ 
  0xFF8000, 0xEE7711, 0xDD6F22, 0xCC6633,
  0xBB5E44, 0xAA5555, 0x994D66, 0x884477,
  0x773C88, 0x663399, 0x552BAA, 0x4422BB,
  0x331ACC, 0x2211DD, 0x1109EE, 0x0000FF
}; */
//const TProgmemRGBPalette16 Blorange_p =
//{  CRGB::DarkOrange,   CRGB::Orange,      CRGB::LightYellow,  CRGB::Orange,
//   CRGB::Tan,          CRGB::DarkOrange,  CRGB::Tan,          CRGB::DarkBlue,
//   CRGB::PapayaWhip,   CRGB::SlateGray,   CRGB::Aqua,         CRGB::DarkOrange,
//   CRGB::DeepSkyBlue,  CRGB::SlateBlue,   CRGB::Blue,         CRGB::Orange };
//
const TProgmemRGBPalette16 Blorange_p =
{
  0xFF8000, 0xFF7000, 0xFF6000, 0xFF5000,
  0x886600, 0x886633, 0x886666, 0x886699,
  0x4499CC, 0x3388DD, 0x2277EE, 0x1166FF,
  0x0066FF, 0x0044FF, 0x0022FF, 0x0000FF
};

const TProgmemRGBPalette16 Aquorange_p =
{
  CRGB::Aqua, 0XFF6600, 0XFF6600, 0XFF6600,
  0XFF6600,   0XFF6600, 0XFF6600, 0XFF6600,
  0XFF6600,   0XFF6600, 0XFF6600, 0XFF6600,
  0XFF6600,   0XFF6600, 0XFF6600, 0XFF6600
};

const TProgmemRGBPalette16 Grurple_p =
{
  0x00FF00, 0x00FF11, 0x00FF22, 0x00FF33,
  0x00CC44, 0x009955, 0x006666, 0x003377,
  0x000088, 0x330099, 0x330066, 0x330033,
  0x220022, 0x110011, 0x070007, 0x030003 
};

const TProgmemRGBPalette16 Dawn_p = 
{
  0x0033FF, 0x0066FF, 0x0099FF, 0x00AAFF,
  0x00FFFF, 0x33FFAA, 0x66FF99, 0x99FF66,
  0xAAFF33, 0xFFFF33, 0xFFAA33, 0xFF9933,
  0xAA6600, 0x993300, 0x660000, 0x990000
};

const TProgmemRGBPalette16 Ruthies_p =
{
  0xBE20F7,   0xBE20F7,   0xBE20F7,   //PURPLE
  0xFF5454,   0xFF5454,   0xFF5454,   //CORAL
  0xFF8F1F,   0xFF8F1F,   0xFF8F1F,   //ORANGE
  0xF7D720,   0xF7D720,   0xF7D720,   //YELLOW
  0x3697C2,   0x3697C2,   0x3697C2,   //BLUE
  0x06BF85                            //TURQUOISE
};

const TProgmemRGBPalette16 AquaGray_p =
{  CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray,
   CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray,
   CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray,
   CRGB::Aqua,         CRGB::Gray,        CRGB::Aqua,         CRGB::Gray};

//const TProgmemRGBPalette16 WhiteGold_p =
//{  CRGB::White,       CRGB::Orange,       CRGB::White,        CRGB::White, 
//   CRGB::White,       CRGB::White,        CRGB::LightYellow,  CRGB::White, 
//   CRGB::PapayaWhip,  CRGB::White,        CRGB::White,        CRGB::White, 
//   CRGB::White,       CRGB::White,        CRGB::White,        CRGB::LemonChiffon };

const TProgmemRGBPalette16 WhiteGold_p =
{  CRGB::White,       CRGB::Orange,       CRGB::FairyLight,   CRGB::FairyLight, 
   CRGB::White,       CRGB::Orange,       CRGB::LightYellow,  CRGB::FairyLight, 
   CRGB::PapayaWhip,  CRGB::FairyLight,   CRGB::FairyLight,   CRGB::White, 
   CRGB::White,       CRGB::Orange,       CRGB::White,        CRGB::LemonChiffon
};

const TProgmemRGBPalette16 RuGBY_p =
{
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue,
  CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::Blue
};


const TProgmemRGBPalette16 RGB_p =
{
  CRGB::White,  CRGB::Red,   CRGB::Green, CRGB::Blue,
  CRGB::Red,    CRGB::Green, CRGB::Blue,  CRGB::Red,
  CRGB::Green,  CRGB::Blue,  CRGB::Red,   CRGB::Green,
  CRGB::Blue,   CRGB::Red,   CRGB::Green, CRGB::Blue
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

//const CRGBPalette16 WhitePurple_p = CRGBPalette16(CRGB::White, CRGB::Purple, CRGB::Indigo);
const TProgmemRGBPalette16 WhitePurple_p =
{
  0xFFFFFF, 0xFFEEFF, 0xFFDDFF, 0xFFCCFF,
  0xFFAAFF, 0xFF99FF, 0xFF88FF, 0xFF77FF,
  0x773377, 0x673367, 0x563356, 0x463346,
  0x330033, 0x230023, 0x130013, 0x030003
};

const TProgmemRGBPalette16 SantaHat_p = 
{
  CRGB::White,  CRGB::White,  CRGB::Red,   CRGB::Red, 
  CRGB::Red,    CRGB::Red,    CRGB::Red,   CRGB::Red,
  CRGB::Red,    CRGB::Red,    CRGB::Red,   CRGB::Red,  
  CRGB::White,  CRGB::White,  CRGB::White, CRGB::White
};

const TProgmemRGBPalette16 Monochrome_p =
{
  CRGB::White,  CRGB::Black,  CRGB::White, CRGB::Black,
  CRGB::White,  CRGB::Black,  CRGB::White, CRGB::Black,
  CRGB::White,  CRGB::Black,  CRGB::White, CRGB::Black,
  CRGB::White,  CRGB::Black,  CRGB::White, CRGB::Black
};

//const CRGBPalette16 Black_p = CRGBPalette16(CRGB::Black); //SCOPE!!
//const TProgmemRGBPalette16 Black_p =
//{
//  0x000000, 0x000000, 0x000000, 0x000000,
//  0x000000, 0x000000, 0x000000, 0x000000,
//  0x000000, 0x000000, 0x000000, 0x000000,
//  0x000000, 0x000000, 0x000000, 0x000000
//};

const TProgmemRGBPalette16 White_p =
{
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF
};

