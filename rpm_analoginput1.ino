// http://playground.arduino.cc/Learning/Tachometer
// This example shows one way of creating an optoswitch
// using an IR LED as emiter and an IR LED receiver as
// light sensor.
// On this case it acts as a tachometer to count the
// revolutions per second of an aeromodelism plane's
// propeller.
//
//           + GROUND                                 +GROUND          
//           |                                        |  
//           <                                        < 
//           > 220 ohm resistor                       > 220 omh resistor
//           <                                        <      
//           |                                        |  
//           |                                        |
//         -----                                    -----
//          / \    >>IR LED emiter >>>>>>>>>>>>>>>>  / \   IR LED receiver
//         -----                                    -----
//           |                                        |
//           |                                        |
//           + +5VCD                                  +  ANALOG INPUT 0
//
// Nicu FLORICA (niq_ro)change sketch and use Hall sensor instead optocoupler and 8-digit 7-segment led diplay with MAX7219
// see movie from https://youtu.be/mTNSfeL_zTw
int val;
long last=0;
int stat=LOW;
int stat2;
int contar=0;

int sens=75;  // this value indicates the limit reading between dark and light,
              // it has to be tested as it may change acording on the 
              // distance the leds are placed.
int nPalas=2; // the number of blades of the propeller

int milisegundos=500; // the time it takes each reading

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
int vs, vsz, vz, vzu, vu, vv;
int vtot; 

void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  
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
  val=analogRead(0);
  if(val<sens)
    stat=LOW;
   else
    stat=HIGH;
   digitalWrite(13,stat); //as iR light is invisible for us, the led on pin 13 
                          //indicate the state of the circuit.

   if(stat2!=stat){  //counts when the state change, thats from (dark to light) or 
                     //from (light to dark), remmember that IR light is invisible for us.
     contar++;
     stat2=stat;
   }
   if(millis()-last>=milisegundos){
     double rps=((double)contar/nPalas)/2.0*1000.0/milisegundos;
     double rpm=((double)contar/nPalas)/2.0*60000.0/(milisegundos);
     Serial.print((contar/2.0));Serial.print("  RPS ");Serial.print(rps);
     Serial.print(" RPM");Serial.print(rpm);Serial.print("  VAL ");Serial.println(val);
     contar=0;
     last=millis();
//  vtot = ((double)contar/nPalas)/2.0*60000.0/(milisegundos);
vtot = rpm;
//vtot = rpm;
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
     
}  // end main loop 
