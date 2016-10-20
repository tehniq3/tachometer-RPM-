// Frequency timer
// Author: Nick Gammon
// Date: 10th February 2012
// http://www.mikrocontroller.net/attachment/189505/Drehzahlmesser.ino

 // source for LEDControl: http://embedded-lab.com/blog/?p=6862
#include "LedControl.h" // https://github.com/wayoda/LedControl
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,11,10,1);
int vtot, vs, vsz, vz, vzu, vu, vv;

// Input: Pin D2

volatile boolean first;
volatile boolean triggered;
volatile unsigned long overflowCount;
volatile unsigned long startTime;
volatile unsigned long finishTime;

// here on rising edge
void isr () 
{
  unsigned int counter = TCNT1;  // quickly save it
  
  // wait until we noticed last one
  if (triggered)
    return;

  if (first)
    {
    startTime = (overflowCount << 16) + counter;
    first = false;
    return;  
    }
    
  finishTime = (overflowCount << 16) + counter;
  triggered = true;
  detachInterrupt(0);   
}  // end of isr

// timer overflows (every 65536 counts)
ISR (TIMER1_OVF_vect) 
{
  overflowCount++;
}  // end of TIMER1_OVF_vect


void prepareForInterrupts ()
  {
  // get ready for next time
  EIFR = bit (INTF0);  // clear flag for interrupt 0
  first = true;
  triggered = false;  // re-arm for next time
  attachInterrupt(0, isr, RISING);     
  }  // end of prepareForInterrupts
  

void setup () 
  {
  Serial.begin(115200);       
  Serial.println("Frequency Counter");
  
  // reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  // Timer 1 - interrupt on overflow
  TIMSK1 = bit (TOIE1);   // enable Timer1 Interrupt
  // zero it
  TCNT1 = 0;  
  overflowCount = 0;  
  // start Timer 1
  TCCR1B =  bit (CS20);  //  no prescaling

  // set up for interrupts
  prepareForInterrupts ();   
  
  // Initialize MAX7219 device
lc.shutdown(0,false); // Enable display 
lc.setIntensity(0,8); // Set brightness level (0 is min, 15 is max) 
lc.clearDisplay(0); // Clear display register 

lc.setChar(0,7,' ',false);// ma asigur ca nu e nimic pe coloana 7 (cea din stanga) 
lc.setRow(0,6,B1100010);  // afisez litera "n" pe coloane 6
lc.setRow(0,5,B0100000);  // afisez litera "i" pe coloane 5
lc.setRow(0,4,B1110011);  // afisez litera "q" pe coloane 4
lc.setRow(0,3,B0000001);  // afisez semnul "-" pe coloane 3
lc.setRow(0,2,B1000010);  // afisez litera "r" pe coloane 2
lc.setRow(0,1,B1100011);  // afisez litera "o" pe coloane 1
lc.setChar(0,0,' ',false);// ma asigur ca nu e nimic pe colanea 0 (din dreapta)
delay(2000);
lc.clearDisplay(0); // Clear display register    
  
  } // end of setup

void loop () 
  {

  if (!triggered) 
  return;
 
  unsigned long elapsedTime = finishTime - startTime;
//  float rpm = 60 * F_CPU / float (elapsedTime);  // each tick is 62.5 nS at 16 MHz
  float rpm = 30 * F_CPU / float (elapsedTime);  // each tick is 62.5 nS at 16 MHz
 
    
    Serial.print ("RPM: ");
    Serial.println (rpm);
  vtot = rpm;

// calculate number for each figure number
vs = vtot /1000;
vsz = vtot - 1000*vs;
vz = vsz /100;
vzu = vsz - 100*vz;
vu = vzu /10; 
vv = vzu - 10*vu; 
//vtot = 0;
lc.setChar(0,7,' ',false); 
//lc.setRow(0,7,B00111110);  // afisez litera "U" pe coloana 7
lc.setChar(0,6,' ',false); 
lc.setChar(0,5,' ',false); 
//lc.setRow(0,5,B00001001);  // afisez "=" pe coloana 5
lc.setChar(0,4,' ',false); 
if (vs == 0) lc.setChar(0,3,' ',false); else
lc.setDigit(0,3,vs, false); // afisez miile pe coloana 3
lc.setDigit(0,2,vz, false); // afisez sutele pe coloana 2
lc.setDigit(0,1,vu, false); // afisez zecile pe coloana 1
lc.setDigit(0,0,vv,false); 
 //  }
  
  //Serial.print ("Took: ");   // Ausgabe der gezÃƒÂ¤hlten counts
  //Serial.print (elapsedTime);
  //Serial.print (" counts. ");

  // so we can read it  
  delay (500);

  prepareForInterrupts ();   
}   // end of loop

