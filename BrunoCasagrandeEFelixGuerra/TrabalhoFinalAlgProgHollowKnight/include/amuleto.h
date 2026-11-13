#ifndef AMULETO_H
#define AMULETO_H

#include "jogador.h"
#include "mapa.h"
#include "raylib.h"
#include <stdbool.h>

#define MAX_AMULETOS 3

typedef enum {
    AMULETO_MAIS_VIDA = 0,
    AMULETO_MAIS_DANO = 1,
    AMULETO_ENERGIA_EXTRA = 2
} TipoAmuleto;

/**
 * Item coletável com efeito permanente enquanto equipado.
 */
typedef struct {
    TipoAmuleto tipo;
    Vector2 pos;
    bool coletado;
    bool equipado;
    Rectangle hitbox;
} Amuleto;

/**
 * Cria os amuletos encontrados no mapa (tile 'A').
 */
void inicializarAmuletosDoMapa(const Mapa *mapa, Amuleto amuletos[], int *totalAmuletos, const Amuleto inventarioAmuletos[]);

/**
 * Atualiza coleta de amuletos por colisão com jogador.
 */
void atualizarColetaAmuletos(Amuleto amuletos[], int totalAmuletos, const Jogador *jogador);

/**
 * Alterna equipamento de amuletos por tipo (teclas 1,2,3 no inventário).
 */
void alternarAmuletoPorTipo(Amuleto amuletos[], int totalAmuletos, TipoAmuleto tipoDesejado);

/**
 * Recalcula atributos do jogador com base nos amuletos equipados.
 */
void aplicarEfeitosAmuletos(const Amuleto amuletos[], int totalAmuletos, Jogador *jogador);

/**
 * Renderiza amuletos não coletados no mapa.
 */
void desenharAmuletos(const Amuleto amuletos[], int totalAmuletos);

#endif
