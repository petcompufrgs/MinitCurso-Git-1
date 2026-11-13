#include "jogador.h"

#define VELOCIDADE_X 240.0f
#define FORCA_PULO 700.0f
#define GRAVIDADE 1500.0f
#define ALCANCE_ATAQUE 50.0f
#define CUSTO_CURA 50
#define CUSTO_HABILIDADE 30

static void atualizarHitbox(Jogador *jogador) {
    jogador->hitbox.x = jogador->pos.x;
    jogador->hitbox.y = jogador->pos.y;
    jogador->hitbox.width = JOGADOR_LARGURA;
    jogador->hitbox.height = JOGADOR_ALTURA;
}

static void resolverColisaoHorizontal(Jogador *jogador, const Mapa *mapa) {
    int minCol = (int)(jogador->hitbox.x / TILE_SIZE) - 1;
    int maxCol = (int)((jogador->hitbox.x + jogador->hitbox.width) / TILE_SIZE) + 1;
    int minRow = (int)(jogador->hitbox.y / TILE_SIZE) - 1;
    int maxRow = (int)((jogador->hitbox.y + jogador->hitbox.height) / TILE_SIZE) + 1;

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            if (!mapaTileSolido(mapa, row, col)) {
                continue;
            }

            Rectangle tileRect = mapaTileRect(row, col);
            if (!CheckCollisionRecs(jogador->hitbox, tileRect)) {
                continue;
            }

            if (jogador->vel.x > 0.0f) {
                jogador->pos.x = tileRect.x - jogador->hitbox.width;
            } else if (jogador->vel.x < 0.0f) {
                jogador->pos.x = tileRect.x + tileRect.width;
            }

            jogador->vel.x = 0.0f;
            atualizarHitbox(jogador);
        }
    }
}

static void resolverColisaoVertical(Jogador *jogador, const Mapa *mapa) {
    int minCol = (int)(jogador->hitbox.x / TILE_SIZE) - 1;
    int maxCol = (int)((jogador->hitbox.x + jogador->hitbox.width) / TILE_SIZE) + 1;
    int minRow = (int)(jogador->hitbox.y / TILE_SIZE) - 1;
    int maxRow = (int)((jogador->hitbox.y + jogador->hitbox.height) / TILE_SIZE) + 1;

    jogador->noChao = false;

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            if (!mapaTileSolido(mapa, row, col)) {
                continue;
            }

            Rectangle tileRect = mapaTileRect(row, col);
            if (!CheckCollisionRecs(jogador->hitbox, tileRect)) {
                continue;
            }

            if (jogador->vel.y > 0.0f) {
                jogador->pos.y = tileRect.y - jogador->hitbox.height;
                jogador->noChao = true;
            } else if (jogador->vel.y < 0.0f) {
                jogador->pos.y = tileRect.y + tileRect.height;
            }

            jogador->vel.y = 0.0f;
            atualizarHitbox(jogador);
        }
    }
}

static void atualizarAtaque(Jogador *jogador, float dt) {
    jogador->timerAtaque -= dt;
    if (jogador->atacando) {
        if (jogador->timerAtaque <= 0.0f) {
            jogador->atacando = false;
            jogador->timerAtaque = 0.3f;
        }
    }
    else if (IsKeyPressed(KEY_S) && !jogador->atacando&& jogador->timerAtaque <= 0.0f) {
        jogador->atacando = true;
        jogador->timerAtaque = 0.18f;
    }

    

    if (jogador->atacando) {
        float ataqueX = jogador->olhandoDireita
            ? jogador->hitbox.x + jogador->hitbox.width
            : jogador->hitbox.x - ALCANCE_ATAQUE;
        jogador->hitboxAtaque = (Rectangle){
            ataqueX,
            jogador->hitbox.y + 16.0f,
            ALCANCE_ATAQUE,
            jogador->hitbox.height - 16.0f
        };
    } else {
        jogador->hitboxAtaque = (Rectangle){ 0 };
    }
}

static void atualizarCura(Jogador *jogador, float dt) {
    int custoCura = CUSTO_CURA;

    if (jogador->timerCura > 0.0f) {
        jogador->timerCura -= dt;
    }

    if (IsKeyDown(KEY_A) && jogador->timerCura <= 0.0f && jogador->vidas < jogador->vidaMax && jogador->energia >= custoCura) {
        jogador->energia -= custoCura;
        jogador->vidas += 1;
        jogador->timerCura = 0.45f;
    }
}

static void atualizarHabilidade(Jogador *jogador, float dt) {
    if (jogador->cooldownHabilidade > 0.0f) {
        jogador->cooldownHabilidade -= dt;
    }

    if (jogador->habilidadeAtiva) {
        jogador->timerHabilidade -= dt;
        if (jogador->timerHabilidade <= 0.0f) {
            jogador->habilidadeAtiva = false;
            jogador->timerHabilidade = 0.0f;
            jogador->hitboxHabilidade = (Rectangle){ 0 };
        }
    }

    if (IsKeyPressed(KEY_D) && jogador->tipoHabilidade != 0 && jogador->cooldownHabilidade <= 0.0f && jogador->energia >= CUSTO_HABILIDADE) {
        jogador->energia -= CUSTO_HABILIDADE;
        jogador->habilidadeAtiva = true;
        jogador->timerHabilidade = 0.25f;
        jogador->cooldownHabilidade = 1.0f;

        if (jogador->tipoHabilidade == 1) {
            float habilidadeX = jogador->olhandoDireita
                ? jogador->hitbox.x + jogador->hitbox.width
                : jogador->hitbox.x - 90.0f;
            jogador->hitboxHabilidade = (Rectangle){
                habilidadeX,
                jogador->hitbox.y + 4.0f,
                130.0f,
                jogador->hitbox.height + 8.0f
            };
        }
    }
}

void inicializarJogador(Jogador *jogador, Vector2 posInicial) {
    jogador->pos = posInicial;
    jogador->vel = (Vector2){ 0.0f, 0.0f };
    jogador->vidas = 5;
    jogador->vidaMax = 5;
    jogador->vidaBase = 5;
    jogador->energia = 100;
    jogador->energiaMax = 100;
    jogador->energiaBase = 100;
    jogador->dano = 1;
    jogador->danoBase = 1;
    jogador->noChao = false;
    jogador->olhandoDireita = true;
    jogador->atacando = false;
    jogador->timerAtaque = 0.0f;
    jogador->timerCura = 0.0f;
    jogador->timerInvulneravel = 0.0f;
    jogador->hitbox = (Rectangle){ posInicial.x, posInicial.y, JOGADOR_LARGURA, JOGADOR_ALTURA };
    jogador->hitboxAtaque = (Rectangle){ 0 };
    jogador->habilidadeAtiva = false;
    jogador->timerHabilidade = 0.0f;
    jogador->cooldownHabilidade = 0.0f;
    jogador->hitboxHabilidade = (Rectangle){ 0 };
    jogador->tipoHabilidade = 0;
}

void atualizarJogador(Jogador *jogador, const Mapa *mapa, float dt) {
    float direcaoX = 0.0f;

    if (IsKeyDown(KEY_RIGHT)) {
        direcaoX += 1.0f;
        jogador->olhandoDireita = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        direcaoX -= 1.0f;
        jogador->olhandoDireita = false;
    }

    jogador->vel.x = direcaoX * VELOCIDADE_X;

    if (IsKeyPressed(KEY_UP) && jogador->noChao) {
        jogador->vel.y = -FORCA_PULO;
        jogador->noChao = false;
    }

    jogador->vel.y += GRAVIDADE * dt;

    jogador->pos.x += jogador->vel.x * dt;
    atualizarHitbox(jogador);
    resolverColisaoHorizontal(jogador, mapa);

    jogador->pos.y += jogador->vel.y * dt;
    atualizarHitbox(jogador);
    resolverColisaoVertical(jogador, mapa);

    if (jogador->timerInvulneravel > 0.0f) {
        jogador->timerInvulneravel -= dt;
    }

    atualizarAtaque(jogador, dt);
    atualizarCura(jogador, dt);
    atualizarHabilidade(jogador, dt);
}

void desenharJogador(const Jogador *jogador) {
    Color corJogador = (jogador->timerInvulneravel > 0.0f) ? LIGHTGRAY : WHITE;

    Rectangle src;
    if (jogador->noChao) {
        src = (Rectangle){ 105.0f, 55.0f, 17.0f, 30.0f };
    } else {
        src = (Rectangle){ 101.0f, 105.0f, 23.0f, 30.0f };
    }

    float scale = JOGADOR_ALTURA / src.height;
    float dstW = src.width * scale;
    Rectangle dest = (Rectangle){ jogador->hitbox.x + (JOGADOR_LARGURA - dstW) * 0.5f, jogador->hitbox.y, dstW, JOGADOR_ALTURA };

    if (!jogador->olhandoDireita) {
        src.width = -src.width;
    }

    DrawTexturePro(texturaSpritesJogador, src, dest, (Vector2){ 0, 0 }, 0.0f, corJogador);

    if (jogador->atacando) {
        // Sprite de ataque na spritesheet
        Rectangle atkSrc = (Rectangle){ 177.0f, 155.0f, 30.0f, 30.0f };
        float srcW = atkSrc.width;

        // Espelha a fonte horizontalmente se necessário (ajusta src.x e depois inverte src.width)
        if (!jogador->olhandoDireita) {
            atkSrc.width = -srcW;
        }

        // A largura de destino deve corresponder à largura de hitboxAtaque
        float dstW = jogador->hitboxAtaque.width;
        float scale = dstW / srcW;
        float dstH = atkSrc.height * scale;

        // Posiciona o sprite para que sua esquerda e largura correspondam a hitboxAtaque.x/width,
        // e centraliza verticalmente dentro de hitboxAtaque
        Rectangle destAtk = (Rectangle){
            jogador->hitboxAtaque.x,
            jogador->hitboxAtaque.y + (jogador->hitboxAtaque.height - dstH) * 0.5f,
            dstW,
            dstH
        };

        DrawTexturePro(texturaSpritesJogador, atkSrc, destAtk, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }


    if (jogador->habilidadeAtiva) {
        DrawRectangleLinesEx(jogador->hitboxHabilidade, 2.0f, SKYBLUE);
    }
}

void aplicarDanoJogador(Jogador *jogador, int dano) {
    if (jogador->timerInvulneravel > 0.0f || jogador->vidas <= 0) {
        return;
    }

    jogador->vidas -= dano;
    if (jogador->vidas < 0) {
        jogador->vidas = 0;
    }
    jogador->timerInvulneravel = 1.2f;
}

bool jogadorAtaqueAtivo(const Jogador *jogador) {
    return jogador->atacando;
}

Rectangle jogadorHitboxAtaque(const Jogador *jogador) {
    return jogador->hitboxAtaque;
}

bool jogadorHabilidadeAtiva(const Jogador *jogador) {
    return jogador->habilidadeAtiva;
}

Rectangle jogadorHitboxHabilidade(const Jogador *jogador) {
    return jogador->hitboxHabilidade;
}

void definirHabilidadeJogador(Jogador *jogador, int tipoHabilidade) {
    jogador->tipoHabilidade = tipoHabilidade;
}
