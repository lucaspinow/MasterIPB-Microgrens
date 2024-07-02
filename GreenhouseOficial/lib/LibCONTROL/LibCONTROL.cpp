/* FUNÇÕES DA BIBLIOTECA DE CONTROLE */

#include "LibCONTROL.h"


Controls::Controls(float* estadoAtuadores, float* sval_sensores, float* val_setpoint, bool* modoOperacao, float* cmd_atuadores, int* val_horario, int* val_horarioBuffer)
    : estadoAtuadores(estadoAtuadores), sval_sensores(sval_sensores), val_setpoint(val_setpoint), modoOperacao(modoOperacao), cmd_atuadores(cmd_atuadores), val_horario(val_horario), val_horarioBuffer(val_horarioBuffer) {}

/* VALOR DE CADA POSIÇÃO - sval_sensores
  [0] - leitura umidade do solo 
  [1] - leitura temperatura do ar 
  [2] - leitura umidade do ar 
  [3] - leitura luminosidade
*/

/* VALOR DE CADA POSIÇÃO - estadoAtuadores
  [0] - valvula
  [1] - led 
  [2] - ventilador 
  [3] - difusor
  [4] - heater
*/

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

// Criação de instâncias específicas de alarme
int horarioLigarLuz[3]        = {7, 0, 0};           
int horarioDesligarLuz[3]     = {20, 0, 0};       
int horarioLigarIrrig[3]      = {7, 0, 0};            
int horarioDesligarIrrig[3]   = {20, 0, 0};     
int horarioLigarVent[3]       = {7, 0, 0};             
int horarioDesligarVent[3]    = {20, 0, 0};      
int horarioLigarDifusor[3]    = {7, 0, 0};       
int horarioDesligarDifusor[3] = {20, 0, 0};
int horarioTimerIrrig[3] = {0, 0, 0};   

int horarioInicioMorning[3] = {7, 0, 0};       
int horarioInicioNight[3]   = {19, 0, 0};

// Criação de instâncias específicas de alarme
Alarme alarmeLuz(horarioLigarLuz, horarioDesligarLuz);

/* VARIAVEIS GERAIS - HISTERESE E FLAGS */
int histUmiAr = 3;    // Histerese da Umidade do AR em %
float histTempAr = 0.3;   // Histerese da Temperatura do AR em celsius
int histIrrig = 1;    // Histerese da Irrigação em %
float histCooler = 1;   // Histerese da Temperatura para ligar o Cooler
float vazao = 1;      // Vazão da bomba de agua - Litro/min
unsigned long timeWaitIrrig = 300000; // 5 minutos com a umidade baixa para acionar a bomba
unsigned long deadTimeIrrig = 3600000; // 1h de espera para verificar a umidade do solo
unsigned long timeOnIrrig = 10000; // Liga por 10 segundos
unsigned long lastTime;

int flagAuto = 0;
bool indicadorMoningNight = false;
bool flagIrrig = false;
bool flagDeadTimeIrrig = false;
bool flagFanHeater = false;
bool setTemp = false;
bool setVent = false;
bool setDif = false;
bool setBomba = false;
bool lastTurno;
bool flagPassouDia = false;


void verificaTurno(int *val_horario)
{

  // Condição para verificar se esta dentro do range para ligar
    if (val_horario[3] < horarioInicioMorning[0]) 
      indicadorMoningNight = false;
    else if (val_horario[3] > horarioInicioMorning[0])
      indicadorMoningNight = true;
    else if (val_horario[4] < horarioInicioMorning[1]) 
      indicadorMoningNight = false;
    else if (val_horario[4] > horarioInicioMorning[1])
      indicadorMoningNight = true;
    else if (val_horario[5] < horarioInicioMorning[2]) 
      indicadorMoningNight = false;
    else
      indicadorMoningNight = true;

    // Condição para verificar se esta dentro do range para desligar
    if (val_horario[3] > horarioInicioNight[0]) 
      indicadorMoningNight = false;
    else if ((val_horario[3] == horarioInicioNight[0]) && (val_horario[4] > horarioInicioNight[1])) 
      indicadorMoningNight = false;
    else if ((val_horario[3] == horarioInicioNight[0]) && (val_horario[4] == horarioInicioNight[1]) && (val_horario[5] > horarioInicioNight[2])) 
      indicadorMoningNight = false;
    /*if (indicadorMoningNight == true)
      Serial.println("Turno: Dia");
    else
      Serial.println("Turno: Noite");*/
}

void copiaHora(int* val_horario1, int* val_horarioBuffer1)
{
  val_horarioBuffer1[0] = val_horario1[0];
  val_horarioBuffer1[1] = val_horario1[1];
  val_horarioBuffer1[2] = val_horario1[2];
}

void somaHora(int* val_horarioBuffer1, float minutos, int* val_horarioNovo)
{
  float segundos = 0;
  int aux;

  aux = minutos;

  if ((float)(minutos - aux) != 0)
    segundos = (float)(minutos - aux) * 60;

  val_horarioNovo[0] = val_horarioBuffer1[0];
  val_horarioNovo[1] = val_horarioBuffer1[1] + aux;
  val_horarioNovo[2] = val_horarioBuffer1[2] + segundos;

  while (val_horarioNovo[2]/60 >= 1)
  {
    val_horarioNovo[1] = val_horarioNovo[1] + 1;
    val_horarioNovo[2] = val_horarioNovo[2] - 60;
  }

  while (val_horarioNovo[1]/60 >= 1)
  {
    val_horarioNovo[0] = val_horarioNovo[0] + 1;
    val_horarioNovo[1] = val_horarioNovo[1] - 60;
  }

  if (val_horarioNovo[0] >= 24)
  {
    val_horarioNovo[0] = val_horarioNovo[0] - 24;
    flagPassouDia = true;
  }
  else
    flagPassouDia = false;
    
}

void printSerialVetor(int* vetor)
{
  Serial.print(vetor[0], DEC);
  Serial.print(":");
  Serial.print(vetor[1], DEC);
  Serial.print(":");
  Serial.println(vetor[2], DEC);
}

void Controls::begin()
{
  // Deixa os alarmes desativados
  alarmeLuz.desativar();            
}

void Controls::controleGeral()
{
  unsigned long currentTime;
  currentTime = millis();
  verificaTurno(val_horario);
  Controls::turno = indicadorMoningNight;

  if (lastTurno != indicadorMoningNight || *modoOperacao == false) // RESET DE VARIAVEIS NA TROCA DO TURNO
  {
    lastTurno = indicadorMoningNight;
    flagAuto = 0;
    flagDeadTimeIrrig = false;
    flagIrrig = false;
    setTemp   = false;
    setVent   = false;
    setDif    = false;
    setBomba  = false;
  }
  
  // Modo Automático
  if (*modoOperacao)
  {
    if (indicadorMoningNight == true) // MORNING
    { 
      if (flagAuto == 0)
      {   
          somaHora(horarioLigarLuz, val_setpoint[3], horarioDesligarLuz); // Realiza o calculo do tempo
          alarmeLuz.atualizarHorarioDesligar(horarioDesligarLuz); // Atualiza os horarios de desligamento
          alarmeLuz.ativar();   // Ativa os alarmes             
          flagAuto = 1;
      }

      /* CONTROLE - ILUMINAÇÃO */
      if (alarmeLuz.verificarAtivacao(val_horario)) 
        estadoAtuadores[1] = val_setpoint[4];
      else
        estadoAtuadores[1] = 0;

      /* CONTROLE - IRRIGACAO */
      // 1 - Condicional para ligar a valvula
      if ((sval_sensores[0] <= (val_setpoint[0] - histIrrig)) && (flagDeadTimeIrrig == false) && (setBomba == false))
      {
        // Espera pelo tempo necessario
        if (flagIrrig == false)
        {
          flagIrrig = true;
          lastTime = currentTime;
        }
        if (currentTime - lastTime >= timeWaitIrrig)
        {
          setBomba = true;
          lastTime = currentTime;
        }
      }
      else
        flagIrrig = false;
      // 2 - Valvula ligada, espera-se o tempo necessário para desliga-la
      if ((setBomba == true) && ((currentTime - lastTime) >= timeOnIrrig))
      {
        setBomba = false;
        lastTime = currentTime;
        flagDeadTimeIrrig = true;
      }
      // 3 - Valvula desligada, espera-se o tempo de deadTime para ter um controle melhor da umidade
      if (flagDeadTimeIrrig == true && ((currentTime - lastTime) >= deadTimeIrrig))
        flagDeadTimeIrrig = false;

      if (setBomba == true)
        estadoAtuadores[0] = 100;
      else
        estadoAtuadores[0] = 0;

      /* CONTROLE - VENTILAÇÃO */
      if ((sval_sensores[1] >= (val_setpoint[1] + histCooler)))
        setVent = true;
      if ((sval_sensores[1] <= (val_setpoint[1])))
        setVent = false;

      if (setVent == true)
        estadoAtuadores[2] = 100;
      else
        estadoAtuadores[2] = 0; 

      /* CONTROLE - DIFUSOR */
      if ((sval_sensores[2] <= (val_setpoint[2] - histUmiAr)) && setVent == false)
        setDif = true;
      if ((sval_sensores[2] >= (val_setpoint[2])) || setVent == true)
        setDif = false;

      if (setDif == true)
        estadoAtuadores[3] = 100;
      else
        estadoAtuadores[3] = 0;

      /* CONTROLE - HEATER */
      if ((sval_sensores[1] <= (val_setpoint[1] - histTempAr)))
        setTemp = true;
      
      if ((sval_sensores[1] >= (val_setpoint[1])))
        setTemp = false;
      
      if (setTemp == true)
        estadoAtuadores[4] = 100;
      else
        estadoAtuadores[4] = 0;
    }
    else
    {
      if (flagAuto == 0)
      {   
          somaHora(horarioLigarLuz, val_setpoint[5], horarioDesligarLuz); // Realiza o calculo do tempo
          alarmeLuz.atualizarHorarioDesligar(horarioDesligarLuz); // Atualiza os horarios de desligamento
          alarmeLuz.ativar();   // Ativa os alarmes             
          flagAuto = 1;
      }

      /* CONTROLE - ILUMINAÇÃO */
      if (alarmeLuz.verificarAtivacao(val_horario)) 
        estadoAtuadores[1] = val_setpoint[9];
      else
        estadoAtuadores[1] = 0;

      /* CONTROLE - IRRIGACAO */
      // 1 - Condicional para ligar a valvula
      if ((sval_sensores[0] <= (val_setpoint[5] - histIrrig)) && (flagDeadTimeIrrig == false) && (setBomba == false))
      {
        // Espera pelo tempo necessario
        if (flagIrrig == false)
        {
          flagIrrig = true;
          lastTime = currentTime;
        }
        if (currentTime - lastTime >= timeWaitIrrig)
        {
          setBomba = true;
          lastTime = currentTime;
        }
      }
      else
        flagIrrig = false;
      // 2 - Valvula ligada, espera-se o tempo necessário para desliga-la
      if ((setBomba == true) && ((currentTime - lastTime) >= timeOnIrrig))
      {
        setBomba = false;
        lastTime = currentTime;
        flagDeadTimeIrrig = true;
      }
      // 3 - Valvula desligada, espera-se o tempo de deadTime para ter um controle melhor da umidade
      if (flagDeadTimeIrrig == true && ((currentTime - lastTime) >= deadTimeIrrig))
        flagDeadTimeIrrig = false;

      if (setBomba == true)
        estadoAtuadores[0] = 100;
      else
        estadoAtuadores[0] = 0;

      /* CONTROLE - VENTILAÇÃO */
      if ((sval_sensores[1] >= (val_setpoint[6] + histCooler)))
        setVent = true;
      if ((sval_sensores[1] <= (val_setpoint[6])))
        setVent = false;

      if (setVent == true)
        estadoAtuadores[2] = 100;
      else
        estadoAtuadores[2] = 0; 

      /* CONTROLE - DIFUSOR */
      if ((sval_sensores[2] <= (val_setpoint[7] - histUmiAr)) && setVent == false)
        setDif = true;
      if ((sval_sensores[2] >= (val_setpoint[7])) || setVent == true)
        setDif = false;

      if (setDif == true)
        estadoAtuadores[3] = 100;
      else
        estadoAtuadores[3] = 0;

      /* CONTROLE - HEATER */
      if ((sval_sensores[1] <= (val_setpoint[6] - histTempAr)))
        setTemp = true;
      
      if ((sval_sensores[1] >= (val_setpoint[6])))
        setTemp = false;
      
      if (setTemp == true)
        estadoAtuadores[4] = 100;
      else
        estadoAtuadores[4] = 0;
    }
  }
  else 
  {
    // Modo Manual
    if (flagAuto == 1)
    {
      // Deixa os alarmes desativados
      alarmeLuz.desativar();
      flagAuto = 0;
    }
      
    estadoAtuadores[0] = cmd_atuadores[0]; // Válvula
    estadoAtuadores[1] = cmd_atuadores[1]; // LED
    estadoAtuadores[2] = cmd_atuadores[2]; // Ventilador
    estadoAtuadores[3] = cmd_atuadores[3]; // Difusor
    estadoAtuadores[4] = cmd_atuadores[4]; // Heater
  }
}


Alarme::Alarme(int* horarioLigar, int* horarioDesligar) {
    for (int i = 0; i < 3; ++i) {
        this->horarioLigar[i] = horarioLigar[i];
        this->horarioDesligar[i] = horarioDesligar[i];
    }
    ativado = false;
}

// Ativa o alarme
void Alarme::ativar() {
    ativado = true;
}

// Desativa o alarme
void Alarme::desativar() {
    ativado = false;
}

// Atualiza o horário de ligar
void Alarme::atualizarHorarioLigar(int* novoHorarioLigar) {
    for (int i = 0; i < 3; ++i) {
        horarioLigar[i] = novoHorarioLigar[i];
    }
}

// Atualiza o horário de desligar
void Alarme::atualizarHorarioDesligar(int* novoHorarioDesligar) {
    for (int i = 0; i < 3; ++i) {
        horarioDesligar[i] = novoHorarioDesligar[i];
    }
}

// Verifica se o alarme está ativado e se deve ser acionado com base no horário atual
bool Alarme::verificarAtivacao(int* horarioAtual) {
    bool ligarFlag, desligarFlag;

    if (!ativado) 
      return false;

    if (flagPassouDia == true)
    {
      if (horarioAtual[3] < horarioInicioNight[0])
        flagPassouDia = false;
      
      ligarFlag = true;
      desligarFlag = true;
    }
    else  
    {
      // Condição para verificar se esta dentro do range para ligar
      if (horarioAtual[3] < horarioLigar[0]) 
        ligarFlag = false;
      else if (horarioAtual[3] > horarioLigar[0])
        ligarFlag = true;
      else if (horarioAtual[4] < horarioLigar[1]) 
        ligarFlag = false;
      else if (horarioAtual[4] > horarioLigar[1])
        ligarFlag = true;
      else if (horarioAtual[5] < horarioLigar[2]) 
        ligarFlag = false;
      else
        ligarFlag = true;

    // Condição para verificar se esta dentro do range para desligar
    
      if (horarioAtual[3] > horarioDesligar[0]) 
        desligarFlag = false;
      else if (horarioAtual[3] < horarioDesligar[0])
        desligarFlag = true;
      else if (horarioAtual[4] > horarioDesligar[1]) 
        desligarFlag = false;
      else if (horarioAtual[4] < horarioDesligar[1])
        desligarFlag = true;
      else if (horarioAtual[5] > horarioDesligar[2]) 
        desligarFlag = false;
      else
        desligarFlag = true;
    }
    
    return (ligarFlag && desligarFlag);
}