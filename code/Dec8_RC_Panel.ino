/*
   Using the nRF24L01 radio module to communicate
   between two Arduinos with much increased reliability following
   various tutorials, conversations, and studying the nRF24L01 datasheet
   and the library reference.

   Transmitter is
   https://github.com/michaelshiloh/resourcesForClasses/tree/master/kicad/Arduino_Shield_RC_Controller

  Receiver is
  https://github.com/instituteforcriticalrobotics/instituteforcriticalrobotics.github.io/tree/main/kicad/nRF_Mega

   This file contains code for both transmitter and receiver.
   Transmitter at the top, receiver at the bottom.
   One of them is commented out.

   These sketches require the RF24 library by TMRh20
   Documentation here: https://nrf24.github.io/RF24/index.html

   change log

   11 Oct 2023 - ms - initial entry based on
                  rf24PerformingRobotsTemplate
   26 Oct 2023 - ms - revised for new board: nRF_Servo_Mega rev 2
   28 Oct 2023 - ms - add demo of NeoMatrix, servo, and Music Maker Shield
*/

// Common code
//

// Common pin usage
// Note there are additional pins unique to transmitter or receiver
//

// nRF24L01 uses SPI which is fixed
// on pins 11, 12, and 13 on the Uno
// and on pins 50, 51, and 52 on the Mega

// It also requires two other signals
// (CE = Chip Enable, CSN = Chip Select Not)
// Which can be any pins:


// For the transmitter
//const int NRF_CE_PIN = A4, NRF_CSN_PIN = A5;

// for the receiver
const int NRF_CE_PIN = A11, NRF_CSN_PIN = A15;

// In summary,
// nRF 24L01 pin    Uno Mega   name
//          1                     GND
//          2                     3.3V
//          3       9   A11       CE
//          4       10  A15       CSN
//          5       13  SCLK
//          6       11  MOSI/COPI
//          7       12  MISO/CIPO

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);  // CE, CSN

//#include <printf.h>  // for debugging

// See note in rf24Handshaking about address selection
//

// Channel and address allocation:
// Fatema, Jannah: Channel 30, addr = 0x76
// Andres, Ryan: Channel 40, addr = 0x73
// Nouf, Shaikha:  Channel 50, addr = 0x7C
// Aadhar, Shanaia: Channel 60, addr = 0xC6
// Akhat, Yunho:  Channel 70, addr = 0xC3
// Aakif, Marta: Channel 80, addr = 0xCC
// Yerk, Hamad: Channel 90, addr = 0x33
const byte addr = 0xCC;              // change as per the above assignment
const int RF24_CHANNEL_NUMBER = 80;  // change as per the above assignment

// Do not make changes here
const byte xmtrAddress[] = { addr, addr, 0xC7, 0xE6, 0xCC };
const byte rcvrAddress[] = { addr, addr, 0xC7, 0xE6, 0x66 };

const int RF24_POWER_LEVEL = RF24_PA_LOW;

// global variables
uint8_t pipeNum;
unsigned int totalTransmitFailures = 0;

struct DataStruct {
  uint8_t stateNumber;
};
DataStruct data;

void setupRF24Common() {

  // RF24 setup
  if (!radio.begin()) {
    Serial.println(F("radio  initialization failed"));
    while (1)
      ;
  } else {
    Serial.println(F("radio successfully initialized"));
  }

  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF24_CHANNEL_NUMBER);
  radio.setPALevel(RF24_POWER_LEVEL);
}

/*
// Transmitter code

// Transmitter pin usage
const int LCD_RS_PIN = 3, LCD_EN_PIN = 2, LCD_D4_PIN = 4, LCD_D5_PIN = 5, LCD_D6_PIN = 6, LCD_D7_PIN = 7;
const int SW1_PIN = 8, SW2_PIN = 9, SW3_PIN = 10, SW4_PIN = A3, SW5_PIN = A2;

// LCD library code
#include <LiquidCrystal.h>

// initialize the library with the relevant pins
LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);


const int NUM_OF_STATES = 22;
char *theStates[] = {"0 starting setup",
                     "1 neck center",
                     "2 neck left",
                     "3 neck right",
                     "4 left & center",
                     "5 right & center",
                     "6 crazy head movement",
                     "7 track 1 ",
                     "8 track 2",
                     "9 track 3",
                     "10 track 4",
                     "11 track 5",
                     "12 standard eyes",
                     "13 track 6",
                     "14 track 7",
                     "15 sad eyes",
                     "16 love eyes",
                     "17 electric eyes",
                     "18 lego eyes",
                     "19 standard eyes",
                     "20 agreed",
                     "21 stop audio",
                    };

void updateLCD() {

  lcd.clear();
  lcd.print(theStates[data.stateNumber]);
  lcd.setCursor(0, 1); // column, line (from 0)
  lcd.print("not transmitted yet");
}

void countDown() {
  data.stateNumber = (data.stateNumber > 0) ? (data.stateNumber - 1) : 0;
  updateLCD();
}

void countUp() {
  if (++data.stateNumber >= NUM_OF_STATES) {
    data.stateNumber = NUM_OF_STATES - 1;
  }
  updateLCD();
}


void spare1() {}
void spare2() {}

void rf24SendData() {

  radio.stopListening(); // go into transmit mode
  // The write() function will block
  // until the message is successfully acknowledged by the receiver
  // or the timeout/retransmit maxima are reached.
  int retval = radio.write(&data, sizeof(data));

  lcd.clear();
  lcd.setCursor(0, 0); // column, line (from 0)
  lcd.print("transmitting");
  lcd.setCursor(14, 0); // column, line (from 0)
  lcd.print(data.stateNumber);

  Serial.print(F(" ... "));
  if (retval) {
    Serial.println(F("success"));
    lcd.setCursor(0, 1); // column, line (from 0)
    lcd.print("success");
  } else {
    totalTransmitFailures++;
    Serial.print(F("failure, total failures = "));
    Serial.println(totalTransmitFailures);

    lcd.setCursor(0, 1); // column, line (from 0)
    lcd.print("error, total=");
    lcd.setCursor(13, 1); // column, line (from 0)
    lcd.print(totalTransmitFailures);
  }
}

class Button
{
    int pinNumber;
    bool previousState;
    void(* buttonFunction)();
  public:

    // Constructor
    Button(int pn, void* bf) {
      pinNumber = pn;
      buttonFunction = bf;
      previousState = 1;
    }

    // update the button
    void update() {
      bool currentState = digitalRead(pinNumber);
      if (currentState == LOW && previousState == HIGH) {
        Serial.print("button on pin ");
        Serial.print(pinNumber);
        Serial.println();
        buttonFunction();
      }
      previousState = currentState;
    }
};

const int NUMBUTTONS = 5;
Button theButtons[] = {Button(SW1_PIN, countDown),
                       Button(SW2_PIN, rf24SendData),
                       Button(SW3_PIN, countUp),
                       Button(SW4_PIN, spare1),
                       Button(SW5_PIN, spare2),
                      };

void setupRF24() {

  setupRF24Common();

  // Set us as a transmitter
  radio.openWritingPipe(xmtrAddress);
  radio.openReadingPipe(1, rcvrAddress);

  // radio.printPrettyDetails();
  Serial.println(F("I am a transmitter"));

  data.stateNumber = 0;
}

void setup() {
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Radio init ....");

  setupRF24();

  // Initialize the switches
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SW3_PIN, INPUT_PULLUP);
  pinMode(SW4_PIN, INPUT_PULLUP);
  pinMode(SW5_PIN, INPUT_PULLUP);
  lcd.setCursor(0, 1); // column, line (from 0)
  lcd.print("setup() finished");

  updateLCD();
}



void loop() {
  for (int i = 0; i < NUMBUTTONS; i++) {
    theButtons[i].update();
  }
  delay(50); // for testing
}


void clearData() {
  // set all fields to 0
  data.stateNumber = 0;
}

// End of transmitter code

*/


// Receiver Code

// Additional libraries for music maker shield
#include <Adafruit_VS1053.h>
#include <SD.h>

// Servo library
#include <Servo.h>

// Additional libraries for graphics on the Neo Pixel Matrix
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#ifndef PSTR
#define PSTR  // Make Arduino Due happy
#endif

// Additional pin usage for receiver

// Adafruit music maker shield
#define SHIELD_RESET -1  // VS1053 reset pin (unused!)
#define SHIELD_CS 7      // VS1053 chip select pin (output)
#define SHIELD_DCS 6     // VS1053 Data/command select pin (output)
#define CARDCS 4         // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3  // VS1053 Data request, ideally an Interrupt pin
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

// Servo motors
const int SERVO_PIN = 16;  // TODO why doesn't pin 21 work?
//const int ANTENNA_SERVO_PIN = 10;
//const int TAIL_SERVO_PIN = 11;
//const int GRABBER_SERVO_PIN = 12;

// Neopixel
const int NEOPIXELPIN = 17;
const int NUMPIXELS = 128;
//#define NEOPIXELPIN 18
//#define NUMPIXELS 64  // change to fit
//Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 16, NEOPIXELPIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
                                               NEO_GRB + NEO_KHZ800);

Servo neck;  // change names to describe what's moving


// change as per your robot
const int NECK_LEFT = 60;
const int NECK_RIGHT = 180;
const int NECK_CENTER = 130;

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)
};

void setup() {
  Serial.begin(9600);
  // printf_begin();

  // Set up all the attached hardware
  setupMusicMakerShield();
  setupServoMotors();
  setupNeoPixels();

  setupRF24();

  // Brief flash to show we're done with setup()
  flashNeoPixels();
}

void setupRF24() {
  setupRF24Common();

  // Set us as a receiver
  radio.openWritingPipe(rcvrAddress);
  radio.openReadingPipe(1, xmtrAddress);

  // radio.printPrettyDetails();
  Serial.println(F("I am a receiver"));
}

void setupMusicMakerShield() {
  if (!musicPlayer.begin()) {  // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1)
      ;
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD card failed or not present"));
    while (1)
      ;  // don't do anything more
  }

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20, 20);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
}

void setupServoMotors() {
  neck.attach(SERVO_PIN);
  neck.write(90);
}

void setupNeoPixels() {
  //  pixels.begin();
  //  pixels.clear();
  //  pixels.show();
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(matrix.Color(200, 0, 0));
}

void flashNeoPixels() {

  // Using the Matrix library
  matrix.fillScreen(matrix.Color(0, 255, 0));
  matrix.show();
  delay(500);
  matrix.fillScreen(0);
  matrix.show();

  //  // all on
  //  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
  //    pixels.setPixelColor(i, pixels.Color(0, 100, 0));
  //  }
  //  pixels.show();
  //  delay(500);
  //
  //  // all off
  //  pixels.clear();
  //  pixels.show();
}


int heart[] = { 9, 10, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 41, 42, 43, 44, 45, 46, 50, 51, 52, 53, 59, 60, 73, 74, 77, 78, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 105, 106, 107, 108, 109, 110, 114, 115, 116, 117, 123, 124 };
//int heartnegative[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 15, 40, 47, 48, 49, 54, 55, 56, 57, 58, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 75, 76, 79, 104, 111, 112, 113, 118, 119, 120, 121, 122, 125, 126, 127, 128};
int standard[] = { 10, 11, 12, 13, 17, 22, 28, 35, 37, 43, 44, 45, 52, 74, 75, 76, 77, 81, 86, 91, 98, 100, 106, 107, 108, 115 };
int electric[] = { 4, 5, 11, 12, 18, 19, 25, 26, 27, 28, 29, 30, 33, 34, 35, 36, 37, 38, 44, 45, 51, 52, 58, 59, 68, 69, 75, 76, 82, 83, 89, 90, 91, 92, 93, 94, 97, 98, 99, 100, 101, 102, 108, 109, 115, 116, 122, 123 };
int lego[] = { 9, 13, 14, 15, 17, 21, 25, 29, 33, 37, 38, 39, 41, 45, 49, 50, 51, 53, 54, 55, 72, 73, 74, 76, 77, 78, 80, 84, 86, 88, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 113, 114, 116, 117, 118 };
int L[] = { 9, 17, 25, 33, 41, 49, 50, 51 };
int E[] = { 13, 14, 15, 21, 29, 37, 38, 39, 45, 53, 54, 55 };
int G[] = { 72, 73, 74, 80, 88, 96, 98, 104, 106, 112, 113, 114 };
int O[] = { 76, 77, 78, 84, 86, 92, 94, 100, 102, 108, 110, 116, 117, 118 };

int sad[] = { 2, 3, 10, 11, 12, 20, 21, 29, 30, 37, 38, 44, 45, 50, 51, 52, 58, 59, 68, 69, 75, 76, 77, 82, 83, 89, 90, 97, 98, 106, 107, 115, 116, 117, 124, 125 };
int sad2[] = { 3, 4, 11, 12, 13, 21, 22, 30, 31, 38, 39, 45, 46, 51, 52, 53, 59, 60, 67, 68, 74, 75, 76, 81, 82, 88, 89, 96, 97, 105, 106, 114, 115, 116, 123, 124 };

int standardclosed[] = { 10, 11, 12, 13, 17, 22, 35, 36, 37, 43, 44, 45, 74, 75, 76, 77, 81, 86, 98, 99, 100, 106, 107, 108 };
int heartmiddle[] = { 18, 21, 25, 26, 27, 28, 29, 30, 33, 34, 35, 36, 37, 38, 42, 43, 44, 45, 51, 52, 82, 85, 89, 90, 91, 92, 93, 94, 97, 98, 99, 100, 101, 102, 106, 107, 108, 109, 115, 116 };
int heartsmall[] = { 26, 29, 34, 35, 36, 37, 43, 44, 90, 93, 98, 99, 100, 101, 107, 108 };

int windowr[] = { 0, 1, 2, 3, 8, 9, 10, 11, 16, 17, 18, 19, 24, 25, 26, 27, 64, 65, 66, 67, 72, 73, 74, 75, 80, 81, 82, 83, 88, 89, 90, 91 };
int windowg[] = { 4, 5, 6, 7, 12, 13, 14, 15, 20, 21, 22, 23, 28, 29, 30, 31, 68, 69, 70, 71, 76, 77, 78, 79, 84, 85, 86, 87, 92, 93, 94, 95 };
int windowb[] = { 32, 33, 34, 35, 40, 41, 42, 43, 48, 49, 50, 51, 56, 57, 58, 59, 96, 97, 98, 99, 104, 105, 106, 107, 112, 113, 114, 115, 120, 121, 122, 123 };
int windowy[] = { 36, 37, 38, 39, 44, 45, 46, 47, 52, 53, 54, 55, 60, 61, 62, 63, 100, 101, 102, 103, 108, 109, 110, 111, 116, 117, 118, 119, 124, 125, 126, 127 };

void loop() {
  // If there is data, read it,
  // and do the needfull
  // Become a receiver
  radio.startListening();
  if (radio.available(&pipeNum)) {
    radio.read(&data, sizeof(data));
    Serial.print(F("message received Data = "));
    Serial.print(data.stateNumber);
    Serial.println();

    switch (data.stateNumber) {
      case 0:  // startup case
        neck.write(NECK_CENTER);

        matrix.clear();


        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
          }
          matrix.show();
          delay(2500);
          matrix.clear();
          for (int i = 0; i < sizeof(standardclosed) / sizeof(standardclosed[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standardclosed[i], 0, 0, 255);                           // draw lego eyes symbol
          }



          matrix.show();
          delay(500);
        }
        matrix.clear();
        for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
          matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
        }
        matrix.show();

        break;

      case 1:  //makes the neck centered
        neck.write(NECK_CENTER);

        break;

      case 2:  //makes the neck left
        neck.write(NECK_LEFT);

        break;
      case 3:  //turns the neck towards right
        neck.write(NECK_RIGHT);

        break;

      case 4:  // left & center
        neck.write(NECK_LEFT);
        delay(1000);
        neck.write(NECK_CENTER);
        break;

      case 5:  //right & center
        neck.write(NECK_RIGHT);
        delay(1000);
        neck.write(NECK_CENTER);
        break;

      case 6:  //crazy head movement
        neck.write(NECK_LEFT);
        delay(800);
        neck.write(NECK_RIGHT);
        delay(800);
        neck.write(NECK_LEFT);
        delay(800);
        neck.write(NECK_CENTER);

        break;

      case 7:  //track 001



        neck.write(NECK_LEFT);
        delay(800);
        neck.write(NECK_RIGHT);
        delay(800);
        neck.write(NECK_LEFT);
        delay(800);
        neck.write(NECK_CENTER);


        Serial.println(F("Playing track 001"));
        musicPlayer.startPlayingFile("/track001.mp3");
        matrix.clear();

        for (int i = 0; i < sizeof(electric) / sizeof(electric[0]); i++) {  // For each pixel...
          matrix.setPixelColor(electric[i], 255, 255, 0);
          matrix.show();
          delay(30);
          // draw electric eyes symbol
        }


        break;

      case 8:  //track 002

        matrix.clear();

        Serial.println(F("Playing track 002"));
        musicPlayer.startPlayingFile("/track002.mp3");
        matrix.clear();

        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(sad) / sizeof(sad[0]); i++) {  // For each pixel...
            matrix.setPixelColor(sad[i], 128, 0, 0);                // draw sad eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
          for (int i = 0; i < sizeof(sad2) / sizeof(sad2[0]); i++) {  // For each pixel...
            matrix.setPixelColor(sad2[i], 128, 0, 0);                 // draw sad eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
        }



        break;


      case 9:  // track 003

        Serial.println(F("Playing track 003"));
        musicPlayer.startPlayingFile("/track003.mp3");
        matrix.clear();


        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
          }
          matrix.show();
          delay(2500);
          matrix.clear();
          for (int i = 0; i < sizeof(standardclosed) / sizeof(standardclosed[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standardclosed[i], 0, 0, 255);                           // draw lego eyes symbol
          }



          matrix.show();
          delay(500);
        }
        matrix.clear();
        for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
          matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
        }
        matrix.show();

        break;


      case 10:  // track 004

        matrix.clear();


        Serial.println(F("Playing track 004"));
        musicPlayer.startPlayingFile("/track004.mp3");


        for (int i = 0; i < sizeof(windowr) / sizeof(windowr[0]); i++) {  // For each pixel...
          matrix.setPixelColor(windowr[i], 255, 0, 0);
          matrix.setPixelColor(windowg[i], 0, 255, 0);
          matrix.setPixelColor(windowb[i], 0, 0, 255);
          matrix.setPixelColor(windowy[i], 255, 255, 0);
          // draw electric eyes symbol
        }
        matrix.show();


        break;


      case 11:  //track 005

        Serial.println(F("Playing track 005"));
        musicPlayer.startPlayingFile("/track005.mp3");

        matrix.clear();

        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(heart) / sizeof(heart[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heart[i], 255, 20, 147);               // draw electric eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
          for (int i = 0; i < sizeof(heartmiddle) / sizeof(heartmiddle[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heartmiddle[i], 255, 20, 147);                     // draw electric eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();

          for (int i = 0; i < sizeof(heartsmall) / sizeof(heartsmall[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heartsmall[i], 255, 20, 147);                    // draw electric eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
          for (int i = 0; i < sizeof(heartmiddle) / sizeof(heartmiddle[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heartmiddle[i], 255, 20, 147);                     // draw lego eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
        }
        for (int i = 0; i < sizeof(heart) / sizeof(heart[0]); i++) {  // For each pixel...
          matrix.setPixelColor(heart[i], 255, 20, 147);               // draw electric eyes symbol
        }
        matrix.show();


        break;


      case 12:  // standard eyes for listening to other monologues

        matrix.clear();

        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
          }
          matrix.show();
          delay(2500);
          matrix.clear();
          for (int i = 0; i < sizeof(standardclosed) / sizeof(standardclosed[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standardclosed[i], 0, 0, 255);                           // draw lego eyes symbol
          }



          matrix.show();
          delay(500);
        }
        matrix.clear();
        for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
          matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
        }
        matrix.show();

        break;


      case 13:  // track 006 - our monologue



        Serial.println(F("Playing track 006"));
        musicPlayer.startPlayingFile("/track006.mp3");
        matrix.clear();

        matrix.clear();
        for (int j = 0; j < 3; j++) {
          matrix.clear();
          for (int i = 0; i < sizeof(L) / sizeof(L[0]); i++) {  // For each pixel...
            matrix.setPixelColor(L[i], 255, 0, 255);            // draw lego eyes symbol
          }
          matrix.show();
          delay(400);
          for (int i = 0; i < sizeof(E) / sizeof(E[0]); i++) {  // For each pixel...
            matrix.setPixelColor(E[i], 255, 0, 255);            // draw lego eyes symbol
          }

          matrix.show();
          delay(400);
          for (int i = 0; i < sizeof(G) / sizeof(G[0]); i++) {  // For each pixel...
            matrix.setPixelColor(G[i], 255, 0, 255);            // draw lego eyes symbol
          }
          matrix.show();
          delay(400);
          for (int i = 0; i < sizeof(O) / sizeof(O[0]); i++) {  // For each pixel...
            matrix.setPixelColor(O[i], 255, 0, 255);            // draw lego eyes symbol
          }
          matrix.show();
          delay(400);
        }

        break;


      case 14:  // track 007


        Serial.println(F("Playing track 007"));
        musicPlayer.startPlayingFile("/track007.mp3");
        matrix.clear();

        matrix.clear();
        for (int j = 0; j < 3; j++) {

          for (int i = 0; i < sizeof(electric) / sizeof(electric[0]); i++) {  // For each pixel...
            matrix.setPixelColor(electric[i], 255, 255, 0);
            matrix.show();
            delay(30);
            // draw electric eyes symbol
          }
          matrix.clear();
        }

        break;



      case 15:  // sad eyes

        matrix.clear();

        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(sad) / sizeof(sad[0]); i++) {  // For each pixel...
            matrix.setPixelColor(sad[i], 128, 0, 0);                // draw sad eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
          for (int i = 0; i < sizeof(sad2) / sizeof(sad2[0]); i++) {  // For each pixel...
            matrix.setPixelColor(sad2[i], 128, 0, 0);                 // draw sad eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
        }

        break;


      case 16:  // heart eyes

        matrix.clear();

        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(heart) / sizeof(heart[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heart[i], 255, 20, 147);               // draw electric eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
          for (int i = 0; i < sizeof(heartmiddle) / sizeof(heartmiddle[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heartmiddle[i], 255, 20, 147);                     // draw electric eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();

          for (int i = 0; i < sizeof(heartsmall) / sizeof(heartsmall[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heartsmall[i], 255, 20, 147);                    // draw electric eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
          for (int i = 0; i < sizeof(heartmiddle) / sizeof(heartmiddle[0]); i++) {  // For each pixel...
            matrix.setPixelColor(heartmiddle[i], 255, 20, 147);                     // draw lego eyes symbol
          }
          matrix.show();
          delay(500);
          matrix.clear();
        }
        for (int i = 0; i < sizeof(heart) / sizeof(heart[0]); i++) {  // For each pixel...
          matrix.setPixelColor(heart[i], 255, 20, 147);               // draw electric eyes symbol
        }
        matrix.show();


        break;

      case 17:  // electric eyes

        matrix.clear();

        for (int i = 0; i < sizeof(electric) / sizeof(electric[0]); i++) {  // For each pixel...
          matrix.setPixelColor(electric[i], 255, 255, 0);
          matrix.show();
          delay(30);
          // draw electric eyes symbol
        }
        //matrix.show();

        break;

      case 18:  // lego eyes

        matrix.clear();
        for (int j = 0; j < 1; j++) {
          matrix.clear();
          for (int i = 0; i < sizeof(L) / sizeof(L[0]); i++) {  // For each pixel...
            matrix.setPixelColor(L[i], 255, 0, 255);            // draw lego eyes symbol
          }
          matrix.show();
          delay(400);
          for (int i = 0; i < sizeof(E) / sizeof(E[0]); i++) {  // For each pixel...
            matrix.setPixelColor(E[i], 255, 0, 255);            // draw lego eyes symbol
          }

          matrix.show();
          delay(400);
          for (int i = 0; i < sizeof(G) / sizeof(G[0]); i++) {  // For each pixel...
            matrix.setPixelColor(G[i], 255, 0, 255);            // draw lego eyes symbol
          }
          matrix.show();
          delay(400);
          for (int i = 0; i < sizeof(O) / sizeof(O[0]); i++) {  // For each pixel...
            matrix.setPixelColor(O[i], 255, 0, 255);            // draw lego eyes symbol
          }
          matrix.show();
          delay(400);
        }

        break;


      case 19:  // standard eyes

        matrix.clear();


        for (int j = 0; j < 3; j++) {
          matrix.clear();

          for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
          }
          matrix.show();
          delay(2500);
          matrix.clear();
          for (int i = 0; i < sizeof(standardclosed) / sizeof(standardclosed[0]); i++) {  // For each pixel...
            matrix.setPixelColor(standardclosed[i], 0, 0, 255);                           // draw lego eyes symbol
          }



          matrix.show();
          delay(500);
        }
        matrix.clear();
        for (int i = 0; i < sizeof(standard) / sizeof(standard[0]); i++) {  // For each pixel...
          matrix.setPixelColor(standard[i], 0, 0, 255);                     // draw electric eyes symbol
        }
        matrix.show();

        break;

      case 20:

        Serial.println(F("Playing track 008"));
        musicPlayer.startPlayingFile("/track008.mp3");
        matrix.clear();


        break;

      case 21:

        musicPlayer.stopPlaying();


      default:
        Serial.println(F("Invalid option"));
    }
  }
}  // end of loop()
