#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "NewPingESP8266.h"

// Definição dos pinos para motores
#define MOTOR_1_PIN_1 5
#define MOTOR_1_PIN_2 4
#define MOTOR_2_PIN_1 0
#define MOTOR_2_PIN_2 2
#define ENABLE_PIN 14  
#define ECHO_PIN 13
#define TRIGGER_PIN 12
#define FILTER_SIZE 5
#define LED_PIN 16
#define BUZZER_SIGNAL 15


int distanceReadings[FILTER_SIZE] = {0};
int indexSensor = 0;


const char* ssid = "starsegescritorio";  // Nome da rede WiFi
const char* password = "esc242369";      // Senha da rede WiFi

// Configurações do MQTT
const char* mqtt_server = "192.168.3.137";
const int mqtt_port = 8883;  // Porta padrão MQTT (ajustada para ESP8266) 8883 --> 1883
const char* mqtt_user = "starseg";
const char* mqtt_password = "STARseg242369";

// Instâncias do WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Duty Cycle para PWM
int dutyCycle = 0;

// sonar instance
NewPingESP8266 sonar(TRIGGER_PIN, ECHO_PIN, 100);
int MAX_DISTANCE = 20;

unsigned long pingTimer;
unsigned long pingSpeed = 50;
bool canStop = true;  // booleano para controlar para que o carrinho pare apenas uma vez quando chega na distancia maxima
int distance = 0;


// Funções para controle do robô
void moveRobot(String message) {

  if (distance < MAX_DISTANCE && distance > 0 && message != "backward" && message != "stop") {
    Serial.println("Robô bloqueado, por favor ande para trás");
    return;
  }

  if (message == "stop") {
    Serial.println("Stopping");
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 0);
    return;
  }

  if (message == "forward") {
    Serial.println("Moving Forward");
    digitalWrite(MOTOR_1_PIN_1, 1);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 1);
    return;
  }

  if (message == "left") {
    Serial.println("Moving Left");
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 1);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 1);
    return;
  }

  if (message == "right") {
    Serial.println("Moving Right");
    digitalWrite(MOTOR_1_PIN_1, 1);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 1);
    digitalWrite(MOTOR_2_PIN_2, 0);
    return;
  }

  if (message == "backward") {
    Serial.println("Moving Backward");
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 1);
    digitalWrite(MOTOR_2_PIN_1, 1);
    digitalWrite(MOTOR_2_PIN_2, 0);
    return;
  }

  return;
}

void changeSpeed(String message) {
  Serial.println("Changing speed to: " + message);
  int value = message.toInt();

  if (value == 0) {
    analogWrite(ENABLE_PIN, 0);
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 0);
  } else {
    dutyCycle = map(value, 10, 100, 200, 255);  // Ajusta o PWM para o intervalo do ESP8266
    analogWriteFreq(20000);
    analogWrite(ENABLE_PIN, dutyCycle);
  }
}

int getFilteredDistance() {
  distanceReadings[indexSensor] = sonar.ping_cm();
  indexSensor = (indexSensor + 1) % FILTER_SIZE;

  int sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += distanceReadings[i];
  }
  return sum / FILTER_SIZE;
}

void handleStopCondition() {
  if (distance < MAX_DISTANCE && distance > 0) {
    if (canStop) {
      moveRobot("stop");
      canStop = 0;
    }
  } else {
    Serial.println(distance);
    canStop = 1;
  }
}

// void setColor(int red, int green, int blue) {
//   analogWrite(RED_LED, red);
//   analogWrite(GREEN_LED, green);
//   analogWrite(BLUE_LED, blue);
// }

void setLed(String message) {
  analogWriteFreq(40000);
  if(message == "false") {
    Serial.println("Acendendo LED");
    analogWrite(LED_PIN, 255);
  } else {
    analogWrite(LED_PIN, 0);
  }
}

void honk(String message) {
  Serial.println(message);
  if(message == "true") {
    tone(BUZZER_SIGNAL, 1000);
  } else {
    noTone(BUZZER_SIGNAL);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (!strcmp(topic, "robot/move")) {
    moveRobot(message);
  } else if (!strcmp(topic, "robot/speed")) {
    changeSpeed(message);
  } 
  else if (!strcmp(topic, "robot/led")) {
    setLed(message);
  } 
  else if (!strcmp(topic, "robot/honk")) {
    honk(message);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      client.subscribe("robot/move");
      client.subscribe("robot/speed");
      client.subscribe("robot/honk");
      client.subscribe("robot/led");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);

  // Configura os pinos do motor como saída
  pinMode(MOTOR_1_PIN_1, OUTPUT);
  pinMode(MOTOR_1_PIN_2, OUTPUT);
  pinMode(MOTOR_2_PIN_1, OUTPUT);
  pinMode(MOTOR_2_PIN_2, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  // Configura o sensor ultrassônico
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // LED
  pinMode(LED_PIN, OUTPUT);
  // // BUZZER
  pinMode(BUZZER_SIGNAL, OUTPUT);

  // Conecta ao WiFi
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi!");

  // Configura MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();

  pingTimer = millis();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() >= pingTimer) {
    pingTimer += pingSpeed;

    distance = getFilteredDistance();

    handleStopCondition();

    // Serial.print("Distância: ");
    // Serial.print(distance);
    // Serial.println(" cm");
  }
}
