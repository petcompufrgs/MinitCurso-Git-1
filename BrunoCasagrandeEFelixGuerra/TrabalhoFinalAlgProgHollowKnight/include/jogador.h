#ifndef JOGADOR_H
#define JOGADOR_H

#include "mapa.h"
#include "raylib.h"
#include <stdbool.h>

/**
 * Dados principais do personagem controlado pelo jogador.
 */
typedef struct {
    Vector2 pos;
    Vector2 vel;
    int vidas;
    int vidaMax;
    int vidaBase;
    int energia;
    int energiaMax;
    int energiaBase;
    int dano;
    int danoBase;
    bool noChao;
    bool olhandoDireita;
    bool atacando;
    float timerAtaque;
    float timerCura;
    float timerInvulneravel;
    Rectangle hitbox;
    Rectangle hitboxAtaque;
    bool habilidadeAtiva;
    float timerHabilidade;
    float cooldownHabilidade;
    Rectangle hitboxHabilidade;
    int tipoHabilidade;
} Jogador;

/**
 * Tamanho base do personagem no mundo.
 */
#define JOGADOR_LARGURA 30.0f
#define JOGADOR_ALTURA 60.0f

/**
 * Inicializa atributos do jogador e posiciona a hitbox.
 */
void inicializarJogador(Jogador *jogador, Vector2 posInicial);

/**
 * Processa entrada, física e colisão com os tiles sólidos do mapa.
 */
void atualizarJogador(Jogador *jogador, const Mapa *mapa, float dt);

/**
 * Renderiza o jogador no mundo.
 */
void desenharJogador(const Jogador *jogador);

// Sprite sheet do jogador carregado por main.c
extern Texture2D texturaSpritesJogador;

/**
 * Aplica dano ao jogador (respeitando invulnerabilidade curta após ser atingido).
 */
void aplicarDanoJogador(Jogador *jogador, int dano);

/**
 * Retorna true quando o hitbox de ataque básico está ativo.
 */
bool jogadorAtaqueAtivo(const Jogador *jogador);

/**
 * Retorna o hitbox do ataque básico.
 */
Rectangle jogadorHitboxAtaque(const Jogador *jogador);

/**
 * Retorna true quando o hitbox da habilidade está ativo.
 */
bool jogadorHabilidadeAtiva(const Jogador *jogador);

/**
 * Retorna o hitbox de habilidade ativa.
 */
Rectangle jogadorHitboxHabilidade(const Jogador *jogador);

/**
 * Define qual habilidade está equipada no jogador.
 */
void definirHabilidadeJogador(Jogador *jogador, int tipoHabilidade);

#endif
