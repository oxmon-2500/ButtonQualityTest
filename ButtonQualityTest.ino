//
//     5V        D4
//     ^     o    .
//     |    /     |     +-----+
//     +---/  ----+-----| 10k |---+
//                      +-----+   |
//                               ___
//                                _

const int BUTTON_PORT = 7; //D7
const int SYSLED = 13; //D13

const int MXSIZ = 200;
int cx;
unsigned long cycl[MXSIZ+4];

enum ST {ST0, ST_NEUTRAL, ST_HIGH, ST_LOW, ST_ERR} state;
unsigned long millis0;
unsigned long cyCt;
unsigned long oneSec;
int dPortVal, dPortVal0;
void report(); //proto

class blinkErr {
private:
  boolean flip;
  unsigned long ms0;
public:
  blinkErr(){
    flip=false;
    ms0=0;
  }
  boolean doBlinkErr(){
    if (millis()-ms0 >=150){
      ms0=millis();
      flip = !flip;  
      digitalWrite(SYSLED, flip ? HIGH : LOW);
    }
    return flip;
  }
} blErr;

void setup(void){
  // start serial port
  Serial.begin(9600);
  Serial.println("Push Button Quality Test");
  millis0 = millis();
  state = ST0;
  cyCt=0;
  dPortVal=0;
  cx=0;
  pinMode(BUTTON_PORT, INPUT);
  pinMode(SYSLED     , OUTPUT);
}

void loop(void){ 
  cyCt++;
  switch(state){
    case ST0:
      //
      if (millis()-millis0 > 1000){
        oneSec = cyCt;
        Serial.println("      cycles (hex)");
        Serial.print(" 1 sec: ");
        Serial.println(oneSec, HEX);
        Serial.print("100 ms:  ");
        Serial.println(oneSec/10, HEX);
        Serial.print(" 10 ms:   ");
        Serial.println(oneSec/100, HEX);
        state = ST_NEUTRAL;
        digitalWrite(SYSLED, HIGH);
        cyCt=0;
      }
      break;
    case ST_NEUTRAL:
      if (digitalRead(BUTTON_PORT)){
        state = ST_HIGH;
        cyCt = 0;
        digitalWrite(SYSLED, LOW);
      }
      break;
    case ST_HIGH:
      if (!digitalRead(BUTTON_PORT)){
        cycl[cx++] = cyCt; if (cx>=MXSIZ) {errState(); break;}
        cyCt = 0;
        state = ST_LOW;
      }
      break;
    case ST_LOW:
      if (cyCt>oneSec){ // no more bouncing
        report();
        digitalWrite(SYSLED, HIGH);
        cyCt=0;
        state = ST_NEUTRAL;
        return;
      }
      if (digitalRead(BUTTON_PORT)){
        cycl[cx++] = cyCt; if (cx>=MXSIZ) {errState(); break;}
        cyCt = 0;
        state = ST_LOW;
      }
      break;
    case ST_ERR:
      blErr.doBlinkErr();
      if (millis()-millis0 > 3000){
        digitalWrite(SYSLED, HIGH);
        state = ST_NEUTRAL;
      }
      break;
  }
}

void report(){
  int ct=0;
  unsigned long last=cycl[0];
  Serial.print(" ");
  Serial.print(cycl[0], HEX);
  for(int ix=1; ix<cx; ++ix){
    if (cycl[ix]==last){
      ct++;
    }else{
      if (ct>0){
        Serial.print("(");
        Serial.print(ct+1);
        Serial.print(")");
        ct = 0;
      }
      Serial.print(" ");
      Serial.print(cycl[ix], HEX);
      last=cycl[ix];
    }
  }
  Serial.println();
  cx = 0;
}
void errState(){
  Serial.println("buffer overflow!");
  state = ST_ERR;
  cx=0;
  millis0=millis();
}
