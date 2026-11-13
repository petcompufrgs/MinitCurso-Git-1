#ifndef CHEFE_H
#define CHEFE_H

#include "jogador.h"
#include "mapa.h"
#include <stdbool.h>

typedef struct {
    Vector2 pos;
    Vector2 vel;
    int vidas;
    int dano;
    bool ativo;
    int tipoAtaqueAtual;
    float timerAtaque;
    float timerInvulneravel;
    float timerDecisao;
    Rectangle hitbox;
    bool projetilAtivo;
    Rectangle hitboxProjetil;
    Vector2 velProjetil;
} Chefe;

/**
 * Inicializa chefe no ponto 'C' do mapa.
 */
void inicializarChefeDoMapa(const Mapa *mapa, Chefe *chefe);

/**
 * Atualiza IA do chefe, ataques e colisões.
 */
void atualizarChefe(Chefe *chefe, const Mapa *mapa, Jogador *jogador, int *pontuacao, float dt);

/**
 * Renderiza chefe e projéteis.
 */
void desenharChefe(const Chefe *chefe);

#endif
