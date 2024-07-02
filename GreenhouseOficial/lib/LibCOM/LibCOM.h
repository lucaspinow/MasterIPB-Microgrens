/* BIBLIOTECA DE COMUNICAÇÃO WIFI E MQTT */

#ifndef LIBCOM_H
#define LIBCOM_H

/* BIBLIOTECAS NECESSÁRIAS*/
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Classe 'Communication'
class Communication {
public:
    // Recebe os parametros de ID e senha do WiFi e servidor MQTT
    Communication(const char* wifiSSID, const char* wifiPassword, const char* mqttServer, int mqttPort, const char* mqttUser, const char* mqttPassword);

    void begin();   // Inicialização da comunicação
    void loop();    // Função para verificação de integridade de conexão
    void subsTopic(char* topico);   // Função que realiza a subscrição de um tópico
    bool publish_mqtt(const char* message, const char* topico); // Realiza a publicação MQTT de um arquivo em um tópico
    void static callback(char *topic, byte *payload, unsigned int length);

    static int setPoint[10];
    static int cmdOperacao[7];
    static bool callCallbackSave;
    bool stateMqtt;
    bool stateWifi;

private:
    const char* wifiSSID;       // ID do WIFI
    const char* wifiPassword;   // Senha do WIFI
    const char* mqttServer;     // Broker MQTT
    int mqttPort;               // Porta MQTT
    const char* mqttUser;       // Usuário MQTT (caso porta 1883)
    const char* mqttPassword;   // Senha MQTT (caso porta 1883)
    uint32_t lastReconnectAttempt = 0;
    WiFiClient espClient;       // Criação do objeto WIFI
    PubSubClient client;        // Criação do objeto 'client' mqtt
    

    bool mqttConnect();         // Função de conexão MQTT 
};

#endif
