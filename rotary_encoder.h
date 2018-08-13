#define ROTARYSTEPS 1
#define ROTARYMIN   0
#define ROTARYMAX   10000
#define MYKTUKAS    4

int lastPos = -1, newPos;
unsigned int button, btn;
unsigned int key;
int buttonState;
int lastButtonState;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

//nuskaitomas myktuko reiksme
void read_key(){
  //Nuskaitomas myktukas
  int reading = digitalRead(MYKTUKAS);
  if (reading != lastButtonState) {lastDebounceTime = millis();}
  if ((millis() - lastDebounceTime) > debounceDelay) {
   if (reading != buttonState) {
    buttonState = reading;
    if (!buttonState) {key = !key; }
    }
  }
  lastButtonState = reading; 
}

void rotary_encoder(){
  //encoder.tick();
  newPos = encoder.getPosition() * ROTARYSTEPS;

  if (newPos < ROTARYMIN) {
    encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
    newPos = ROTARYMIN;

  } else if (newPos > ROTARYMAX) {
    encoder.setPosition(ROTARYMAX / ROTARYSTEPS);
    newPos = ROTARYMAX;
  } 
  
    //Tikrinam ar nera nuspaustas myktukas
    read_key();

    if(key){button = 1; btn = 1; key = 0;}else
    if(newPos > lastPos){button = 2; btn = 2;}else 
    if(newPos < lastPos){button = 3; btn = 3;}else{button = 0;}
    
    if (lastPos != newPos) {
      //Serial.print(newPos);
      //Serial.println();
      lastPos = newPos;
    }
}
