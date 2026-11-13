#ifndef INIMIGO_H
#define INIMIGO_H

#include "jogador.h"
#include "mapa.h"
#include <stdbool.h>

#define MAX_INIMIGOS 10

typedef struct {
    Vector2 pos;
    Vector2 vel;
    int vidas;
    int dano;
    bool ativo;
    bool olhandoDireita;
    float timerMovimento;
    float timerInvulneravel;
    Rectangle hitbox;
    int direcaoAtual;
} Inimigo;

/**
 * Lê os pontos 'M' do mapa e instancia inimigos.
 */
void inicializarInimigosDoMapa(const Mapa *mapa, Inimigo inimigos[], int *totalInimigos);

/**
 * Atualiza IA, movimentação e colisões com jogador/ataques.
 */
void atualizarInimigos(Inimigo inimigos[], int totalInimigos, const Mapa *mapa, Jogador *jogador, int *pontuacao, float dt);

/**
 * Renderiza inimigos ativos.
 */
void desenharInimigos(const Inimigo inimigos[], int totalInimigos);

/**
 * Retorna quantos inimigos ainda estão ativos.
 */
int contarInimigosAtivos(const Inimigo inimigos[], int totalInimigos);

#endif
