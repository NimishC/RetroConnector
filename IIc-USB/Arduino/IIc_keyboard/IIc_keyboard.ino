/* requires keypad library by Mark Stanley, Alexander Brevig from:
http://www.arduino.cc/playground/Code/Keypad

Uses the PJRC Teensy++ 2.0 and Teensyduino libraries, specifically USB HID and Keyboard():
http://www.pjrc.com/teensy/teensyduino.html

Modified 20131228 NimishC: Adapted key map to Teensy 3.0
*/

#include <Keypad.h>



/* 
Declares the matrix rows/cols of the Apple IIe keyboard. 

More information here:
http://apple2.info/wiki/index.php?title=Pinouts#Apple_.2F.2Fe_Motherboard_keyboard_connector


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



Matrix
	J9  Teensy

X0	31	8
1	24      15
2	22      16
3	8       7
4	14      20
5	6       5
6	10      22
7	1       0
8	4       3
9	2       1

Y0	12      21
1	33      10
2	18      18
3	20      17
4	3       2
5	5       4
6	7       6
7	9       23

NC	13
	17
	30

GND	21      

SFT	34      12
CTL	32      9
CMD     16      19    
CMD	26      14
CAPS	28      8


J9 pinout
1	X7
2	X9
3	Y4
4	X8
5	Y5
6	X5
7	Y6
8	X3
9	Y7
10	X6
11		DISK LED (5v)
12	Y0
13		NC
14	X4
15		KEYBOARD SW
16	CLOSED APPLE
17		NC
18	Y2
19		40/80 COL SWITCH
20	Y3
21		GROUND ???
22	X2
23		GROUND ???
24	X1
25	POWER LED (5v)
26	OPEN APPLE
27		LED GROUND ???
28	CAPS LOCK
29		RESET
30		NC
31	X0
32	CONTROL
33	Y1
34	SHIFT




*/

byte rowPins[ROWS] = { // Y0 - Y9
  21,10,18,17,2,4,6,23}; //connect to the row pinouts of the keypad

byte colPins[COLS] = { // X0 - X7
  8,15,16,7,20,5,22,0,3,1 }; //connect to the column pinouts of the keypad

Keypad KPD = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// the following pins are special in that they are dis/connected to ground, instead of to a row/col
/*** 
open/closed apple are grounded through resistors on the //c. need to detect voltage drop as analogread, not digital.
must be on analog pin.
***/
const int  SHIFTPin = 12;    // the pin that the shift key is attached to
const int  CTRLPin = 9;    // the pin that the control key is attached to
const int  APPLEPin1 = 19;    // the pin that the open-apple key is attached to
const int  APPLEPin2 = 14;    // the pin that the closed-apple key is attached to
const int CAPSPin = 8; // the caps lock pin

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

  digitalWrite(APPLEPin1, HIGH);
  digitalWrite(APPLEPin2, HIGH);
  
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



   char OAPPLEState = analogRead(APPLEPin2);
   char CAPPLEState = analogRead(APPLEPin1);


// *** NOW USING CLOSED APPLE AS ALT/OPTION
    if (OAPPLEState < 20) {
      modifierKeys[2] =  MODIFIERKEY_GUI;
      digitalWrite(APPLEPin1, HIGH);
    } else {
      modifierKeys[2] = 0;
      digitalWrite(APPLEPin1, HIGH);
    }



    if (CAPPLEState < 20) {
      modifierKeys[3] =  MODIFIERKEY_ALT;
      digitalWrite(APPLEPin2, HIGH);
    } else {
      modifierKeys[3] = 0;
      digitalWrite(APPLEPin2, HIGH);
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

                 //   Serial.println( KPD.key[0].kchar, HEX );

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




