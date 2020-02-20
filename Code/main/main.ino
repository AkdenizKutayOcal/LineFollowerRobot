#include <SparkFun_TB6612.h> //Motor Surucu Kutuphanesi

int SENSOR_LIMIT = 0; 

//////////////////////////// 
// Motor Surucu Pinleri
///////////////////////////
 
#define AIN1 4
#define BIN1 7
#define AIN2 5
#define BIN2 8
#define PWMA 3 
#define PWMB 9
#define STBY 6
#define LOWSPEED 70
#define MIDSPEED 150
#define MAXSPEED 255
#define DELAY 500


const int offsetA = 1;
const int offsetB = 1;

///////////////////////////
// Sag ve Sol Motorlar 
//////////////////////////

Motor motorR = Motor(AIN1, AIN2, PWMA, offsetA, STBY); //sag motor
Motor motorL = Motor(BIN2, BIN1, PWMB, offsetB, STBY);  //sol motor


/////////////////////////
// Cizgi Sensorleri
////////////////////////


#define lineL_1_pin A7
#define lineL_2_pin A0
#define lineL_3_pin A1

#define lineM_1_pin A2
#define lineM_2_pin A3

#define lineR_1_pin A4 
#define lineR_2_pin A5  
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
    int sensorValues[8];
  double k = 0;
  
  readSensors(sensorValues);
  k = findK(sensorValues);
  
  if(k == 1 || k == -1 || k==0){  //orta sensorler beyazsa veya hepsi beyazsa ileri git
    Serial.println("ileri");
    forward(motorR,motorL,MIDSPEED);
    
  }

  else if(k>1){     
    Serial.println("sag");
    int Speed = abs(LOWSPEED*k);
    right(motorR,motorL,Speed);
    
  }

  else if(k<-1){
    Serial.println("sol");
    int Speed = abs(LOWSPEED*k);
    left(motorR,motorL,Speed);

  }
  
  else{//hepsi siyah
    
    Serial.println("dur");
    brake(motorR,motorL);
  }
    
    if(butonDurumu==LOW)
    {
      b=0;
    }
  }
  
   else
  {
    Serial.println("button stop");
    
    if(butonDurumu==LOW)
    {
      b=0;
    }
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
  sensorValues[7] = analogRead(lineR_2_pin);

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
  Serial.print(sensorValues[7]);
  
/*  
  Serial.print("\nCisim Sensoru");
  Serial.print(cisim);
*/
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

      if(sensorValues[0]>SENSOR_LIMIT && sensorValues[7]<SENSOR_LIMIT){

        k = -3.5;
        
      }
  
      else if(sensorValues[0]<SENSOR_LIMIT && sensorValues[7]>SENSOR_LIMIT){

         k = 3.5;     
      }

      else{

        int centerOfWhites = totalOfWhites / numberOfWhites;
        k = centerOfWhites - 4.5; 
      }
    }
  }

  else{//hepsi beyaz

    k=0;
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

  SENSOR_LIMIT = (minimum+maximum)/2;
}

//Stage 2 ye girerken keskin sola dönüşte mal oluyor
//Stage3 ten 4 e giris sırasında serit değiştirme methodu lazım
