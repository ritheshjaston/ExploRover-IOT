#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include "DHTesp.h"
#include <HTTPClient.h>
#include <ESP32Servo.h>
#define buzer 13
#define led3 5

DHTesp dht;
Servo myservo;
int servopin = 32;


//-----------------------------------------------
const char* ssid = "Explo";
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
// #define MQ135_DOUT_PIN 15
#define MQ135_ANALOG_PIN 34

String Air_quality = "Good";
String message = "Hello Sir!";
// ---------------------------------------------
#define ldrPin 19
String don = "";

//-----------------------------------------------
//auto configure
bool autoconfig = false;
//-----------------------------------------
// triggerLOOpcontrol
int trig_count = 0;
//-----------------------------------------------
bool moveforward = true;
int degree = 0;
//-----------------------------------------------
// #define LED 0
//-----------------------------------------------
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
//-----------------------------------------------
boolean LEDonoff = false;
boolean stopper = false;
String JSONtxt;
//-----------------------------------------------
#include "html_page.h"
#include "functions.h"
//----------------------------------------------
const char* url = "https://script.google.com/macros/s/AKfycbwkW8k-8zRpOODwVbSsmTJzMYVbGFPKMjIBXiEcZwUvViHBv9w66MZ8uaVEK35bCgrb/exec";
int cou = 0;
int airQuality;
float temperature;
float humidity;
int mgf;
int ldrValue;
//====================================================================
void setup() {
  pinMode(led3, OUTPUT);
  digitalWrite(led3, LOW);
  myservo.attach(servopin);
  dht.setup(0, DHTesp::DHT11);
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
  Serial.begin(115200);
  // pinMode(MQ135_DOUT_PIN, INPUT);
  pinMode(MQ135_ANALOG_PIN, INPUT);

  pinMode(ldrPin, INPUT);
  pinMode(buzer, OUTPUT);
  //-----------------------------------------------
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    buzzer();
  }
  WiFi.mode(WIFI_STA);
  Serial.println();
  MDNS.begin("esp32");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  //-----------------------------------------------
  server.on("/", webpage);
  //-----------------------------------------------
  server.begin();
  webSocket.begin();
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
void loop() {
  webSocket.loop();
  server.handleClient();
  trig_count = trig_count + 1;

  if (autoconfig) {
    message = "AUTO MODE";
    if (measureDistance(TRIG_PIN_1, ECHO_PIN_1) > 6 || measureDistance(TRIG_PIN_2, ECHO_PIN_2) < 15) {
      stopMotors();
      moveBackward();
      delay(500);
      stopMotors();
      buzzer();
      analogWrite(motor2PWM, 140);
      analogWrite(motor1PWM, 140);
      if (lefdist() > rightdist()) {
        myservo.write(115);
        turnLeft();
        delay(500);
        stopMotors();
      } else {
        myservo.write(115);
        turnRight();
        delay(500);
        stopMotors();
      }
      analogWrite(motor2PWM, 80);
      analogWrite(motor1PWM, 80);
    } else {
      moveForward();
    }
  } else {
    //ultrasonic sensor
    if (measureDistance(TRIG_PIN_1, ECHO_PIN_1) > 6) {
      moveforward = false;
      message = "OBSTACLE DETECTED AT GROUND";
      JSONtxt = "{\"temperature\":\"" + String(temperature) + "\", \"humidity\":\"" + String(humidity) + "\", \"msg\":\"" + message + "\", \"mgf\":\"" + String(mgf) + "\", \"air\":\"" + Air_quality + "\", \"don\":\"" + don + "\"}";
      webSocket.broadcastTXT(JSONtxt);
      if (stopper == false) {
        stopMotors();
        stopper = true;
        buzzer();
      }

    } else if (measureDistance(TRIG_PIN_2, ECHO_PIN_2) < 20) {
      moveforward = false;
      message = "OBSTACLE DETECTED AT FRONT";
      JSONtxt = "{\"temperature\":\"" + String(temperature) + "\", \"humidity\":\"" + String(humidity) + "\", \"msg\":\"" + message + "\", \"mgf\":\"" + String(mgf) + "\", \"air\":\"" + Air_quality + "\", \"don\":\"" + don + "\"}";
      webSocket.broadcastTXT(JSONtxt);
      if (stopper == false) {
        stopMotors();
        stopper = true;
        buzzer();
      }
    } else {
      stopper = false;
      message = "ROBO";
      moveforward = true;
    }
  }
  // airQuality = digitalRead(MQ135_DOUT_PIN);
 airQuality = analogRead(MQ135_ANALOG_PIN);
 Serial.println(airQuality);
  // if (airQuality == HIGH) {
  //   digitalWrite(buzer, LOW);
  //   // Serial.println("Air quality is good");
  //   Air_quality = "GOOD";
  // } else {
  //   digitalWrite(buzer, HIGH);
  //   // Serial.println("Air quality is poor");
  //   Air_quality = "BAD";
  // }
  if (airQuality > 400) { // Adjust someThreshold based on your sensor and requirements
    // digitalWrite(buzer, LOW);
    Air_quality = "GOOD";
  } else {
    // digitalWrite(buzer, HIGH);
    Air_quality = "BAD";
  }

  // trig_count=0;
  temperature = dht.getTemperature();
  humidity = dht.getHumidity();
  mgf = hallRead();
  ldrValue = digitalRead(ldrPin);
  if (ldrValue) {
    don = "NIGHT";
  } else {
    don = "DAY";
  }
  JSONtxt = "{\"temperature\":\"" + String(temperature) + "\", \"humidity\":\"" + String(humidity) + "\", \"msg\":\"" + message + "\", \"mgf\":\"" + String(mgf) + "\", \"air\":\"" + Air_quality + "\", \"don\":\"" + don + "\"}";
  webSocket.broadcastTXT(JSONtxt);
  cou++;
  if (cou == 200) {
    stopMotors();
    JSONtxt = "{\"temperature\":\"" + String(temperature) + "\", \"humidity\":\"" + String(humidity) + "\", \"msg\":\"" + "Data Analysed Now Sending to the cloud DB" + "\", \"mgf\":\"" + String(mgf) + "\", \"air\":\"" + Air_quality + "\", \"don\":\"" + don + "\"}";
    webSocket.broadcastTXT(JSONtxt);
    cou = 0;
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "data=" + String(temperature) + ":" + String(humidity) + ":" + String(mgf) + ":" + airQuality + ":" + don;
    digitalWrite(led3, HIGH);
    Serial.print(http.POST(postData));
    http.end();
    digitalWrite(led3, LOW);
  }
}
