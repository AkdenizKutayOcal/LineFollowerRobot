
int SENSOR_LIMIT = 600; 
double c =0;
int stage = 1;
int countAllWhites = 1;
int sensorValues[8];

//////////////////////////// 
// Motor Surucu Pinleri
///////////////////////////
 
#define AIN1 4
#define BIN1 8
#define AIN2 5
#define BIN2 7
#define PWMA 3 
#define PWMB 9
#define STBY 6
#define LOWSPEED 60
#define MIDSPEED 60
#define MAXSPEED 205
#define DELAY 500


const int offsetA = 1;
const int offsetB = 1;

/////////////////////////
// Cizgi Sensorleri
////////////////////////


#define lineL_1_pin A7
#define lineL_2_pin A5
#define lineL_3_pin A4

#define lineM_1_pin A3
#define lineM_2_pin A2

#define lineR_1_pin A1 
#define lineR_2_pin A0  
#define lineR_3_pin A6  

int lineL_1;  
int lineL_2;  
int lineL_3; 

int lineM_1;  
int lineM_2; 

int lineR_1;  
int lineR_2; 
int lineR_3;

/////////////////////////
// Cisim Sensoru
////////////////////////

#define cisim_pin 2

int cisim;

/////////////////////////
// Button
////////////////////////

boolean button= false;
int butonDurumu=0;
int b=0;
int state=0;
boolean calibrasyon = false;
#define button_pin 12
 int minimum = 1000;
    int maximum = 0;


void setup(){

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  pinMode(lineL_1_pin, INPUT);        
  pinMode(lineL_2_pin, INPUT);
  pinMode(lineL_3_pin, INPUT);
  pinMode(lineM_1_pin, INPUT);        
  pinMode(lineM_2_pin, INPUT);
  pinMode(lineR_1_pin, INPUT);        
  pinMode(lineR_2_pin, INPUT);
  pinMode(lineR_3_pin, INPUT);

  pinMode(cisim_pin, INPUT);

  pinMode(button_pin, INPUT);
  
  Serial.begin(9600);

}


void loop(){
  
  digitalWrite(STBY,HIGH);
  butonDurumu= digitalRead(button_pin);
 
  
    if(butonDurumu==HIGH)
    {
      for(b;b<1;b++)
      {
      if(button==false)
      {
        button= true;
        state++;
      }
      else
      {
        button= false;
      }
    }
  }

  if(button==true && state==1)
  {
    Serial.println("Kalibrasyon");
   

    calibration();
    Serial.println(SENSOR_LIMIT);
    
    if(butonDurumu==LOW)
    {
      b=0;
    }
  }

  else if(button==true&& state>=2)
  {
    Serial.println("kod");
    readSensors(sensorValues);
    
    if(cisim==0 && stage==3){

      while(stage==3){

        if(digitalRead(cisim_pin)==0 && ((analogRead(lineM_1_pin)>SENSOR_LIMIT)||(analogRead(lineM_2_pin)>SENSOR_LIMIT))){
          
          brake();
        }
        else if(digitalRead(cisim_pin)==1 && ((analogRead(lineM_1_pin)>SENSOR_LIMIT)||(analogRead(lineM_2_pin)>SENSOR_LIMIT))){
          forward(MIDSPEED);
          delay(50);
          brake();
        }
        else if(digitalRead(cisim_pin)==0 && ((analogRead(lineM_1_pin)<SENSOR_LIMIT)||(analogRead(lineM_2_pin)<SENSOR_LIMIT))){
          brake();
        }
        else{
          seritDegistirSol();
          while(countAllWhites !=5){
            lineFollow();
          }

          seritDegistirSag();
          stage++;
        }
        
      }
      
    }

    else if(cisim==0&& stage==2){
       
      while(digitalRead(cisim_pin)==0){
        brake();
      }

      stage++;
      
    }
    
    else if(cisim==0 && stage==1){
      Serial.println("Serit değiştir");

      seritDegistirSol();
      
      stage++;
      
    }

    else{

      lineFollow();
    }
    
        
    if(butonDurumu==LOW)
    {
      b=0;
    }
  }
  
   else
  {
    Serial.println("button stop");
    brake();
    
    if(butonDurumu==LOW)
    {
      b=0;
    }
  }
 
}

void lineFollow(){

  
  double k = 0;
  
  readSensors(sensorValues);
  k = findK(sensorValues);

  
  if(k == 1 || k == -1 || k==0 || k==0.5 || k==-0.5 ){  //orta sensorler beyazsa veya hepsi beyazsa ileri git
    Serial.println("ileri");
    forward(MIDSPEED);
    
  }

  else if(k>1){     
    Serial.println("sag");
    k = abs(k);
    int i=0;
    while(analogRead(lineM_2_pin)<SENSOR_LIMIT&&analogRead(lineM_1_pin)<SENSOR_LIMIT){

      left(LOWSPEED,k);
      if(k<5)
      k += 0.3;
    }
    
    
  }

  else if(k<-1){
    Serial.println("sol");
    k = abs(k);
    int i=0;
    while(analogRead(lineM_2_pin)<SENSOR_LIMIT&&analogRead(lineM_1_pin)<SENSOR_LIMIT){
       
      right(LOWSPEED,k);
      if(k<5)
      k += 0.3;
}
  }
  /* else if(k==0.2){
     Serial.println("saga keskin");
     l90();
   }

   else if(k==-0.2){
     Serial.println("sola keskin");
     r90();
   }

  */
  
  else{//hepsi siyah

    if(countAllWhites>=5){
      forward(20);
      delay(100);
      brake();  
    }
    
    Serial.println("dur");
    brake();
  } 
}

void readSensors(int sensorValues[]){

  sensorValues[0] = analogRead(lineL_1_pin);
  sensorValues[1] = analogRead(lineL_2_pin);
  sensorValues[2] = analogRead(lineL_3_pin);

  sensorValues[3] = analogRead(lineM_1_pin);
  sensorValues[4] = analogRead(lineM_2_pin);
  
  sensorValues[5] = analogRead(lineR_1_pin);
  sensorValues[6] = analogRead(lineR_2_pin);
  sensorValues[7] = analogRead(lineR_3_pin);

  cisim = digitalRead(cisim_pin);

  
  Serial.print("Line Sensor Values: ");
  Serial.print(sensorValues[0]);
  Serial.print(' ');
  Serial.print(sensorValues[1]);
  Serial.print(' ');
  Serial.print(sensorValues[2]);
  Serial.print(' ');
  Serial.print(sensorValues[3]);
  Serial.print(' ');
  Serial.print(sensorValues[4]);
  Serial.print(' ');
  Serial.print(sensorValues[5]);
  Serial.print(' ');
  Serial.print(sensorValues[6]);
  Serial.print(' ');
  Serial.println(sensorValues[7]);
  
 
  Serial.print("Cisim Sensoru ");
  Serial.println(cisim);

}

double findK(int sensorValues[]){

  double k = 0.0;
  int numberOfWhites = 0;
  int totalOfWhites = 0;
  int indexOfLastWhite = 0;

  for(int i=1; i<9; i++){

    if(sensorValues[i-1]>SENSOR_LIMIT){

        numberOfWhites++;
        totalOfWhites += i;
        indexOfLastWhite = i;
     }
    
  }
  
  if(numberOfWhites==0){  
    k = 0.1;
    
  }
  
  else if(numberOfWhites >0 && numberOfWhites<3){  

    int centerOfWhites = totalOfWhites / numberOfWhites;
    k = centerOfWhites - 4.5;  
    
  }

  else if(numberOfWhites >3 && numberOfWhites<8){
    
    boolean consecative = true;
    
    //eger aralik varsa en son beyaza gore islem yap

    //eger aralik yoksa, sagda mi solda mi belirle
    //onlarin da en uc degerlerine gore islem yap

    
    for(int i=indexOfLastWhite; i>0 ; i--){

      if(sensorValues[i-1]<SENSOR_LIMIT){
        consecative = false;
        break;  
      }
    }

    if(!consecative){

      k = indexOfLastWhite-4.5;
    }

    else{

      if(sensorValues[0]>SENSOR_LIMIT && sensorValues[7]<SENSOR_LIMIT){  //instead of sending -3.5 or 3.5 send special 90 degree turn values 

        //k = -0.2;
        k = -4.5;
        
      }
  
      else if(sensorValues[0]<SENSOR_LIMIT && sensorValues[7]>SENSOR_LIMIT){

         //k = 0.2;  
         k = 4.5;   
      }

      else{

        int centerOfWhites = totalOfWhites / numberOfWhites;
        k = centerOfWhites - 4.5; 
      }
    }
  }

  else{//hepsi beyaz

    k=0;
    countAllWhites++;
  }

  return k;

}

void calibration(){

  int tempSensorValues[8];

  readSensors(tempSensorValues);
  
  for(int i=0; i<8; i++){

    if(tempSensorValues[i]<minimum){
      minimum = tempSensorValues[i];
    }

    else if(tempSensorValues[i]>maximum){
      maximum = tempSensorValues[i];
    }
  }

  SENSOR_LIMIT = 3*(minimum+maximum)/5;
}

void forward(int speedValue){
  
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  analogWrite(PWMA, speedValue);
  
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  analogWrite(PWMB, speedValue);
  
  }

void brake(){

  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,HIGH);
  analogWrite(PWMA, 0);
  
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,HIGH);
  analogWrite(PWMB, 0);

}

void left(int speedValue,double k){

  int speedL = speedValue/k;
  int speedR = speedValue;
  
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  analogWrite(PWMA, speedL);
  
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  analogWrite(PWMB, speedR);
  
}


void right(int speedValue, double k){

  int speedR = speedValue/k;
  int speedL = speedValue;
  
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  analogWrite(PWMA, speedL);
  
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  analogWrite(PWMB, speedR);
  
}

void seritDegistirSol(){

  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  analogWrite(PWMA, 100);
  
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  analogWrite(PWMB, 50);

  delay(500);

  while(!(analogRead(lineR_2_pin)>SENSOR_LIMIT )){
    Serial.println("go forward");
    digitalWrite(AIN1,HIGH);
    digitalWrite(AIN2,LOW);
    analogWrite(PWMA, 50);
  
    digitalWrite(BIN1,HIGH);
    digitalWrite(BIN2,LOW);
    analogWrite(PWMB, 50);
    
  }
}

void seritDegistirSag(){

  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  analogWrite(PWMA, 0);
  
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  analogWrite(PWMB, 40);

  delay(500);

  while(!(analogRead(lineM_1_pin)>SENSOR_LIMIT || analogRead(lineM_2_pin)>SENSOR_LIMIT)){

    digitalWrite(AIN1,HIGH);
    digitalWrite(AIN2,LOW);
    analogWrite(PWMA, 50);
  
    digitalWrite(BIN1,HIGH);
    digitalWrite(BIN2,LOW);
    analogWrite(PWMB, 50);
    
  }

}

void r90(){

  do{

    digitalWrite(AIN1,HIGH);
    digitalWrite(AIN2,LOW);
    analogWrite(PWMA, MAXSPEED);
  
    digitalWrite(BIN1,HIGH);
    digitalWrite(BIN2,LOW);
    analogWrite(PWMB, -MAXSPEED);
    
  }while(!(analogRead(lineR_3_pin)<SENSOR_LIMIT) && (analogRead(lineR_2_pin)<SENSOR_LIMIT) && ((analogRead(lineM_1_pin)>SENSOR_LIMIT) || (analogRead(lineM_2_pin)>SENSOR_LIMIT)));

}

void l90(){

  do{

    digitalWrite(AIN1,HIGH);
    digitalWrite(AIN2,LOW);
    analogWrite(PWMA, -MAXSPEED);
  
    digitalWrite(BIN1,HIGH);
    digitalWrite(BIN2,LOW);
    analogWrite(PWMB, MAXSPEED);

  }while(!(analogRead(lineL_1_pin)<SENSOR_LIMIT) && (analogRead(lineL_2_pin)<SENSOR_LIMIT) && ((analogRead(lineM_1_pin)>SENSOR_LIMIT) || (analogRead(lineM_2_pin)>SENSOR_LIMIT)));

}
