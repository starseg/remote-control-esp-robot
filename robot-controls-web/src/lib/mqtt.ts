import mqtt from "mqtt";

const MQTT_URL = process.env.MQTT_URL || "";
const MQTT_USER = process.env.MQTT_USER || "";
const MQTT_PASSWORD = process.env.MQTT_PASSWORD || "";

// Configurações do MQTT
const mqtt_server = "ws://192.168.3.137";
const mqtt_port = 8884;
const mqtt_user = "starseg";
const mqtt_password = "STARseg242369";
// Cria a instância do cliente MQTT
export const client = mqtt.connect(mqtt_server, {
  port: mqtt_port,
  username: mqtt_user,
  password: mqtt_password,
});

const mqtt_streaming = "robot/streaming";
const mqqtt_move = "robot/move";
const mqtt_speed = "robot/speed";

// client.on("connect", () => {
//   console.log("Conectado ao MQTT via WebSocket", client.options);
//   client.subscribe(mqtt_streaming);
//   console.log("Inscrito ao tópico de streaming da câmera");
// });
