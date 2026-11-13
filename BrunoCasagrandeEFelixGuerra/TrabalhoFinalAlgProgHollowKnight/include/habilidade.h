#ifndef HABILIDADE_H
#define HABILIDADE_H

#include "jogador.h"
#include "mapa.h"
#include <stdbool.h>

#define MAX_HABILIDADES 16

typedef enum {
    HABILIDADE_NENHUMA = 0,
    HABILIDADE_ONDA = 1
} TipoHabilidade;

/**
 * Item de habilidade coletável e equipável.
 */
typedef struct {
    TipoHabilidade tipo;
    Vector2 pos;
    bool coletada;
    bool equipada;
    Rectangle hitbox;
} Habilidade;

/**
 * Inicializa habilidades encontradas no mapa (tile 'H').
 */
void inicializarHabilidadesDoMapa(const Mapa *mapa, Habilidade habilidades[], int *totalHabilidades);

/**
 * Atualiza a coleta por colisão com o jogador.
 */
void atualizarColetaHabilidades(Habilidade habilidades[], int totalHabilidades, const Jogador *jogador);

/**
 * Alterna equipamento da primeira habilidade coletada.
 */
void alternarEquipamentoHabilidade(Habilidade habilidades[], int totalHabilidades, Jogador *jogador);

/**
 * Renderiza habilidades ainda não coletadas.
 */
void desenharHabilidades(const Habilidade habilidades[], int totalHabilidades);

#endif
