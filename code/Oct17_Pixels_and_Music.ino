#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN        6 
#define NUMPIXELS 64 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 



int electric [24] = { 4,5 ,11,12, 18, 19, 25, 26, 27, 28, 29, 30, 33, 34, 35, 36, 37, 38,44, 45,51, 52, 58,59};
int smile [10] = { 18, 21, 26, 29, 41, 46, 50, 51, 52, 53};
int heart [18] = {9, 10, 13, 14, 16, 19, 20, 23,24,31,32,39,41,46,50, 53, 59, 60};
int tracknumber = 1;




// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
// create breakout-example object!
//Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
// create shield-example object!
Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);



void setup() {

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  pixels.begin();

  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Library Test");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,50);

  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
}


void loop() { 
  // needs a fourth track to start playing for the while loop to execute
  // unsure how to go around it
  musicPlayer.playFullFile("/track004.mp3");
  Serial.println(F("Next track"));


  while (musicPlayer.playingMusic) {
    if (musicPlayer.startPlayingFile("/track001.mp3")) {
      for(int i=0; i<24; i++) {
          pixels.setPixelColor(electric[i], pixels.Color(255, 255, 0));   // draw yellow electric symbol
          pixels.show();   // Send the updated pixel colors to the hardware.
          delay(DELAYVAL); // Pause before next pass through loop
      }
      Serial.println(F("Finished drawing electric"));
      pixels.clear();
      musicPlayer.stopPlaying();
    } 
    Serial.print(".");
    delay(1000);
  }


   musicPlayer.playFullFile("/track004.mp3");
  Serial.println(F("Next track"));

   while (musicPlayer.playingMusic) {
    Serial.println(F("Entered second loop"));
    if (musicPlayer.startPlayingFile("/track002.mp3")) {
      for(int i=0; i<10; i++) {
          pixels.setPixelColor(smile[i], pixels.Color(102, 51, 153)); // draw purple smile
          pixels.show();   // Send the updated pixel colors to the hardware.
          delay(DELAYVAL); // Pause before next pass through loop
      }
      Serial.println(F("Finished drawing smile"));
      pixels.clear();
      musicPlayer.stopPlaying();
    } 
    Serial.print(".");
    delay(1000);
  }


  
  musicPlayer.playFullFile("/track004.mp3");
  Serial.println(F("Next track"));

   while (musicPlayer.playingMusic) {
    Serial.println(F("Entered third loop"));
    if (musicPlayer.startPlayingFile("/track003.mp3")) {
      for(int i=0; i<18; i++) {
          pixels.setPixelColor(heart[i], pixels.Color(255, 0, 255)); // draw pink heart
          pixels.show();   // Send the updated pixel colors to the hardware.
          delay(DELAYVAL); // Pause before next pass through loop
      }
      Serial.println(F("Finished drawing a heart"));
      pixels.clear();
      musicPlayer.stopPlaying();
    } 
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Done playing music");

}




/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
