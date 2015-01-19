/*
 * HE300 Automatic Protocol Transmitter 'Simulator'
 *
 * This demo code emits a homeeasy automatic protocol ON message 
 * then 3 seconds later, an off message
 *
 * You dont need to learn the Arduino into the socket, because it can 
 * pretend to be a transmitter you already have.
 *
 * Use the Automatic protocol reciever code above to find out
 * your transmitter address 8-)
 *
 * *Only* tested with one HE300 transmitter (HE Address 272946) with its
 * slider switch set to 1 which appears to be 0 in fact.
 * Seems to work with a switch socket (HE302S) and can turn it on and off 
 * without the ardunio being learned into the socket.
 *
 * Martyn Henderson 02/10/2009  http://martgadget.blogspot.com
 *
 * Cheers to Barnaby and Peter, because without the code above
 * that shows the receiver output this wouldnt have been possible!
 *
 * If you make something cool using HomeEasy and Arduino, please 
 * post back here with a link to your code so we can all enjoy.
 */



bool bit2[26]={};              // 26 bit global to store the HE device address.
//PINS
int txPin = 4;                 // 433mhz transmitter on pin2
int ledPin = 13;
int pushPin = 7;             //simple push pin connected to pin7

boolean on = false;          //the remote socket is considered off to start with
unsigned long senderCode = 16479282;

void setup()
{

  pinMode(txPin, OUTPUT);      // transmitter pin.
  pinMode(ledPin, OUTPUT);   
  pinMode(pushPin,INPUT);
  
  Serial.begin(9600);         // console port

  Serial.println("go");
  
  itob(senderCode,26);            // convert our device code to binary settinh the bit2 array

  /*transmit(true);            // send ON
  delay(10);                 // wait (socket ignores us it appears unless we do this)
  transmit(true);            // send ON again

  delay(3000);               // wait 3 sec

  transmit(false);           // and off again.
  delay(10);
  transmit(false);

  digitalWrite(ledPin, LOW);*/
  Serial.println("stop");    // done.

}

void loop()
{
//detect if we click the Puch Button
  unsigned int t = 0;
   t = pulseIn(pushPin, HIGH);
   Serial.println(t);
   if (t > 1){
     on = !on;
     Serial.println(on);
     digitalWrite(ledPin, on);
     transmit(on);            // send ON
     delay(10);                 // wait (socket ignores us it appears unless we do this)
     transmit(on);            // send ON again
   }
}


void transmit(int blnOn)
{
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
  //Serial.print(b);
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


void itob(unsigned long integer, int length)
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
