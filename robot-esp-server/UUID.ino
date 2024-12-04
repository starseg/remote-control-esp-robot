#include <Preferences.h>

Preferences preferences;

String generateUUID() {
    char uuid[37];
    snprintf(uuid, sizeof(uuid),
             "%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
             random(0xFFFF), random(0xFFFF),   // 8 caracteres
             random(0xFFFF),                  // 4 caracteres
             random(0x0FFF) | 0x4000,         // 4 caracteres (versão 4 UUID)
             random(0x3FFF) | 0x8000,         // 4 caracteres (RFC 4122)
             random(0xFFFF), random(0xFFFF), random(0xFFFF)); // 12 caracteres
    return String(uuid);
}

// Função para verificar e salvar o UUID na memória flash
String getOrCreateUUID() {
    preferences.begin("device-config", false); // Abre a NVS com namespace "device-config"
    String uuid = preferences.getString("deviceId", ""); // Tenta carregar o ID armazenado

    if (uuid == "") {
      // Se não existir, gera um novo UUID
      uuid = generateUUID();
      preferences.putString("deviceId", uuid); // Armazena na memória flash
      Serial.println("Novo UUID gerado e armazenado: " + uuid);
    } else {
      Serial.println("UUID existente carregado: " + uuid);
    }

    preferences.end(); // Fecha a NVS
    return uuid;
}