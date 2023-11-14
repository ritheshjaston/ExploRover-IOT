//ultrasonic
int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(0);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(6);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance_cm = duration * 0.034 / 2;

  // Serial.print("Distance: ");
  // Serial.print(distance_cm);
  // Serial.println(" cm");
  return distance_cm;
}
//motors control
void moveForward() {
  myservo.write(115);
  digitalWrite(motor1IN1, HIGH);
  digitalWrite(motor1IN2, LOW);
  digitalWrite(motor2IN1, LOW);
  digitalWrite(motor2IN2, HIGH);
 
}
void moveBackward() {
  digitalWrite(motor1IN1, LOW);
  digitalWrite(motor1IN2, HIGH);
  digitalWrite(motor2IN1, HIGH);
  digitalWrite(motor2IN2, LOW);
  
}

void stopMotors() {
  digitalWrite(motor1IN1, LOW);
  digitalWrite(motor1IN2, LOW);
  digitalWrite(motor2IN1, LOW);
  digitalWrite(motor2IN2, LOW);
}

void turnRight() {
  digitalWrite(motor1IN1, HIGH);
  digitalWrite(motor1IN2, LOW);
  digitalWrite(motor2IN1, HIGH);
  digitalWrite(motor2IN2, LOW);
}

void turnLeft() {
  digitalWrite(motor1IN1, LOW);
  digitalWrite(motor1IN2, HIGH); 
  digitalWrite(motor2IN1, LOW);
  digitalWrite(motor2IN2, HIGH);
}

int lefdist(){
  myservo.write(180);
  delay(2000);
  return measureDistance(TRIG_PIN_2, ECHO_PIN_2);
}
int rightdist(){
  myservo.write(50);
  delay(2000);
  return measureDistance(TRIG_PIN_2, ECHO_PIN_2);
}
//motor control close





//=======================================
//handle function: send webpage to client
//=======================================
void webpage()
{
  server.send(200,"text/html", webpageCode);
}
//=====================================================
//function process event: new data received from client
//=====================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t welength)
{
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);

  if(type == WStype_TEXT) //receive text from client
  {
    byte separator=payloadString.indexOf('=');
    String var = payloadString.substring(0,separator);
    Serial.print("var= ");
    Serial.println(var);
    String val = payloadString.substring(separator+1);
    // Serial.print("val= ");
    // Serial.println(val);
    // Serial.println(" ");
   
    //action control
    if(var=="auto"){
      if(autoconfig){
        autoconfig=false;
        stopMotors();
      }else{
        analogWrite(motor2PWM, 80);
        analogWrite(motor1PWM, 80);
        autoconfig=true;
        stopMotors();
      }
    }else if(var == "forward" && moveforward==true)
    {
      moveForward();
    }else if(var == "left"){
      turnLeft();
    }else if(var == "right"){
      turnRight();
    }else if(var == "backward"){
      moveBackward();
    }else if(var == "stop" ){
      stopMotors();
    }
    // motor  speed control
    else if(var == "slow" && autoconfig==false){
      analogWrite(motor2PWM, 100);
      analogWrite(motor1PWM, 100);
    }else if(var == "medium" && autoconfig==false){
      analogWrite(motor2PWM, 170);
      analogWrite(motor1PWM, 170);
    }else if(var == "fast" && autoconfig==false){
      analogWrite(motor2PWM, 255);
      analogWrite(motor1PWM, 255);
    }else{
      degree=var.toInt();
      myservo.write(degree);

    }
  }
}

