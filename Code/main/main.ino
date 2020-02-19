#include <SparkFun_TB6612.h> //Motor Surucu Kutuphanesi

#define SENSOR_LIMIT 500 //500 alti beyaz ustu siyah

//////////////////////////// 
// Motor Surucu Pinleri
///////////////////////////
 
#define AIN1 
#define BIN1 
#define AIN2 
#define BIN2 
#define PWMA 
#define PWMB 
#define STBY 
#define LOWSPEED 70
#define MIDSPEED 150
#define MAXSPEED 255
#define DELAY 
#define BACKSPIN 

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


#define lineL_1_pin    
#define lineL_2_pin

#define lineM_1_pin    
#define lineM_2_pin 
 
#define lineR_1_pin    
#define lineR_2_pin      

int lineL_1;  
int lineL_2;  

int lineM_1;  
int lineM_2; 

int lineR_1;  
int lineR_2; 

/////////////////////////
// Cisim Sensoru
////////////////////////

#define cisim_pin

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

  
  int k = 0;
  int sensorValues[6];

  readSensors(sensorValues);
  k = findK(sensorValues);

  if(k <= 0.5||k >= -0.5){

    forward(motorR,motorL,MIDSPEED);
    
  }

  else if(k>0.5){

    left(motorR,motorL,LOWSPEED*k);
    
  }

  else if(k<-0.5){

    right(motorR,motorL,LOWSPEED*k);

  }

  else{

      brake(motorR,motorL);
  }
 
}

void readSensors(int sensorValues[]){

  int sensorValues [6];
   
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
  Serial.print(lineL_1 + lineL_2 + lineM_1 + lineM_2 + lineR_1 + lineR_2);
  Serial.print("\nCisim Sensoru");
  Serial.print(cisim);

}

double findK(int sensorValues[]){

  int currentWhite = 0;
  
  for(int i=1; i<7; i++){

     if(sensorValues[i-1]<SENSOR_LIMIT){

        currentWhite = i;
        break;
     }
      
  }

  return currentWhite-3.5;

}
