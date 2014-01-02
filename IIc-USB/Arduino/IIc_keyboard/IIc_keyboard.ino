/* requires keypad library by Mark Stanley, Alexander Brevig from:
http://www.arduino.cc/playground/Code/Keypad

Uses the PJRC Teensy++ 2.0 and Teensyduino libraries, specifically USB HID and Keyboard():
http://www.pjrc.com/teensy/teensyduino.html

Modified 20131228 NimishC: Adapted key map to Teensy 3.0
Modified 20140101 NimishC: Changed check for Open+Closed Apple to digitalRead, for ALPS keyboard
*/

#include <Keypad.h>



/* 
Declares the matrix rows/cols of the Apple IIc keyboard. 
*/

const byte ROWS = 8; // rows
const byte COLS = 10; // columns
char keys[ROWS][COLS] = {


{	KEY_ESC,	KEY_1,	KEY_2,	KEY_3,	KEY_4,	KEY_6,	KEY_5,	KEY_7,	KEY_8,	KEY_9 },

{	KEY_TAB,	KEY_Q,	KEY_W,	KEY_E,	KEY_R,	KEY_Y,	KEY_T,	KEY_U,	KEY_I,	KEY_O},

{	KEY_A,	KEY_D,	KEY_S,	KEY_H,	KEY_F,	KEY_G,	KEY_J,	KEY_K,	KEY_SEMICOLON,	KEY_L},

{	KEY_Z,	KEY_X,	KEY_C,	KEY_V,	KEY_B,	KEY_N,	KEY_M,	KEY_COMMA,	KEY_PERIOD,	KEY_SLASH},

{	0,	0,	0,	0,	0,	0,	KEY_BACKSLASH,	KEY_EQUAL,	KEY_0,	KEY_MINUS},

{	0,	0,	0,	0,	0,	0,	KEY_TILDE,	KEY_P,	KEY_LEFT_BRACE,	KEY_RIGHT_BRACE},

{	0, 	0,       0 ,  	0,	0,	0,	KEY_ENTER,	KEY_UP,	KEY_SPACE,	KEY_QUOTE},

{	0,	0,	0,	0,	0,	0,	KEY_BACKSPACE,	KEY_DOWN,	KEY_LEFT,	KEY_RIGHT},



};



/*
In reserve, for Dvorak key modificationing

{	KEY_ESC,	KEY_1,     KEY_2,	KEY_3,	  KEY_4,    KEY_6,	KEY_5,	KEY_7,	KEY_8,	KEY_9 },

{	KEY_TAB,	KEY_Q,	KEY_W,	KEY_E,	KEY_R,	KEY_Y,	KEY_T,	KEY_U,	KEY_I,	KEY_O},

{	KEY_A,	KEY_E,	KEY_O,	KEY_D,	KEY_U,	KEY_I,	KEY_H,	KEY_T,	KEY_MINUS,	KEY_N},

{	KEY_Z,	KEY_X,	KEY_C,	KEY_V,	KEY_B,	KEY_N,	KEY_M,	KEY_COMMA,	KEY_PERIOD,	KEY_SLASH},

{	0,	0,	0,	0,	0,	0,	KEY_BACKSLASH,	KEY_EQUAL,	KEY_0,	KEY_MINUS},

{	0,	0,	0,	0,	0,	0,	KEY_TILDE,	KEY_P,	KEY_LEFT_BRACE,	KEY_RIGHT_BRACE},

{	0, 	0,       0 ,  	0,	0,	0,	KEY_ENTER,	KEY_UP,	KEY_SPACE,	KEY_QUOTE},

{	0,	0,	0,	0,	0,	0,	KEY_BACKSPACE,	KEY_DOWN,	KEY_LEFT,	KEY_RIGHT},



};

*/




/*
Keyboard matrix

	X0	1	2	3	4	5	6	7	8	9
Y0	ESC	1	2	3	4	6	5	7	8	9

1	TAB	Q	W	E	R	Y	T	U	I	O

2	A	D	S	H	F	G	J	K	;	L

3	Z	X	C	V	B	N	M	<	>	?

4							\	+	0	-

5							~	P	[	]

6							RET	UP	SPC	"

7							DEL	DN	LT	RT

J9 pin layout - keyboard connector facing you

                  ______________
              Y1 | 18        17 | Shift
              X0 | 19        16 | CTRL
           RESET | 20        15 | NC?
         LEDGND? | 21        14 | Caps Lock
             +5V | 22        13 | Closed Apple
             NC? | 23        12 | X1
             GND | 24        11 | X2
         40/80SW | 25        10 | Y3
             NC? | 26         9 | Y2
          KBDSW [  27         8 | Open Apple
             NC? | 28         7 | X4
         DISKLED | 29         6 | Y0
              Y7 | 30         5 | X6
              Y6 | 31         4 | X3
              Y5 | 32         3 | X5
              Y4 | 33         2 | X8
              X7 | 34         1 | X9
                 |______________|


J9 pin   Teensy pin
1        1
2        3
3        5
4        7
5        22
6        21
7        20
9        18
10       17
11       16
12       15
13       14
14       13
16       9
17       12
18       10
19       8
30       23
31       6
32       4
33       2
34       0

*/

byte rowPins[ROWS] = { // Y0 - Y9
  21,10,18,17,2,4,6,23}; //connect to the row pinouts of the keypad

byte colPins[COLS] = { // X0 - X7
  8,15,16,7,20,5,22,0,3,1 }; //connect to the column pinouts of the keypad

Keypad KPD = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// the following pins are special in that they are dis/connected to ground, instead of to a row/col
/*** 
open/closed apple are low by default, and are raised to +5 when pressed.
***/
const int  SHIFTPin = 12;    // the pin that the shift key is attached to
const int  CTRLPin = 9;    // the pin that the control key is attached to
const int  APPLEPin1 = 19;    // the pin that the open-apple key is attached to
const int  APPLEPin2 = 14;    // the pin that the closed-apple key is attached to
const int CAPSPin = 13; // the caps lock pin

char modifierKeys[4];

 #define KEY_CAPS_UNLOCK  0

   boolean resetCapsLock = false;  // Allows one caps unlock signal.
   unsigned long dTime = 0;
   char CAPSState;  // Initialize this to a reasonable value.

   boolean FKEYS = false; // used to set numbers to F-Key equivalent. currently tied to caps lock


void setup(){

  pinMode(SHIFTPin, INPUT);
  pinMode(CTRLPin, INPUT);
  pinMode(APPLEPin1, INPUT);
  pinMode(APPLEPin2, INPUT);

  digitalWrite(APPLEPin1, LOW);
  digitalWrite(APPLEPin2, LOW);
  
  digitalWrite(SHIFTPin, HIGH);
  digitalWrite(CTRLPin, HIGH);
  
  pinMode(CAPSPin, INPUT);
  digitalWrite(CAPSPin, HIGH);
  

}

void loop()
{

//probably should be on an interrupt, to catch high->low transition 

   // Only do something if the pin is different from previous state.
   if ( (CAPSState!=digitalRead(CAPSPin)) && !resetCapsLock) {
       CAPSState = digitalRead(CAPSPin);    // Remember new CAPSState.
       Keyboard.set_key6(KEY_CAPS_LOCK);    // Send KEY_CAPS_LOCK.
       dTime = millis();                    // Reset delay timer.
       resetCapsLock = true;
   }
   if ( resetCapsLock && (millis()-dTime) > 10)  {
       Keyboard.set_key6(KEY_CAPS_UNLOCK);
       resetCapsLock = false;
   }

FKEYS = CAPSState;


/*char CAPSState = digitalRead(CAPSPin);
    if (CAPSState == LOW) {
      Keyboard.set_key6(KEY_CAPS_LOCK);
    } else {
      Keyboard.set_key6(0);
    }

  */
  
   char SHIFTState = digitalRead(SHIFTPin);

    if (SHIFTState == LOW) {
      modifierKeys[0] = MODIFIERKEY_SHIFT;
      digitalWrite(SHIFTPin, HIGH);
    } else {
      digitalWrite(SHIFTPin, HIGH);
      modifierKeys[0] = 0;
    }

   char CTRLState = digitalRead(CTRLPin);

    if (CTRLState == LOW) {
      modifierKeys[1] = MODIFIERKEY_CTRL;
      digitalWrite(CTRLPin, HIGH);
    } else {
      modifierKeys[1] = 0;
      digitalWrite(CTRLPin, HIGH);
    }


// Modified from ye olde version - for ALPS keyboards, use digitalRead instead. 
   char OAPPLEState = digitalRead(APPLEPin2);
   char CAPPLEState = digitalRead(APPLEPin1);


// *** NOW USING CLOSED APPLE AS ALT/OPTION
    if (OAPPLEState == HIGH) {
      modifierKeys[2] =  MODIFIERKEY_GUI;
      digitalWrite(APPLEPin1, LOW);
    } else {
      modifierKeys[2] = 0;
      digitalWrite(APPLEPin1, LOW);
    }



    if (CAPPLEState == HIGH) {
      modifierKeys[3] =  MODIFIERKEY_ALT;
      digitalWrite(APPLEPin2, LOW);
    } else {
      modifierKeys[3] = 0;
      digitalWrite(APPLEPin2, LOW);
    }


// *** NOW USING CLOSED APPLE AS ALT/OPTION

    Keyboard.set_modifier( modifierKeys[0] | modifierKeys[1] | modifierKeys[2] | modifierKeys[3] );

    KPD.getKeys(); // Scan for all pressed keys. 6 Max, + 4 modifiers. Should be plenty, but can be extended to 10+

	// Set keyboard keys to default values.
	Keyboard.set_key1(0);
	Keyboard.set_key2(0);
	Keyboard.set_key3(0);
	Keyboard.set_key4(0);
	Keyboard.set_key5(0);
	//Keyboard.set_key6(0);
       
        /* based on suggestion from Craig Brooks <s.craig.brooks@gmail.com>
        uses CAPS LOCK to turn number keys into F-Key equivalent.
        */

	// Update keyboard keys to active values.
	if( KPD.key[0].kchar && ( KPD.key[0].kstate==PRESSED || KPD.key[0].kstate==HOLD )) {

            //Serial.println(FKEYS);
        
        
                if (FKEYS) {
                  if((KPD.key[0].kchar >= 0x1E) &&  (KPD.key[0].kchar <= 0x27)){
                    KPD.key[0].kchar += 0x1C;

                    //for debugging
                    //Serial.println( KPD.key[0].kchar, HEX );

                  }
                  

                  
                }
  
      		Keyboard.set_key1( KPD.key[0].kchar );

  
        }

	if( KPD.key[1].kchar && ( KPD.key[1].kstate==PRESSED || KPD.key[1].kstate==HOLD ))
		Keyboard.set_key2( KPD.key[1].kchar );

	if( KPD.key[2].kchar && ( KPD.key[2].kstate==PRESSED || KPD.key[2].kstate==HOLD ))
		Keyboard.set_key3( KPD.key[2].kchar );

	if( KPD.key[3].kchar && ( KPD.key[3].kstate==PRESSED || KPD.key[3].kstate==HOLD ))
		Keyboard.set_key4( KPD.key[3].kchar );

	if( KPD.key[4].kchar && ( KPD.key[4].kstate==PRESSED || KPD.key[4].kstate==HOLD ))
		Keyboard.set_key5( KPD.key[4].kchar );

	//if( KPD.key[5].kchar && ( KPD.key[5].kstate==PRESSED || KPD.key[5].kstate==HOLD ))
		//Keyboard.set_key6( KPD.key[5].kchar );

	Keyboard.send_now();
	Keyboard.set_modifier(0);

}




