// Patterns
const byte O_Still = 99;
const byte O_SlowCycle = 0;
const byte O_Cascade = 111;
const byte O_TwinCascade = 1;
const byte O_Sparkle = 5;
const byte O_FireWater = 15;
const byte O_Fire = 17;
const byte O_Water = 16;
const byte O_TwinkleFOX = 18;

//Specials
const byte O_SantaHat = 2;
const byte O_Lightning = 3;
const byte O_TwinkleStar = 4;
const byte O_SparkleStar = 112;
const byte O_Police = 6;
const byte O_Classic = 12;
const byte O_PausePlay = 254;
const byte O_FadeOut = 255;

//Palettes
const byte O_TwinkleFOXfairy = 20;
const byte O_CloudColors = 21;
const byte O_RainbowColors = 22;
const byte O_LavaColors = 23;
const byte O_OceanColors = 24;
const byte O_ForestColors = 25;
const byte O_RainbowStripeColors = 26;
const byte O_PartyColors = 27;
const byte O_WhiteGold = 28;
const byte O_Fire_p = 29;
const byte O_Water_p = 30;
const byte O_Blorange = 31;
const byte O_Classic_p = 32;
const byte O_Holly = 33;
const byte O_RedWhite = 34;
const byte O_Pastel = 35;
const byte O_Stars = 36;
const byte O_Grurple = 37;
const byte O_AquaGray = 38;
const byte O_OneColour = 39;

// =============================================================================
void doWeb() {
// =============================================================================
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("New client");
  while(!client.available()){
    FastLED.delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
  if (request.indexOf("/LED=254") != -1) {
    if (iLEDstatus == 2) {
      iLEDstatus = 1;
    } else {
      iLEDstatus = 2;
    }
  } else if (request.indexOf("/LED=255") != -1 || request.indexOf("/LED=254") != -1) {
    iLEDstatus = 0;
  }else{
    if (request.indexOf("/LED=") != -1) {
      iLEDstatus=1;
      iModePrevious = iMode;
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
  client.println("* {margin: 0; padding:0; box-sizing: border-box; font-family: 'Helvetica', Arial; font-weight: bold; font-size: xx-large;}");
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
//  client.println("    float: left; width: 98%;");
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
  client.println("      <p align=center>-~=#X ----- Patterns ----- X#=~-</p>"); // <========================================================== dodgy carl code!
//  client.println("      <li><a href='LED=99'>Still</a></li>");
  client.println("      <li><a href='LED=" + String(O_Still) + "'>Still</a></li>");
  client.println("      <li><a href='LED=" + String(O_SlowCycle) + "'>Slow Cycle</a></li>");
  client.println("      <li><a href='LED=" + String(O_TwinkleFOX) + "'>TwinkleFOX</a></li>");
  client.println("      <li><a href='LED=" + String(O_Sparkle) + "'>Sparkle</a></li>");
  client.println("      <li><a href='LED=" + String(O_Cascade) + "'>Cascade</a></li>");
  client.println("      <li><a href='LED=" + String(O_TwinCascade) + "'>Twin Cascade</a></li>");
  client.println("      <li><a href='LED=" + String(O_FireWater) + "'>Fire / Water</a></li>");
  client.println("      <li><a href='LED=0'>-</a></li>");
  client.println("      <p align=center>-~=#X ----- Specials ----- X#=~-</p>"); // <========================================================== dodgy carl code!
  client.println("      <li><a href='LED=" + String(O_Fire) + "'>Fire</a></li>");
  client.println("      <li><a href='LED=" + String(O_Water) + "'>Water</a></li>");
  client.println("      <li><a href='LED=" + String(O_SantaHat) + "'>Santa&lsquo;s Hat</a></li>");
  client.println("      <li><a href='LED=" + String(O_Lightning) + "'>Lightning</a></li>");
  client.println("      <li><a href='LED=" + String(O_TwinkleStar) + "'>Twinkle, Twinkle Little Star</a></li>");
  client.println("      <li><a href='LED=" + String(O_SparkleStar) + "'>Sparkle Stars</a></li>");
  client.println("      <li><a href='LED=" + String(O_Police) + "'>Stop - Police!</a></li>");
  client.println("      <li><a href='LED=12'>Classic</a></li>");
//  client.println("      <li><a href='LED=0'>-</a></li>");
  client.println("      <li><a href='LED=254'>Pause / Play</a></li>");
  client.println("      <li><a href='LED=255'>Fade &lsquo;em out</a></li>");
  client.println("      <p align=center>-~=#X ----- Colour Schemes ----- X#=~-</p>"); // <========================================================== dodgy carl code!
  client.println("      <li><a href='LED=22'>RainbowColors</a></li>");
  client.println("      <li><a href='LED=26'>RainbowStripeColors</a></li>");
  client.println("      <li><a href='LED=27'>PartyColors</a></li>");
  client.println("      <li><a href='LED=32'>Classic</a></li>");
  client.println("      <li><a href='LED=29'>Fire</a></li>");
  client.println("      <li><a href='LED=23'>LavaColors</a></li>");
  client.println("      <li><a href='LED=30'>Water</a></li>");
  client.println("      <li><a href='LED=24'>OceanColors</a></li>");
  client.println("      <li><a href='LED=21'>CloudColors</a></li>");
  client.println("      <li><a href='LED=25'>ForestColors</a></li>");
  client.println("      <li><a href='LED=28'>WhiteGold</a></li>");
  client.println("      <li><a href='LED=31'>Blorange</a></li>");
  client.println("      <li><a href='LED=37'>Grurple</a></li>");
  client.println("      <li><a href='LED=38'>AquaGray</a></li>");
  client.println("      <li><a href='LED=33'>Holly</a></li>");
  client.println("      <li><a href='LED=34'>RedWhite</a></li>");
  client.println("      <li><a href='LED=35'>Pastel</a></li>");
  client.println("      <li><a href='LED=20'>TwinkleFOX fairy</a></li>");
  client.println("      <li><a href='LED=36'>Stars</a></li>");
//  client.println("      <li><a href='LED=39'>All One Colour:</a></li>");
//  client.println("      <li><form><input type='text' name='Hue' value='167'></form></li>"); // <========================================================== dodgy carl code!
//  client.println("      <form><input type='text' name='Hue' value='" + iOneColHue + "'></form>"); // <========================================================== dodgy carl code!
  client.println("    </ul><br><br><br>");
  client.println("  </body>");
  client.println("</html>");
//---------------------------------- Marvin menu -------------------------------
  FastLED.delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}

