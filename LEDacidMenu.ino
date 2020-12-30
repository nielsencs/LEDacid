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
  if (request.indexOf("/LED=" + String(O_PausePlay)) != -1) {
    if (iLEDstatus == 2) {
      iLEDstatus = 1;
    } else {
      iLEDstatus = 2;
    }
  } else if (request.indexOf("/LED=" + String(O_FadeOut)) != -1 || request.indexOf("/LED=" + String(O_PausePlay)) != -1) {
    iLEDstatus = 0;
  }else{
    if (request.indexOf("/LED=") != -1) {
      iLEDstatus=1;
      iModePrevious = iMode;
      iMode = request.substring(request.indexOf("/LED=")+5, request.indexOf(" HTTP")).toInt();
      bFirstTimeRound = true;
      if (request.indexOf("/Hue=" + String(O_OneColour)) != -1) {
        iOneColHue = request.substring(request.indexOf("/Hue=")+5, request.indexOf(" HTTP")).toInt();
      }
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
  client.println("* {margin: 0; padding:0; box-sizing: border-box; font-family: 'Helvetica', Arial; font-weight: bold; font-size: large;}");
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
//  client.println("    float: left; width: 98%;");
//  client.println("    float: left; width: 48%;");
  client.println("    float: left; width: 31%;");
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
  client.println("      <p align=center>-~=#X ----- Old Favourites ----- X#=~-</p>"); // <========================================================== dodgy carl code!
  client.println("      <li><a href='LED=" + String(O_TwinkleFOXClassic) +                    "'>Twinkle Fairy</a></li>");
  client.println("      <li><a href='LED=" + String(O_SlowCycleRainbow) +                     "'>Rainbow Slow Cycle</a></li>");
  client.println("      <li><a href='LED=" + String(O_TwinkleFOXHolly) +                      "'>Twinkle Holly</a></li>");
  
  client.println("      <li><a href='LED=" + String(O_SlowCycleRainbowStripe) +               "'>Rainbow Trickle</a></li>");
  client.println("      <li><a href='LED=" + String(O_SparkleStar) +                          "'>Sparkle Stars</a></li>");
  client.println("      <li><a href='LED=" + String(O_SparkleWhiteGold) +                     "'>WhiteGold Sparkle</a></li>");
  
  client.println("      <li><a href='LED=" + String(O_Snowing) +                              "'>It&rsquo;s Snowing!</a></li>");
  client.println("      <li><a href='LED=" + String(O_TwinkleStar) +                          "'>Twinkle, Twinkle Little Star</a></li>");
  client.println("      <li><a href='LED=" + String(O_SparkleBlorange) +                      "'>Sparkle Blorange</a></li>");
//  client.println("      <p align=center>-~=#X ----- Specials ----- X#=~-</p>"); // <========================================================== dodgy carl code!
  client.println("      <li><a href='LED=" + String(O_Fire) +                                 "'>Fire</a></li>");
  client.println("      <li><a href='LED=" + String(O_Water) +                                "'>Water</a></li>");
  client.println("      <li><a href='LED=" + String(O_SantaHat) +                             "'>Santa&rsquo;s Hat</a></li>");
  
  client.println("      <li><a href='LED=" + String(O_Lightning) +                            "'>Lightning</a></li>");
  client.println("      <li><a href='LED=" + String(O_Police) +                               "'>Stop - Police!</a></li>");
  client.println("      <li><a href='LED=" + String(O_SlowCycle) +                            "'>-</a></li>");
  client.println("      <p align=center>-~=#X ----- Patterns ----- X#=~-</p>"); // <========================================================== dodgy carl code!
  client.println("      <li><a href='LED=" + String(O_Still) +                                "'>Still</a></li>");
  client.println("      <li><a href='LED=" + String(O_FadePalette) +                          "'>Fade Palette</a></li>");
  client.println("      <li><a href='LED=" + String(O_SlowCycle) +                            "'>Slow Cycle</a></li>");
  client.println("      <li><a href='LED=" + String(O_TwinkleFOX) +                           "'>Twinkle</a></li>");
  client.println("      <li><a href='LED=" + String(O_SparkleSoft) +                          "'>Soft Sparkle</a></li>");
  client.println("      <li><a href='LED=" + String(O_SparkleHard) +                          "'>Sparkle</a></li>");
  client.println("      <li><a href='LED=" + String(O_FireWater) +                            "'>Fire / Water</a></li>");
  client.println("      <li><a href='LED=" + String(O_Cascade) +                              "'>Cascade</a></li>");
  client.println("      <li><a href='LED=" + String(O_TwinCascade) +                          "'>Twin Cascade</a></li>");
  client.println("      <li><a href='LED=" + String(O_Classic) +                              "'>Classic</a></li>");
  client.println("      <li><a href='LED=" + String(O_TopStar) +                              "'>Top Star</a></li>");
  client.println("      <li><a href='LED=" + String(O_PausePlay) +                            "'>Pause / Play</a></li>");
  client.println("      <li><a href='LED=" + String(O_FadeOut) +                              "'>Fade &lsquo;em out</a></li>");
  client.println("      <p align=center>-~=#X ----- Palettes ----- X#=~-</p>"); // <========================================================== dodgy carl code!
  client.println("      <li><a href='LED=" + String(O_RainbowColors_p) +                       "'>RainbowColors</a></li>");
  client.println("      <li><a href='LED=" + String(O_RainbowStripeColors_p) +                 "'>RainbowStripeColors</a></li>");
  client.println("      <li><a href='LED=" + String(O_RuGBY_p) +                               "'>RuGBY</a></li>");
  client.println("      <li><a href='LED=" + String(O_PartyColors_p) +                         "'>PartyColors</a></li>");
  client.println("      <li><a href='LED=" + String(O_Pastel_p) +                              "'>Pastel</a></li>");
  client.println("      <li><a href='LED=" + String(O_Dawn_p) +                                "'>Dawn</a></li>");
  client.println("      <li><a href='LED=" + String(O_Fire_p) +                                "'>Fire</a></li>");
  client.println("      <li><a href='LED=" + String(O_Water_p) +                               "'>Water</a></li>");
  client.println("      <li><a href='LED=" + String(O_OceanColors_p) +                         "'>OceanColors</a></li>");
  client.println("      <li><a href='LED=" + String(O_CloudColors_p) +                         "'>CloudColors</a></li>");
  client.println("      <li><a href='LED=" + String(O_ForestColors_p) +                        "'>ForestColors</a></li>");
  client.println("      <li><a href='LED=" + String(O_WhitePurple_p) +                         "'>WhitePurple</a></li>");
  client.println("      <li><a href='LED=" + String(O_WhiteGold_p) +                           "'>WhiteGold</a></li>");
  client.println("      <li><a href='LED=" + String(O_Blorange_p) +                            "'>Blorange</a></li>");
  client.println("      <li><a href='LED=" + String(O_Aquorange_p) +                           "'>Aquorange</a></li>");
  client.println("      <li><a href='LED=" + String(O_Grurple_p) +                             "'>Grurple</a></li>");
  client.println("      <li><a href='LED=" + String(O_Ruthies_p) +                            "'>Ruthie&rsquo;s</a></li>");
  client.println("      <li><a href='LED=" + String(O_AquaGray_p) +                            "'>AquaGray</a></li>");
  client.println("      <li><a href='LED=" + String(O_Holly_p) +                               "'>Holly</a></li>");
  client.println("      <li><a href='LED=" + String(O_FairyLight_p) +                          "'>Fairy Lights</a></li>");
  client.println("      <li><a href='LED=" + String(O_Stars_p) +                               "'>Stars</a></li>");
  client.println("      <li><a href='LED=" + String(O_SantaHat_p) +                            "'>Santa&rsquo;s Hat</a></li>");
  client.println("      <li><a href='LED=" + String(O_Monochrome_p) +                          "'>Monochrome</a></li>");
  client.println("      <li><a href='LED=" + String(O_Black_p) +                               "'>Black</a></li>");
//  client.println("      <li><a href='LED=" + String(O_OneColour) +                              "&Hue=0'>All One Colour:</a></li>");
//  client.println("      <form><input type='text' name='Hue' value='" + String(iOneColHue) + "'></form>"); // <========================================================== dodgy carl code!
//  client.println("      <input type='range' min='0' max='255' value='128' class='slider' id='iOneColHue'>"); // <========================================================== dodgy carl code!
//  client.println('      <form action="/get"><input type="hidden" id="LED" value="' + String(O_OneColour) + '"><input type="range" min="0" max="255" value="128" class="slider" id="OneColHue"><input type="submit" value="All One Colour"></form>');
  client.println("    </ul><br><br><br>");
  client.println("  </body>");
  client.println("</html>");
//---------------------------------- Marvin menu -------------------------------
  FastLED.delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
