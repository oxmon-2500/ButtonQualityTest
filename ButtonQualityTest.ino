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

const int MXSIZ = 600;
int cx;
unsigned int cycl[MXSIZ+4];

enum ST {ST0, ST_NEUTRAL, ST_HIGH, ST_LOW, ST_ERR} state;
unsigned long millis0;
unsigned long cyCt;
unsigned long oneSec;
unsigned int mxBouncing; //maximum cycles of bouncing
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
    if (millis()-ms0 >=100){
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
        Serial.println("+-time--+-cycles-+");
        Serial.print  ("| 1 sec |  ");
        Serial.print  (oneSec, HEX);
        Serial.println(" |");
        Serial.print  ("|100 ms |   ");
        Serial.print  (oneSec/10, HEX);
        Serial.println(" |");
        Serial.print  ("| 10 ms |    ");
        Serial.print  (oneSec/100, HEX);
        Serial.println(" |");
        Serial.println("+-------+--------+");
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

unsigned int cyclToNs(unsigned int cyNo){
  return (unsigned long)cyNo * 1000000l / oneSec;
}

unsigned int maxBouncingMs(){
  if (cx==1){
    return cyclToNs(mxBouncing);
  }
  int pmx=0, px;
  //find max value position
  for(px=0; px<cx; ++px){
    if (cycl[px]>cycl[pmx]){
      pmx = px;
    }
  }
  // count bouncing cycles before and after maxValue
  unsigned int bcBefore=0;
  for(px=0; px<pmx; ++px){
    bcBefore+=cycl[px];
  }
  unsigned int bcAfter=0;
  for(px=pmx+1; px<cx; ++px){
    bcAfter+=cycl[px];
  }
  unsigned int maxB = bcBefore>bcAfter? bcBefore : bcAfter; //max of both  
  mxBouncing = maxB > mxBouncing? maxB : mxBouncing;
  return cyclToNs(mxBouncing);
}

void report(){
  int ct=0;
  unsigned int last=cycl[0];
  Serial.print(" ");
  Serial.print(cycl[0], HEX);
  for(int ix=1; ix<cx; ++ix){
    if (cycl[ix]==last){
      ct++;
    }else{
      if (ct>0){
        Serial.print("*");
        Serial.print(ct+1, HEX);
        ct = 0;
      }
      Serial.print(" ");
      Serial.print(cycl[ix], HEX);
      last=cycl[ix];
    }
  }
  if (ct>0){
    Serial.print("*");
    Serial.print(ct+1, HEX);
  }
  Serial.print(" ");
  Serial.print(maxBouncingMs());
  Serial.println("ns");
  cx = 0;
}
void errState(){
  Serial.println("buffer overflow!");
  state = ST_ERR;
  cx=0;
  millis0=millis();
}
