/* FUNÇÕES DA BIBLIOTECA DE COMUNICAÇÃO WIFI E MQTT */

#include "LibCOM.h"

// Definição das variáveis estáticas da classe Communication
int Communication::setPoint[10];
int Communication::cmdOperacao[7];
bool Communication::callCallbackSave=false;
int flagModoOperacao = 0;
#define TOPICO_SUBS1 "IPB/USER01/MODULO01/COMANDO/RECEITA"
#define TOPICO_SUBS2 "IPB/USER01/MODULO01/COMANDO/CMDS"


// Definição dos parametros da classe Communication
Communication::Communication(const char* wifiSSID, const char* wifiPassword, const char* mqttServer, int mqttPort, const char* mqttUser, const char* mqttPassword)
    : wifiSSID(wifiSSID), wifiPassword(wifiPassword), mqttServer(mqttServer), mqttPort(mqttPort), mqttUser(mqttUser), mqttPassword(mqttPassword), client(espClient) {}


// Função de Inicialização Wi-Fi e MQTT
void Communication::begin() 
{
  Communication::stateMqtt = false;
  Communication::stateWifi = false;

  Serial.print("Connecting to ");
  Serial.println(wifiSSID);
  WiFi.begin(wifiSSID, wifiPassword);

  // Esperar até que a conexão Wi-Fi seja estabelecida
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
  }
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  client.setBufferSize(4096);

  // Lidar com a falha de conexão MQTT, talvez tentar novamente
  if (!mqttConnect()) 
    Serial.println("Failed to connect to MQTT server.");
    

  /*while (!client.connected())
  {
    Serial.println("Conecntando ao MQTT...");
    if (client.connect("ESPClient")) 
      {
        Serial.println("conectado");
      } 
      else 
      {
        Serial.print("falha com estado ");
        Serial.print(client.state());
        delay(2000);
      }
  }*/
}

void Communication::loop() {
    long now = millis();

    // Verificar a conexão Wi-Fi
    if (WiFi.status() == WL_CONNECTED) {
        Communication::stateWifi = true;
        // Se não estivermos conectados ao MQTT, tentar reconectar
        if (!client.connected() && (now - lastReconnectAttempt > 10000L)) {
            Communication::stateMqtt = false;
            lastReconnectAttempt = now;
            if (mqttConnect()) {
                Serial.println("Connected to MQTT server.");
                lastReconnectAttempt = 0;
            }
        } else {
            Communication::stateMqtt = true;
            client.loop();
        }
    } else {
        Communication::stateWifi = false;
        Communication::stateMqtt = false;
        // Se a conexão Wi-Fi cair, tentar reconectar
        if (now - lastReconnectAttempt > 10000L) {
            Serial.println("Reconnecting to Wi-Fi");
            WiFi.reconnect();
            lastReconnectAttempt = now;
        }
    }
}

// Função de conexão MQTT
bool Communication::mqttConnect() 
{   
   if (client.connect("espClient"))
   {
      client.subscribe(TOPICO_SUBS1);
      client.subscribe(TOPICO_SUBS2);
      client.setBufferSize(1024);
      return true;
   }
   else 
   {
      Serial.print("Falha na conexão MQTT, erro: ");
      Serial.println(client.state());
      return false;
   }
}

bool Communication::publish_mqtt(const char* message, const char* topico) {
  if (client.connected() && WiFi.status() == WL_CONNECTED) {
    return client.publish(topico, message);
  } else {
    Serial.println("Erro: Wi-Fi ou conexão MQTT não está disponível.");
    return false;
  }
}

void Communication::subsTopic(char* topico){
  if (client.connected() && WiFi.status() == WL_CONNECTED) {
    client.subscribe(topico);
  } else {
    Serial.println("Erro: Wi-Fi ou conexão MQTT não está disponível.");
  }
}


void Communication::callback(char *topic, byte *payload, unsigned int length)
{
  char jsonPayload[length + 1];

  /* IMPRIME NO SERIAL */
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  Serial.print("Conteúdo da mensagem: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  /* DECODIFICACAO DO JSON */

  memcpy(jsonPayload, payload, length);
  jsonPayload[length] = '\0'; // Adicione o caractere nulo no final

  // Crie um objeto JSON
  JsonDocument doc, docAux;

  // Faça o parsing do JSON
  deserializeJson(doc, jsonPayload);

  if (strcmp(topic, "IPB/USER01/MODULO01/COMANDO/RECEITA") == 0)
  {
    docAux = doc["morningSetpoint"];
    Communication::setPoint[0] = docAux["idealSoilMoisture"];
    Communication::setPoint[1] = docAux["idealTemperature"];
    Communication::setPoint[2] = docAux["idealHumidity"];
    Communication::setPoint[3] = docAux["luminosityTime"];
    Communication::setPoint[4] = docAux["luminosityLux"];

    docAux = doc["nightSetpoint"];
    Communication::setPoint[5] = docAux["idealSoilMoisture"];
    Communication::setPoint[6] = docAux["idealTemperature"];
    Communication::setPoint[7] = docAux["idealHumidity"];
    Communication::setPoint[8] = docAux["luminosityTime"];
    Communication::setPoint[9] = docAux["luminosityLux"];

    Communication::callCallbackSave = true;
  }

  if (strcmp(topic, "IPB/USER01/MODULO01/COMANDO/CMDS") == 0)
  {
    docAux = doc["cmdActuators"];
    Communication::cmdOperacao[0] = docAux["waterValve"];
    Communication::cmdOperacao[1] = docAux["ledStrip"];
    Communication::cmdOperacao[2] = docAux["ventilationFan"];
    Communication::cmdOperacao[3] = doc["operationMode"];
    Communication::cmdOperacao[4] = docAux["atomizer"];
    Communication::cmdOperacao[5] = docAux["heatingPad"];
  }

  if (flagModoOperacao != Communication::cmdOperacao[3])
  {
    flagModoOperacao = Communication::cmdOperacao[3];
    Communication::callCallbackSave = true;
  }

  
  //old
  /*if (strcmp(topic, "IPB/USER01/MODULO01/COMANDO/RECEITA") == 0)
  {
    Communication::setPoint[0] = doc["SetUmidadeSolo"];
    Communication::setPoint[1] = doc["SetTemperatura"];
    Communication::setPoint[2] = doc["SetUmidadeAr"];
    Communication::setPoint[3] = doc["SetTimeLuz"];
    Communication::setPoint[4] = doc["SetLuz"];
    Communication::setPoint[5] = doc["SetAgua"];
    Communication::setPoint[6] = doc["SetTimeCooler"];
    Communication::setPoint[7] = doc["SetDifusor"];
    Communication::callCallbackSave = true;
  }
  if (strcmp(topic, "IPB/USER01/MODULO01/COMANDO/CMDS") == 0)
  {
    Communication::cmdOperacao[0] = doc["CmdValvula"];
    Communication::cmdOperacao[1] = doc["CmdFitaLed"];
    Communication::cmdOperacao[2] = doc["CmdVentilador"];
    Communication::cmdOperacao[3] = doc["CmdModoOperacao"];
    Communication::cmdOperacao[4] = doc["CmdDifusor"];
    Communication::cmdOperacao[5] = doc["CmdHeater"];
  }*/
}