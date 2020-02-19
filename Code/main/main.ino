#include <SparkFun_TB6612.h> //Motor Surucu Kutuphanesi

#define SENSOR_LIMIT 500 //500 alti beyaz ustu siyah

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


#define lineL_1_pin A5
#define lineL_2_pin A4

#define lineM_1_pin A3
#define lineM_2_pin A2
 
#define lineR_1_pin A1 
#define lineR_2_pin A0    

int lineL_1;  
int lineL_2;  

int lineM_1;  
int lineM_2; 

int lineR_1;  
int lineR_2; 

/////////////////////////
// Cisim Sensoru
////////////////////////

#define cisim_pin 2

int cisim;


void setup(){

  pinMode(lineL_1_pin, INPUT);        
  pinMode(lineL_2_pin, INPUT);
  pinMode(lineM_1_pin, INPUT);        
  pinMode(lineM_2_pin, INPUT);
  pinMode(lineR_1_pin, INPUT);        
  pinMode(lineR_2_pin, INPUT);

  pinMode(cisim_pin, INPUT);
  
  Serial.begin(9600);

}


void loop(){

  
  
  int sensorValues[6];
  double k = 0;
  readSensors(sensorValues);
  k = findK(sensorValues);
  Serial.print("k :");
  Serial.println(k);

  if(k == 0.5 || k == -0.5 || k==0){  //orta sensorler beyazsa veya hepsi beyazsa ileri git
    Serial.println("ileri");
    forward(motorR,motorL,MIDSPEED);
    
  }

  else if(k>0.5){     
    Serial.println("sol");
    int Speed = abs(LOWSPEED*k);
    left(motorR,motorL,Speed);
    
  }

  else if(k<-0.5){
    Serial.println("sag");
    int Speed = abs(LOWSPEED*k);
    right(motorR,motorL,Speed);

  }
  
  else{
    Serial.println("dur");
      brake(motorR,motorL);
  }
 
}

void readSensors(int sensorValues[]){

   
  /*
  lineL_1 = analogRead(lineL_1_pin);
  lineL_2 = analogRead(lineL_2_pin);

  lineM_1 = analogRead(lineM_1_pin);
  lineM_2 = analogRead(lineM_2_pin);
  
  lineR_1 = analogRead(lineR_1_pin);
  lineR_2 = analogRead(lineR_2_pin);
  */

  sensorValues[0] = analogRead(lineL_1_pin);
  sensorValues[1] = analogRead(lineL_2_pin);

  sensorValues[2] = analogRead(lineM_1_pin);
  sensorValues[3] = analogRead(lineM_2_pin);
  
  sensorValues[4] = analogRead(lineR_1_pin);
  sensorValues[5] = analogRead(lineR_2_pin);

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
/*  
  Serial.print("\nCisim Sensoru");
  Serial.print(cisim);
*/
}

double findK(int sensorValues[]){

  int currentWhite = 0;
  int numberOfWhites = 0;

  for(int i=1; i<7; i++){

    if(sensorValues[i-1]<SENSOR_LIMIT){

        numberOfWhites++;
     }
    
  }
  Serial.println(numberOfWhites);
  
  if(numberOfWhites==0){
    return 0.1;
    
  }
  else if(numberOfWhites<3&&numberOfWhites>0){

    for(int i=1; i<7; i++){

     if(sensorValues[i-1]<SENSOR_LIMIT){

        currentWhite = i;
        
     }
    
    }
    
    return currentWhite-3.5;
  }

  else{
    return 0;
  }
  

}


//Stage 2 ye girerken keskin sola dönüşte mal oluyor
//Stage3 ten 4 e giris sırasında serit değiştirme methodu lazım
