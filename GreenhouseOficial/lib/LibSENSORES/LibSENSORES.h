/* BIBLIOTECA DOS SENSORES */

#ifndef LIBSENSORES_H
#define LIBSENSORES_H

/* BIBLIOTECAS NECESSÁRIAS*/
#include <DHT.h>
#include <DHT_U.h>
#include <BH1750.h> 
#include <RTClib.h>

/* DEFINIÇÕES DE PARAMETROS */
#define DHTTYPE DHT22           // Versão do DHT utilizado
#define umid_solo_escala 4096   // 12 bits para conversão A/D
#define umid_solo_max 100.0     // Umidade máxima
#define umid_solo_min 0.0       // Umidade mínima


// Classe 'Sensors'
class Sensors {
public:
    Sensors(float* sval_sensores, int pinDHT, int pinSoilMoisture);   // Recebe os pinos dos sensores como entrada para configuração

    void readSensors();                 // Realiza a leitura dos sensores e disponibiliza os resultados nas variáveis inseridas como parâmetro
    void readDate(int* val_horario);    // Realiza a leitura de tempo e atualiza os vetores de horario
    void begin();                       // Função de Inicialização dos sensores 
    
private:
    int pinDHT;             // Pino do sensor DHT
    int pinSoilMoisture;    // Pino do sensor Umidade de Solo
    float* sval_sensores;   // Vetor dos sensores
    DHT dht;                // Criação do objeto dht
    BH1750 lightMeter;      // Criação do objeto lightMeter
    RTC_DS3231 rtc;         // Criação do objeto RTC
};

#endif
