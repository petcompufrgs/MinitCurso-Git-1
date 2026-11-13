#include "inimigo.h"

#define INIMIGO_LARGURA 24.0f
#define INIMIGO_ALTURA 40.0f
#define INIMIGO_VELOCIDADE 90.0f
#define INIMIGO_GRAVIDADE 1200.0f
#define INIMIGO_INVENCIBILIDADE 0.6f

static Rectangle tileRect(int linha, int coluna) {
    return (Rectangle){ (float)(coluna * TILE_SIZE), (float)(linha * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
}

static void resolverX(Inimigo *inimigo, const Mapa *mapa) {
    int minCol = (int)(inimigo->hitbox.x / TILE_SIZE) - 1;
    int maxCol = (int)((inimigo->hitbox.x + inimigo->hitbox.width) / TILE_SIZE) + 1;
    int minRow = (int)(inimigo->hitbox.y / TILE_SIZE) - 1;
    int maxRow = (int)((inimigo->hitbox.y + inimigo->hitbox.height) / TILE_SIZE) + 1;

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            if (!mapaTileSolido(mapa, row, col)) continue;
            Rectangle bloco = tileRect(row, col);
            if (!CheckCollisionRecs(inimigo->hitbox, bloco)) continue;

            if (inimigo->vel.x > 0.0f) {
                inimigo->pos.x = bloco.x - inimigo->hitbox.width;
            } else if (inimigo->vel.x < 0.0f) {
                inimigo->pos.x = bloco.x + bloco.width;
            }
            inimigo->vel.x = 0.0f;
            inimigo->direcaoAtual *= -1;
            inimigo->hitbox.x = inimigo->pos.x;
        }
    }
}

static void resolverY(Inimigo *inimigo, const Mapa *mapa) {
    int minCol = (int)(inimigo->hitbox.x / TILE_SIZE) - 1;
    int maxCol = (int)((inimigo->hitbox.x + inimigo->hitbox.width) / TILE_SIZE) + 1;
    int minRow = (int)(inimigo->hitbox.y / TILE_SIZE) - 1;
    int maxRow = (int)((inimigo->hitbox.y + inimigo->hitbox.height) / TILE_SIZE) + 1;

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            if (!mapaTileSolido(mapa, row, col)) continue;
            Rectangle bloco = tileRect(row, col);
            if (!CheckCollisionRecs(inimigo->hitbox, bloco)) continue;

            if (inimigo->vel.y > 0.0f) {
                inimigo->pos.y = bloco.y - inimigo->hitbox.height;
            } else if (inimigo->vel.y < 0.0f) {
                inimigo->pos.y = bloco.y + bloco.height;
            }
            inimigo->vel.y = 0.0f;
            inimigo->hitbox.y = inimigo->pos.y;
        }
    }
}

void inicializarInimigosDoMapa(const Mapa *mapa, Inimigo inimigos[], int *totalInimigos) {
    int total = 0;

    for (int linha = 0; linha < MAP_ROWS; linha++) {
        for (int coluna = 0; coluna < MAP_COLS; coluna++) {
            if (mapa->tiles[linha][coluna] != 'M' || total >= MAX_INIMIGOS) {
                continue;
            }

            inimigos[total].pos = (Vector2){ (float)(coluna * TILE_SIZE), (float)(linha * TILE_SIZE - 8) };
            inimigos[total].vel = (Vector2){ 0.0f, 0.0f };
            inimigos[total].vidas = 2;
            inimigos[total].dano = 1;
            inimigos[total].ativo = true;
            inimigos[total].olhandoDireita = true;
            inimigos[total].timerMovimento = 0.6f;
            inimigos[total].timerInvulneravel = 0.0f;
            inimigos[total].direcaoAtual = GetRandomValue(-1, 1);
            inimigos[total].hitbox = (Rectangle){ inimigos[total].pos.x, inimigos[total].pos.y, INIMIGO_LARGURA, INIMIGO_ALTURA };
            total++;
        }
    }

    *totalInimigos = total;
}

void atualizarInimigos(Inimigo inimigos[], int totalInimigos, const Mapa *mapa, Jogador *jogador, int *pontuacao, float dt) {
    for (int i = 0; i < totalInimigos; i++) {
        Inimigo *inimigo = &inimigos[i];
        if (!inimigo->ativo) continue;

        inimigo->timerMovimento -= dt;
        if (inimigo->timerMovimento <= 0.0f) {
            inimigo->direcaoAtual = GetRandomValue(-1, 1);
            inimigo->timerMovimento = 1.2f;
        }

        inimigo->vel.x = inimigo->direcaoAtual * INIMIGO_VELOCIDADE;
        inimigo->olhandoDireita = inimigo->vel.x >= 0.0f;
        inimigo->vel.y += INIMIGO_GRAVIDADE * dt;

        inimigo->pos.x += inimigo->vel.x * dt;
        inimigo->hitbox.x = inimigo->pos.x;
        resolverX(inimigo, mapa);

        inimigo->pos.y += inimigo->vel.y * dt;
        inimigo->hitbox.y = inimigo->pos.y;
        resolverY(inimigo, mapa);

        inimigo->timerInvulneravel -= dt;

        if (CheckCollisionRecs(inimigo->hitbox, jogador->hitbox)) {
            aplicarDanoJogador(jogador, inimigo->dano);
        }

        if (inimigo->timerInvulneravel <= 0.0f) {
            if (jogadorAtaqueAtivo(jogador) && CheckCollisionRecs(inimigo->hitbox, jogadorHitboxAtaque(jogador))) {
                inimigo->vidas -= jogador->dano;
                inimigo->timerInvulneravel = INIMIGO_INVENCIBILIDADE;
                jogador->energia += jogador->energiaMax * 0.1f;
            if (jogador->energia > jogador->energiaMax) {
                jogador->energia = jogador->energiaMax;
            }

            } else if (jogadorHabilidadeAtiva(jogador) && CheckCollisionRecs(inimigo->hitbox, jogadorHitboxHabilidade(jogador))) {
                inimigo->vidas -= jogador->dano + 1;
                inimigo->timerInvulneravel = INIMIGO_INVENCIBILIDADE;
            }
        }

        if (inimigo->vidas <= 0) {
            inimigo->ativo = false;
            *pontuacao += 100;
        }
    }
}

void desenharInimigos(const Inimigo inimigos[], int totalInimigos) {
    for (int i = 0; i < totalInimigos; i++) {
        if (!inimigos[i].ativo) continue;
        DrawRectangleRec(inimigos[i].hitbox, MAROON);
    }
}

int contarInimigosAtivos(const Inimigo inimigos[], int totalInimigos) {
    int total = 0;
    for (int i = 0; i < totalInimigos; i++) {
        if (inimigos[i].ativo) total++;
    }
    return total;
}
