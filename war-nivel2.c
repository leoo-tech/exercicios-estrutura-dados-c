// WAR - Nível Aventureiro (Nível 2)
// Requisitos: alocação dinâmica, ponteiros, ataque com rand(), atualização de dados, exibição pós-ataque, free()
// Autor: você + Copilot :)
// Bibliotecas necessárias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ---- Definições e Estruturas ----
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// ---- Prototipação ----
Territorio* criarMapa(int n);
void cadastrarTerritorios(Territorio* mapa, int n);
void exibirTerritorios(const Territorio* mapa, int n);
int escolherIndice(const char* prompt, int n);
void atacar(Territorio* atacante, Territorio* defensor);
void liberarMemoria(Territorio* mapa);

// ---- Funções Auxiliares ----
static int dado() { // retorna [1..6]
    return (rand() % 6) + 1;
}

static int max_int(int a, int b) { return (a > b) ? a : b; }

// ---- Implementações ----
Territorio* criarMapa(int n) {
    // calloc zera a memória e evita lixo inicial
    Territorio* mapa = (Territorio*)calloc((size_t)n, sizeof(Territorio));
    if (!mapa) {
        fprintf(stderr, "Erro: falha ao alocar memoria para %d territorios.\n", n);
        exit(EXIT_FAILURE);
    }
    return mapa;
}

void cadastrarTerritorios(Territorio* mapa, int n) {
    printf("\n--- Cadastro de Territorios do Jogo WAR ---\n");
    for (int i = 0; i < n; i++) {
        printf("\n--- Cadastrando Territorio %d ---\n", i + 1);

        printf("Digite o nome do territorio: ");
        // scanf com %29s para evitar overflow de buffer
        scanf("%29s", mapa[i].nome);

        printf("Digite a cor do exercito: ");
        scanf("%9s", mapa[i].cor);

        do {
            printf("Digite a quantidade de tropas (>= 1): ");
            if (scanf("%d", &mapa[i].tropas) != 1) {
                // limpeza simples do buffer em caso de entrada invalida
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                mapa[i].tropas = 0;
            }
        } while (mapa[i].tropas < 1);
    }
}

void exibirTerritorios(const Territorio* mapa, int n) {
    printf("\n\n--- Relatorio de Territorios ---\n");
    for (int i = 0; i < n; i++) {
        printf("----------------------------------------\n");
        printf("#%d | Territorio: %s\n", i, mapa[i].nome);
        printf("    Cor do Exercito: %s\n", mapa[i].cor);
        printf("    Quantidade de Tropas: %d\n", mapa[i].tropas);
    }
    printf("----------------------------------------\n");
}

// Escolhe um indice valido no intervalo [0..n-1]
int escolherIndice(const char* prompt, int n) {
    int idx;
    while (1) {
        printf("%s (0..%d): ", prompt, n - 1);
        if (scanf("%d", &idx) == 1 && idx >= 0 && idx < n) {
            return idx;
        }
        // limpar buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Entrada invalida. Tente novamente.\n");
    }
}

// Regra de ataque:
// - Rola-se 1 dado para cada lado [1..6]
// - Se atacante > defensor: atacante vence
//    -> Defensor muda de cor para a do atacante
//    -> Metade das tropas do atacante se movem para o defensor (ocupacao)
//       * mover = atacante->tropas / 2  (garante pelo menos 1 porque exigimos >=2 para atacar)
//       * defensor->tropas = mover
//       * atacante->tropas -= mover
// - Se atacante <= defensor: atacante perde 1 tropa (nao negativa)
void atacar(Territorio* atacante, Territorio* defensor) {
    printf("\n=== Fase de Ataque: %s (%s) -> %s (%s) ===\n",
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);

    // Validacoes basicas aqui (defensivas)
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("Acao invalida: nao eh permitido atacar um territorio da mesma cor.\n");
        return;
    }
    if (atacante->tropas < 2) {
        printf("Acao invalida: o territorio atacante precisa ter pelo menos 2 tropas para atacar.\n");
        return;
    }
    if (atacante->tropas <= 0 || defensor->tropas <= 0) {
        printf("Acao invalida: territorios sem tropas nao podem participar de ataques.\n");
        return;
    }

    int dadoAtq = dado();
    int dadoDef = dado();
    printf("Rolagens -> Atacante: %d | Defensor: %d\n", dadoAtq, dadoDef);

    if (dadoAtq > dadoDef) {
        // Vitoria do atacante: conquista o territorio
        int mover = atacante->tropas / 2; // metade
        if (mover < 1) mover = 1;

        printf("Resultado: %s conquista %s!\n", atacante->nome, defensor->nome);
        // Atualiza cor do defensor
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor) - 1] = '\0';

        // Tropas do defensor passam a ser as tropas movidas
        defensor->tropas = mover;
        atacante->tropas -= mover;

        printf("Ocupacao: mover %d tropas para %s. %s agora tem %d tropas.\n",
               mover, defensor->nome, atacante->nome, atacante->tropas);
    } else {
        // Derrota/empate do atacante: perde 1 tropa
        atacante->tropas = max_int(0, atacante->tropas - 1);
        printf("Resultado: defesa resiste! %s perde 1 tropa. Tropas restantes: %d.\n",
               atacante->nome, atacante->tropas);
    }
}

void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

int main(void) {
    srand((unsigned)time(NULL)); // garante aleatoriedade

    int n;
    printf("--- WAR (Nivel Aventureiro) ---\n");
    do {
        printf("Informe o numero de territorios (>= 2): ");
        if (scanf("%d", &n) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            n = 0;
        }
    } while (n < 2);

    Territorio* mapa = criarMapa(n);
    cadastrarTerritorios(mapa, n);

    int opcao;
    do {
        printf("\n===== MENU =====\n");
        printf("1 - Exibir territorios\n");
        printf("2 - Realizar ataque\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opcao) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            opcao = -1;
        }

        if (opcao == 1) {
            exibirTerritorios(mapa, n);
        } else if (opcao == 2) {
            exibirTerritorios(mapa, n);
            printf("\nDica: escolha indices diferentes e cores diferentes.\n");
            int idxAtq = escolherIndice("Indice do territorio ATACANTE", n);
            int idxDef = escolherIndice("Indice do territorio DEFENSOR", n);

            if (idxAtq == idxDef) {
                printf("Acao invalida: atacante e defensor nao podem ser o mesmo territorio.\n");
                continue;
            }
            if (strcmp(mapa[idxAtq].cor, mapa[idxDef].cor) == 0) {
                printf("Acao invalida: nao eh permitido atacar um territorio da mesma cor.\n");
                continue;
            }
            if (mapa[idxAtq].tropas < 2) {
                printf("Acao invalida: o territorio atacante precisa ter pelo menos 2 tropas.\n");
                continue;
            }

            atacar(&mapa[idxAtq], &mapa[idxDef]); // uso de ponteiros
            // Exibição pos-ataque
            exibirTerritorios(mapa, n);
        } else if (opcao == 0) {
            printf("Encerrando...\n");
        } else {
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    liberarMemoria(mapa); // free no final
    return 0;
}
