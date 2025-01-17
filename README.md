# Remote Control ESP Robot

**Projeto de controle remoto de robôs via celular**, utilizando comunicação MQTT e ESP8266. Este repositório contém a configuração completa para rodar o servidor MQTT, o código do ESP8266 e a interface web desenvolvida com Next.js e TypeScript.

---

## 📂 Estrutura do Repositório

- **/mosquitto**  
  Configuração do servidor MQTT em um container Docker.

- **/robot-controls-web**  
  Interface web desenvolvida com Next.js + TypeScript, permitindo o controle remoto do robô. Inclui funcionalidades adicionais como controle de LED e buzzer.

- **/robot-esp-server**  
  Código-fonte do ESP8266 para controle do robô, incluindo suporte a LED e buzzer.

---

## 🚀 Como Configurar

### 1. Configurar o Servidor MQTT
1. Certifique-se de ter o Docker instalado.
2. Navegue até a pasta `/mosquitto`:
   ```bash
   cd mosquitto
3. Suba o container do servidor MQTT:
  ```bash
  docker-compose up -d
  ```
4. O servidor MQTT estará disponível na porta configurada no arquivo `docker-compose.yml`.

---

### 2. Configurar o ESP8266
1. Navegue até a pasta `/robot-esp-server`.
2. Abra o código no Arduino IDE ou editor de sua preferência.
3. Configure as seguintes informações:
   - Credenciais da rede Wi-Fi.
   - Endereço do servidor MQTT (IP do container Docker).
4. Compile e faça o upload do código para o ESP8266.

---

### 3. Configurar a Interface Web
1. Navegue até a pasta `/robot-controls-web`:
   ```bash
   cd robot-controls-web
   ```
2. Instale as dependências:
   ```bash
   pnpm install
   ```
3. Inicie o servidor de desenvolvimento:
   ```bash
   pnpm dev
   ```
4. Acesse a interface web em http://localhost:3000.

---

### 📱 Funcionalidades
- Controle remoto do robô via interface web responsiva.
- Comunicação em tempo real via MQTT.
- Suporte a LED, buzzer e comandos adicionais.
- Configuração modular para fácil adaptação e expansão.

---

### 🛠 Requisitos
- Hardware: ESP8266, motores, LED, buzzer e componentes do robô.
- Software: Docker, Node.js, PNPM.
- Bibliotecas ESP8266: MQTT, Wi-Fi (instaladas na Arduino IDE).
   
   
