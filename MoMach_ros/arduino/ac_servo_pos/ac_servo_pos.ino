const int motorDirPin = 8;
const int motorPWMPin = 9;

const int encoderPinA = 2;
const int encoderPinB = 3;

int encoderPos = 0;
int A = 0;
int B = 0;

void doEncoderA(){
   A = digitalRead(encoderPinA);
   B = digitalRead(encoderPinB);
  if (digitalRead(encoderPinA) == HIGH) { 
    if (digitalRead(encoderPinB) == LOW) encoderPos+= 1;
    else encoderPos-= 1;
  }
  else                            
  { 
    if (digitalRead(encoderPinB) == HIGH) encoderPos+= 1;
    else encoderPos-= 1;
  }
}
void doEncoderB(){
   A = digitalRead(encoderPinA);
   B = digitalRead(encoderPinB);
  if (digitalRead(encoderPinB) == HIGH) { 
    if (digitalRead(encoderPinA) == HIGH) encoderPos+= 1;
    else encoderPos-= 1;
  }
  else                            
  { 
    if (digitalRead(encoderPinA) == LOW) encoderPos+= 1;
    else encoderPos-= 1;
  }
}
void setup() {
  pinMode(motorDirPin, OUTPUT);
  pinMode(motorPWMPin, OUTPUT);
  attachInterrupt(0, doEncoderA, CHANGE);
  attachInterrupt(1, doEncoderB, CHANGE);
  Serial.begin(9600);
}

void loop() {
  for(int i =0; i< 1000; i++){
    digitalWrite(motorDirPin, HIGH);
    analogWrite(motorPWMPin, 50); // dir이 high이면 255-vel, low이면 vel
    Serial.print(" encoderPos HIGH : ");
    Serial.println(encoderPos);
    Serial.print("A : ");
    Serial.println(A);
    Serial.print("B : ");
    Serial.println(B);
    
    delay(1); //ms 
  }
  for(int i =0; i< 1000; i++){
    digitalWrite(motorDirPin, LOW);
    analogWrite(motorPWMPin, 100); // dir이 high이면 255-vel, low이면 vel
    //Serial.print("encoderPos LOW : ");
    Serial.print(" encoderPos low : ");
    Serial.println(encoderPos);
    Serial.print("A : ");
    Serial.println(A);
    Serial.print("B : ");
    Serial.println(B);
    
    delay(1); //ms 
  }
  for(int i =0; i< 5000; i++){
    digitalWrite(motorDirPin, LOW);
    analogWrite(motorPWMPin, 0); // dir이 high이면 255-vel, low이면 vel
    Serial.print(" encoderPos stop : ");
    Serial.println(encoderPos);
    Serial.print("A : ");
    Serial.println(A);
    Serial.print("B : ");
    Serial.println(B);
    
    delay(1); //ms 
  }
}
