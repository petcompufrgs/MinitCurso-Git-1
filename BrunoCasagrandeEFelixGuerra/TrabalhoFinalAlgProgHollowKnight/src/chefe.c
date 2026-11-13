#include "chefe.h"
#include <math.h>
#define CHEFE_LARGURA 48.0f
#define CHEFE_ALTURA 63.0f
#define CHEFE_GRAVIDADE 1100.0f
#define CHEFE_VELOCIDADE 120.0f
#define CHEFE_INVENCIBILIDADE 0.6f

static Rectangle tileRect(int linha, int coluna) {
    return (Rectangle){ (float)(coluna * TILE_SIZE), (float)(linha * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
}

static void resolverColisaoChefeX(Chefe *chefe, const Mapa *mapa) {
    int minCol = (int)(chefe->hitbox.x / TILE_SIZE) - 1;
    int maxCol = (int)((chefe->hitbox.x + chefe->hitbox.width) / TILE_SIZE) + 1;
    int minRow = (int)(chefe->hitbox.y / TILE_SIZE) - 1;
    int maxRow = (int)((chefe->hitbox.y + chefe->hitbox.height) / TILE_SIZE) + 1;

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            if (!mapaTileSolido(mapa, row, col)) continue;
            Rectangle bloco = tileRect(row, col);
            if (!CheckCollisionRecs(chefe->hitbox, bloco)) continue;

            if (chefe->vel.x > 0.0f) chefe->pos.x = bloco.x - chefe->hitbox.width;
            if (chefe->vel.x < 0.0f) chefe->pos.x = bloco.x + bloco.width;
            chefe->vel.x = 0.0f;
            chefe->hitbox.x = chefe->pos.x;
        }
    }
}

static void resolverColisaoChefeY(Chefe *chefe, const Mapa *mapa) {
    int minCol = (int)(chefe->hitbox.x / TILE_SIZE) - 1;
    int maxCol = (int)((chefe->hitbox.x + chefe->hitbox.width) / TILE_SIZE) + 1;
    int minRow = (int)(chefe->hitbox.y / TILE_SIZE) - 1;
    int maxRow = (int)((chefe->hitbox.y + chefe->hitbox.height) / TILE_SIZE) + 1;

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            if (!mapaTileSolido(mapa, row, col)) continue;
            Rectangle bloco = tileRect(row, col);
            if (!CheckCollisionRecs(chefe->hitbox, bloco)) continue;

            if (chefe->vel.y > 0.0f) chefe->pos.y = bloco.y - chefe->hitbox.height;
            if (chefe->vel.y < 0.0f) chefe->pos.y = bloco.y + bloco.height;
            chefe->vel.y = 0.0f;
            chefe->hitbox.y = chefe->pos.y;
        }
    }
}

void inicializarChefeDoMapa(const Mapa *mapa, Chefe *chefe) {
    bool encontrou = false;

    for (int linha = 0; linha < MAP_ROWS && !encontrou; linha++) {
        for (int coluna = 0; coluna < MAP_COLS; coluna++) {
            if (mapa->tiles[linha][coluna] != 'C') continue;

            chefe->pos = (Vector2){ (float)(coluna * TILE_SIZE), (float)(linha * TILE_SIZE - 16) };
            encontrou = true;
            break;
        }
    }

    if (!encontrou) {
        chefe->pos = (Vector2){ 900.0f, 100.0f };
    }

    chefe->vel = (Vector2){ 0.0f, 0.0f };
    chefe->vidas = 5;
    chefe->dano = 2;
    chefe->ativo = true;
    chefe->tipoAtaqueAtual = 0;
    chefe->timerAtaque = 0.0f;
    chefe->timerInvulneravel = 0.0f;
    chefe->timerDecisao = 1.0f;
    chefe->hitbox = (Rectangle){ chefe->pos.x, chefe->pos.y, CHEFE_LARGURA, CHEFE_ALTURA };
    chefe->projetilAtivo = false;
    chefe->hitboxProjetil = (Rectangle){ 0 };
    chefe->velProjetil = (Vector2){ 0.0f, 0.0f };
}

void atualizarChefe(Chefe *chefe, const Mapa *mapa, Jogador *jogador, int *pontuacao, float dt) {
    if (!chefe->ativo) return;

    chefe->timerAtaque -= dt;
    chefe->timerInvulneravel -= dt;
    chefe->timerDecisao -= dt;


    float dx = jogador->pos.x - chefe->pos.x;
    if (fabsf(chefe->vel.x) > CHEFE_VELOCIDADE) {
        float desaceleracao = 500.0f * dt;
        if (chefe->vel.x > 0.0f) {
            chefe->vel.x = fmaxf(chefe->vel.x - desaceleracao, CHEFE_VELOCIDADE);
        } else {
            chefe->vel.x = fminf(chefe->vel.x + desaceleracao, -CHEFE_VELOCIDADE);
        }
    } else {
        if (dx > 8.0f) chefe->vel.x = CHEFE_VELOCIDADE;
        else if (dx < -8.0f) chefe->vel.x = -CHEFE_VELOCIDADE;
        else chefe->vel.x = 0.0f;
    }

    chefe->vel.y += CHEFE_GRAVIDADE * dt;

    chefe->pos.x += chefe->vel.x * dt;
    chefe->hitbox.x = chefe->pos.x;
    resolverColisaoChefeX(chefe, mapa);

    chefe->pos.y += chefe->vel.y * dt;
    chefe->hitbox.y = chefe->pos.y;
    resolverColisaoChefeY(chefe, mapa);

    if (fabsf(chefe->pos.x - jogador->pos.x) < 100.0f) {//ataque de investida em que o chefe fica parado e se move rapidamente em direção ao jogador
        if (chefe->timerAtaque <= 0.0f) {
            chefe->vel.x = (jogador->pos.x < chefe->pos.x) ? -300.0f : 300.0f;
            chefe->timerAtaque = 5.0f;
        }
    
    } else {
        if (!chefe->projetilAtivo && chefe->timerAtaque <= 0.0f) {
            chefe->projetilAtivo = true;
            chefe->hitboxProjetil = (Rectangle){ chefe->hitbox.x + 16.0f, chefe->hitbox.y + 20.0f, 16.0f, 16.0f };
            chefe->velProjetil = (Vector2){ (jogador->hitbox.x < chefe->hitbox.x) ? -260.0f : 260.0f, 0.0f };
            chefe->timerAtaque = 1.1f;
        }
    }
    if (CheckCollisionRecs(chefe->hitbox, jogador->hitbox)) {
            aplicarDanoJogador(jogador, chefe->dano);
        }
    

    if (chefe->projetilAtivo) {
        chefe->hitboxProjetil.x += chefe->velProjetil.x * dt;
        if (CheckCollisionRecs(chefe->hitboxProjetil, jogador->hitbox)) {
            aplicarDanoJogador(jogador, chefe->dano);
            chefe->projetilAtivo = false;
        }
        if (chefe->hitboxProjetil.x < 0.0f || chefe->hitboxProjetil.x > MAP_COLS * TILE_SIZE) {
            chefe->projetilAtivo = false;
        }
    }
    if (chefe->timerInvulneravel <= 0.0f) {
        if (jogadorAtaqueAtivo(jogador) && CheckCollisionRecs(chefe->hitbox, jogadorHitboxAtaque(jogador))) {
            chefe->vidas -= jogador->dano;
            jogador->energia += jogador->energiaMax * 0.1f; // Recupera 10% da energia máxima do jogador ao acertar o chefe
            if (jogador->energia > jogador->energiaMax) {
                jogador->energia = jogador->energiaMax;
            }
            chefe->timerInvulneravel = CHEFE_INVENCIBILIDADE;
        } else if (jogadorHabilidadeAtiva(jogador) && CheckCollisionRecs(chefe->hitbox, jogadorHitboxHabilidade(jogador))) {
            chefe->vidas -= jogador->dano + 1;
            chefe->timerInvulneravel = CHEFE_INVENCIBILIDADE;
        }
    }

    if (chefe->vidas <= 0) {
        chefe->ativo = false;
        *pontuacao += 500;
        return;
    }

    
}

void desenharChefe(const Chefe *chefe) {
    if (!chefe->ativo) return;

    DrawRectangleRec(chefe->hitbox, PURPLE);
    DrawRectangle((int)chefe->hitbox.x, (int)chefe->hitbox.y - 10, chefe->vidas * 10, 6, RED);

    if (chefe->projetilAtivo) {
        DrawRectangleRec(chefe->hitboxProjetil, ORANGE);
    }
}
