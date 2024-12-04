import mqtt from "mqtt";

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

client.subscribe("robot/speed");
