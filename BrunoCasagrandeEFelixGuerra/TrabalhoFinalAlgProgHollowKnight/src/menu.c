#include "menu.h"

MenuPrincipalAcao atualizarDesenharMenuPrincipal(void) {
    const char *opcoes[] = { "Jogar (J)", "Novo Jogo (N)", "Ajuda (A)", "Sair (S)" };
    Rectangle botoes[4];
    static int selecionado = 0;
    Vector2 mouse = GetMousePosition();

    DrawText("Hollow Knight", 420, 140, 54, WHITE);
    DrawText("INF01202 - Projeto Final", 420, 205, 20, LIGHTGRAY);
    DrawText("Bruno Rafael Casagrande e Felix Celia Guerra", 420, 225, 20, LIGHTGRAY);

    for (int i = 0; i < 4; i++) {
        botoes[i] = (Rectangle){ 460.0f, 290.0f + i * 70.0f, 280.0f, 54.0f };
        bool emCima = CheckCollisionPointRec(mouse, botoes[i]);
        if (emCima) selecionado = i;
        DrawRectangleRec(botoes[i], (selecionado == i) ? DARKBLUE : DARKGRAY);
        DrawText(opcoes[i], (int)botoes[i].x + 18, (int)botoes[i].y + 15, 24, WHITE);
    }

    if (IsKeyPressed(KEY_DOWN)) selecionado = (selecionado + 1) % 4;
    if (IsKeyPressed(KEY_UP)) selecionado = (selecionado + 3) % 4;

    if (IsKeyPressed(KEY_J)) return MENU_ACAO_JOGAR;
    if (IsKeyPressed(KEY_N)) return MENU_ACAO_NOVO_JOGO;
    if (IsKeyPressed(KEY_A)) return MENU_ACAO_AJUDA;
    if (IsKeyPressed(KEY_S)) return MENU_ACAO_SAIR;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < 4; i++) {
            if (!CheckCollisionPointRec(mouse, botoes[i])) continue;
            return (MenuPrincipalAcao)(i + 1);
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        return (MenuPrincipalAcao)(selecionado + 1);
    }

    return MENU_ACAO_NENHUMA;
}

MenuPauseAcao atualizarDesenharMenuPause(void) {
    const char *opcoes[] = { "Continuar (C)", "Inventario (I)", "Salvar (J)", "Sair (S)" };
    Rectangle botoes[4];
    static int selecionado = 0;
    Vector2 mouse = GetMousePosition();

    DrawRectangle(350, 140, 500, 450, Fade(BLACK, 0.8f));
    DrawText("Pause", 560, 180, 40, WHITE);

    for (int i = 0; i < 4; i++) {
        botoes[i] = (Rectangle){ 430.0f, 260.0f + i * 70.0f, 340.0f, 54.0f };
        bool emCima = CheckCollisionPointRec(mouse, botoes[i]);
        if (emCima) selecionado = i;
        DrawRectangleRec(botoes[i], (selecionado == i) ? DARKGREEN : DARKGRAY);
        DrawText(opcoes[i], (int)botoes[i].x + 16, (int)botoes[i].y + 15, 24, WHITE);
    }

    if (IsKeyPressed(KEY_DOWN)) selecionado = (selecionado + 1) % 4;
    if (IsKeyPressed(KEY_UP)) selecionado = (selecionado + 3) % 4;

    if (IsKeyPressed(KEY_C)) return PAUSE_ACAO_CONTINUAR;
    if (IsKeyPressed(KEY_I)) return PAUSE_ACAO_INVENTARIO;
    if (IsKeyPressed(KEY_J)) return PAUSE_ACAO_SALVAR;
    if (IsKeyPressed(KEY_S)) return PAUSE_ACAO_SAIR_MENU;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < 4; i++) {
            if (!CheckCollisionPointRec(mouse, botoes[i])) continue;
            return (MenuPauseAcao)(i + 1);
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        return (MenuPauseAcao)(selecionado + 1);
    }

    return PAUSE_ACAO_NENHUMA;
}

void desenharTelaAjuda(void) {
    DrawText("AJUDA", 560, 60, 42, WHITE);
    DrawText("Controles:", 80, 140, 30, GOLD);
    DrawText("Setas esquerda/direita: mover", 80, 190, 24, WHITE);
    DrawText("Seta para cima: pular", 80, 225, 24, WHITE);
    DrawText("S: ataque basico", 80, 260, 24, WHITE);
    DrawText("A (segurar): cura (custa energia)", 80, 295, 24, WHITE);
    DrawText("D: habilidade equipada", 80, 330, 24, WHITE);
    DrawText("TAB: inventario", 80, 365, 24, WHITE);
    DrawText("ESC: pause/voltar", 80, 400, 24, WHITE);
    DrawText("Amuletos: +vida, +dano, +energia", 80, 455, 24, SKYBLUE);
    DrawText("Monstros: inimigos normais (M) e chefe (C)", 80, 490, 24, ORANGE);
    DrawText("Pressione ESC para voltar ao menu", 80, 560, 26, LIGHTGRAY);
}

void desenharTelaInventario(bool podeEquiparNaTelaAtual) {
    DrawText("INVENTARIO", 500, 70, 40, WHITE);
    DrawText("1: alternar amuleto de vida", 130, 170, 24, WHITE);
    DrawText("2: alternar amuleto de dano", 130, 205, 24, WHITE);
    DrawText("3: alternar amuleto de energia", 130, 240, 24, WHITE);
    DrawText("H: alternar habilidade coletada", 130, 275, 24, WHITE);
    DrawText("ESC: voltar", 130, 325, 24, GRAY);

    if (podeEquiparNaTelaAtual) {
        DrawText("Equipamento permitido (vila).", 130, 390, 24, GREEN);
    } else {
        DrawText("Troca de amuletos/habilidades so e permitida na vila.", 130, 390, 24, RED);
    }
}
