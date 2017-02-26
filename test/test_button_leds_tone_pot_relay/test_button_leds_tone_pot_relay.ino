#define eraseMEM 3
#define ledMODE 7
#define ledACTION 6
#define BUZZ 5
#define POT 0
#define RELAY 4

void setup() {
  pinMode(eraseMEM, INPUT);
  pinMode(ledMODE, OUTPUT);
  pinMode(ledACTION, OUTPUT);  
  pinMode(RELAY, OUTPUT);
}

void loop() {
  int t = map(analogRead(POT),0,1024,0,1000);
  if(digitalRead(eraseMEM)) {
    digitalWrite(ledACTION, HIGH);
    digitalWrite(ledMODE, LOW);
    noTone(BUZZ);
    digitalWrite(RELAY, LOW);
    }
  else
    {
    digitalWrite(ledMODE, HIGH);
    digitalWrite(ledACTION, LOW);
    tone(BUZZ,t);
    digitalWrite(RELAY, HIGH);    
    }
}
