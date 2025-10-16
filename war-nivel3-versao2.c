#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Estruturas e Constantes ---
#define NUM_MISSOES 4

struct Territorio
{
  char nome[30];
  char cor[10];
  int tropas;
};

// --- Protótipos das Funções ---
void cadastrarTerritorios(struct Territorio *mapa, int tamanho);
void exibirTerritorios(const struct Territorio *mapa, int tamanho);
void atacar(struct Territorio *atacante, struct Territorio *defensor);
void atribuirMissao(char **destinoMissao, const char *missoes[], int totalMissoes);
int verificarMissao(const char *missao, const struct Territorio *mapa, int tamanho, const char *corJogador, const char *corInimiga);
void liberarMemoria(struct Territorio *mapa, char *missao1, char *missao2);

int main()
{
  srand(time(NULL));

  // --- Vetor de Missões ---
  // O cardápio de propósitos para a guerra.
  const char *missoes[NUM_MISSOES] = {
      "Conquistar 3 territorios",
      "Conquistar 4 territorios",
      "Eliminar a cor Verde",
      "Eliminar a cor Azul"};

  int numTerritorios;
  printf("Digite o numero total de territorios no mapa (minimo 4): ");
  scanf("%d", &numTerritorios);
  if (numTerritorios < 4)
    numTerritorios = 4; // Evitando um mapa trivial demais.

  // --- Alocação Dinâmica ---
  struct Territorio *mapa = (struct Territorio *)malloc(numTerritorios * sizeof(struct Territorio));
  char *missaoJogador1 = NULL;
  char *missaoJogador2 = NULL;

  if (mapa == NULL)
  {
    printf("Erro critico: Falha ao alocar memoria para o mapa.\n");
    return 1;
  }

  // --- Setup do Jogo ---
  const char *corJogador1 = "Azul";
  const char *corJogador2 = "Verde";

  cadastrarTerritorios(mapa, numTerritorios);

  atribuirMissao(&missaoJogador1, missoes, NUM_MISSOES);
  atribuirMissao(&missaoJogador2, missoes, NUM_MISSOES);

  printf("\n--- Missoes distribuidas! ---\n");
  printf("Jogador 1 (%s): %s\n", corJogador1, missaoJogador1);
  printf("Jogador 2 (%s): %s\n", corJogador2, missaoJogador2);

  // --- Game Loop ---
  // O ciclo de sofrimento só termina quando alguém vence.
  int jogadorAtual = 1;
  int jogoAcabou = 0;

  while (!jogoAcabou)
  {
    exibirTerritorios(mapa, numTerritorios);

    printf("\n--- Turno do Jogador %d (%s) ---\n", jogadorAtual, (jogadorAtual == 1) ? corJogador1 : corJogador2);

    int idAtacante, idDefensor;
    printf("Escolha o territorio atacante (indice): ");
    scanf("%d", &idAtacante);
    printf("Escolha o territorio defensor (indice): ");
    scanf("%d", &idDefensor);

    if (idAtacante < 0 || idAtacante >= numTerritorios || idDefensor < 0 || idDefensor >= numTerritorios)
    {
      printf("Indices invalidos. Turno perdido.\n");
    }
    else if (strcmp(mapa[idAtacante].cor, (jogadorAtual == 1) ? corJogador1 : corJogador2) != 0)
    {
      printf("Ataque invalido: Voce so pode atacar com seus proprios territorios.\n");
    }
    else if (strcmp(mapa[idAtacante].cor, mapa[idDefensor].cor) == 0)
    {
      printf("Ataque invalido: Nao pode atacar um territorio aliado.\n");
    }
    else
    {
      atacar(&mapa[idAtacante], &mapa[idDefensor]);
    }

    // --- Verificação de Vitória ---
    if (verificarMissao(missaoJogador1, mapa, numTerritorios, corJogador1, corJogador2))
    {
      printf("\n\nJOGADOR 1 (%s) CUMPRIU A MISSAO E VENCEU O JOGO!\n", corJogador1);
      jogoAcabou = 1;
    }
    else if (verificarMissao(missaoJogador2, mapa, numTerritorios, corJogador2, corJogador1))
    {
      printf("\n\nJOGADOR 2 (%s) CUMPRIU A MISSAO E VENCEU O JOGO!\n", corJogador2);
      jogoAcabou = 1;
    }

    // Troca o turno
    jogadorAtual = (jogadorAtual == 1) ? 2 : 1;
  }

  liberarMemoria(mapa, missaoJogador1, missaoJogador2);
  return 0;
}

// Implementações das funções... (as mesmas de antes, com as novas adicionadas)

void cadastrarTerritorios(struct Territorio *mapa, int tamanho)
{
  printf("\n--- Cadastro de Territorios ---\n");
  for (int i = 0; i < tamanho; i++)
  {
    printf("\n--- Territorio %d ---\n", i);
    printf("Nome: ");
    scanf("%s", (mapa + i)->nome);
    printf("Cor (Azul ou Verde): ");
    scanf("%s", (mapa + i)->cor);
    printf("Tropas: ");
    scanf("%d", &(mapa + i)->tropas);
  }
}

void exibirTerritorios(const struct Territorio *mapa, int tamanho)
{
  printf("\n\n--- Estado Atual do Mapa ---\n");
  for (int i = 0; i < tamanho; i++)
  {
    printf("Indice: %-2d | Territorio: %-15s | Cor: %-10s | Tropas: %d\n", i, (mapa + i)->nome, (mapa + i)->cor, (mapa + i)->tropas);
  }
  printf("----------------------------------------------------------------\n");
}

void atacar(struct Territorio *atacante, struct Territorio *defensor)
{
  int dadoAtaque = rand() % 6 + 1;
  int dadoDefesa = rand() % 6 + 1;
  printf("Batalha: %s (%d tropas) ataca %s (%d tropas)\n", atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);
  printf("... Dados -> Atacante: %d | Defensor: %d\n", dadoAtaque, dadoDefesa);

  if (dadoAtaque > dadoDefesa)
  {
    printf("VITORIA DO ATACANTE! O territorio %s foi conquistado.\n", defensor->nome);
    strcpy(defensor->cor, atacante->cor);
    int tropasMovidas = (atacante->tropas > 1) ? atacante->tropas / 2 : 1;
    defensor->tropas = tropasMovidas;
    atacante->tropas -= tropasMovidas;
  }
  else
  {
    printf("VITORIA DO DEFENSOR! O territorio %s resistiu.\n", defensor->nome);
    if (atacante->tropas > 1)
      atacante->tropas -= 1;
  }
}

/**
 * @brief Sorteia uma missão e aloca memória para ela.
 * @param destinoMissao Ponteiro para o ponteiro que armazenará a missão. (char**)
 * @param missoes Vetor de strings com as missões disponíveis.
 * @param totalMissoes Tamanho do vetor de missões.
 */
void atribuirMissao(char **destinoMissao, const char *missoes[], int totalMissoes)
{
  int indiceSorteado = rand() % totalMissoes;
  // Aloca memória do tamanho exato da missão sorteada + 1 para o '\0'.
  *destinoMissao = (char *)malloc(strlen(missoes[indiceSorteado]) + 1);
  if (*destinoMissao != NULL)
  {
    strcpy(*destinoMissao, missoes[indiceSorteado]);
  }
}

/**
 * @brief Verifica se a condição de vitória de uma missão foi atendida.
 * @param missao A string da missão a ser verificada.
 * @param mapa O estado atual do jogo.
 * @param tamanho O número de territórios no mapa.
 * @param corJogador A cor do jogador cuja missão está sendo verificada.
 * @param corInimiga A cor do oponente.
 * @return 1 se a missão foi cumprida, 0 caso contrário.
 */
int verificarMissao(const char *missao, const struct Territorio *mapa, int tamanho, const char *corJogador, const char *corInimiga)
{
  // Lógica para "Conquistar X territorios"
  if (strstr(missao, "Conquistar") != NULL)
  {
    int numParaConquistar = 0;
    sscanf(missao, "Conquistar %d territorios", &numParaConquistar);

    int contador = 0;
    for (int i = 0; i < tamanho; i++)
    {
      if (strcmp((mapa + i)->cor, corJogador) == 0)
      {
        contador++;
      }
    }
    return contador >= numParaConquistar;
  }

  // Lógica para "Eliminar a cor Y"
  if (strstr(missao, "Eliminar") != NULL)
  {
    for (int i = 0; i < tamanho; i++)
    {
      if (strcmp((mapa + i)->cor, corInimiga) == 0)
      {
        return 0; // Inimigo ainda existe no mapa.
      }
    }
    return 1; // Nenhum território com a cor inimiga foi encontrado.
  }

  return 0; // Missão desconhecida ou não cumprida.
}

void liberarMemoria(struct Territorio *mapa, char *missao1, char *missao2)
{
  printf("\nLiberando toda a memoria alocada... Fim de jogo.\n");
  free(mapa);
  free(missao1);
  free(missao2);
}