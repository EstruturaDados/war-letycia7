// war.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef struct {
    char nome[30];
    char cor[16];
    int tropas;
} Territorio;

// ---------- Prots ----------
void cadastrarTerritorios(Territorio *mapa, int n);
void exibirMapa(Territorio *mapa, int n);
void atacar(Territorio *atacante, Territorio *defensor);
void liberarMemoria(Territorio *mapa, char *missao1, char *missao2);
void atribuirMissao(char *destino, char *missoes[], int totalMissoes);
int verificarMissao(char *missao, Territorio *mapa, int n);
void str_tolower(char *s);
void ler_linha(char *buf, int tamanho);

// ---------- Main ----------
int main(void) {
    srand((unsigned)time(NULL));

    int n;
    printf("Digite o numero de territorios: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Entrada invalida. Finalizando.\n");
        return 1;
    }
    getchar(); // limpar newline

    Territorio *mapa = (Territorio *)calloc((size_t)n, sizeof(Territorio));
    if (!mapa) {
        perror("calloc");
        return 1;
    }

    cadastrarTerritorios(mapa, n);

    // Missões pré-definidas
    char *missoes[] = {
        "Conquistar 3 territorios seguidos",
        "Eliminar todas as tropas vermelhas",
        "Dominar 2 territorios com mais de 10 tropas",
        "Ter o maior numero total de tropas",
        "Conquistar um territorio azul"
    };
    int totalMissoes = sizeof(missoes) / sizeof(missoes[0]);

    char *missao1 = (char *)malloc(100);
    char *missao2 = (char *)malloc(100);
    if (!missao1 || !missao2) {
        perror("malloc");
        liberarMemoria(mapa, missao1, missao2);
        return 1;
    }

    atribuirMissao(missao1, missoes, totalMissoes);
    atribuirMissao(missao2, missoes, totalMissoes);

    printf("\nMissao do Jogador 1: %s\n", missao1);
    printf("Missao do Jogador 2: %s\n\n", missao2);

    int atacante, defensor, opcao;
    while (1) {
        exibirMapa(mapa, n);

        printf("\nEscolha o territorio atacante (0 a %d): ", n - 1);
        if (scanf("%d", &atacante) != 1) { printf("Entrada invalida.\n"); break; }
        printf("Escolha o territorio defensor (0 a %d): ", n - 1);
        if (scanf("%d", &defensor) != 1) { printf("Entrada invalida.\n"); break; }

        if (atacante < 0 || atacante >= n || defensor < 0 || defensor >= n) {
            printf("Indice fora do intervalo. Tente novamente.\n");
            continue;
        }

        if (strcmp(mapa[atacante].cor, mapa[defensor].cor) == 0) {
            printf("Voce nao pode atacar um territorio da mesma cor!\n");
            continue;
        }

        if (mapa[atacante].tropas <= 1) {
            printf("Territorio atacante precisa ter pelo menos 2 tropas para atacar.\n");
            continue;
        }

        atacar(&mapa[atacante], &mapa[defensor]);

        if (verificarMissao(missao1, mapa, n)) {
            printf("\n🎉 Jogador 1 venceu! Missao: %s\n", missao1);
            break;
        }
        if (verificarMissao(missao2, mapa, n)) {
            printf("\n🎉 Jogador 2 venceu! Missao: %s\n", missao2);
            break;
        }

        printf("\nDeseja continuar? (1 = sim / 0 = nao): ");
        if (scanf("%d", &opcao) != 1) { printf("Entrada invalida.\n"); break; }
        if (opcao == 0) break;
    }

    liberarMemoria(mapa, missao1, missao2);
    return 0;
}

// ---------- Funções ----------

void cadastrarTerritorios(Territorio *mapa, int n) {
    for (int i = 0; i < n; i++) {
        printf("\nNome do territorio %d: ", i);
        ler_linha(mapa[i].nome, (int)sizeof(mapa[i].nome));
        printf("Cor do territorio: ");
        ler_linha(mapa[i].cor, (int)sizeof(mapa[i].cor));
        str_tolower(mapa[i].cor);

        printf("Numero de tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1) {
            printf("Entrada invalida. Digite um numero: ");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
        }
        getchar(); // limpar newline após o numero
    }
}

void exibirMapa(Territorio *mapa, int n) {
    printf("\n=== MAPA ATUAL ===\n");
    for (int i = 0; i < n; i++) {
        printf("[%d] %-15s | Cor: %-10s | Tropas: %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

void atacar(Territorio *atacante, Territorio *defensor) {
    int dadoA = rand() % 6 + 1;
    int dadoD = rand() % 6 + 1;

    printf("\nAtaque de %s (%s) -> %s (%s)\n",
           atacante->nome, atacante->cor,
           defensor->nome, defensor->cor);

    printf("Dado atacante: %d | Dado defensor: %d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        printf("Atacante venceu!\n");
        // conquista: defensor troca de cor e recebe metade (arredonda para baixo)
        int tropasParaDefensor = atacante->tropas / 2;
        // atualiza defensor
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor)-1] = '\0';
        defensor->tropas = tropasParaDefensor;
        // diminui as tropas do atacante que moveram
        atacante->tropas -= tropasParaDefensor;
        if (atacante->tropas < 0) atacante->tropas = 0;
    } else {
        printf("Defensor resistiu! Atacante perde 1 tropa.\n");
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
    }
}

void atribuirMissao(char *destino, char *missoes[], int totalMissoes) {
    int sorteio = rand() % totalMissoes;
    strncpy(destino, missoes[sorteio], 99);
    destino[99] = '\0';
}

int verificarMissao(char *missao, Territorio *mapa, int n) {
    // missao: checar palavras-chaves para decidir
    char copia[128];
    strncpy(copia, missao, sizeof(copia)-1);
    copia[sizeof(copia)-1] = '\0';
    str_tolower(copia);

    // 1) Eliminar todas as tropas vermelhas
    if (strstr(copia, "vermelhas") || strstr(copia, "vermelha")) {
        for (int i = 0; i < n; i++) {
            if (strcmp(mapa[i].cor, "vermelha") == 0 && mapa[i].tropas > 0)
                return 0; // ainda existe vermelha com tropas
        }
        return 1;
    }

    // 2) Conquistar 3 territorios seguidos
    if (strstr(copia, "3 territorios") || strstr(copia, "3 territorios seguidos")) {
        int consec = 1;
        for (int i = 1; i < n; i++) {
            if (strcmp(mapa[i].cor, mapa[i-1].cor) == 0) {
                consec++;
                if (consec >= 3) return 1;
            } else {
                consec = 1;
            }
        }
        return 0;
    }

    // 3) Dominar 2 territorios com mais de 10 tropas
    if (strstr(copia, "2 territorios") && strstr(copia, "10 tropas")) {
        int count = 0;
        for (int i = 0; i < n; i++) {
            if (mapa[i].tropas > 10) count++;
            if (count >= 2) return 1;
        }
        return 0;
    }

    // 4) Ter o maior numero total de tropas
    if (strstr(copia, "maior numero total") || strstr(copia, "maior numero")) {
        // simplificação: verifica se existe alguma cor que tem o total de tropas maior que todas as outras
        // aqui assumimos que a missão é "Ter o maior numero total de tropas" globalmente satisfeita se
        // alguma cor tem total >= soma das demais (ou simplesmente maior que quaisquer outras).
        // Calcula soma por cor — simplificamos para até 10 cores distintas.
        char cores[10][16];
        int soma[10] = {0};
        int coresCount = 0;
        for (int i = 0; i < n; i++) {
            int idx = -1;
            for (int j = 0; j < coresCount; j++) {
                if (strcmp(mapa[i].cor, cores[j]) == 0) { idx = j; break; }
            }
            if (idx == -1) {
                if (coresCount < 10) {
                    strncpy(cores[coresCount], mapa[i].cor, 15);
                    cores[coresCount][15] = '\0';
                    soma[coresCount] = mapa[i].tropas;
                    coresCount++;
                }
            } else {
                soma[idx] += mapa[i].tropas;
            }
        }
        if (coresCount == 0) return 0;
        int max = soma[0], maxidx = 0;
        for (int i = 1; i < coresCount; i++) {
            if (soma[i] > max) { max = soma[i]; maxidx = i; }
        }
        // verifica se há empate pelo maior
        int numMax = 0;
        for (int i = 0; i < coresCount; i++) if (soma[i] == max) numMax++;
        if (numMax == 1) return 1;
        return 0;
    }

    // 5) Conquistar um territorio azul
    if (strstr(copia, "azul")) {
        for (int i = 0; i < n; i++) {
            if (strcmp(mapa[i].cor, "azul") == 0) return 1;
        }
        return 0;
    }

    // Se não souber interpretar a missão, retornar falso
    return 0;
}

void liberarMemoria(Territorio *mapa, char *missao1, char *missao2) {
    if (mapa) free(mapa);
    if (missao1) free(missao1);
    if (missao2) free(missao2);
}

// ---------- Helpers ----------
void str_tolower(char *s) {
    for (; *s; ++s) *s = (char)tolower((unsigned char)*s);
}

void ler_linha(char *buf, int tamanho) {
    if (!fgets(buf, tamanho, stdin)) {
        buf[0] = '\0';
        return;
    }
    // remover newline final
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}
