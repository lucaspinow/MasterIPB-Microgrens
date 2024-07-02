/* 
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  # # # # # # TESE MESTRADO - ESTUFA AUTOMATIZADA # # # # # # #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

  IPB - INSTITUTO POLITÉCNICO DE BRAGANÇA
  UTFPR - UNIVERSIDADE FEDERAL DO PARANÁ 
  AUTOR: LUCAS SANTOS PINOW - 31/10/2023 - 20/06/2024
*/

/* COMENTÁRIOS GERAIS */

// # # # # # # # # BIBLIOTECAS # # # # # # # # # # #
#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include "LibCONTROL.h"
#include "LibSENSORES.h"
#include "LibCOM.h"
#include "LibGERAIS.h"
#include "esp_task_wdt.h"

/* # # # # # # # CONSTANTES - EVENTOS - DEFINES # # # # # # # # # */

/* # # CONSTANTES # # */

// WIFI E MQTT
const char* mqttServer = "test.mosquitto.org"; //Servidor MQTT
//const char* mqttServer = "192.168.137.1"; //Servidor MQTT
//const char* mqttServer = "192.168.1.73"; //Servidor MQTT
const int mqttPort = 1883; //Porta 1883 - sem criptografia / com autenticação
const char* mqttUser = "";
const char* mqttPassword = "";


const char* wifi_ssid = "";        // Nome da rede
const char* wifi_password = "";    // Senha da rede 
 
/* # # DEFINES # # */

// PINOS ESP 32
#define spin_umid_solo 34
#define spin_dht 15
#define spin_valve 33
#define spin_difusor 32
#define spin_ventilador 25
#define spin_LED 16
#define spin_heater 18


// OUTROS
#define T_timeout 10000
#define taxa_bauds 115200
#define USER "USER01"
#define MODULE "MODULO01"
#define TOPICO_PUB "IPB/USER01/MODULO01/ESTADO"
#define TOPICO_PUB2 "IPB/USER01/MODULO01/STATE"
#define TOPICO_SUBS1 "IPB/USER01/MODULO01/COMANDO/RECEITA"
#define TOPICO_SUBS2 "IPB/USER01/MODULO01/COMANDO/CMDS"
#define ID "ESP32-Greenhouse-Module-A-CeDRI"
#define TYPE "GenericGreenhouseModule"
#define addressLocality "Bragança"
#define addressCountry "Portugal"
#define dataProvider "greenhouse"
#define hasAgriParcelParent "APP-Node-red-ControlGreenhouse"


/* # # # # # # # VARIÁVEIS GLOBAIS # # # # # # # # # # */
bool modoOperacao = 0;        // Variável que define o modo de operação AUTOMÁTICO = 1 ou MANUAL = 0
int tela_atual = 1;           // Variável que indica a tela atual no displayOLED

const unsigned long CONTROL_INTERVAL = 500; // Intervalo de controle dos atuadores em milissegundos
const unsigned long COMMS_INTERVAL = 100; // Intervalo de comunicação em milissegundos
const unsigned long PUB_INTERVAL = 10000; // Intervalo de comunicação em milissegundos
const unsigned long SCREEN_INTERVAL = 5000; // Intervalo de comunicação em milissegundos

unsigned long lastControlTime = 100; // Momento da última execução do controle dos atuadores
unsigned long lastCommsTime = 2000;   // Momento da última execução da comunicação
unsigned long lastPubTime = 5000;   // Momento da última execução da comunicação
unsigned long lastScreenTime = 8000;   // Momento da última execução da comunicação


/* Vetores BUFFER-ESTADO */ 
bool firstLoop = true;
float sval_sensores[4];       // Vetor que contém os valores dos sensores em ordem
char* sval_descricao[4] = {"umidade_solo", "temperatura_ar", "umidade_ar", "luminosidade"};
float aval_atuadores[5];        // Vetor que contém o estado dos atuadores em ordem
float cmd_atuadores[5];         // Vetor que contém os comandos vindo do servidor remoto
char* aval_descricao[5] = {"valvula", "led", "ventilador", "difusor", "heater"};
float val_setpoint[10] = {70, 25, 80, 720, 5000, 50, 14, 60, 0, 300};         // Vetor que contém os valores de SETPOINT (receita)
int val_horario[6];            // Vetor que contem a data e hora atual
float fval_horario[6];         // Vetor que contem a data e hora atual em float
int val_horarioBuffer[6];      // Vetor que contem a data e hora salva
char* horario_descricao[6] = {"ano", "mes", "dia", "hora", "minuto", "segundo"}; 

/* SETPOINT (RECEITA) - val_setpoint
  [0] - SETPOINT MORNING - umidade do solo 
  [1] - SETPOINT MORNING - temperatura do ar 
  [2] - SETPOINT MORNING - umidade do ar 
  [3] - SETPOINT MORNING - Tempo de luminosidade
  [4] - SETPOINT MORNING - Valor em LUX da luminosidade
  [5] - SETPOINT NIGHT - umidade do solo 
  [6] - SETPOINT NIGHT - temperatura do ar 
  [7] - SETPOINT NIGHT - umidade do ar 
  [8] - SETPOINT NIGHT - Tempo de luminosidade
  [9] - SETPOINT NIGHT - Valor em LUX da luminosidade
*/

/* VALOR DE CADA POSIÇÃO - sval_sensores
  [0] - leitura umidade do solo 
  [1] - leitura temperatura do ar 
  [2] - leitura umidade do ar 
  [3] - leitura luminosidade
*/
/* VALOR DE CADA POSIÇÃO - aval_atuadores e cmd_atuadores
  [0] - valvula
  [1] - led 
  [2] - ventilador 
  [3] - difusor
  [4] - heater
*/

/* # # # # # INICIALIZAÇÕES DE VARIÁVEIS DE BIBLIOTECAS # # # # # */

Communication Comunicacao(wifi_ssid, wifi_password, mqttServer, mqttPort, mqttUser, mqttPassword);  // Objeto da Lib WIFI e MQTT
Sensors Sensores(sval_sensores, spin_dht, spin_umid_solo);                                          // Objeto da Lib SENSORES
Controls Controle(aval_atuadores, sval_sensores, val_setpoint, &modoOperacao, cmd_atuadores, val_horario, val_horarioBuffer);  // Objeto da Lib CONTROL


/* # # # PROTÓTIPOS DE FUNÇÕES # # # */
void leitura_serial_mqtt();     // Realiza a impressão serial e publlicação MQTT
void atualiza_displayOLED();    // Atualiza a tela do display OLED
void chamaControle();           // Responsável por fazer a aquisição de dados dos sensores e pela ação de controle
void atualizarAtuadores();      // Responsável por atualizar as saidas digitais
void controlarVaporizador();    // Responsável pelo controle de pulsos do difusor


/* # # # # # # # # FUNÇÕES # # # # # # # # # # # */

void leitura_serial_mqtt()
{
    char json1[256];
    char json2[256];
    char json3[256];
    char out[1200];
    char out1[800];
    time_t timestamp[1];
    float timestamp1[1];
    char* timestampDesc[1] = {"Timestamp"};

    // Impressão Serial
    imprime_serial(sval_sensores, val_setpoint, aval_atuadores, modoOperacao);
    serialTime(val_horario);

    // Conversao horario paara o vetor int
    timestamp[0] = converterParaTimestampInt(val_horario);
    timestamp1[0] = timestamp[0];

    // Publicação MQTT
    criarJSON(sval_sensores, sval_descricao, sizeof(sval_sensores) / sizeof(sval_sensores[0]), json1);
    criarJSON(aval_atuadores, aval_descricao, sizeof(aval_atuadores) / sizeof(aval_atuadores[0]), json2);
    criarJSON(timestamp1, timestampDesc, sizeof(timestamp1) / sizeof(timestamp1[0]), json3);
    unificaJSON(json1, json2, json3, out);

    if (Comunicacao.publish_mqtt(out, TOPICO_PUB)) 
        Serial.println("Success sending message");
    else 
        Serial.println("Error sending message");
      
    newJSON(sval_sensores, aval_atuadores, timestamp[0], ID, TYPE, addressLocality, addressCountry, dataProvider, hasAgriParcelParent, Controle.turno, modoOperacao, out1);

    if (Comunicacao.publish_mqtt(out1, TOPICO_PUB2)) 
        Serial.println("Success sending message (NEW JSON)");
    else 
        Serial.println("Error sending message (NEW JSON)");
}

void atualiza_displayOLED()
{
  switch (tela_atual)
  {
    case 1:
      tela01(sval_sensores);
      tela_atual = 2;
      break;
    case 2:
      tela02(aval_atuadores, modoOperacao);
      tela_atual = 3;
      break;
    case 3:
      tela03(aval_atuadores, modoOperacao);
      tela_atual = 4;
      break;
    case 4:
      tela04(val_setpoint);
      tela_atual = 5;
      break;
    case 5:
      tela05(val_setpoint);
      tela_atual = 6;
      break;
    case 6:
      tela06(val_horario, Controle.turno, Comunicacao.stateMqtt, Comunicacao.stateWifi);
      tela_atual = 1;
      break;
    
    default:
      tela_atual = 1;
      break;
  }
}

void chamaControle()
{
  if (firstLoop == false) // Recepção dos valores de setpoint
  {   
    val_setpoint[0] = Comunicacao.setPoint[0]; // DIA - Umidade do Solo
    val_setpoint[1] = Comunicacao.setPoint[1]; // DIA - Temperatura
    val_setpoint[2] = Comunicacao.setPoint[2]; // DIA - Umidade do Ar
    val_setpoint[3] = Comunicacao.setPoint[3]; // DIA - Set Time da luminosidade
    val_setpoint[4] = Comunicacao.setPoint[4]; // DIA - Set Lux da luminosidade
    val_setpoint[5] = Comunicacao.setPoint[5]; // NOITE - Umidade do Solo
    val_setpoint[6] = Comunicacao.setPoint[6]; // NOITE - Temperatura
    val_setpoint[7] = Comunicacao.setPoint[7]; // NOITE - Umidade do Ar
    val_setpoint[8] = Comunicacao.setPoint[8]; // NOITE - Set Time da luminosidade
    val_setpoint[9] = Comunicacao.setPoint[9]; // NOITE - Set Lux da luminosidade
  }
  else // Caso seja a primeira inicialização, carrega os valores padrão
  {
    Comunicacao.setPoint[0] = val_setpoint[0];
    Comunicacao.setPoint[1] = val_setpoint[1];
    Comunicacao.setPoint[2] = val_setpoint[2];
    Comunicacao.setPoint[3] = val_setpoint[3];
    Comunicacao.setPoint[4] = val_setpoint[4];
    Comunicacao.setPoint[5] = val_setpoint[5];
    Comunicacao.setPoint[6] = val_setpoint[6];
    Comunicacao.setPoint[7] = val_setpoint[7];
    Comunicacao.setPoint[8] = val_setpoint[8];
    Comunicacao.setPoint[9] = val_setpoint[9];
    Comunicacao.cmdOperacao[3] = modoOperacao;
  }
    
  /* Atuadores */
  cmd_atuadores[0] = Comunicacao.cmdOperacao[0]; // Válvula
  cmd_atuadores[1] = Comunicacao.cmdOperacao[1]; // Led
  cmd_atuadores[2] = Comunicacao.cmdOperacao[2]; // Ventilador
  modoOperacao     = Comunicacao.cmdOperacao[3]; // Modo de Operacao
  cmd_atuadores[3] = Comunicacao.cmdOperacao[4]; // Difusor
  cmd_atuadores[4] = Comunicacao.cmdOperacao[5]; // Heater

  /* Salva a nova receita na memoria apos receber alguma atualizacao de setpoint */ 
  if(Comunicacao.callCallbackSave == true)
  {
    Serial.println("Salvando na EEPROM!");
    Comunicacao.callCallbackSave = 0;
    salvaEEPROM(val_setpoint, modoOperacao);
  }

  /* Leitura dos Sensores, Horario e Ação de Controle*/
  Sensores.readSensors(); 
  Sensores.readDate(val_horario);
  Controle.controleGeral(); // Atualiza o vetor aval_atuadores automaticamente

  firstLoop = false;
}

void controlarVaporizador() // OLD - APENAS PARA O ANTIGO DIFUSOR
{
    static bool vaporizadorLigado = false;

    if (aval_atuadores[3] > 0 && !vaporizadorLigado) 
    {
        // Liga o vaporizador
        digitalWrite(spin_difusor, HIGH);
        delay(200);
        digitalWrite(spin_difusor, LOW);
        vaporizadorLigado = true;
    } 
    else if (aval_atuadores[3] == 0 && vaporizadorLigado) 
    {
        // Desliga o vaporizador
        for (int i = 0; i < 2; i++) {
            digitalWrite(spin_difusor, HIGH);
            delay(200);
            digitalWrite(spin_difusor, LOW);
            delay(200);
        }
        vaporizadorLigado = false;
    }
}

void atualizarAtuadores() 
{
    // Atualiza os valores dos atuadores
    analogWrite(spin_valve, conversor_escala(aval_atuadores[0], 0, 100, 0, 255));
    analogWrite(spin_LED, conversor_escala(aval_atuadores[1], 0, 100, 0, 255));
    analogWrite(spin_ventilador, conversor_escala(aval_atuadores[2], 0, 100, 0, 255));
    analogWrite(spin_heater, conversor_escala(aval_atuadores[4], 0, 100, 0, 255));
    /* NOVO DIFUSOR - CASO ESTEJA UTILIZANDO O ANTIGO, COMENTAR A CONDICAO ABAIXO! */
    if (aval_atuadores[3] > 0)
      digitalWrite(spin_difusor, HIGH);
    else
      digitalWrite(spin_difusor, LOW);
}


// FUNÇÃO DE INICIALIZAÇÃO
void setup() 
{ 
    int timeout, valid=0;

    // Inicialização dos Atuadores
    pinMode(spin_valve, OUTPUT);
    pinMode(spin_LED, OUTPUT);
    pinMode(spin_ventilador, OUTPUT);
    pinMode(spin_difusor, OUTPUT);
    pinMode(spin_heater, OUTPUT);
    
    Serial.begin(taxa_bauds);                        // Inicialização da comunicação serial
    while (!Serial && (timeout < T_timeout))
    {
      timeout = timeout + 10;
      delay(10);
    }
      
    /* SENSORES E AÇÃO DE CONTROLE */
    Sensores.begin();            // Inicialização das leituras dos sensores
    Controle.begin();            // Inicialização da ação de controle
    

    /* DISPLAY OLED E RECEITA */
    iniOLED();        // Inicialização do displayOLED
    iniEEPROM();      // Inicialização da Memória EEPROM
    modoOperacao = carregaEEPROM(val_setpoint);  // Carrega a ultima receita solicitada
    

    /* COMUNICAÇÃO */
    Comunicacao.begin();
    Comunicacao.subsTopic(TOPICO_SUBS1);            // Tópico que será subscrito
    Comunicacao.subsTopic(TOPICO_SUBS2);            // Tópico que será subscrito
    
    esp_task_wdt_delete(NULL);
}

void loop() {
    unsigned long currentTime = millis();

    // Verifica se já passou tempo suficiente para executar o controle dos atuadores
    if (currentTime - lastControlTime >= CONTROL_INTERVAL) {
        lastControlTime = currentTime;
        chamaControle();
        atualizarAtuadores();
        //controlarVaporizador(); UTILIZADO APENAS PARA O ANTIGO VAPORIZADOR
    }

    // Verifica se já passou tempo suficiente para executar o loop comunicação
    if (currentTime - lastCommsTime >= COMMS_INTERVAL) {
        lastCommsTime = currentTime;
        Comunicacao.loop();
    }

    // Verifica se já passou tempo suficiente para executar a impressão serial e publicação MQTT
    if (currentTime - lastPubTime >= PUB_INTERVAL) {
        lastPubTime = currentTime;
        leitura_serial_mqtt();
    }

    // Verifica se já passou tempo suficiente para executar a atualização de telas do display OLED
    if (currentTime - lastScreenTime >= SCREEN_INTERVAL) {
        lastScreenTime = currentTime;
        atualiza_displayOLED();
    }
}


