#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"
#include <stdbool.h>

#define MAP_ROWS 16
#define MAP_COLS 150
#define TILE_SIZE 32
#define PAREDES (Color){ 10, 13, 20, 255 }
#define FUNDO (Color){30, 35, 60, 255 }
#define BRANCO (Color){184, 174, 194, 255 }
#define FUNDOMAISESCURO (Color){ 20, 25, 40, 255 }
/**
 * Estrutura que guarda os tiles do mapa e pontos relevantes lidos do arquivo.
 */
typedef struct {
    char tiles[MAP_ROWS][MAP_COLS];
    int largura;
    int altura;
    Vector2 spawnJogador;
    bool temSpawnJogador;
} Mapa;

/**
 * Lê o arquivo de mapa em texto e preenche a matriz de tiles.
 * Retorna true em caso de sucesso.
 */
bool carregarMapa(Mapa *mapa, const char *arquivo);

void PintarFundo(Mapa *mapa, int x, int y);
/* Função recursiva para preencher o fundo do mapa a partir do ponto de spawn do jogador.
 * Marca os tiles de fundo com 'F' para serem desenhados posteriormente.
 */

/**
 * Desenha os tiles do mapa no mundo 2D.
 */
void desenharMapa(const Mapa *mapa);

/**
 * Informa se o tile da posição (linha, coluna) é sólido.
 */
bool mapaTileSolido(const Mapa *mapa, int linha, int coluna);

/**
 * Retorna o retângulo de mundo correspondente a um tile.
 */
Rectangle mapaTileRect(int linha, int coluna);

#endif
