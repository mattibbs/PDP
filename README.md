# PDP
Puerta del Perros con Arduino

A Arduino based system for controlling pedestrian (and canine) access via an existing electric gate that utilises a NICE Mindy A60 gate controller.

Registering cards and the security list.

  A master card controls the registering of operating cards, but cannot operate the gate.

  Learning LED; 
    Off - gate mode (or no power)
    Flashing - no master card registered
    Steady - learn mode (returns to gate mode 5 seconds after last learning read)

  Action LED;
    Off - not activated by a card
    Flashing - unknown card
    Steady - known card

  Initially no cards are registered as the master card or in the security list. First the master card must be registered.

  When the Learning and Action LEDs flash alternatly and a buzzer will sound.  
  The first card tagged will be registered as the master card. 
  The LEDs will then turn off and the buzzer will stop confirming registration of the master card. 
  A serial message ###:###:###:###:MDR is sent for logging.

  With the LEDs off, tag the master card, the Learning LED flashes and a buzzer sounds. 
  Tag a new card within 5 seconds to register this new card in the security list. 
  The Learning LED will then turn off, confirming registration of the card in the security list. 
  A serial message ###:###:###:###:SAV is sent for logging.

  To deregister a card from the security list. Tag the master card to enter learning mode. 
  Within 5 seconds tag a card already registered in the security list. 
  This card is now removed from the security list. 
  The Learning LED will then turn off, confirming deregistration of the card from the security list. 
  A serial message ###:###:###:###:DEL is sent for logging. 
  This method will not remove the master card.

  To deregister ALL the cards, including the master card. 
  Press the clear memory button on the PDP hat. 
  The learning mode LED will then begin flashing, confirming deregistration of ALL the cards.

  Lost card deregistration. 
  Deregister ALL cards. 
  Reregister master. 
  Reregister each available card.

Gate operation.

  Tag a card that is not registered on the security list. 
  The system makes one short error beep and the gate is not instructed to operate. 
  A serial message ###:###:###:###:UNK is sent for logging.

  Tag the master card.
  The system goes into learning mode and the gate is not instructed to operate.
  Tag a card that is registered on the security list.
  If the gate is initially closed;
    A pulse is sent to the gate controller instructing it to open the pedestrian gate.
    The time taken to open the pedestrian gate is set by the delay pot.
    When the gate open delay time has elapsed a pulse is sent to the controller instructing it to stop moving the pedestrian gate.
    Then after 60 seconds, a pulse is sent to the gate controller instructing it to close the pedestrian gate. 
    The time taken to open the pedestrian gate is set by the delay pot. It is 150% of the opening time.
  If the main gate is open then the operation has no effect. This is stipulated by the gate controller.
  The system makes deep causion beeps while the gate is opening and high warning beeps while the gate is closing. 
  When the gate is open during the waiting phase it is possible to tag a card (that is registered in the security list) and 
  have the gate close immediatly, so you don't need to wait for 60 seconds for it to close.
  A serial message ###:###:###:###:OPR is sent for logging.
  The existing radio remote control for the NICE Mindy gate control unit will continue to work as designed for main 
  gate vehicular access.

Technologies
  Arduino
  RFID
  EEPROM
  LED 
  Tone for sound
  Relay interfacing to NICE Mindy
  ADC conversion and mapping
  Serial comms

This is a fully end user ready solution that is used daily.  It is not a prototype.
