//-----------------------------------------------
// http://playground.arduino.cc/Main/ReadingRPM
// http://www.themakersworkbench.com/content/tutorial/reading-pc-fan-rpm-arduino
// adapted sketch by Nicu FLORICA (niq_ro) - see video: https://youtu.be/jqF1CF34O2s

 volatile byte half_revolutions;
 unsigned int rpm;
 unsigned long timeold;
 const byte interruptPin = 2;
 
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
 
 void setup()
 {
   Serial.begin(9600);
   attachInterrupt(0, rpm_fun, RISING);
//   attachInterrupt(digitalPinToInterrupt(interruptPin), rpm_fun, RISING);

   half_revolutions = 0;
   rpm = 0;
   timeold = 0;

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
   
 }
 void loop()
 {

   if (half_revolutions >= 20) { 
     //Update RPM every 20 counts, increase this for better RPM resolution,
     //decrease for faster update
     rpm = 30.0*1000/(millis() - timeold)*half_revolutions;
     timeold = millis();
     half_revolutions = 0;
     Serial.print(rpm,DEC);
     Serial.println("rpm");
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
   }
// delay(2500);
 
 }  // end main loop
 
 void rpm_fun()
 {
   half_revolutions++;
   //Each rotation, this interrupt function is run twice
 }
//-----------------------------------------------
