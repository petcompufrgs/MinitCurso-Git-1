#ifndef SAVE_H
#define SAVE_H

#include "amuleto.h"
#include "habilidade.h"
#include <stdbool.h>

typedef struct {
    int faseAtual;
    int vidas;
    int energia;
    int pontuacao;
    int totalAmuletos;
    int totalHabilidades;
    int amuletosColetados[MAX_AMULETOS];
    int amuletosEquipados[MAX_AMULETOS];
    int habilidadesColetadas[MAX_HABILIDADES];
    int habilidadesEquipadas[MAX_HABILIDADES];
} GameSave;

/**
 * Salva progresso em arquivo texto.
 */
bool salvarJogo(const char *arquivo, const GameSave *save);

/**
 * Carrega progresso salvo.
 */
bool carregarJogo(const char *arquivo, GameSave *save);

#endif
