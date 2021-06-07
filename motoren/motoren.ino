
//pwm possible on pins 3,5,6,9,10,11 on arduino uno
#define speedA 9
#define speedB 10
#define motorVijf 3
#define motorTien 4
#define motorTwintig 5

int motorSpeed = 75;


void setup() {
   pinMode(motorVijf, OUTPUT);
   pinMode(motorTien, OUTPUT);
   pinMode(motorTwintig, OUTPUT);
   pinMode(speedA, OUTPUT);
   pinMode(speedB, OUTPUT);
   
   Serial.begin(9600);
}

void motor(int motor){
  digitalWrite(motor, HIGH);
  //testen hoelang draaien voor 1 kaart
  delay(2500);
  Serial.println(motor);
  resetMotoren(motorVijf, motorTien, motorTwintig);
}
  
void resetMotoren(int motor1, int motor2, int motor3){
  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);
  
  }

void loop() {
  analogWrite(speedA, motorSpeed);
  analogWrite(speedB, motorSpeed);
  motor(motorVijf);
  delay(100);
  motor(motorTien);
  delay(100);
  motor(motorTwintig);
  delay(100);
  

  
  
  
}
