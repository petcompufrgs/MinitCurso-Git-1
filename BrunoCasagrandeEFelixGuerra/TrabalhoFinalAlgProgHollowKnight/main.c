#include "amuleto.h"
#include "chefe.h"
#include "habilidade.h"
#include "hud.h"
#include "inimigo.h"
#include "jogador.h"
#include "mapa.h"
#include "menu.h"
#include "raylib.h"
#include "save.h"

Texture2D texturaSpritesJogador = { 0 };

#include <stdio.h>
#include <string.h>

/**
 * Estados globais do jogo (máquina de estados principal).
 */
typedef enum {
    STATE_MENU,
    STATE_VILA,
    STATE_FASE,
    STATE_PAUSE,
    STATE_AJUDA,
    STATE_INVENTARIO,
    STATE_GAME_OVER,
    STATE_VITORIA
} GameState;

static const char *arquivoTunelPorFase(int faseAtual) {
    if (faseAtual == 1) return "assets\\mapas\\tunel1.txt";
    if (faseAtual == 2) return "assets\\mapas\\tunel2.txt";
    return "assets\\mapas\\tunel3.txt";
}

static bool carregarMapaComFallback(Mapa *mapa, const char *arquivo) {
    if (carregarMapa(mapa, arquivo)) {
        return true;
    }

    char caminhoFallback[256];
    snprintf(caminhoFallback, sizeof(caminhoFallback), "..\\%s", arquivo);
    return carregarMapa(mapa, caminhoFallback);
}

static void inicializarInventarioBase(Amuleto inventarioAmuletos[3], Habilidade inventarioHabilidades[1]) {
    for (int i = 0; i < 3; i++) {
        inventarioAmuletos[i].tipo = (TipoAmuleto)i;
        inventarioAmuletos[i].coletado = false;
        inventarioAmuletos[i].equipado = false;
    }

    inventarioHabilidades[0].tipo = HABILIDADE_ONDA;
    inventarioHabilidades[0].coletada = false;
    inventarioHabilidades[0].equipada = false;
}

static bool prepararFase(
    int faseAtual,
    Mapa *mapaFase,
    Jogador *jogador,
    Inimigo inimigos[],
    int *totalInimigos,
    Chefe *chefe,
    Amuleto amuletosMapa[],
    int *totalAmuletosMapa,
    Habilidade habilidadesMapa[],
    int *totalHabilidadesMapa,
    int vidasPersistidas,
    int energiaPersistida,
    const Amuleto inventarioAmuletos[3],
    const Habilidade inventarioHabilidades[1],
    Camera2D *camera
) {
    const char *arquivoFase = arquivoTunelPorFase(faseAtual);
    if (!carregarMapaComFallback(mapaFase, arquivoFase)) {
        return false;
    }

    inicializarInimigosDoMapa(mapaFase, inimigos, totalInimigos);
    inicializarChefeDoMapa(mapaFase, chefe);
    inicializarAmuletosDoMapa(mapaFase, amuletosMapa, totalAmuletosMapa,inventarioAmuletos);
    inicializarHabilidadesDoMapa(mapaFase, habilidadesMapa, totalHabilidadesMapa);

    Vector2 spawnPadrao = (Vector2){ 64.0f, 64.0f };
    Vector2 spawn = mapaFase->temSpawnJogador ? mapaFase->spawnJogador : spawnPadrao;
    inicializarJogador(jogador, spawn);
    jogador->vidas = vidasPersistidas;
    jogador->energia = energiaPersistida;

    for (int i = 0; i < *totalAmuletosMapa; i++) {
        TipoAmuleto tipo = amuletosMapa[i].tipo;
        amuletosMapa[i].coletado = inventarioAmuletos[tipo].coletado;
    }
    for (int i = 0; i < *totalHabilidadesMapa; i++) {
        habilidadesMapa[i].coletada = inventarioHabilidades[0].coletada;
    }

    aplicarEfeitosAmuletos(inventarioAmuletos, 3, jogador);
    definirHabilidadeJogador(jogador, inventarioHabilidades[0].equipada ? inventarioHabilidades[0].tipo : HABILIDADE_NENHUMA);

    camera->target = (Vector2){ jogador->pos.x, GetScreenHeight() / 2.0f };
    camera->offset = (Vector2){ GetScreenWidth() / 4.0f, GetScreenHeight() / 2.0f };
    camera->rotation = 0.0f;
    camera->zoom = 1.0f;
    return true;
}

int main(void) {
    const int larguraJanela = 1200;
    const int alturaJanela = 720;
    const char *arquivoSave = "saves\\save.dat";

    Mapa mapaFase = { 0 };
    Mapa mapaVila = { 0 };
    Jogador jogador;
    Camera2D camera = { 0 };
    Texture2D spriteBanco = { 0 };
    Inimigo inimigos[MAX_INIMIGOS] = { 0 };
    int totalInimigos = 0;
    Chefe chefe = { 0 };
    Amuleto amuletosMapa[MAX_AMULETOS] = { 0 };
    int totalAmuletosMapa = 0;
    Habilidade habilidadesMapa[MAX_HABILIDADES] = { 0 };
    int totalHabilidadesMapa = 0;
    Amuleto inventarioAmuletos[3] = { 0 };
    Habilidade inventarioHabilidades[1] = { 0 };
    bool faseCarregada = false;
    int faseAtual = 1;
    int pontuacao = 0;
    int vidasPersistidas = 5;
    int energiaPersistida = 100;
    GameState estadoRetornoInventario = STATE_FASE;
    bool executando = true;

    InitWindow(larguraJanela, alturaJanela, "Hollow Knight");
    SetTargetFPS(60);
    SetExitKey(0);
    spriteBanco = LoadTexture("assets/sprites/SpriteBancoPorMrMeSoLonely-1.png.png");
    texturaSpritesJogador = LoadTexture("assets/sprites/spritesbyXteper-1.png.png");
    inicializarJogador(&jogador, (Vector2){ 64.0f, 64.0f });
    inicializarInventarioBase(inventarioAmuletos, inventarioHabilidades);
    carregarMapaComFallback(&mapaVila, "assets\\mapas\\vila.txt");

    GameState estadoAtual = STATE_MENU;

    while (executando && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (estadoAtual) {
            case STATE_MENU:
            {
                MenuPrincipalAcao acao = atualizarDesenharMenuPrincipal();
                if (acao == MENU_ACAO_JOGAR) {
                    GameSave save = { 0 };
                    if (carregarJogo(arquivoSave, &save)) {
                        faseAtual = save.faseAtual;
                        if (faseAtual < 1) faseAtual = 1;
                        if (faseAtual > 3) faseAtual = 3;
                        vidasPersistidas = save.vidas;
                        energiaPersistida = save.energia;
                        pontuacao = save.pontuacao;

                        for (int i = 0; i < 3 && i < save.totalAmuletos; i++) {
                            inventarioAmuletos[i].coletado = save.amuletosColetados[i] != 0;
                            inventarioAmuletos[i].equipado = save.amuletosEquipados[i] != 0;
                        }
                        if (save.totalHabilidades > 0) {
                            inventarioHabilidades[0].coletada = save.habilidadesColetadas[0] != 0;
                            inventarioHabilidades[0].equipada = save.habilidadesEquipadas[0] != 0;
                        }
                    }
                    estadoAtual = STATE_VILA;
                } else if (acao == MENU_ACAO_NOVO_JOGO) {
                    faseAtual = 1;
                    pontuacao = 0;
                    vidasPersistidas = 5;
                    energiaPersistida = 100;
                    inicializarInventarioBase(inventarioAmuletos, inventarioHabilidades);
                    estadoAtual = STATE_VILA;
                } else if (acao == MENU_ACAO_AJUDA) {
                    estadoAtual = STATE_AJUDA;
                } else if (acao == MENU_ACAO_SAIR) {
                    executando = false;
                }
                break;
            }

            case STATE_VILA:
                BeginMode2D((Camera2D){ .target = { 0, 0 }, .offset = { 0, 0 }, .zoom = 1.0f, .rotation = 0.0f });
                desenharMapa(&mapaVila);
                EndMode2D();
                float scale = 1.0f / 5.0f;
                float scaledWidth = spriteBanco.width * scale;
                float scaledHeight = spriteBanco.height * scale;
                Vector2 spritePos = {
                    (GetScreenWidth() - scaledWidth) * 0.5f+ 40.0f,
                    (GetScreenHeight() - scaledHeight) * 0.5f + 40.0f
                };
                DrawTextureEx(spriteBanco, spritePos, 0.0f, scale, WHITE);

                DrawText("VILA", 20, 20, 32, WHITE);
                DrawText("F: entrar no poco e iniciar fase", 20, 60, 22, WHITE);
                DrawText("TAB: inventario", 20, 90, 22, WHITE);
                DrawText("ESC: voltar ao menu", 20, 120, 22, WHITE);
                DrawText(TextFormat("Proxima fase: %d", faseAtual), 20, 150, 22, GOLD);

                if (IsKeyPressed(KEY_F)) {
                    faseCarregada = prepararFase(
                        faseAtual, &mapaFase, &jogador, inimigos, &totalInimigos, &chefe,
                        amuletosMapa, &totalAmuletosMapa, habilidadesMapa, &totalHabilidadesMapa,
                        vidasPersistidas, energiaPersistida, inventarioAmuletos, inventarioHabilidades, &camera
                    );
                    if (faseCarregada) {
                        estadoAtual = STATE_FASE;
                    }
                }
                if (IsKeyPressed(KEY_TAB)) {
                    estadoRetornoInventario = STATE_VILA;
                    estadoAtual = STATE_INVENTARIO;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    estadoAtual = STATE_MENU;
                }
                break;

            case STATE_FASE:
                if (faseCarregada) {
                    float dt = GetFrameTime();
                    atualizarJogador(&jogador, &mapaFase, dt);
                    atualizarInimigos(inimigos, totalInimigos, &mapaFase, &jogador, &pontuacao, dt);
                    atualizarChefe(&chefe, &mapaFase, &jogador, &pontuacao, dt);
                    atualizarColetaAmuletos(amuletosMapa, totalAmuletosMapa, &jogador);
                    atualizarColetaHabilidades(habilidadesMapa, totalHabilidadesMapa, &jogador);
                    for (int i = 0; i < totalAmuletosMapa; i++) {
                        if (amuletosMapa[i].coletado) {
                            inventarioAmuletos[amuletosMapa[i].tipo].coletado = true;
                        }
                    }
                    for (int i = 0; i < totalHabilidadesMapa; i++) {
                        if (habilidadesMapa[i].coletada) {
                            inventarioHabilidades[0].coletada = true;
                        }
                    }

                    aplicarEfeitosAmuletos(inventarioAmuletos, 3, &jogador);
                    definirHabilidadeJogador(&jogador, inventarioHabilidades[0].equipada ? HABILIDADE_ONDA : HABILIDADE_NENHUMA);

                    vidasPersistidas = jogador.vidas;
                    energiaPersistida = jogador.energia;

                    camera.target = (Vector2){ jogador.pos.x + JOGADOR_LARGURA * 0.5f, GetScreenHeight() / 2.0f };

                    BeginMode2D(camera);
                    desenharMapa(&mapaFase);
                    desenharAmuletos(amuletosMapa, totalAmuletosMapa);
                    desenharHabilidades(habilidadesMapa, totalHabilidadesMapa);
                    desenharInimigos(inimigos, totalInimigos);
                    desenharChefe(&chefe);
                    desenharJogador(&jogador);
                    EndMode2D();

                    desenharHud(&jogador, pontuacao, faseAtual);

                    if (jogador.vidas <= 0) {
                        estadoAtual = STATE_GAME_OVER;
                    }
                    if (!chefe.ativo) {
                        faseAtual++;
                        faseCarregada = false;
                        if (faseAtual > 3) {
                            estadoAtual = STATE_VITORIA;
                        } else {
                            estadoAtual = STATE_VILA;
                        }
                    }
                } else {
                    DrawText("Falha ao carregar mapa da fase.", 40, 40, 24, RED);
                }

                DrawText("ESC abre pause | TAB inventario", 20, 145, 20, GRAY);
                DrawText(TextFormat("Inimigos vivos: %d", contarInimigosAtivos(inimigos, totalInimigos)), 20, 170, 20, LIGHTGRAY);
                if (IsKeyPressed(KEY_ESCAPE)) {
                    estadoAtual = STATE_PAUSE;
                }
                if (IsKeyPressed(KEY_TAB)) {
                    estadoRetornoInventario = STATE_FASE;
                    estadoAtual = STATE_INVENTARIO;
                }
                break;

            case STATE_PAUSE:
            {
                MenuPauseAcao acaoPause = atualizarDesenharMenuPause();
                if (acaoPause == PAUSE_ACAO_CONTINUAR) {
                    estadoAtual = STATE_FASE;
                } else if (acaoPause == PAUSE_ACAO_INVENTARIO) {
                    estadoRetornoInventario = STATE_FASE;
                    estadoAtual = STATE_INVENTARIO;
                } else if (acaoPause == PAUSE_ACAO_SALVAR) {
                    GameSave save = { 0 };
                    save.faseAtual = faseAtual;
                    save.vidas = vidasPersistidas;
                    save.energia = energiaPersistida;
                    save.pontuacao = pontuacao;
                    save.totalAmuletos = 3;
                    save.totalHabilidades = 1;
                    for (int i = 0; i < 3; i++) {
                        save.amuletosColetados[i] = inventarioAmuletos[i].coletado ? 1 : 0;
                        save.amuletosEquipados[i] = inventarioAmuletos[i].equipado ? 1 : 0;
                    }
                    save.habilidadesColetadas[0] = inventarioHabilidades[0].coletada ? 1 : 0;
                    save.habilidadesEquipadas[0] = inventarioHabilidades[0].equipada ? 1 : 0;
                    salvarJogo(arquivoSave, &save);
                } else if (acaoPause == PAUSE_ACAO_SAIR_MENU) {
                    faseCarregada = false;
                    estadoAtual = STATE_MENU;
                }
                break;
            }

            case STATE_AJUDA:
                desenharTelaAjuda();
                if (IsKeyPressed(KEY_ESCAPE)) {
                    estadoAtual = STATE_MENU;
                }
                break;

            case STATE_INVENTARIO:
            {
                bool podeEquipar = (estadoRetornoInventario == STATE_VILA);
                desenharTelaInventario(podeEquipar);

                for (int i = 0; i < 3; i++) {
                    DrawText(
                        TextFormat("Amuleto %d - Coletado: %s | Equipado: %s", i + 1,
                            inventarioAmuletos[i].coletado ? "sim" : "nao",
                            inventarioAmuletos[i].equipado ? "sim" : "nao"),
                        130, 470 + i * 30, 20, LIGHTGRAY
                    );
                }
                DrawText(
                    TextFormat("Habilidade Onda - Coletada: %s | Equipada: %s",
                        inventarioHabilidades[0].coletada ? "sim" : "nao",
                        inventarioHabilidades[0].equipada ? "sim" : "nao"),
                    130, 570, 20, LIGHTGRAY
                );

                if (podeEquipar) {
                    if (IsKeyPressed(KEY_ONE) && inventarioAmuletos[0].coletado) {
                        inventarioAmuletos[0].equipado = !inventarioAmuletos[0].equipado;
                    }
                    if (IsKeyPressed(KEY_TWO) && inventarioAmuletos[1].coletado) {
                        inventarioAmuletos[1].equipado = !inventarioAmuletos[1].equipado;
                    }
                    if (IsKeyPressed(KEY_THREE) && inventarioAmuletos[2].coletado) {
                        inventarioAmuletos[2].equipado = !inventarioAmuletos[2].equipado;
                    }
                    if (IsKeyPressed(KEY_H) && inventarioHabilidades[0].coletada) {
                        inventarioHabilidades[0].equipada = !inventarioHabilidades[0].equipada;
                    }
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    estadoAtual = estadoRetornoInventario;
                }
                break;
            }

            case STATE_GAME_OVER:
                DrawText("GAME OVER", 470, 300, 64, RED);
                DrawText("Pressione ENTER para voltar ao menu", 380, 390, 28, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    estadoAtual = STATE_MENU;
                    faseAtual = 1;
                    pontuacao = 0;
                    vidasPersistidas = 5;
                    energiaPersistida = 100;
                    inicializarInventarioBase(inventarioAmuletos, inventarioHabilidades);
                }
                break;

            case STATE_VITORIA:
                DrawText("VITORIA!", 500, 300, 64, GREEN);
                DrawText(TextFormat("Pontuacao final: %d", pontuacao), 470, 380, 30, WHITE);
                DrawText("Pressione ENTER para voltar ao menu", 380, 440, 28, WHITE);
                if (IsKeyPressed(KEY_ENTER)) {
                    estadoAtual = STATE_MENU;
                    faseAtual = 1;
                }
                break;
        }

        EndDrawing();
    }

    UnloadTexture(spriteBanco);
    UnloadTexture(texturaSpritesJogador);
    CloseWindow();
    return 0;
}
