# PDP

**Puerta del Perros con Arduino**

An Arduino based system for controlling pedestrian (and canine) access via an existing electric gate that utilises a NICE Mindy A60 gate controller.

Registering cards and the security list
=======================================
A Master card controls the registering and de-registering of operating cards, but cannot operate the gate.
    
Learning LED;
*************
+ Off- gate mode (or no power).  
+ Flashing- (with Action LED) no Master card registered.  
+ Flashing- learn mode (returns to gate mode 5 seconds after last learning read).  
    
Action LED; 
***********
+ Off - not activated by a card.  
+ Flashing - (with Learning LED) no Master card registered.
+ Flashing - gate moving.
+ Steady - gate about to move. 
    
No Master
*********
+ Initially no cards are registered as the Master card or in the security list.
+ First the Master card must be registered.
+ When the Learning and Action LEDs flash alternately and a buzzer will sound.
+ The first card tagged will be registered as the Master card. 
+ The LEDs will then turn off and the buzzer will stop confirming registration of the Master card. 
+ A serial message ###:###:###:###:MDR is sent for logging

To register a card in the security list
***************************************
+ With the LEDs off, tag the Master card, the Learning LED flashes and a buzzer sounds. 
+ Tag a new card within 5 seconds to register this new card in the security list. 
+ The Learning LED will then turn off and the buzzer will stop, confirming registration of the card in the security list. 
+ A serial message ###:###:###:###:SAV is sent for logging.
    
To de-register a card from the security list
********************************************
+ Tag the Master card to enter learning mode. 
+ Within 5 seconds tag a card that is already registered in the security list. 
+ This card is now removed from the security list. 
+ The Learning LED will then turn off and the buzzer will stop, confirming de-registration of the card from the security list. 
+ A serial message ###:###:###:###:DEL is sent for logging. 
+ This method will not remove the Master card.
    
To de-register ALL the cards, including the master card 
*******************************************************
+ Press the clear memory button on the PDP hat. 
+ The Learning and Action LEDs will flash alternately and a buzzer will sound, confirming de-registration of ALL the cards.
    
Lost card de-registration
************************* 
+ Press the clear memory button on the PDP hat to de-register ALL cards. 
+ The first card tagged will be registered as the Master card. 
+ Tag the Master card and Re-register each available card.

Memory EEPROM/RAM
*****************
+ In normal operation the IDs of the cards will be saved to EEPROM so that they are persistant even when the system powers down.
+ For testing it is also possible change a constant in  the PDP sketch to use RAM instead of EEPROM for testing, in order to save EEPROM memory writes.
    
Gate operation
==============
Unknown card
************
+ Tag a card that is not registered on the security list. 
+ The system makes one short "error" beep and the gate is not instructed to operate. 
+ A serial message ###:###:###:###:UNK is sent for logging.
    
Master card
***********
+ Tag the Master card. 
+ The system goes into learning mode and the gate is not instructed to operate. 

Registered card
***************
+ Tag a card that is registered on the security list. 
+ A pulse is sent to the gate controller instructing it to open the pedestrian gate. 
+ The "open gate delay time" for the pedestrian gate is set by the delay potentiometer on the PDP hat. 
+ When the "open gate delay time" has elapsed a pulse is sent to the controller instructing it to stop moving the pedestrian gate.
+ Then after 60 seconds waiting delay, a pulse is sent to the gate controller instructing it to close the pedestrian gate. 
+ The time taken to close the pedestrian gate is set by the delay potentiometer on the PDP hat. 
+ The "close gate delay time" is 150% of the "open gate delay time" plus a "gate shutdown delay". 
+ The system makes deep caution beeps while the gate is opening and high warning beeps while the gate is closing.   
+ A serial message ###:###:###:###:OPR is sent for logging.
+ If the main vehicular gate is open then the pedestrian gate operation has no effect. This is stipulated by the gate controller. 
+ The existing radio remote control for the gate control unit will continue to work as designed for main gate vehicular access.
  
Manual early closing
********************
+ When the gate is opening or during the waiting phase the system will recognise if a card, that is registered in the security list, is tagged.
+ A pulse will be sent to the controller instructing the gate to stop and then to close immediately
+ So the pedestrian can close the gate immediatly and not have to wait for 60 seconds for the gate to close automatically. 
    
Serial logging
**************
+ If the debug level constant, in the PDP sketch, is set to 100 then logging is written to the serial port.  
+ A simple python program "serial_comms.py" has been developed.  
+ This program monitors the serial port and writes any logging messages recieved to a simple XML file.  
+ An HTML program index.html has been developed.
+ This program uses ajax to read the logging XML file and displays the logging history on a simple webpage.
  
Technologies;
=============
+ Arduino 
+ RFID 
+ EEPROM 
+ LED 
+ Tone for sound 
+ Relay interfacing to NICE Mindy 
+ ADC conversion and mapping 
+ Serial comms
+ HTML
+ CSS
+ AJAX
+ XML
+ C
+ Python
  
This is a fully end user ready RFID gate controller integration solution that is used daily.
