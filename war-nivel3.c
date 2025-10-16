// WAR - Nível 3 (Missões Estratégicas)
// Objetivo: adicionar missões estratégicas individuais, sorteadas e verificadas ao longo do jogo.
// Requisitos atendidos: alocação dinâmica, ponteiros, modularização, srand/rand, verificação de vitória por missão, free().
// Compilação sugerida: gcc -O2 -Wall -Wextra -std=c11 war-nivel3.c -o war-nivel3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// -------------------- Estruturas --------------------
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

typedef struct {
    char nome[30];
    char cor[10];
    char* missao;           // Armazenada dinamicamente (malloc)
    int missaoExibida;      // Exibir apenas uma vez
} Jogador;

// -------------------- Prototipos --------------------
// Territórios
Territorio* criarMapa(int n);
void cadastrarTerritorios(Territorio* mapa, int n);
void exibirTerritorios(const Territorio* mapa, int n);
void liberarMapa(Territorio* mapa);

// Ataque
void atacar(Territorio* atacante, Territorio* defensor);

// Missões
int comprimentoMaximoMissoes(char* missoes[], int totalMissoes);
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
void exibirMissao(const char* nomeJogador, const char* missao);
int verificarMissao(char* missao, Territorio* mapa, int tamanho);
int verificarMissaoJogador(char* missao, Territorio* mapa, int tamanho, const char* corJogador);

// Jogo
void cadastrarJogadores(Jogador* jogadores, int qtdJogadores, int maxLenMissao);
void liberarJogadores(Jogador* jogadores, int qtdJogadores);
int escolherIndice(const char* prompt, int n);
int podeAtacar(const Territorio* mapa, int idxAtq, int idxDef, const char* corJogador);
int contarTerritoriosPorCor(const Territorio* mapa, int n, const char* cor);
int somaTropasPorCor(const Territorio* mapa, int n, const char* cor);
int existeTerritorioComTropas(const Territorio* mapa, int n, const char* cor, int tropasMin);

// -------------------- Variáveis globais (contexto de verificação) --------------------
// OBS: Para cumprir o protótipo exigido (verificarMissao(char*,Territorio*,int)),
// usamos uma variável global para indicar a cor do jogador em verificação.
static char g_corEmVerificacao[10] = "";

// -------------------- Utilitários --------------------
static int dado() { return (rand() % 6) + 1; }  // [1..6]
static int max_int(int a, int b) { return (a > b) ? a : b; }

// -------------------- Implementações: Territórios --------------------
Territorio* criarMapa(int n) {
    Territorio* mapa = (Territorio*)calloc((size_t)n, sizeof(Territorio));
    if (!mapa) {
        fprintf(stderr, "Erro: falha ao alocar memoria para %d territorios.\n", n);
        exit(EXIT_FAILURE);
    }
    return mapa;
}

void cadastrarTerritorios(Territorio* mapa, int n) {
    printf("\n--- Cadastro de Territorios ---\n");
    for (int i = 0; i < n; i++) {
        printf("\n--- Territorio %d ---\n", i + 1);
        printf("Nome: ");
        scanf("%29s", mapa[i].nome);
        printf("Cor do exercito (ex.: Azul, Vermelho, Verde): ");
        scanf("%9s", mapa[i].cor);
        do {
            printf("Tropas (>= 1): ");
            if (scanf("%d", &mapa[i].tropas) != 1) {
                int c; while ((c = getchar()) != '\n' && c != EOF) {}
                mapa[i].tropas = 0;
            }
        } while (mapa[i].tropas < 1);
    }
}

void exibirTerritorios(const Territorio* mapa, int n) {
    printf("\n\n--- Mapa ---\n");
    for (int i = 0; i < n; i++) {
        printf("----------------------------------------\n");
        printf("#%d | %s\n", i, mapa[i].nome);
        printf("    Cor: %s\n", mapa[i].cor);
        printf("    Tropas: %d\n", mapa[i].tropas);
    }
    printf("----------------------------------------\n");
}

void liberarMapa(Territorio* mapa) {
    free(mapa);
}

// -------------------- Implementações: Ataque --------------------
void atacar(Territorio* atacante, Territorio* defensor) {
    printf("\n=== Ataque: %s (%s) -> %s (%s) ===\n",
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);

    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("Invalido: nao pode atacar territorio da mesma cor.\n");
        return;
    }
    if (atacante->tropas < 2) {
        printf("Invalido: atacante precisa ter pelo menos 2 tropas.\n");
        return;
    }

    int dA = dado();
    int dD = dado();
    printf("Rolagens -> Atacante: %d | Defensor: %d\n", dA, dD);

    if (dA > dD) {
        int mover = atacante->tropas / 2; // metade das tropas
        if (mover < 1) mover = 1;
        printf("Resultado: %s conquista %s!\n", atacante->nome, defensor->nome);

        // Assume controle
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor) - 1] = '\0';

        // Ocupação
        defensor->tropas = mover;
        atacante->tropas -= mover;
        printf("Ocupacao: mover %d tropas. %s agora tem %d tropas.\n",
               mover, atacante->nome, atacante->tropas);
    } else {
        atacante->tropas = max_int(0, atacante->tropas - 1);
        printf("Defesa resiste! %s perde 1 tropa (restam %d).\n",
               atacante->nome, atacante->tropas);
    }
}

// -------------------- Implementações: Missões --------------------
// Retorna o maior comprimento entre as strings de missoes (para alocar destino com folga)
int comprimentoMaximoMissoes(char* missoes[], int totalMissoes) {
    int maxLen = 0;
    for (int i = 0; i < totalMissoes; ++i) {
        int len = (int)strlen(missoes[i]);
        if (len > maxLen) maxLen = len;
    }
    return maxLen;
}

// Copia uma missao sorteada para "destino" usando strcpy (destino deve ter sido alocado antes)
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int sorteio = rand() % totalMissoes;
    strcpy(destino, missoes[sorteio]);
}

void exibirMissao(const char* nomeJogador, const char* missao) {
    printf("\n>>> Missao do jogador %s: %s\n", nomeJogador, missao);
}

// Funções auxiliares para verificações
int contarTerritoriosPorCor(const Territorio* mapa, int n, const char* cor) {
    int cont = 0;
    for (int i = 0; i < n; ++i)
        if (strcmp(mapa[i].cor, cor) == 0)
            cont++;
    return cont;
}

int somaTropasPorCor(const Territorio* mapa, int n, const char* cor) {
    int soma = 0;
    for (int i = 0; i < n; ++i)
        if (strcmp(mapa[i].cor, cor) == 0)
            soma += mapa[i].tropas;
    return soma;
}

int existeTerritorioComTropas(const Territorio* mapa, int n, const char* cor, int tropasMin) {
    for (int i = 0; i < n; ++i)
        if (strcmp(mapa[i].cor, cor) == 0 && mapa[i].tropas >= tropasMin)
            return 1;
    return 0;
}

// Wrapper para cumprir a assinatura exigida, usando g_corEmVerificacao como contexto
int verificarMissaoJogador(char* missao, Territorio* mapa, int tamanho, const char* corJogador) {
    strncpy(g_corEmVerificacao, corJogador, sizeof(g_corEmVerificacao) - 1);
    g_corEmVerificacao[sizeof(g_corEmVerificacao) - 1] = '\0';
    return verificarMissao(missao, mapa, tamanho);
}

// Verifica se a string de missão foi cumprida, usando a cor em g_corEmVerificacao como "jogador atual".
// Missões pré-definidas (exemplos):
//  - "Controlar 3 ou mais territorios"
//  - "Controlar 5 ou mais territorios"
//  - "Possuir um territorio com 8 ou mais tropas"
//  - "Possuir ao menos 10 tropas somadas"
//  - "Eliminar todas as tropas da cor Vermelho" (ou Azul/Verde/Amarelo)
int verificarMissao(char* missao, Territorio* mapa, int tamanho) {
    // Normaliza missão para facilitar comparações simples (sem alocar muito)
    // Aqui faremos comparações por substring.

    // 1) Controlar >= 3 territorios
    if (strstr(missao, "Controlar 3") != NULL) {
        int qt = contarTerritoriosPorCor(mapa, tamanho, g_corEmVerificacao);
        return (qt >= 3);
    }
    // 2) Controlar >= 5 territorios
    if (strstr(missao, "Controlar 5") != NULL) {
        int qt = contarTerritoriosPorCor(mapa, tamanho, g_corEmVerificacao);
        return (qt >= 5);
    }
    // 3) Um territorio com >= 8 tropas
    if (strstr(missao, "8 ou mais tropas") != NULL) {
        return existeTerritorioComTropas(mapa, tamanho, g_corEmVerificacao, 8);
    }
    // 4) Somar >= 10 tropas
    if (strstr(missao, "10 tropas somadas") != NULL) {
        int soma = somaTropasPorCor(mapa, tamanho, g_corEmVerificacao);
        return (soma >= 10);
    }
    // 5) Eliminar todas as tropas de determinada cor
    if (strstr(missao, "Eliminar todas as tropas da cor") != NULL) {
        // Extrai a cor alvo a partir do final da string (ultima palavra)
        char alvo[10] = "";
        const char* p = strrchr(missao, ' ');
        if (p) {
            strncpy(alvo, p + 1, sizeof(alvo) - 1);
            alvo[sizeof(alvo) - 1] = '\0';
            // Remover pontuação final se houver (ex.: ponto)
            size_t L = strlen(alvo);
            if (L > 0 && (alvo[L - 1] == '.' || alvo[L - 1] == '!' || alvo[L - 1] == '\n'))
                alvo[L - 1] = '\0';
        }
        // Considera "eliminada" se nao existir nenhum territorio com a cor alvo
        // ou se todos os territorios dessa cor tiverem tropas == 0
        int existe = 0, vivos = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, alvo) == 0) {
                existe = 1;
                if (mapa[i].tropas > 0) vivos = 1;
            }
        }
        return (existe == 0) || (vivos == 0);
    }

    // Caso padrão: missão desconhecida -> não cumprida
    return 0;
}

// -------------------- Implementações: Jogo/Jogadores --------------------
void cadastrarJogadores(Jogador* jogadores, int qtdJogadores, int maxLenMissao) {
    for (int i = 0; i < qtdJogadores; ++i) {
        printf("\n--- Cadastro do Jogador %d ---\n", i + 1);
        printf("Nome: ");
        scanf("%29s", jogadores[i].nome);
        printf("Cor do exercito (ex.: Azul, Vermelho, Verde): ");
        scanf("%9s", jogadores[i].cor);
        jogadores[i].missao = (char*)malloc((size_t)maxLenMissao + 1);
        if (!jogadores[i].missao) {
            fprintf(stderr, "Erro ao alocar memoria para missao do jogador %d.\n", i + 1);
            exit(EXIT_FAILURE);
        }
        jogadores[i].missao[0] = '\0';
        jogadores[i].missaoExibida = 0;
    }
}

void liberarJogadores(Jogador* jogadores, int qtdJogadores) {
    for (int i = 0; i < qtdJogadores; ++i) {
        free(jogadores[i].missao);
        jogadores[i].missao = NULL;
    }
}

int escolherIndice(const char* prompt, int n) {
    int idx;
    while (1) {
        printf("%s (0..%d): ", prompt, n - 1);
        if (scanf("%d", &idx) == 1 && idx >= 0 && idx < n) return idx;
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Entrada invalida. Tente novamente.\n");
    }
}

int podeAtacar(const Territorio* mapa, int idxAtq, int idxDef, const char* corJogador) {
    if (idxAtq == idxDef) return 0;
    if (strcmp(mapa[idxAtq].cor, corJogador) != 0) return 0; // atacante tem que ser do jogador
    if (strcmp(mapa[idxDef].cor, corJogador) == 0) return 0; // defensor inimigo
    if (mapa[idxAtq].tropas < 2) return 0; // precisa de 2+ tropas
    return 1;
}

// -------------------- Função principal --------------------
int main(void) {
    srand((unsigned)time(NULL));

    // Missoes pré-definidas (ao menos 5)
    char* missoes[] = {
        "Controlar 3 ou mais territorios",
        "Controlar 5 ou mais territorios",
        "Possuir um territorio com 8 ou mais tropas",
        "Possuir ao menos 10 tropas somadas",
        "Eliminar todas as tropas da cor Vermelho",
        "Eliminar todas as tropas da cor Azul"
    };
    const int TOTAL_MISSOES = (int)(sizeof(missoes) / sizeof(missoes[0]));

    int n; // quantidade de territorios
    printf("--- WAR (Nivel 3: Missoes Estrategicas) ---\n");
    do {
        printf("Informe o numero de territorios (>= 2): ");
        if (scanf("%d", &n) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            n = 0;
        }
    } while (n < 2);

    Territorio* mapa = criarMapa(n);
    cadastrarTerritorios(mapa, n);

    // Cadastro de jogadores (2 jogadores para este desafio)
    const int QTD_JOGADORES = 2;
    Jogador jogadores[QTD_JOGADORES];

    // Descobre comprimento máximo das missoes para alocar destino
    int maxLenMissao = comprimentoMaximoMissoes(missoes, TOTAL_MISSOES);
    cadastrarJogadores(jogadores, QTD_JOGADORES, maxLenMissao);

    // Atribui missões (usa strcpy no destino alocado dinamicamente)
    for (int i = 0; i < QTD_JOGADORES; ++i) {
        // Tenta evitar atribuir missão "Eliminar cor X" quando X == cor do jogador
        // Faz até 10 tentativas de sorteio simples
        int tentativas = 10;
        do {
            atribuirMissao(jogadores[i].missao, missoes, TOTAL_MISSOES);
            // Se a missão menciona "cor <cor do jogador>", sorteia de novo
            char padrao[64];
            snprintf(padrao, sizeof(padrao), "cor %s", jogadores[i].cor);
            if (strstr(jogadores[i].missao, padrao) != NULL) {
                tentativas--; // re-sorteia
            } else {
                break;
            }
        } while (tentativas > 0);

        // Exibe apenas uma vez no início
        exibirMissao(jogadores[i].nome, jogadores[i].missao);
        jogadores[i].missaoExibida = 1;
    }

    // Loop de turnos simples
    int jogadorDaVez = 0; // alterna 0 e 1
    int executando = 1;
    while (executando) {
        printf("\n===== TURNO: %s (%s) =====\n", jogadores[jogadorDaVez].nome, jogadores[jogadorDaVez].cor);
        printf("1 - Exibir mapa\n");
        printf("2 - Atacar\n");
        printf("0 - Encerrar jogo\n");
        printf("Escolha: ");

        int op;
        if (scanf("%d", &op) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            op = -1;
        }

        if (op == 1) {
            exibirTerritorios(mapa, n);
        } else if (op == 2) {
            exibirTerritorios(mapa, n);
            printf("\nSelecione indices validos (atacante seu e defensor inimigo).\n");
            int iAtq = escolherIndice("Indice do territorio ATACANTE", n);
            int iDef = escolherIndice("Indice do territorio DEFENSOR", n);
            if (!podeAtacar(mapa, iAtq, iDef, jogadores[jogadorDaVez].cor)) {
                printf("Ataque invalido. Verifique cor e tropas.\n");
            } else {
                atacar(&mapa[iAtq], &mapa[iDef]);
                // Exibição pos-ataque
                exibirTerritorios(mapa, n);
            }

            // Verifica silenciosamente se algum jogador cumpriu a missão
            int vencedor = -1;
            for (int j = 0; j < QTD_JOGADORES; ++j) {
                if (verificarMissaoJogador(jogadores[j].missao, mapa, n, jogadores[j].cor)) {
                    vencedor = j;
                    break;
                }
            }
            if (vencedor >= 0) {
                printf("\n*** VITORIA! %s cumpriu a missao: %s ***\n",
                       jogadores[vencedor].nome, jogadores[vencedor].missao);
                break; // encerra o jogo
            }

            // Passa a vez
            jogadorDaVez = 1 - jogadorDaVez;
        } else if (op == 0) {
            printf("Encerrando...\n");
            executando = 0;
        } else {
            printf("Opcao invalida.\n");
        }
    }

    // Liberação de memória
    liberarJogadores(jogadores, QTD_JOGADORES);
    liberarMapa(mapa);

    return 0;
}
