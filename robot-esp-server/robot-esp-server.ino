#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include "img_converters.h"
#include <HTTPClient.h>

#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
  #define RED_LED_PIN       33
  #define LED_PIN            4
#else
  #error "Camera model not selected"
#endif


#define MOTOR_1_PIN_1    12
#define MOTOR_1_PIN_2    13
#define MOTOR_2_PIN_1    15
#define MOTOR_2_PIN_2    14
#define enablePin  2

// Setting PWM properties
const int freq = 30000;
const int reso = 8;
int dutyCycle = 0;

const char* ssid = "starsegescritorio";  // Nome da rede WiFi
const char* password = "esc242369";  // Senha da rede WiFi

// Configurações do MQTT
const char* mqtt_server = "192.168.3.137";
const int mqtt_port = 8883;
const char* mqtt_user = "starseg";
const char* mqtt_password = "STARseg242369";


// Configuração dos tópicos
// const char* mqtt_streaming = "robot/streaming";

// const char* mqtt_cert = R"(
// -----BEGIN CERTIFICATE-----
// MIIFCzCCA/OgAwIBAgISBCtYGv7FR2wkunFzzlcW/S0UMA0GCSqGSIb3DQEBCwUA
// MDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQwwCgYDVQQD
// EwNSMTEwHhcNMjQxMDI0MjM0NDE1WhcNMjUwMTIyMjM0NDE0WjAfMR0wGwYDVQQD
// DBQqLnMxLmV1LmhpdmVtcS5jbG91ZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC
// AQoCggEBAKVuz2sMPmxx2w/f81/YAEKTbNZMJPk2+ooLFg5hxXvReF+AwIT4XvZ+
// MLhSKvFxmghJF+BB9WyhqrcJLGDCP4s6SOLWTYixEoTcaLUviqqn+06kYqDJ6E83
// NGsc7T42DlPnzqcZZjPRed9rt4CP3RgeZlWyYZgiD8FoJG9gie8ytihF/FkGZT8T
// N4Vkl2vQa3mfBWeeKrcuhcLPxqIWDz/30iYfLtEe5JYYScoCKTXcP9SUStjpR8pD
// vfOWdvasOAuBy7yBbx01/4lcQt50hfbhTR/K14/D4rNkuuvU7ktSQnoxVXC8YDwG
// zkny10DFt65mVYLNZcBQtOLHHOZGV30CAwEAAaOCAiswggInMA4GA1UdDwEB/wQE
// AwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIw
// ADAdBgNVHQ4EFgQUgsEjDU35+EWJKBsFxJ0lM0PXMi4wHwYDVR0jBBgwFoAUxc9G
// pOr0w8B6bJXELbBeki8m47kwVwYIKwYBBQUHAQEESzBJMCIGCCsGAQUFBzABhhZo
// dHRwOi8vcjExLm8ubGVuY3Iub3JnMCMGCCsGAQUFBzAChhdodHRwOi8vcjExLmku
// bGVuY3Iub3JnLzAzBgNVHREELDAqghQqLnMxLmV1LmhpdmVtcS5jbG91ZIISczEu
// ZXUuaGl2ZW1xLmNsb3VkMBMGA1UdIAQMMAowCAYGZ4EMAQIBMIIBAwYKKwYBBAHW
// eQIEAgSB9ASB8QDvAHYAzxFW7tUufK/zh1vZaS6b6RpxZ0qwF+ysAdJbd87MOwgA
// AAGSwSAw7AAABAMARzBFAiAwAiRSrhzlH221ZTJtp3j4/+gIDD94ERv5rj/1ibVF
// WQIhALrBnoA3Eph3agnYXQDVv6vSFfTzFLk9pNRGMAH5GzkfAHUA5tIxY0B3jMEQ
// QQbXcbnOwdJA9paEhvu6hzId/R43jlAAAAGSwSAwzwAABAMARjBEAiBYLoQtnHmj
// pXSeGlMficdDWtNEYBrn1FE7y6vlHZlD6QIgZNqsjSiq1jlNbpo7nRHvWdW/hNtr
// iUXMkxSOP9f7EMYwDQYJKoZIhvcNAQELBQADggEBADxje/Hu4QNC1MKeSLLDr/Bn
// 6jrX8sJL0yCTAMBeY644P4e9oJC2aN46ngywLW/I+Kjvv0Tz3k/42ROBJIRNqmxK
// GBYoWraNCBksPQewbZLMi1mYhpraLxDzSSNewfxxBm8d6vzJMaGKQNktZNPgmQ+O
// vqerlcHEWUVG4uN+LPwd4/f43e2ahkM3FDXBsCVXGDw2kvBgBPjOL4i9l3dm/jNJ
// sVR15uXB7eu2NupOhw3WLBlkOeJvfAZtE+7KcBuGPsPJTC5R2CyYx9s+tQl96YDp
// wJwYKWUXSk+J/O5T+bvOXxhg1uyXfVB6wrEd+lm6ZAfB577s6CokXkDs0UKc1Wo=
// -----END CERTIFICATE-----

// -----BEGIN CERTIFICATE-----
// MIIFBjCCAu6gAwIBAgIRAIp9PhPWLzDvI4a9KQdrNPgwDQYJKoZIhvcNAQELBQAw
// TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
// cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw
// WhcNMjcwMzEyMjM1OTU5WjAzMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg
// RW5jcnlwdDEMMAoGA1UEAxMDUjExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB
// CgKCAQEAuoe8XBsAOcvKCs3UZxD5ATylTqVhyybKUvsVAbe5KPUoHu0nsyQYOWcJ
// DAjs4DqwO3cOvfPlOVRBDE6uQdaZdN5R2+97/1i9qLcT9t4x1fJyyXJqC4N0lZxG
// AGQUmfOx2SLZzaiSqhwmej/+71gFewiVgdtxD4774zEJuwm+UE1fj5F2PVqdnoPy
// 6cRms+EGZkNIGIBloDcYmpuEMpexsr3E+BUAnSeI++JjF5ZsmydnS8TbKF5pwnnw
// SVzgJFDhxLyhBax7QG0AtMJBP6dYuC/FXJuluwme8f7rsIU5/agK70XEeOtlKsLP
// Xzze41xNG/cLJyuqC0J3U095ah2H2QIDAQABo4H4MIH1MA4GA1UdDwEB/wQEAwIB
// hjAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwEgYDVR0TAQH/BAgwBgEB
// /wIBADAdBgNVHQ4EFgQUxc9GpOr0w8B6bJXELbBeki8m47kwHwYDVR0jBBgwFoAU
// ebRZ5nu25eQBc4AIiMgaWPbpm24wMgYIKwYBBQUHAQEEJjAkMCIGCCsGAQUFBzAC
// hhZodHRwOi8veDEuaS5sZW5jci5vcmcvMBMGA1UdIAQMMAowCAYGZ4EMAQIBMCcG
// A1UdHwQgMB4wHKAaoBiGFmh0dHA6Ly94MS5jLmxlbmNyLm9yZy8wDQYJKoZIhvcN
// AQELBQADggIBAE7iiV0KAxyQOND1H/lxXPjDj7I3iHpvsCUf7b632IYGjukJhM1y
// v4Hz/MrPU0jtvfZpQtSlET41yBOykh0FX+ou1Nj4ScOt9ZmWnO8m2OG0JAtIIE38
// 01S0qcYhyOE2G/93ZCkXufBL713qzXnQv5C/viOykNpKqUgxdKlEC+Hi9i2DcaR1
// e9KUwQUZRhy5j/PEdEglKg3l9dtD4tuTm7kZtB8v32oOjzHTYw+7KdzdZiw/sBtn
// UfhBPORNuay4pJxmY/WrhSMdzFO2q3Gu3MUBcdo27goYKjL9CTF8j/Zz55yctUoV
// aneCWs/ajUX+HypkBTA+c8LGDLnWO2NKq0YD/pnARkAnYGPfUDoHR9gVSp/qRx+Z
// WghiDLZsMwhN1zjtSC0uBWiugF3vTNzYIEFfaPG7Ws3jDrAMMYebQ95JQ+HIBD/R
// PBuHRTBpqKlyDnkSHDHYPiNX3adPoPAcgdF3H2/W0rmoswMWgTlLn1Wu0mrks7/q
// pdWfS6PJ1jty80r2VKsM/Dj3YIDfbjXKdaFU5C+8bhfJGqU3taKauuz0wHVGT3eo
// 6FlWkWYtbt4pgdamlwVeZEW+LM7qZEJEsMNPrfC03APKmZsJgpWCDWOKZvkZcvjV
// uYkQ4omYCTX5ohy+knMjdOmdH9c7SpqEWBDC86fiNex+O0XOMEZSa8DA
// -----END CERTIFICATE-----

// -----BEGIN CERTIFICATE-----
// MIICGzCCAaGgAwIBAgIQQdKd0XLq7qeAwSxs6S+HUjAKBggqhkjOPQQDAzBPMQsw
// CQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFyY2gg
// R3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMjAeFw0yMDA5MDQwMDAwMDBaFw00
// MDA5MTcxNjAwMDBaME8xCzAJBgNVBAYTAlVTMSkwJwYDVQQKEyBJbnRlcm5ldCBT
// ZWN1cml0eSBSZXNlYXJjaCBHcm91cDEVMBMGA1UEAxMMSVNSRyBSb290IFgyMHYw
// EAYHKoZIzj0CAQYFK4EEACIDYgAEzZvVn4CDCuwJSvMWSj5cz3es3mcFDR0HttwW
// +1qLFNvicWDEukWVEYmO6gbf9yoWHKS5xcUy4APgHoIYOIvXRdgKam7mAHf7AlF9
// ItgKbppbd9/w+kHsOdx1ymgHDB/qo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0T
// AQH/BAUwAwEB/zAdBgNVHQ4EFgQUfEKWrt5LSDv6kviejM9ti6lyN5UwCgYIKoZI
// zj0EAwMDaAAwZQIwe3lORlCEwkSHRhtFcP9Ymd70/aTSVaYgLXTWNLxBo1BfASdW
// tL4ndQavEi51mI38AjEAi/V3bNTIZargCyzuFJ0nN6T5U6VR5CmD1/iQMVtCnwr1
// /q4AaOeMSQ+2b1tbFfLn
// -----END CERTIFICATE-----
// )";

// Instâncias do WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// void publishFrame() {
//   camera_fb_t *fb = esp_camera_fb_get();
//   size_t _jpg_buf_len = 0;
//   uint8_t * _jpg_buf = NULL;
//   if (!fb) {
//     Serial.println("Falha ao capturar frame");
//     return;
//   }
//   if (fb->format != PIXFORMAT_JPEG) {
//     // Convertendo para JPEG
//     bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
//     esp_camera_fb_return(fb);
//     if (!jpeg_converted) {
//       Serial.println("Falha na conversão para JPEG");
//       return;
//     }
  
//    // Publicar o frame como mensagem MQTT
//    Serial.println("Publicando Frame");
//    client.publish("robot/streaming", _jpg_buf, _jpg_buf_len);
//    esp_camera_fb_return(fb);
//   }
// }

// void sendFrame() {
//     HTTPClient http;
//     http.begin("http://192.168.3.137:3000/api/stream");
//     http.addHeader("Content-Type", "image/jpeg");

//     camera_fb_t* fb = esp_camera_fb_get();
//     if (fb) {
//         int httpCode = http.POST(fb->buf, fb->len);
//         if (httpCode > 0) {
//             // Se o código de status for positivo, a requisição foi bem-sucedida
//             Serial.println("Imagem enviada com sucesso");
//             Serial.print("Código HTTP: ");
//             Serial.println(httpCode);
//         } else {
//             // Se o código de status for negativo, ocorreu um erro
//             //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//         }
//         esp_camera_fb_return(fb);
//     }  else {
//         Serial.println("Falha ao capturar frame da câmera");
//     }
//     http.end();
// }

void moveRobot(String message) {
  if (message == "forward") {
    Serial.println("Moving Forward");
    digitalWrite(MOTOR_1_PIN_1, 1);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 1);
  } 

  else if (message == "left") {
    Serial.println("Moving Left");
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 1);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 1);
  }

  else if (message == "right") {
    Serial.println("Moving Right");
    digitalWrite(MOTOR_1_PIN_1, 1);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 1);
    digitalWrite(MOTOR_2_PIN_2, 0);
  }

  else if (message == "backward") {
    Serial.println("Moving Backward");
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 1);
    digitalWrite(MOTOR_2_PIN_1, 1);
    digitalWrite(MOTOR_2_PIN_2, 0);
  }

  else if (message == "stop") {
    Serial.println("Stopping");
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 0);
  }
}

void changeSpeed(String message) {
   Serial.println("Changing speed to: " + message);
   int value = message.toInt();

   if (value == 0) {
    ledcWrite(enablePin, 0);
    digitalWrite(MOTOR_1_PIN_1, 0);
    digitalWrite(MOTOR_1_PIN_2, 0);
    digitalWrite(MOTOR_2_PIN_1, 0);
    digitalWrite(MOTOR_2_PIN_2, 0); 
  } else { 
    dutyCycle = map(value, 20, 100, 200, 255);
    ledcWrite(enablePin, dutyCycle);
  }  
}

void callback(char* topic, byte* payload, unsigned int length) {

  // Converte os dados recebidos para uma string e imprime
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (!strcmp(topic, "robot/move")) {
    moveRobot(message);
  } else if (!strcmp(topic, "robot/speed")) {
    changeSpeed(message);
  }
}

void setup() {
  Serial.begin(115200);

  // Set the Motor pins as outputs
  pinMode(MOTOR_1_PIN_1, OUTPUT);
  pinMode(MOTOR_1_PIN_2, OUTPUT);
  pinMode(MOTOR_2_PIN_1, OUTPUT);
  pinMode(MOTOR_2_PIN_2, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // Configure PWM Pins
  ledcAttach(enablePin, freq, reso);
  ledcAttach(LED_PIN, freq, reso);

  // Initialize PWM with duty cycle
  ledcWrite(enablePin, 200);
  ledcWrite(LED_PIN, 0);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi!");

  // Configura MQTT
  client.setServer(mqtt_server, mqtt_port);
  reconnect();

  client.setCallback(callback);

  client.subscribe("robot/move");
  client.subscribe("robot/speed");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void loop() {
  // Mantém a conexão MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // sendFrame();
  delay(1000/30);
  //publishFrame();
}
