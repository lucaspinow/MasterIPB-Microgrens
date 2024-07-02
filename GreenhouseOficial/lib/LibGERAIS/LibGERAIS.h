/* BIBLIOTECA DE FUNÇÕES GERAIS */

/* 
    1 - Criação dos arquivos JSON
    2 - Impressão Serial
    3 - Interface OLED
    4 - Interface EEPROM
    5 - 

*/

#ifndef LIBGERAIS_H
#define LIBGERAIS_H

#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include "Adafruit_EEPROM_I2C.h"
#include "Adafruit_FRAM_I2C.h"

void criarJSON(float *vet_valores, char **vet_descricao, int tam, char *out);           // Cria um arquivo JSON e publica no broker MQTT
void unificaJSON(char *json1, char *json2, char *json3, char *out);                     // Cria um arquivo JSON e publica no broker MQTT
void imprime_serial(float *sval_sensores, float* val_setpoint, float* aval_atuadores, bool modoOperacao);   // Imprime as informações no serial
int conversor_escala(int val, int val_min, int val_max, int nval_min, int nval_max);    // Conversor de escala
void tela01(float *sval_sensores);                          // Tela 01 - displayOLED
void tela02(float *aval_atuadores, bool modoOperacao);      // Tela 02 - displayOLED
void tela03(float *aval_atuadores, bool modoOperacao);      // Tela 03 - displayOLED
void tela04(float *val_setpoint);                           // Tela 04 - displayOLED
void tela05(float *val_setpoint);                           // Tela 05 - displayOLED
void tela06(int *val_horario, bool turno, bool stateMqtt, bool stateWifi);   // Tela 06 - displayOLED
void serialTime(int* val_horario);  // Imprime o horario do RTC na tela serial
void iniOLED();
time_t converterParaTimestampInt(int* val_horario);
void iniEEPROM();
void salvaEEPROM(float* setpoint, bool modoOperacao);
bool carregaEEPROM(float* setpoint);
void printDisplayVar(int variavel);
void newJSON(float *vet_sensores, float *vet_atuadores, time_t timestamp, char *ID, char *TYPE, char *ADDLOC, char *ADDCOUNTRY, char *dataProvider, char *hasAgriParcelParent, bool turno, bool modo, char *out);


#endif
