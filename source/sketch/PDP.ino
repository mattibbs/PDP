/*libraries*/
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h> 

/*pin constants*/
//      SCK       13 
//      MISO      12
//      MOSI      11
#define SDA_PIN   10
#define RST_PIN   9
#define LEDMODE   7
#define LEDACTION 6
#define BUZZ      5
#define GATE      4
#define ERASEMEM  3
#define DELAYPOT  0

/*tone frequencies constants*/
#define BUZZLOW 1000
#define BUZZHIGH 2000
#define BUZZERROR 100

/*temporal constants*/
#define TIMFLASH 500
#define TIMLEARN 5000
#define TIMBUZZ 200
#define TIMGATE 1000
#define TIMUNKNOWN 1000
#define TIMREAD 500
#define TIMWAIT 60000
#define TIMMINDELAY 5000
#define TIMMAXDELAY 15000

/*memory constants*/
#define MEM 40 //10 cards
#define ID_BYTES 4
#define MEMTOUSE 0 //0=ROM 1=RAM

/* misc constants*/
#define debugLEVEL 100 //0 most verbose, 20 operational description, 100 serial logging
#define TRUE 1
#define FALSE 0

/*types*/
enum Mode
        {
          Empty,
          Learn,
          Operate,
          Erase
        };

/*global variables*/
Mode mode;
byte *master[ID_BYTES];
byte reg[MEM]; //Use this non persistant memory for testing to avoid damaging the EEPROM

MFRC522 mfrc522(SDA_PIN, RST_PIN); 

void setup() {
  /* Initialise
   * ========== 
   *    Set up digital pins
   *    Set up Serial comms
   *    Set up SPI
   *    Set up mfrc522
   *    Check EEPROM size
   *    Check memory for master card
   *    Initialise mode
   *    Clear all memory if no master card
   */
  pinMode(LEDMODE, OUTPUT);
  pinMode(LEDACTION, OUTPUT);
  pinMode(GATE, OUTPUT);  
  pinMode(ERASEMEM, INPUT);
  digitalWrite(GATE, HIGH);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  if(EEPROM.length()<MEM) {
    while(FALSE) {
      error(String("Insuficient memory. Adjust MEM constant and restart."));
      delay(TIMBUZZ);
    }
  }
  checkMaster();
  if(mode==Empty) 
    eraseAll();  
  digitalWrite(LEDMODE,HIGH);
  digitalWrite(LEDACTION,HIGH);
  tone(BUZZ,BUZZLOW);
  delay(TIMFLASH);
  noTone(BUZZ);
  digitalWrite(LEDMODE,LOW);
  digitalWrite(LEDACTION,LOW);
}

void debug(String msg, int level) {
  /* Write messages to Serial
   * ========================
   *    Used for debug and logging
   *        level 0 most verbose 
   *        level 20 operational description
   *        level 100 serial logging
   */
  if (level >= debugLEVEL) 
    Serial.println(String(msg));
}

void logWrite(byte *id, String type) {
  /* Write a log
   * ===========
   *    Serial comms to an external logger. 
   *    To allow actions in the RFID access system to be logged.
   *        ###:###:###:###:MSR Save this RFID as a master into the persistant memory
   *        ###:###:###:###:SAV Save this RFID as an operator into the persistant memory
   *        ###:###:###:###:DEL Delete this RFID from the persistant memory
   *        ###:###:###:###:OPR This known RFID operated the gate
   *         ###:###:###:###:UNK Unknown RFID attempted to gain access
   * 
   *    This required debug level 100 to operate withour debug noise.
   */
  debug(printID(id)+type,100);
}

String printID(byte *id) {
  /* Format id for logging
   * =====================
   *    Convert the 4 bytes into a ":" separated String object
   *    return a String
   */
  String msg="";
  for(int i=0; i<ID_BYTES; i++) 
    msg=msg+id[i]+String(":");
  return msg;
}

byte memReadByte(int loc) {
  /* Read a byte from memory
   * =======================
   *    Reads a byte from memory
   *    Uses RAM or EEPROM depending on a constant
   *    If RAM is used then all memory is lost when power is cut or Arduino is restarted
   *    ROM has a limited number of writes.
   *    returns a byte
   */
  return (MEMTOUSE==0) ? (EEPROM.read(loc)) : (reg[loc]);
}

void memWriteByte(byte id, int loc) {
  /* Write a byte to memory
   * ======================
   *    Writes a byte to memory
   *    Uses RAM or EEPROM depending on a constant
   *    If RAM is used then all memory is lost when power is cut or Arduino is restarted
   *    ROM has a limited number of writes.
   */
  if(MEMTOUSE==0) 
    EEPROM.write(loc,id);
  else 
    reg[loc]=id;
}

void memWriteWord (byte *id, int loc) {
  /* Write a 4 byte id to memory
   * ===========================
   */
  for(int i=0; i<ID_BYTES; ++i)
    memWriteByte(id[i],loc+i);
}

void checkMaster() {
  /* Check if the master record exists
   * =================================
   *    Test if a master card has been stored
   *    Set the global mode to Empty if no master has been stored or Operate if a master exists
   */
  byte x=0;
  for(int i=0; i<ID_BYTES; ++i) {
    master[i]=memReadByte(i);
    x=x||master[i];
    }
  mode = (x==0) ? (Empty) : (Operate);
}

void error(String err) {
  /* Notify user of error
   * ====================
   *    Makes a deep buzz
   *    Sends the error message to the serial output
   */
  debug(String("Error : ")+err,100);
  tone(BUZZ,BUZZLOW,TIMBUZZ);  
}

bool thisMemory(byte *id, int loc) {
  /* Test if the id exists in this block of 4 bytes of memory
   * ========================================================
   *    returns Boolean
   */
  for(int i=0; i<ID_BYTES; i++) {
    //debug(String("Mem Byte : ")+memReadByte(loc+i),1);
    //debug(String("ID Byte : ")+id[i],1);
    if(memReadByte(loc+i)!=id[i]) 
      return FALSE;
  }
  return TRUE;  
}

int inMem(byte *id) {
  /* Test if this id exists in memory
   * ================================
   *    returns memory location or zero if the id does not exist in memory
   */
  //debug(String("In Mem : ")+printID(id),1);
  for(int loc=ID_BYTES; loc<MEM; loc+=ID_BYTES) {
    //debug(String("Test : ")+loc,1);
    if(thisMemory(id, loc)) 
      return loc;
  }
  //debug(String("Location Not found"),1);
  return 0;
}

bool inMemory(byte *id) {
  /* Test if this id exists in memory
   * ================================
   *    returns Boolean
   */
  //debug(String("In Mem : ")+printID(id),1);
  return (inMem(id)>0) ? (TRUE) : (FALSE);
}

bool isMaster(byte *id) {
  /* Test if this is the master id
   * =============================
   *    returns Boolean
   */
  //debug(String("Is Master New ID : ")+printID(id),1);
  //debug(String("Existing Master  : ")+printID(master),1);
  for(int i=0; i<ID_BYTES; ++i) {
    if(master[i]!=id[i]) 
      return FALSE;
  }
  //debug(String("IS Master"),1);
  return TRUE;
}

void deleteID(byte *id) {
  /* Delete an access card
   * =====================
   *    Free up a memory location.
   *    Send a log message
   */
  debug(String("Delete ID : ")+printID(id),20);
  int loc=inMem(id);
  for(int i=0; i<ID_BYTES; ++i) {
    if(memReadByte(loc+i)==id[i]) 
      memWriteByte(0, loc+i);
  } 
  logWrite(id,String("DEL"));
  mode=Operate;
}

void operateGate(byte *id) {
  /* Cycle the gate  
   * ==============
   * Integrates with "NICE Mindy A60" swinging gate controller with pedestrian entry enabled via CLOSE gate input
   *    Send a log message
   *    Read the delay potentiometer.
   *    First pulse starts open movement. 
   *        Low pitch buzz and flashing LED during open movement. 
   *        Time for open movement set by potentiometer. (Minimum 5 seconds, maximum 15 seconds)
   *    Second pulse stops open movement to allow pedestrian passage. 
   *        Static LED during wait for pedestrian passage. 
   *        Time for pedestrian passage is 1 minute, but can be preempted by reading a valid card during this waiting period.
   *    Third pulse starts close movement
   *        High pitch buzz and flashing LED during close movement. 
   *        Time for close movement is set by potentiometer at 150% of open movement time.
   */
  debug(String("Operate Gate"),20);
  logWrite(id,String("OPR"));
  int openDelay = map(analogRead(DELAYPOT),0,1024,TIMMINDELAY,TIMMAXDELAY);
  bool earlyClose = FALSE;
  //debug(String("Send High to gate controller"),5);
  /*
   * Pulse gate controller and delay for some seconds
   * flashing LED and sounding low beep
   */
  unsigned long startOpen=millis();
  unsigned long tim=millis();
  digitalWrite(GATE, LOW);
  debug(String("Opening"),20);
  while(startOpen+openDelay>millis()) {
    if(tim+TIMFLASH<millis()) {
      if(!digitalRead(LEDACTION)) {
        digitalWrite(LEDACTION,HIGH);
        tone(BUZZ,BUZZLOW);
        }
      else {
        digitalWrite(LEDACTION,LOW);
        noTone(BUZZ);
        }
      tim=millis();
      }
      if(startOpen+TIMGATE<millis()) 
        digitalWrite(GATE, HIGH);
      if (digitalRead(GATE)) {
        byte card[ID_BYTES] = {0,0,0,0};
        byte *id = card;
        id = readRFID(id);
        if(id[0]>0 & inMemory(id)) 
          earlyClose = TRUE;
          openDelay = millis - startOpen;
          break;
      }
    }
  /*
   * Pulse gate controller and delay for some seconds
   * static LED and sounding no beep
   */
  unsigned long startWait=millis();
  digitalWrite(LEDACTION,HIGH);
  digitalWrite(GATE, LOW);
  debug(String("Stopping"),20);
  noTone(BUZZ);
  while(startWait+TIMWAIT>millis()) {
    if(startWait+TIMGATE<millis()) 
      digitalWrite(GATE, HIGH);
    if (digitalRead(GATE)) {
      byte card[ID_BYTES] = {0,0,0,0};
      byte *id = card;
      id = readRFID(id);
      if(id[0]>0 & inMemory(id)||earlyClose) 
        break;
      }
    }
  /*
   * Pulse gate controller and delay for some seconds
   * flashing LED and sounding high beep
   */    
  unsigned long startClose=millis();
  tim=millis();
  digitalWrite(GATE, LOW);
  debug(String("Closing"),20);  
  while(startClose+(openDelay*1.5)>millis()) {
    if(tim+TIMBUZZ<millis()) {
      if(!digitalRead(LEDACTION)) {
        digitalWrite(LEDACTION,HIGH);
        tone(BUZZ,BUZZHIGH);
        }
      else {
        digitalWrite(LEDACTION,LOW);
        noTone(BUZZ);
        }
      tim=millis();
      }
      if(startClose+TIMGATE<millis()) 
        digitalWrite(GATE, HIGH);
    }
  /*
   * Make sure LED is off and no beep
   */
  digitalWrite(LEDACTION,LOW);
  noTone(BUZZ);
}

void saveMaster(byte *id) {
  /* Save the master card 
   * ====================
   *    Write the master card id to the first memory location 
   *    Send a log message
   */
  debug(String("Save Master : ")+printID(id),20);
  memWriteWord(id,0);
  logWrite(id,String("MSR"));
  checkMaster();
}

void saveID(byte *id) {
  /* Save an access card
   * ===================
   *    Save an access card id to the next availble memory location
   *    Send a log message
   */
  debug(String("Save ID : ")+printID(id),20);
  if(isMaster(id)) 
    error(String("Master ID cannot be saved as operator"));
  else {
    byte card[ID_BYTES] = {0,0,0,0};
    byte *empty;
    empty = card;
    int loc=inMem(empty);
    if (loc>0) 
      memWriteWord(id,loc);
    else 
      error(String("Memory full."));
    mode=Operate;
    if(!inMemory(id)) 
      error(String("Not saved."));    
    else 
      logWrite(id,String("SAV"));
  }
}

void unknownID(byte *id) {
  /* Unknow access card
   * ==================
   *    Buzz if an unknown card seeks entry
   *    Send a log message
   */
  debug(String("Unknown ID"),20);
  logWrite(id,String("UNK"));
  tone(BUZZ,BUZZERROR);
  delay(TIMUNKNOWN);
  noTone(BUZZ);
}

void eraseAll() {
  /* Erase all cards
   * ===============
   *    Erase all cards including the master card
   */
  debug(String("Erase all memories"),20);
  for(int i=0; i<MEM; ++i) {
    if(memReadByte(i) != 0) 
      memWriteByte(0, i);
  }
  checkMaster();
  delay(TIMREAD);
}

byte * readRFID(byte *id) {
  /* Read card RFID
   * ==============
   *    Read a card and grab the card id
   *    returns a pointer to an array of 4 bytes of memory
   */
  if (mfrc522.PICC_IsNewCardPresent()) {
    //debug(String("New card"),2);
    if (mfrc522.PICC_ReadCardSerial()) {
      for (int i=0; i<ID_BYTES; i++) { 
        id[i] = mfrc522.uid.uidByte[i];
        //debug(String("Byte : ")+i+String("  Data : ")+id[i],20);
        }
      mfrc522.PICC_HaltA(); // Stop reading
      }
      //logWrite(id,String("CRD"));
    }
  return id;  
}

void checkRead(){
  /* Read a card
   * ===========
   *    Read a card and decide what to do depending on the Mode and if the card is known
   */
  byte card[ID_BYTES] = {0,0,0,0};
  byte *id = card;
  id = readRFID(id);
  if(id[0]>0){
    if(isMaster(id)) 
      mode=Learn;
    else {
      if(inMemory(id)) {
        switch(mode) {
          case Learn : {
            deleteID(id); 
            break;
            }
          case Operate : {
            operateGate(id); 
            break;
            }
          }
        }
      else {
        switch(mode) {
          case Empty : {
            saveMaster(id); 
            break;
            }
          case Learn : {
            saveID(id); 
            break;
            }
          case Operate : {
            unknownID(id); 
            break;
            }
          }
        }      
      }
      delay(TIMREAD);
      //for(int i=0; i<MEM; i+=ID_BYTES) {
        //debug(String("Byte ")+i+String(" > ")+memReadByte(i)+String(":")+memReadByte(i+1)+String(":")+memReadByte(i+2)+String(":")+memReadByte(i+3),1);
    //}
  }
}

void no_master(){
  /* Master card recording mode
   * ==========================
   *    Change into a master card recording mode
   *    Flashing LED and low pitch buzzing
   */
  debug(String("No Master Registered"),20);
  unsigned long tim=millis();
  int ctr=0;
  while(mode==Empty) {
    if(tim+TIMFLASH<millis()) {
      ctr++;
      if(!digitalRead(LEDMODE)) {
        digitalWrite(LEDMODE,HIGH);
        digitalWrite(LEDACTION,LOW);
        tone(BUZZ,BUZZLOW);
        }
      else {
        digitalWrite(LEDMODE,LOW);
        digitalWrite(LEDACTION,HIGH);
        noTone(BUZZ);
        }
      tim=millis();
      }
    checkRead();
  }
  digitalWrite(LEDMODE,LOW);
  digitalWrite(LEDACTION,LOW);
  tone(BUZZ,BUZZHIGH,TIMFLASH*2);
}

void learn(){
  /* Learning mode
   * =============
   *    Change into a learning mode
   *    Static LED with buzzing at first low pitch then high pitch
   */
  debug(String("Learn"),20);
  unsigned long startLearn=millis();
  unsigned long startBuzz=millis();
  int whatBuzz=0;
  int ctr=0;
  bool buzz=FALSE;
  while(startLearn+TIMLEARN>millis()) {
    if(startBuzz+TIMBUZZ<millis()) {
      ctr++;
      if(ctr<TIMLEARN/(TIMBUZZ*2))
        whatBuzz=BUZZLOW;
      else 
        whatBuzz=BUZZHIGH;
      if(!buzz) {
        tone(BUZZ,whatBuzz);
        digitalWrite(LEDMODE,HIGH);
        buzz=TRUE;
        }
      else {
        noTone(BUZZ);
        digitalWrite(LEDMODE,LOW);
        buzz=FALSE;
      }
      startBuzz=millis();
    }
    checkRead();
    if(mode==Operate) 
      break;
  }
  digitalWrite(LEDMODE,LOW);
  tone(BUZZ,BUZZHIGH); 
  delay(TIMFLASH*2);
  noTone(BUZZ);
  mode=Operate;
}

void normal(){
  /* Normal operation
   * ================
   *    Normal operation searching for an access card
   *    No buzzing, no LEDs
   */
  //debug(String("Waiting"),0);
  digitalWrite(LEDMODE,LOW);
  noTone(BUZZ);
  checkRead();  
}

void loop() {
  /* Depending on the mode do some stuff
   * ===================================
   */
  switch(mode) {
    case Empty : {
      no_master(); 
      break;
      }
    case Learn : {
      learn(); 
      break;
      }
    case Erase : {
      eraseAll(); 
      break;
      }
    default : {
      normal(); 
      break;
      }
  }
  if(digitalRead(ERASEMEM)==LOW) {
    debug(String("Errase button"),0);
    mode=Erase;
  }
}
