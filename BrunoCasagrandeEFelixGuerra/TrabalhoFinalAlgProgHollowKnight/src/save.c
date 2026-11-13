#include "save.h"

#include <stdio.h>

bool salvarJogo(const char *arquivo, const GameSave *save) {
    FILE *fp = fopen(arquivo, "w");
    if (fp == NULL) {
        return false;
    }

    fprintf(fp, "%d %d %d %d\n", save->faseAtual, save->vidas, save->energia, save->pontuacao);
    fprintf(fp, "%d %d\n", save->totalAmuletos, save->totalHabilidades);

    for (int i = 0; i < save->totalAmuletos; i++) {
        fprintf(fp, "%d %d\n", save->amuletosColetados[i], save->amuletosEquipados[i]);
    }
    for (int i = 0; i < save->totalHabilidades; i++) {
        fprintf(fp, "%d %d\n", save->habilidadesColetadas[i], save->habilidadesEquipadas[i]);
    }

    fclose(fp);
    return true;
}

bool carregarJogo(const char *arquivo, GameSave *save) {
    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL) {
        return false;
    }

    if (fscanf(fp, "%d %d %d %d", &save->faseAtual, &save->vidas, &save->energia, &save->pontuacao) != 4) {
        fclose(fp);
        return false;
    }

    if (fscanf(fp, "%d %d", &save->totalAmuletos, &save->totalHabilidades) != 2) {
        fclose(fp);
        return false;
    }

    if (save->totalAmuletos > MAX_AMULETOS) save->totalAmuletos = MAX_AMULETOS;
    if (save->totalHabilidades > MAX_HABILIDADES) save->totalHabilidades = MAX_HABILIDADES;

    for (int i = 0; i < save->totalAmuletos; i++) {
        if (fscanf(fp, "%d %d", &save->amuletosColetados[i], &save->amuletosEquipados[i]) != 2) {
            fclose(fp);
            return false;
        }
    }
    for (int i = 0; i < save->totalHabilidades; i++) {
        if (fscanf(fp, "%d %d", &save->habilidadesColetadas[i], &save->habilidadesEquipadas[i]) != 2) {
            fclose(fp);
            return false;
        }
    }

    fclose(fp);
    return true;
}
