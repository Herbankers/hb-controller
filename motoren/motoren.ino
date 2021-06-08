#include <stdio.h>
#include <string.h>
//pwm possible on pins 3,5,6,9,10,11 on arduino uno
#define speedA 3
#define speedB 10
#define motorVijf 2
#define motorTien 4
#define motorTwintig 5


int motorSpeed = 75;
int incomingByte;
String data;


void setup() {
   pinMode(motorVijf, OUTPUT);
   pinMode(motorTien, OUTPUT);
   pinMode(motorTwintig, OUTPUT);
   pinMode(speedA, OUTPUT);
   pinMode(speedB, OUTPUT);
   Serial.begin(9600);
}

void motor(int motor, int amount){
  for(int i = 0; i < amount; i++){
    digitalWrite(motor, HIGH);
    //testen hoelang draaien voor 1 kaart
    delay(2500);
    resetMotoren(motorVijf, motorTien, motorTwintig);
    delay(500);
  }
}
  
void resetMotoren(int motor1, int motor2, int motor3){
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  
  }

void readInput(){
  if(Serial.available() > 0){
    incomingByte = Serial.read();
    data += char(incomingByte);
    if(char(incomingByte) == '\0'){
      Serial.println(data);
      parseInput();
    }
  }
}

void parseInput(){
  //data string is remembered and elements are being removed so can call function with null
  
  String check= strtok(data.c_str(), ",");
  if(check == "M"){
    Serial.println("Motor");
    String motor1 = strtok(NULL, ",");
    String motor2 = strtok(NULL, ",");
    String motor3 = strtok(NULL, ",");

    int motor1Amount = motor1.toInt();
    int motor2Amount = motor2.toInt();
    int motor3Amount = motor3.toInt();

    motor(motorVijf, motor1Amount);
    motor(motorTien, motor2Amount);
    motor(motorTwintig, motor3Amount);
    }
    
  else if(check == "B"){
    Serial.println("Bon");
    char *bedrag = strtok(NULL, ",");
    char *tijd = strtok(NULL, ",");
    char *datum = strtok(NULL, ",");
    char *rekeningNummer = strtok(NULL, ",");
    char *locatie = strtok(NULL, ",");
  }
}



void loop() {
  analogWrite(speedA, motorSpeed);
  analogWrite(speedB, motorSpeed);
  readInput();
  //motor(motorVijf);
  //motor(motorTien);
  //motor(motorTwintig);
  

  
  
  
}
