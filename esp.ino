#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include "DHTesp.h"
DHTesp dht;
#include<ESP32Servo.h>
Servo myservo;
int servopin=5;
//-----------------------------------------------
const char* ssid = "T";
const char* password = "77951130";

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const int motor1IN1 = 14;  // replace with your GPIO pin
const int motor1IN2 = 27;  // replace with your GPIO pin
const int motor1PWM = 12;  // replace with your GPIO pin

const int motor2IN1 = 26;  // replace with your GPIO pin
const int motor2IN2 = 25;  // replace with your GPIO pin
const int motor2PWM = 33;  // replace with your GPIO pin
//-----------------------------------------------
//ultrasonic Sensors
#define TRIG_PIN_1 2
#define ECHO_PIN_1 4
#define TRIG_PIN_2 16
#define ECHO_PIN_2 17
//-----------------------------------------------
#define MQ135_DOUT_PIN 15
String Air_quality="Good";
String message="Hello Sir!";
// ---------------------------------------------
#define ldrPin 19
String don="";
//-----------------------------------------------
//auto configure
bool autoconfig=false;
//-----------------------------------------
// triggerLOOpcontrol
int trig_count=0;
//-----------------------------------------------
bool moveforward=true;
int degree=0;
//-----------------------------------------------
// #define LED 0
//-----------------------------------------------
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
//-----------------------------------------------
boolean LEDonoff=false; 
boolean stopper=false; 

String JSONtxt;
//-----------------------------------------------
#include "html_page.h"
#include "functions.h"
//====================================================================
void setup()
{
  myservo.attach(servopin);
  dht.setup(0, DHTesp::DHT11);
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
  Serial.begin(115200); 
  pinMode(MQ135_DOUT_PIN, INPUT);
  pinMode(ldrPin, INPUT);
  //-----------------------------------------------
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){Serial.print("."); delay(500);}
  WiFi.mode(WIFI_STA);
  Serial.println();
  MDNS.begin("esp32");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  //-----------------------------------------------
  server.on("/", webpage);
  //-----------------------------------------------
  server.begin(); webSocket.begin();
  webSocket.onEvent(webSocketEvent);
//--------------------------------------------------
  //Motor pin
  pinMode(motor1IN1, OUTPUT);
  pinMode(motor1IN2, OUTPUT);
  pinMode(motor1PWM, OUTPUT);  //ENA

  pinMode(motor2IN1, OUTPUT);
  pinMode(motor2IN2, OUTPUT);
  pinMode(motor2PWM, OUTPUT);  //ENB
//-----------------------------------------------
}

//====================================================================
void loop()
{
  webSocket.loop(); server.handleClient();
  trig_count=trig_count+1;

  if(autoconfig){
    message="AUTO MODE";
      if(measureDistance(TRIG_PIN_1, ECHO_PIN_1)>6 || measureDistance(TRIG_PIN_2, ECHO_PIN_2)<15){
        stopMotors();
        moveBackward();
        delay(500);
        stopMotors();
        delay(100);
        if(lefdist()>rightdist()){
          myservo.write(115);
          turnLeft();
          delay(500);
          stopMotors();
        }else{
          myservo.write(115);
          turnRight();
          delay(500);
          stopMotors();
        }
      }else{
        moveForward();
      }
  }else{
    //ultrasonic sensor  
    if(measureDistance(TRIG_PIN_1, ECHO_PIN_1)>6){
      message="OBSTACLE DETECTED AT GROUND";
      moveforward=false;
      if (stopper == false){
          stopMotors();
          stopper = true;
      }

    }else if(measureDistance(TRIG_PIN_2, ECHO_PIN_2)<15){
      message="OBSTACLE DETECTED AT FRONT";
      moveforward=false;
      if (stopper == false){
          stopMotors();
          stopper = true;
      }    
    }else{
      stopper=false;
      message="EXPLOROVER";
      moveforward=true;
    }
  }
  int airQuality = digitalRead(MQ135_DOUT_PIN);
  
  if (airQuality == HIGH) {
    // Serial.println("Air quality is good");
    Air_quality="GOOD";
  } else {
    // Serial.println("Air quality is poor");
    Air_quality="BAD";
  }
  
    // trig_count=0;
    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    int mgf=hallRead();
    int ldrValue = digitalRead(ldrPin);
    if(ldrValue){
        don="NIGHT";
    }else
    {
      don="DAY";
    }
    JSONtxt = "{\"temperature\":\""+ String(temperature) + "\", \"humidity\":\"" + String(humidity) + "\", \"msg\":\"" + message +"\", \"mgf\":\"" + String(mgf) +"\", \"air\":\"" + Air_quality +"\", \"don\":\"" + don + "\"}";
    webSocket.broadcastTXT(JSONtxt);
  
  




}