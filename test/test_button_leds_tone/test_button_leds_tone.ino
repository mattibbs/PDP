#define eraseMEM 3
#define ledMODE 7
#define ledACTION 6

void setup() {
  pinMode(eraseMEM, INPUT);
  pinMode(ledMODE, OUTPUT);
  pinMode(ledACTION, OUTPUT);  
}

void loop() {
  if(digitalRead(eraseMEM)) {
    digitalWrite(ledACTION, HIGH);
    digitalWrite(ledMODE, LOW);
    noTone(5);
    }
  else
    {
    digitalWrite(ledMODE, HIGH);
    digitalWrite(ledACTION, LOW);
    tone(5,1000);
    
    }
}
