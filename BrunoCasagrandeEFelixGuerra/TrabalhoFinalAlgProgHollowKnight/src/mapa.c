#include "mapa.h"

#include <stdio.h>
#include <string.h>

static bool ehCaractereValido(char c) {
    return c == 'J' || c == 'C' || c == 'M' || c == 'P' || c == 'W' || c == 'A' || c == 'H' || c == ' '|| c == 'F' || c == 'L';
}

void PintarFundo(Mapa *mapa, int x, int y)
{    if (mapa == NULL) {
        return;
    }
    if (x < 0 || x >= MAP_COLS || y < 0 || y >= MAP_ROWS) {
        return;
    }
    if (mapa->tiles[y][x] !=' ') {
        return;
    }
    mapa->tiles[y][x] = 'F';

    PintarFundo(mapa, x + 1, y);
    PintarFundo(mapa, x - 1, y);
    PintarFundo(mapa, x, y + 1);
    PintarFundo(mapa, x, y - 1);
}
bool carregarMapa(Mapa *mapa, const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    char linhaBuffer[MAP_COLS + 4];
    int linha = 0;

    if (mapa == NULL || fp == NULL) {
        if (fp != NULL) {
            fclose(fp);
        }
        return false;
    }

    mapa->largura = MAP_COLS;
    mapa->altura = MAP_ROWS;
    mapa->temSpawnJogador = false;

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            mapa->tiles[r][c] = ' ';
        }
    }


    while (linha < MAP_ROWS && fgets(linhaBuffer, sizeof(linhaBuffer), fp) != NULL) {
        size_t tamanho = strcspn(linhaBuffer, "\r\n");
        for (int coluna = 0; coluna < MAP_COLS && coluna < (int)tamanho; coluna++) {
            char valor = linhaBuffer[coluna];
            mapa->tiles[linha][coluna] = ehCaractereValido(valor) ? valor : ' ';

            if (mapa->tiles[linha][coluna] == 'J') {
                mapa->spawnJogador = (Vector2){ (float)(coluna * TILE_SIZE), (float)(linha * TILE_SIZE) };
                mapa->temSpawnJogador = true;
                mapa->tiles[linha][coluna] = ' ';
            }
            
        }
        linha++;
    }
    PintarFundo(mapa, mapa->spawnJogador.x / TILE_SIZE, mapa->spawnJogador.y / TILE_SIZE);
    fclose(fp);
    return true;
}

void desenharMapa(const Mapa *mapa) {
    if (mapa == NULL) {
        return;
    }

    for (int linha = 0; linha < MAP_ROWS; linha++) {
        for (int coluna = 0; coluna < MAP_COLS; coluna++) {
            Rectangle tileRect = mapaTileRect(linha, coluna);
            char tile = mapa->tiles[linha][coluna];

            if (tile == 'P') {
                DrawRectangleRec(tileRect,PAREDES);
            }
            else if (tile == 'F'||tile == 'A'||tile == 'H'||tile == 'C'||tile == 'M'||tile == 'C') {
                DrawRectangleRec(tileRect,FUNDO);
            }
            else if (tile == 'W') {
                DrawRectangleRec(tileRect,BRANCO);
            }
            else if (tile == 'L') {
                DrawRectangleRec(tileRect,FUNDOMAISESCURO);
            }
        }
    }
}

bool mapaTileSolido(const Mapa *mapa, int linha, int coluna) {
    if (mapa == NULL) {
        return false;
    }

    if (coluna < 0 || coluna >= MAP_COLS) {
        return true;
    }
    if (linha >= MAP_ROWS) {
        return true;
    }
    if (linha < 0) {
        return false;
    }

    return mapa->tiles[linha][coluna] == 'P';
}

Rectangle mapaTileRect(int linha, int coluna) {
    return (Rectangle){
        (float)(coluna * TILE_SIZE),
        (float)(linha * TILE_SIZE),
        (float)TILE_SIZE,
        (float)TILE_SIZE
    };
}
