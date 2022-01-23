#include <Arduino.h>

/*
|   4-bit LCD pin:
|   (Just write, so pin RW grounded)
*/
const int _data_pins[4]   =   { 0,   4,   5,   16 };  //  { db4, db5, db6, db7 }
const int rs              =   12;                     //  Low: to send command | High: to send chars    
const int en              =   14;                     //  Pulse to send bits

/*
|   Functions
*/
void initializeLCD(void);
void pulseEn(void);
void send4Bits(uint8_t value);
void writeOnLCD(uint8_t value, uint8_t mode) ;

/*
============
|   Setup
============
*/

void setup() {
  Serial.begin(9600); // Initializing Serial.

  initializeLCD();    // Functions to initialize the LCD, pins and commands (based HD44780 datasheet).

  // Clear Display
  writeOnLCD(0x01, LOW); // HIGH to send chars.
                         // LOW to send command.

  writeOnLCD('R', HIGH); // H, e, l, l, o, !
  writeOnLCD('1', HIGH);
  writeOnLCD('D', HIGH);
  writeOnLCD('D', HIGH);
  writeOnLCD('A', HIGH);
  writeOnLCD('X', HIGH);
}

/*
============
|   Loop
============
*/

void loop() {

  
}

/*
============
| Functions 
============
*/

void initializeLCD() {
  // Initializing Pins Mode
  for (int pin = 0; pin < 4; pin++) {
    pinMode(_data_pins[pin], OUTPUT);
  }

  pinMode(rs, OUTPUT);
  pinMode(en, OUTPUT);
  digitalWrite(rs, LOW); // Command mode
  
  delayMicroseconds(100);   // Wait 40ms to energy the 16x2 (datasheet)
  
  // Trying 3x to set 4-bits mode:
  for (int i = 0; i < 4; i++) {
    send4Bits(0x03);          // Setting 4-bits mode, three times (datasheet).
    delayMicroseconds(4500);  // Waiting 4500 µs → 4.5 ms
  }

  delayMicroseconds(150);
  send4Bits(0x02);            // Set to 4-bit interface
  delayMicroseconds(150);
}

void pulseEn() {
  digitalWrite(en,    LOW);
  delayMicroseconds(5);
  digitalWrite(en,   HIGH);
  delayMicroseconds(5);
  digitalWrite(en,    LOW);
  delayMicroseconds(100);
}

void send4Bits(uint8_t value) {
  // Wrinting the instructions with 4 bits:
  for (int i = 0; i < 4; i++) {
    digitalWrite(_data_pins[i], (value>>i) & 0x01); // Get value and turn an binary value
  }
  
  pulseEn();
}

void writeOnLCD(uint8_t value, uint8_t mode) {
  delayMicroseconds(4500); // Delay to change rs mode without problems.

  digitalWrite(rs, mode); /* Set rs in write mode (Write char     HIGH
                                                   Send command   LOW)
                          */

  for (int h = 4; h < 8; h++) {
    digitalWrite(_data_pins[h-4], (((value)>>h) & 0x01));
  }
  /*
    Send the least significant bit of second part
    of 4 in one byte, for exemple: (H / 0x48) → (0100) 1000
    on pin, first send  [0] → 0
                        [1] → 0
                        [2] → 1
                        [3] → 0
  */
             //   DB | 4 | 5 | 6 | 7 | 
  pulseEn(); // Send | 0 | 1 | 0 | 0 |

  for (int h = 0; h < 4; h++) {
    digitalWrite(_data_pins[h], (((value)>>h) & 0x01));
  }
  /*
    Send the least significant bit of first part
    of 4 in one byte, for exemple: (H / 0x48) → 0100 (1000)
    on pin, first send  [0] → 1
                        [1] → 0
                        [2] → 0
                        [3] → 0
  */

             //   DB | 4 | 5 | 6 | 7 | 
  pulseEn(); // Send | 1 | 0 | 0 | 0 |
}