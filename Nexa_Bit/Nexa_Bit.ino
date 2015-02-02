/*  
 * Nexa Automatic Protocol Transmitter 
 *
 * This code listens to the SIGnal channel of an LittleBit. On HIGH it
 * emits a Nexa automatic protocol ON message. When it falls to LOW 
 * it emits an off message
 *
 * The board also contains a slide selector so the user can switch to TOGGLE mode
 * In TOGGLE mode the circuit behaves lika a Toggle Latch. It stays in 
 * the same ON/OFF state even when the SIGnal falls to LOW. 
 * When SIGnal rises to HIGH the next time it toggles the emition signal
 * from ON to OFF or OFF to ON
 --------------------------------
 relationship beteween toggle, socketON and toggle:
 --------------------------------
 toggle      00000000  1111111111
 
 SIG         00110010  0110011010
 socketON    00110010  0111100011
 --------------------------------
 *
 *
 * Thanks to Martyn Henderson for laying out the ground work 
 * http://martgadget.blogspot.com
 *
 */
#include <Arduino.h>
#include <WProgram.h>


bool bit2[26]={};              // 26 bit global to store the Nexa/HE device address.
//PINS
int txPin = 1;                 // 433mhz transmitter on pin 1
int ledPin = 0;

//INPUT PINS
int pushPin = 4;             //simple push pin triggers random transmitter address (not implemented)
int signalPin = 2;          //analog input signal from prev LittleBit. Treated digitally (HIGH/LOW)
int sliderSelectorPin = 3;  //Switch with two states: TOGGLE or ON/OFF

boolean socketOn = false;          //the remote socket is considered off to start with
boolean toggle = false;      //wheteher the circuit should toggle between on/off like a latch
boolean previousSignal = LOW;    //signal from previous bit is threshold at 50%

unsigned long senderCode = 16479282;

void setup()
{

  pinMode(txPin, OUTPUT);      // transmitter pin.
  pinMode(ledPin, OUTPUT);   
  pinMode(pushPin,INPUT);
  pinMode(sliderSelectorPin,INPUT);
  pinMode(signalPin,INPUT);
  

  
  integerToBitArray(senderCode,26);            // convert our device code to binary settinh the bit2 array

 

}

void loop()
{
//detect slider position
  
  int signal = digitalRead(signalPin); //HIGH OR LOW
  int toggle = digitalRead(sliderSelectorPin);
  
  
  
  //Detect signal change
  if (signal != previousSignal){
   if (signal == HIGH){ //we have a RISE
    if (toggle){
      socketOn = !socketOn;
    }else{
      socketOn = true;
    }
      transmit(socketOn);            // send ON
      delay(10);                 // wait (socket ignores us it appears unless we do this)
      transmit(socketOn);            // send ON again
      
   }else{ //LOW - we have a FALL
     if (!toggle){
      socketOn = false;
      transmit(socketOn);            // send ON
      delay(10);                 // wait (socket ignores us it appears unless we do this)
      transmit(socketOn);            // send ON again
      
    }
   } 
  }
  previousSignal = signal;
   
  
}


void transmit(int blnOn)
{
  digitalWrite(ledPin, blnOn);   // turn the LED on (HIGH is the voltage level)

  int i;
  // Do the latch sequence.. 
  digitalWrite(txPin, HIGH);
  delayMicroseconds(275);     // bit of radio shouting before we start. 
  digitalWrite(txPin, LOW);
  delayMicroseconds(9900);     // low for 9900 for latch 1
  digitalWrite(txPin, HIGH);   // high again 
  delayMicroseconds(275);      // wait a moment 275
  digitalWrite(txPin, LOW);    // low again for 2675 - latch 2.
  delayMicroseconds(2675);
  // End on a high 
  digitalWrite(txPin, HIGH);

// Send HE Device Address..
  // e.g. 1000010101000110010  272946 in binary.
  for(i=0; i<26;i++)
  {
    sendPair(bit2[i]);
  }
  
  // Send 26th bit - group 1/0
  sendPair(false);

  // Send 27th bit - on/off 1/0
  sendPair(blnOn);

  // last 4 bits - recipient   -- button 1 on the HE300 set to 
  // slider position I in this example:

  sendPair(false);
  sendPair(false);
  sendPair(false);
  sendPair(false);

  digitalWrite(txPin, HIGH);   // high again (shut up)
  delayMicroseconds(275);      // wait a moment
  digitalWrite(txPin, LOW);    // low again for 2675 - latch 2.

}


void sendBit(boolean b) {
  if (b) {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(310);   //275 orinally, but tweaked.
    digitalWrite(txPin, LOW);
    delayMicroseconds(1340);  //1225 orinally, but tweaked.
  }
  else {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(310);   //275 orinally, but tweaked.
    digitalWrite(txPin, LOW);
    delayMicroseconds(310);   //275 orinally, but tweaked.
  }
}

void sendPair(boolean b) {
  // Send the Manchester Encoded data 01 or 10, never 11 or 00
  if(b)
  {
    sendBit(true);
    sendBit(false);
  }
  else
  {
  sendBit(false);
  sendBit(true);
  }
}

//Hack integer into bits and put into the global bit2[] array
void integerToBitArray(unsigned long integer, int length)
{  //needs bit2[length]
  // Convert long device code into binary (stores in global bit2 array.)
 for (int i=0; i<length; i++){
   if ((integer / power2(length-1-i))==1){
     integer-=power2(length-1-i);
     bit2[i]=1;
   }
   else bit2[i]=0;
 }
}

unsigned long power2(int power){    //gives 2 to the (power)
 unsigned long integer=1;          
 for (int i=0; i<power; i++){      
   integer*=2;
 }
 return integer;
}
