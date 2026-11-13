#include "amuleto.h"

static TipoAmuleto tipoPorIndice(int indice) {
    int valor = indice % 3;
    if (valor == 0) return AMULETO_MAIS_VIDA;
    if (valor == 1) return AMULETO_MAIS_DANO;
    return AMULETO_ENERGIA_EXTRA;
}

void inicializarAmuletosDoMapa(const Mapa *mapa, Amuleto amuletos[], int *totalAmuletos, const Amuleto inventarioAmuletos[]) {
    int total = 0;
   

    for (int linha = 0; linha < MAP_ROWS; linha++) {
        for (int coluna = 0; coluna < MAP_COLS; coluna++) {
            if (mapa->tiles[linha][coluna] != 'A' || total >= MAX_AMULETOS) {
                continue;
            }
 // Encontrar o primeiro amuleto não coletado no inventário
             while(true){
        if (inventarioAmuletos[total].coletado) {
            total++;
        } else {
            break;
        }
        if(total>=3){
            return;
        }
    }
            amuletos[total].tipo = tipoPorIndice(total);
            amuletos[total].coletado = false;
            amuletos[total].equipado = false;
            amuletos[total].pos = (Vector2){ (float)(coluna * TILE_SIZE + 8), (float)(linha * TILE_SIZE + 8) };
            amuletos[total].hitbox = (Rectangle){ amuletos[total].pos.x, amuletos[total].pos.y, 16.0f, 16.0f };
            total++;
        }
    }

    *totalAmuletos = total;
}

void atualizarColetaAmuletos(Amuleto amuletos[], int totalAmuletos, const Jogador *jogador) {
    for (int i = 0; i < totalAmuletos; i++) {
        if (amuletos[i].coletado) {
            continue;
        }

        if (CheckCollisionRecs(amuletos[i].hitbox, jogador->hitbox)) {
            amuletos[i].coletado = true;
        }
    }
}

void alternarAmuletoPorTipo(Amuleto amuletos[], int totalAmuletos, TipoAmuleto tipoDesejado) {
    for (int i = 0; i < totalAmuletos; i++) {
        if (!amuletos[i].coletado || amuletos[i].tipo != tipoDesejado) {
            continue;
        }
        amuletos[i].equipado = !amuletos[i].equipado;
        break;
    }
}

void aplicarEfeitosAmuletos(const Amuleto amuletos[], int totalAmuletos, Jogador *jogador) {
    jogador->vidaMax = jogador->vidaBase;
    jogador->dano = jogador->danoBase;
    jogador->energiaMax = jogador->energiaBase;

    for (int i = 0; i < totalAmuletos; i++) {
        if (!amuletos[i].equipado) {
            continue;
        }

        if (amuletos[i].tipo == AMULETO_MAIS_VIDA) {
            jogador->vidaMax += 2;
        } else if (amuletos[i].tipo == AMULETO_MAIS_DANO) {
            jogador->dano += 1;
        } else if (amuletos[i].tipo == AMULETO_ENERGIA_EXTRA) {
            jogador->energiaMax += 50;
        }
    }

    if (jogador->vidas > jogador->vidaMax) {
        jogador->vidas = jogador->vidaMax;
    }
    if (jogador->energia > jogador->energiaMax) {
        jogador->energia = jogador->energiaMax;
    }
}

void desenharAmuletos(const Amuleto amuletos[], int totalAmuletos) {
    for (int i = 0; i < totalAmuletos; i++) {
        if (amuletos[i].coletado) {
            continue;
        }

        Color cor = GOLD;
        if (amuletos[i].tipo == AMULETO_MAIS_DANO) cor = ORANGE;
        if (amuletos[i].tipo == AMULETO_ENERGIA_EXTRA) cor = SKYBLUE;
        DrawCircleV((Vector2){ amuletos[i].hitbox.x + 8.0f, amuletos[i].hitbox.y + 8.0f }, 8.0f, cor);
    }
}
