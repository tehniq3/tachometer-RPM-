// Frequency timer
// Author: Nick Gammon
// Date: 10th February 2012
// http://www.mikrocontroller.net/attachment/189505/Drehzahlmesser.ino
// adapt for LCD1602 by Nicu FLORICA (niq_ro) from http://www.tehnic.go.ro
// http://www.arduinotehniq.com

#include <LiquidCrystal.h>
// folosesc libraria pentru afisaje LCD simple
LiquidCrystal lcd(3, 4, 5, 6, 7, 8, 9);
// indic modul de legare, vezi mai jos:
/*                             -------------------
                               |  LCD  | Arduino |
                               -------------------
 LCD RS la Arduino D10         |  RS   |   D4   |
 LCD Enable la Arduino D3      |  E    |   D5    |
 LCD D4 la Arduino D4          |  D4   |   D6    |
 LCD D5 la Arduino D5          |  D5   |   D7    |
 LCD D6 la Arduino D6          |  D6   |   D8    |
 LCD D7 la Arduino D7          |  D7   |   D9    |
 LCD R/W la Arduino GND        |  R/W  |   GND   |
                               -------------------
 LCD VO la cursor potentiometru de 10-20k pus intre +5V si GND
 LED fundal alimentat prin rezistenta de 150-200 ohmi de la +5V
*/

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
  Serial.println("RPM / tachometer");

 lcd.begin(16, 2);   // se initializeaza afisajul alfanumeric cu 16 colaone si 2 linii    
 lcd.setCursor(2, 0); // se pozitioneaza cursorul in coloana a 2-a din stanga sus
 lcd.print("Tachometer"); // scriu un text
 lcd.setCursor(2, 1); // se pozitioneaza cursorul in coloana a 2-a pe randul al doilea de sus in jos
 lcd.print("by niq_ro"); // print a text
 delay (2000);
 lcd.clear(); // sterg ecranul
 
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
  
  } // end of setup

void loop () 
  {

  if (!triggered)
    return;
 
  unsigned long elapsedTime = finishTime - startTime;
  float rpm = 60 * F_CPU / float (elapsedTime);  // each tick is 62.5 nS at 16 MHz
  
 
    
    Serial.print ("RPM: ");
    Serial.println (rpm);

 lcd.setCursor(3, 1); 
 //lcd.print("Turatie: ");
 if (rpm < 1000) lcd.print(" ");  // daca valoarea procentului are doar 3 cifre
 if (rpm < 100) lcd.print(" ");  // daca valoarea procentului are doar 2 cifre
 if (rpm < 10) lcd.print(" ");  // daca valoarea procentului are doar o cifra
 lcd.print(rpm);    // afisez valoarea procentului
 lcd.print("rpm   ");        // afisez semnul de %

  
  
  //Serial.print ("Took: ");   // Ausgabe der gezÃƒÂ¤hlten counts
  //Serial.print (elapsedTime);
  //Serial.print (" counts. ");

  // so we can read it  
  delay (500);

  prepareForInterrupts ();   
}   // end of loop
