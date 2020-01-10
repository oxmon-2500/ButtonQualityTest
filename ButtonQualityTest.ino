//
//     5V        D4
//     ^     o    .
//     |    /     |     +-----+
//     +---/  ----+-----| 10k |---+
//                      +-----+   |
//                               ___
//                                _

const int BUTTON_PORT = 4; //Dx
const int SYSLED = 13; //D13

const int MXSIZ = 280;
int cx;
unsigned long cycl[MXSIZ+2];
unsigned long cyCt;
enum ST {ST0, ST_NEUTRAL, ST_HIGH, ST_LOW} state;
unsigned long millis0;
unsigned long oneSec;
unsigned int mxBouncing; //maximum cycles of bouncing
int dPortVal, dPortVal0;
void report()      ; //proto
void reportHeader(); //proto

void setup(void){
  // start serial port
  Serial.begin(115200);
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
        reportHeader();
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
        cycl[cx++] = cyCt; if (cx>=MXSIZ) {cx=0;}
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
        cycl[cx++] = cyCt; if (cx>=MXSIZ) {cx=0;}
        cyCt = 0;
        state = ST_LOW;
      }
      break;
  }
}

unsigned int cyclToMicSec(unsigned int cyNo){
  return (unsigned long)cyNo * 1000000l / oneSec;
}

unsigned int maxBouncingMics(){
  if (cx==1){
    return cyclToMicSec(mxBouncing);
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
  return cyclToMicSec(mxBouncing);
}

void reportHeader(){
  Serial.println("    +--------- max bouncing time in µSec");
  Serial.println("    |      +-- time in cycles: Hi Lo Hi ...");
  Serial.println("    |      |");
  Serial.println("+---o--+---o-------------");
}
void report(){
  int ct=0;
  unsigned int last=cycl[0];
  char bf[15];
  sprintf(bf, "|%5d | ", maxBouncingMics());
  Serial.print(bf);
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
  Serial.println();
  cx = 0;
}
