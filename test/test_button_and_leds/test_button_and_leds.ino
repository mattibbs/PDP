#define eraseMEM 3
#define ledMODE 7
#define ledACTION 6

void setup() {
  pinMode(eraseMEM, INPUT);
  pinMode(ledMODE, OUTPUT);
  pinMode(ledACTION, OUTPUT);  
}

void loop() {
  digitalWrite(ledACTION,digitalRead(eraseMEM));
  digitalWrite(ledMODE,!digitalRead(eraseMEM));
}
