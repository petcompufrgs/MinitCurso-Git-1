#include "habilidade.h"

void inicializarHabilidadesDoMapa(const Mapa *mapa, Habilidade habilidades[], int *totalHabilidades) {
    int total = 0;

    for (int linha = 0; linha < MAP_ROWS; linha++) {
        for (int coluna = 0; coluna < MAP_COLS; coluna++) {
            if (mapa->tiles[linha][coluna] != 'H' || total >= MAX_HABILIDADES) {
                continue;
            }

            habilidades[total].tipo = HABILIDADE_ONDA;
            habilidades[total].coletada = false;
            habilidades[total].equipada = false;
            habilidades[total].pos = (Vector2){ (float)(coluna * TILE_SIZE + 8), (float)(linha * TILE_SIZE + 8) };
            habilidades[total].hitbox = (Rectangle){ habilidades[total].pos.x, habilidades[total].pos.y, 16.0f, 16.0f };
            total++;
        }
    }

    *totalHabilidades = total;
}

void atualizarColetaHabilidades(Habilidade habilidades[], int totalHabilidades, const Jogador *jogador) {
    for (int i = 0; i < totalHabilidades; i++) {
        if (habilidades[i].coletada) {
            continue;
        }

        if (CheckCollisionRecs(habilidades[i].hitbox, jogador->hitbox)) {
            habilidades[i].coletada = true;
        }
    }
}

void alternarEquipamentoHabilidade(Habilidade habilidades[], int totalHabilidades, Jogador *jogador) {
    for (int i = 0; i < totalHabilidades; i++) {
        if (!habilidades[i].coletada) {
            continue;
        }

        habilidades[i].equipada = !habilidades[i].equipada;
        definirHabilidadeJogador(jogador, habilidades[i].equipada ? (int)habilidades[i].tipo : HABILIDADE_NENHUMA);
        break;
    }
}

void desenharHabilidades(const Habilidade habilidades[], int totalHabilidades) {
    for (int i = 0; i < totalHabilidades; i++) {
        if (habilidades[i].coletada) {
            continue;
        }
        DrawRectangleRec(habilidades[i].hitbox, SKYBLUE);
    }
}
