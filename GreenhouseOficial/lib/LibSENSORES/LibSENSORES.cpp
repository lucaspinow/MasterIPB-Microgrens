/* FUNÇÕES DA BIBLIOTECA DOS SENSORES */

#include "LibSENSORES.h"

Sensors::Sensors(float* sval_sensores, int pinDHT, int pinSoilMoisture)
    : sval_sensores(sval_sensores), pinDHT(pinDHT), pinSoilMoisture(pinSoilMoisture), dht(pinDHT, DHTTYPE) {}

int umidadeMinima = 3200; // Valor mínimo de umidade (0%)
int umidadeMaxima = 1350; // Valor máximo de umidade (100%)
int quant = 0;
float medHumiditySoil = 0, auxHumiditySoil, lastHumiditySoil;

// Realiza a leitura dos sensores
void Sensors::readSensors() 
{
    // Calculo umidade do solo (fazer calibração)
    float soilMoisture = analogRead(pinSoilMoisture);
    float humiditySoil;

    // Mapeia o valor lido para o intervalo de 0 a 100 (percentual de umidade)
    auxHumiditySoil = map(soilMoisture, umidadeMinima, umidadeMaxima, 0, 100);
    if (auxHumiditySoil < 0)
      auxHumiditySoil = 0;
    if (auxHumiditySoil > 100)
      auxHumiditySoil = 100;

    if (auxHumiditySoil != 0) {
      if (quant == 0)
          {
            lastHumiditySoil = auxHumiditySoil;
            quant = quant + 1;
          }
          if (quant < 20)
            medHumiditySoil = medHumiditySoil + auxHumiditySoil;
          if (quant == 20)
          {
            lastHumiditySoil = medHumiditySoil/20;
            medHumiditySoil = 0;
            quant = 0;
          }
          quant = quant + 1;
    }
    humiditySoil = lastHumiditySoil;

    


    //float humiditySoil = ((soilMoisture / umid_solo_escala)) * (umid_solo_max - umid_solo_min) + umid_solo_min;

    // Atribuição no vetor sensor
    sval_sensores[0] = humiditySoil; // Leitura da Umidade do Solo
    sval_sensores[1] = dht.readTemperature(); // Leitura da Temperatura e Umidade do Ar
    sval_sensores[2] = dht.readHumidity(); 

    if (lightMeter.measurementReady())
      sval_sensores[3] = lightMeter.readLightLevel(); // Leitura da Luminosidade
}

void Sensors::readDate(int* val_horario)
{
    DateTime now = rtc.now();

    val_horario[0] = now.year();    // Ano
    val_horario[1] = now.month();   // Mes
    val_horario[2] = now.day();     // Dia
    val_horario[3] = now.hour();    // Hora
    val_horario[4] = now.minute();  // Minuto
    val_horario[5] = now.second();  // Segundo
}

// Inicializa os sensores
void Sensors::begin()
{        
  // Inicializa a comunicação I2C
  Wire.begin();         

  // Inicializa o módulo DHT
  dht.begin();

  // Inicializa a medição de Luminosidade
  if (!lightMeter.begin())      
    Serial.println("Erro ao iniciar o BH1750!");

  // Inicializa o módulo RTC
  if (!rtc.begin())
    Serial.println("Erro ao iniciar o RTC!");
  delay(500);
}