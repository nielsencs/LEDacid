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

#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p =
{  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A mostly (dark) green palette with red berries.
//#define Holly_Green 0x00580c
#define Holly_Green 0x00500A
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p =
{  Holly_Red,   Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green  
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
{  CRGB::Green,        CRGB::Green,       CRGB::Green,        CRGB::Purple,
   CRGB::DarkGreen,    CRGB::DarkGreen,   CRGB::Indigo,       CRGB::Green,
   CRGB::Blue,         CRGB::Magenta,     CRGB::Aqua,         CRGB::Orchid,
   CRGB::MidnightBlue, CRGB::DarkGreen,   CRGB::Purple,       CRGB::Plum
};

const TProgmemRGBPalette16 Dawn_p = 
{
  0x1166FF, 0x2266EE, 0x3366DD, 0x4466CC,
  0x557799, 0x668899, 0x889999, 0xAA8899,
  0xAA8888, 0xEEEE99, 0xDDDD99, 0xCCCC99,
  0xBB9999, 0xAA6633, 0x995010, 0x994000
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
const CRGBPalette16 Black_p = CRGBPalette16(CRGB::Black);
const CRGBPalette16 WhitePurple_p = CRGBPalette16(CRGB::White, CRGB::Purple, CRGB::Indigo);

