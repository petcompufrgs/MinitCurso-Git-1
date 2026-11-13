#ifndef HUD_H
#define HUD_H

#include "jogador.h"

/**
 * Desenha vidas, energia, pontuação e fase.
 */
void desenharHud(const Jogador *jogador, int pontuacao, int faseAtual);

#endif
