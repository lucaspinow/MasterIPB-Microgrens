/* BIBLIOTECA DE CONTROLE */

#ifndef CONTROLE_H
#define CONTROLE_H

/* BIBLIOTECAS NECESSÁRIAS*/
#include <Arduino.h>

/* DEFINIÇÕES DE PARAMETROS */
void copiaHora(int* val_horario1, int* val_horarioBuffer1);
void verificaTurno(int* val_horario);

// Classe 'Controls'
class Controls {
public:
    Controls(float* estadoAtuadores, float* sval_sensores, float* val_setpoint, bool* modoOperacao, float* cmd_atuadores, int* val_horario, int* val_horarioBuffer);  

    void begin();           // Inicialização
    void controleGeral();   // Controle de Iluminação, Ventilação e Umidade
    bool turno;              // Indica o turno atual

private:
    float* estadoAtuadores;  // Ponteiro para estado dos atuadores
    float* sval_sensores;    // Ponteiro para estado dos sensores
    float* val_setpoint;     // Ponteiro para estado do setpoint
    float* cmd_atuadores;    // Ponteiro para cmd dos atuadores
    bool* modoOperacao;       // Indica o modo de operacao AUTOMÁTICO = 1 ou MANUAL = 0
    int* val_horarioBuffer;  // Indica o horário em que comecou as operacoes
    int* val_horarioBuffer2; // Indica o horário em que comecou as operacoes
    int* val_horario;        // Indica o horário atual
    
};

/* DEFINITION OF ALARM CLASS*/
class Alarme {
public:
    Alarme(int* horarioLigar, int* horarioDesligar);
    void ativar();  // Ativa o alarme
    void desativar();   // Desativa o alarme
    void atualizarHorarioLigar(int* novoHorarioLigar); // Atualiza o horário de ligar
    void atualizarHorarioDesligar(int* novoHorarioDesligar); // Atualiza o horário de desligar
    bool verificarAtivacao(int* horarioAtual); // Verifica se o alarme está ativado e se deve ser acionado com base no horário atual

private:
    int horarioLigar[3];
    int horarioDesligar[3];
    bool ativado;
};

#endif
