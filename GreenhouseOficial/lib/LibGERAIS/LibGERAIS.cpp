/* FUNÇÕES DA BIBLIOTECA DE FUNÇÕES GERAIS */

#include "LibGERAIS.h"


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_EEPROM_I2C i2ceeprom;

#define LOGO_HEIGHT   48
#define LOGO_WIDTH    48

void testdrawbitmap(void);
bool estadoEEPROM=false;


/* Logo Projeto */
const unsigned char epd_bitmap__08bd3148_2e98_40c5_84dd_9a785c8547c7 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x66, 0x80, 0x00, 0x00, 0x00, 0x00, 0xf6, 
	0x9c, 0x00, 0x00, 0x00, 0x00, 0xdd, 0xbc, 0x00, 0x00, 0x01, 0x20, 0xef, 0x64, 0x00, 0x00, 0x01, 
	0x20, 0xc6, 0xdc, 0x00, 0x00, 0x01, 0x22, 0x76, 0x98, 0x00, 0x00, 0x00, 0xe6, 0x3b, 0x38, 0x00, 
	0x00, 0x00, 0x00, 0x0d, 0x40, 0x00, 0x00, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x20, 0x00, 0x04, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x06, 0x84, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x06, 
	0xd8, 0x8c, 0xc0, 0x00, 0x00, 0x0c, 0x01, 0x10, 0x20, 0x00, 0x00, 0x09, 0x22, 0x30, 0xa0, 0x00, 
	0x00, 0x12, 0x00, 0x23, 0xf0, 0x00, 0x00, 0x12, 0x46, 0x70, 0x70, 0x00, 0x00, 0x12, 0x00, 0x70, 
	0xf0, 0x00, 0x00, 0x12, 0x00, 0x30, 0x70, 0x00, 0x00, 0x10, 0x7c, 0x31, 0xf0, 0x00, 0x00, 0x13, 
	0x6f, 0x38, 0xf0, 0x00, 0x00, 0x12, 0x23, 0xb9, 0xf0, 0x00, 0x00, 0x10, 0x00, 0x38, 0xf0, 0x00, 
	0x00, 0x13, 0xff, 0xbd, 0xf0, 0x00, 0x00, 0x17, 0xff, 0xfd, 0xf0, 0x00, 0x00, 0x20, 0x00, 0x02, 
	0x08, 0x00, 0x00, 0x27, 0x7f, 0xe0, 0x28, 0x00, 0x00, 0x1f, 0xff, 0xf3, 0x70, 0x00, 0x00, 0x1e, 
	0x8f, 0xb8, 0xf0, 0x00, 0x00, 0x0f, 0xff, 0xdf, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 
	0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 1040)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
	epd_bitmap__08bd3148_2e98_40c5_84dd_9a785c8547c7
};

void iniOLED()
{
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao inicializar o display OLED"));
    delay(500);
    //for(;;);
  }
}

void testdrawbitmap(void) {
  display.drawBitmap(
    /*(display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,*/
    87,7,
    epd_bitmap__08bd3148_2e98_40c5_84dd_9a785c8547c7, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

void tela01(float *sval_sensores)
{
  // Limpar o buffer do display
  display.clearDisplay();

  // Definir cor do texto (branco)
  display.setTextColor(SSD1306_WHITE);
  
  // Definir tamanho do texto (1 é o menor, aumente para tornar o texto maior)
  display.setTextSize(1);

  // Escrever as informações no display
  display.setCursor(0, 0);
  display.println("      Modulo A  (1/6)");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE); // Linha separadora
  
  display.setCursor(0, 13);
  display.print("Temp: ");
  display.print(sval_sensores[1]);
  display.print((char)247);
  display.println("C");

  display.setCursor(0, 23);
  display.print("Solo: ");
  display.print((int)sval_sensores[0]);
  display.println("%");

  display.setCursor(0, 33);
  display.print("Ar: ");
  display.print((int)sval_sensores[2]);
  display.println("%");

  display.setCursor(0, 43);
  display.print("Luz: ");
  display.print((int)sval_sensores[3]);
  display.println(" lux");
  
  display.drawLine(0, 53, SCREEN_WIDTH, 53, SSD1306_WHITE); // Linha separadora
  display.setCursor(0, 56);
  display.println("UTFPR    IPB    CeDRI");
  
  testdrawbitmap();

  // Mostrar o que foi escrito no display
  display.display();
}

void tela02(float *aval_atuadores, bool modoOperacao)
{
  // Limpar o buffer do display
  display.clearDisplay();

  // Definir cor do texto (branco)
  display.setTextColor(SSD1306_WHITE);
  
  // Definir tamanho do texto (1 é o menor, aumente para tornar o texto maior)
  display.setTextSize(1);

  // Escrever as informações no display
  display.setCursor(0, 0);
  display.println("      Modulo A  (2/6)");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE); // Linha separadora
  
  display.setCursor(0, 13);
  if (modoOperacao == 1)
    display.println("Modo: Auto");
  else 
    display.println("Modo: Manual");

  display.setCursor(0, 23);
  if (aval_atuadores[0] == 1)
    display.println("Bomba: ON");
  else
    display.println("Bomba: OFF");

  display.setCursor(0, 33);
  display.print("Vent: ");
  display.print((int)aval_atuadores[2]);
  display.println("%");

  display.setCursor(0, 43);
  display.print("LED: ");
  display.print((int)aval_atuadores[1]);
  display.println("%");
  
  display.drawLine(0, 53, SCREEN_WIDTH, 53, SSD1306_WHITE); // Linha separadora
  display.setCursor(0, 56);
  display.println("UTFPR    IPB    CeDRI");
  
  testdrawbitmap();

  // Mostrar o que foi escrito no display
  display.display();
}

void tela03(float *aval_atuadores, bool modoOperacao)
{
  // Limpar o buffer do display
  display.clearDisplay();

  // Definir cor do texto (branco)
  display.setTextColor(SSD1306_WHITE);
  
  // Definir tamanho do texto (1 é o menor, aumente para tornar o texto maior)
  display.setTextSize(1);

  // Escrever as informações no display
  display.setCursor(0, 0);
  display.println("      Modulo A  (3/6)");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE); // Linha separadora
  
  display.setCursor(0, 13);
  if (modoOperacao == 1)
    display.println("Modo: Auto");
  else 
    display.println("Modo: Manual");

  display.setCursor(0, 23);
  if (aval_atuadores[3] > 0)
    display.println("Difusor: ON");
  else
    display.println("Difusor: OFF");

  display.setCursor(0, 33);
  if (aval_atuadores[4] > 0)
    display.println("Heater: ON");
  else
    display.println("Heater: OFF");
  
  display.drawLine(0, 53, SCREEN_WIDTH, 53, SSD1306_WHITE); // Linha separadora
  display.setCursor(0, 56);
  display.println("UTFPR    IPB    CeDRI");
  
  testdrawbitmap();

  // Mostrar o que foi escrito no display
  display.display();
}

void tela04(float *val_setpoint)
{
  // Limpar o buffer do display
  display.clearDisplay();

  // Definir cor do texto (branco)
  display.setTextColor(SSD1306_WHITE);
  
  // Definir tamanho do texto (1 é o menor, aumente para tornar o texto maior)
  display.setTextSize(1);

  // Escrever as informações no display
  display.setCursor(0, 0);
  display.println("      Modulo A  (4/6)");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE); // Linha separadora
  
  display.setCursor(0, 13);
  display.print("AM-SP-Temp: ");
  display.print((int)val_setpoint[1]);
  display.print((char)247);
  display.println("C");

  display.setCursor(0, 23);
  display.print("AM-SP-Solo: ");
  display.print((int)val_setpoint[0]);
  display.println("%");

  display.setCursor(0, 33);
  display.print("AM-SP-Ar: ");
  display.print((int)val_setpoint[2]);
  display.println("%");

  display.setCursor(0, 43);
  display.print("AM-SP-Luz: ");
  display.print((int)val_setpoint[3]);
  display.println(" m");
  
  display.drawLine(0, 53, SCREEN_WIDTH, 53, SSD1306_WHITE); // Linha separadora
  display.setCursor(0, 56);
  display.println("UTFPR    IPB    CeDRI");
  
  testdrawbitmap();

  // Mostrar o que foi escrito no display
  display.display();
}

void tela05(float *val_setpoint)
{
  // Limpar o buffer do display
  display.clearDisplay();

  // Definir cor do texto (branco)
  display.setTextColor(SSD1306_WHITE);
  
  // Definir tamanho do texto (1 é o menor, aumente para tornar o texto maior)
  display.setTextSize(1);

  // Escrever as informações no display
  display.setCursor(0, 0);
  display.println("      Modulo A  (5/6)");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE); // Linha separadora
  
  display.setCursor(0, 13);
  display.print("PM-SP-Temp: ");
  display.print((int)val_setpoint[6]);
  display.print((char)247);
  display.println("C");

  display.setCursor(0, 23);
  display.print("PM-SP-Solo: ");
  display.print((int)val_setpoint[5]);
  display.println("%");

  display.setCursor(0, 33);
  display.print("PM-SP-Ar: ");
  display.print((int)val_setpoint[7]);
  display.println("%");

  display.setCursor(0, 43);
  display.print("PM-SP-Luz: ");
  display.print((int)val_setpoint[8]);
  display.println(" m");
  
  display.drawLine(0, 53, SCREEN_WIDTH, 53, SSD1306_WHITE); // Linha separadora
  display.setCursor(0, 56);
  display.println("UTFPR    IPB    CeDRI");
  
  testdrawbitmap();

  // Mostrar o que foi escrito no display
  display.display();
}

void tela06(int *val_horario, bool turno, bool stateMqtt, bool stateWifi)
{
  // Limpar o buffer do display
  display.clearDisplay();

  // Definir cor do texto (branco)
  display.setTextColor(SSD1306_WHITE);
  
  // Definir tamanho do texto (1 é o menor, aumente para tornar o texto maior)
  display.setTextSize(1);

  // Escrever as informações no display
  display.setCursor(0, 0);
  display.println("      Modulo A  (6/6)");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE); // Linha separadora
  
  display.setCursor(0, 23);
  display.print("WIFI: ");
  if (stateWifi == true)
    display.println("ON");
  else
    display.println("OFF");

  display.setCursor(0, 33);
  display.print("MQTT: ");
  if (stateMqtt == true)
    display.println("ON");
  else
    display.println("OFF");

  display.setCursor(0, 43);
  display.print("TURNO: ");
  if (turno == true)
    display.println("DIA");
  else
    display.println("NOITE");

  display.setCursor(0, 13);
  printDisplayVar(val_horario[3]);
  display.print(":");
  printDisplayVar(val_horario[4]);
  display.print("  ");
  printDisplayVar(val_horario[2]);
  display.print("/");
  printDisplayVar(val_horario[1]);
  display.print("/");
  printDisplayVar(val_horario[0]);
  
  display.drawLine(0, 53, SCREEN_WIDTH, 53, SSD1306_WHITE); // Linha separadora
  display.setCursor(0, 56);
  display.println("UTFPR    IPB    CeDRI");
  
  testdrawbitmap();

  // Mostrar o que foi escrito no display
  display.display();
}

void printDisplayVar(int variavel)
{
  if (variavel < 10)
    display.print("0");
  display.print(variavel);
}


void serialTime(int* val_horario)
{
  Serial.print("Data: ");
  Serial.print(val_horario[2], DEC);
  Serial.print("/");
  Serial.print(val_horario[1], DEC);
  Serial.print("/");
  Serial.println(val_horario[0], DEC);
  Serial.print("  ");
  Serial.print(val_horario[3], DEC);
  Serial.print(":");
  Serial.print(val_horario[4], DEC);
  Serial.print(":");
  Serial.println(val_horario[5], DEC);
}

// Função para converter vetor de data e hora para timestamp em float
time_t converterParaTimestampInt(int* val_horario) {
    struct tm tempo;
    tempo.tm_year = val_horario[0] - 1900; // tm_year conta os anos desde 1900
    tempo.tm_mon = val_horario[1] - 1; // tm_mon começa de 0 para janeiro
    tempo.tm_mday = val_horario[2];
    tempo.tm_hour = val_horario[3];
    tempo.tm_min = val_horario[4];
    tempo.tm_sec = val_horario[5];
    tempo.tm_isdst = -1; // Informação de horário de verão desconhecida

    time_t timestamp = mktime(&tempo); // Converte para time_t
    return static_cast<int>(timestamp); // Converte time_t para int e retorna
}

// Faz uma conversão de escalas 
int conversor_escala(int val, int val_min, int val_max, int nval_min, int nval_max)
{
  int out;
  out = ((val - val_min) * (nval_max - nval_min) / (val_max - val_min))  + nval_min;
  return out;
}


void newJSON(float *vet_sensores, float *vet_atuadores, time_t timestamp, char *ID, char *TYPE, char *ADDLOC, char *ADDCOUNTRY, char *dataProvider, char *hasAgriParcelParent, bool turno, bool modo, char *out)
{
  JsonDocument doc, docAux;

  doc["id"] = ID;
  doc["type"] = TYPE;
  docAux["addressLocality"] = ADDLOC;
  docAux["addressCountry"] = ADDCOUNTRY;
  doc["address"] = docAux;
  doc["dataProvider"] = dataProvider;
  doc["dateObserved"] = timestamp;
  doc["hasAgriParcelParent"] = hasAgriParcelParent;
  doc["currentMode"] = modo;
  doc["operationShift"] = turno;
  doc["relativeHumidity"] = vet_sensores[2];
  doc["soilMoisture"] = vet_sensores[0];
  doc["temperature"] = vet_sensores[1];
  doc["illuminance"] = vet_sensores[3];
  docAux.clear();
  docAux["waterValve"] = vet_atuadores[0];
  docAux["ledStrip"] = vet_atuadores[1];
  docAux["ventilationFan"] = vet_atuadores[2];
  docAux["atomizer"] = vet_atuadores[3];
  docAux["heatingPad"] = vet_atuadores[4];
  doc["stateActuators"] = docAux;
  docAux.clear();

  serializeJsonPretty(doc, out, 800);
  doc.clear();
}

// Cria um arquivo JSON
void criarJSON(float *vet_valores, char **vet_descricao, int tam, char *out)
{
    // Cria um documento JSON
    DynamicJsonDocument doc(400);

    // Atribui os objetos ao documento JSON
    for (int i = 0; i < tam; i++)
        doc[vet_descricao[i]] = vet_valores[i];
        
    // Serializa o JSON para o buffer
    int b = serializeJson(doc, out, 256);
}

// Cria um arquivo JSON 
void unificaJSON(char *json1, char *json2, char *json3, char *out)
{
    // Cria documentos JSON para cada saída
    DynamicJsonDocument doc1(400);
    DynamicJsonDocument doc2(400);
    DynamicJsonDocument doc3(400);

    // Desserializa as saídas JSON para os documentos
    deserializeJson(doc1, json1);
    deserializeJson(doc2, json2);
    deserializeJson(doc3, json3);

    // Cria um novo documento JSON para unir as informações
    DynamicJsonDocument mergedDoc(800);

    // Adiciona os documentos anteriores como atributos do novo documento
    mergedDoc["SENSORES"] = doc1;
    mergedDoc["ATUADORES"] = doc2;
    mergedDoc["GERAL"] = doc3;

    // Serializa o JSON para o buffer
    serializeJson(mergedDoc, out, 1200);
}

// Função que imprime no monitor Serial os dados obtidos pelos sensores
void imprime_serial(float *sval_sensores, float* val_setpoint, float* aval_atuadores, bool modoOperacao)
{
    Serial.println("LEITURA DOS SENSORES - ESTUFA");
    Serial.println("");

    // Impressão da Umidade do Solo
    Serial.print("Umidade do Solo: ");
    Serial.print(sval_sensores[0]);
    Serial.println(" %");

    // Impressão da Temperatura do Ar
    Serial.print("Temperatura: ");
    Serial.print(sval_sensores[1]);

    Serial.println(" °C");

    // Impressão da Umidade do Ar
    Serial.print("Umidade do Ar: ");
    Serial.print(sval_sensores[2]);
    Serial.println(" %");

    // Iluminação
    Serial.print("Luminosidade: ");
    Serial.print(sval_sensores[3]);
    Serial.println(" lux");

    // Pula linha
    Serial.println("");

    // RECEITA
    Serial.print("SP MORNING - UMIDADE SOLO: ");
    Serial.print(val_setpoint[0]);
    Serial.println(" %");

    Serial.print("SP MORNING - TEMPERATURA: ");
    Serial.print(val_setpoint[1]);
    Serial.println(" °C");

    Serial.print("SP MORNING - UMIDADE AR: ");
    Serial.print(val_setpoint[2]);
    Serial.println(" %");

    Serial.print("SP MORNING - QUANTIDADE DE LUX: ");
    Serial.print(val_setpoint[4]);
    Serial.println(" lux");

    Serial.print("SP MORNING - TEMPO ILUMINACAO: ");
    Serial.print(val_setpoint[3]);
    Serial.println(" min");

    Serial.print("SP NIGHT - UMIDADE SOLO: ");
    Serial.print(val_setpoint[5]);
    Serial.println(" %");

    Serial.print("SP NIGHT - TEMPERATURA: ");
    Serial.print(val_setpoint[6]);
    Serial.println(" °C");

    Serial.print("SP NIGHT - UMIDADE AR: ");
    Serial.print(val_setpoint[7]);
    Serial.println(" %");

    Serial.print("SP NIGHT - QUANTIDADE DE LUX: ");
    Serial.print(val_setpoint[9]);
    Serial.println(" lux");

    Serial.print("SP NIGHT - TEMPO ILUMINACAO: ");
    Serial.print(val_setpoint[8]);
    Serial.println(" min");

    // ATUADORES
    Serial.print("CMD - MODO DE OPERACAO: ");
    if (modoOperacao == 1)
      Serial.println("AUTOMÁTICO");
    else
      Serial.println("MANUAL");
    Serial.print("CMD - DIFUSOR: ");
    if (aval_atuadores[3] >= 1)
      Serial.println("LIGADO");
    else
      Serial.println("DESLIGADO");
    Serial.print("CMD - VENTILADOR: ");
    Serial.print(aval_atuadores[2]);
    Serial.println(" %");
    Serial.print("CMD - FITA LED: ");
    Serial.print(aval_atuadores[1]);
    Serial.println(" %");
    Serial.print("CMD - VALVULA: ");
    Serial.print(aval_atuadores[0]);
    Serial.println(" %");
    Serial.print("CMD - HEATER: ");
    Serial.print(aval_atuadores[4]);
    Serial.println(" %");
}


void iniEEPROM()
{
  estadoEEPROM = false;

  if (i2ceeprom.begin(0x57))
  {
    Serial.println("Found I2C EEPROM");
    estadoEEPROM = true;
  }
  else
    Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
}

// Save the receips
void salvaEEPROM(float* setpoint, bool modoOperacao)
{
  int i;
  if (estadoEEPROM) {
    for(i = 0; i < 10; i++)
    {
      i2ceeprom.writeObject(4*i, setpoint[i]);
      Serial.print("Salvando na memoria: ");
      Serial.println(i);
    }
    i2ceeprom.writeObject(4*10, modoOperacao);
    Serial.println(modoOperacao);
  }
      
}
bool carregaEEPROM(float* setpoint)
{
  int i;
  bool modoOperacao;
  if (estadoEEPROM) {
    for(i = 0; i < 10; i++)
    {
      Serial.print("Lendo na memoria: ");
      Serial.println(i);
      i2ceeprom.readObject(4*i, setpoint[i]);
    }
    i2ceeprom.readObject(4*10, modoOperacao);
  }
  return modoOperacao;
}