#include <stdio.h>
#include <stdlib.h> // Para malloc, free, rand, srand
#include <string.h> // Para strcpy, strcmp
#include <time.h>   // Para time

// A struct de sempre. A anatomia de um campo de batalha.
struct Territorio
{
  char nome[30];
  char cor[10];
  int tropas;
};

// --- Protótipos das Funções ---
// Gosto de avisar o compilador do que está por vir. É mais organizado.
void cadastrarTerritorios(struct Territorio *mapa, int tamanho);
void exibirTerritorios(const struct Territorio *mapa, int tamanho);
void atacar(struct Territorio *atacante, struct Territorio *defensor);
void liberarMemoria(struct Territorio *mapa);

int main()
{
  // O universo precisa de um começo. E de aleatoriedade para ser cruel.
  srand(time(NULL));

  int numTerritorios;
  printf("Digite o numero total de territorios no mapa: ");
  scanf("%d", &numTerritorios);

  // --- Alocação Dinâmica de Memória ---
  // Pedindo ao sistema um pedaço de memória. Seja gentil.
  struct Territorio *mapa = (struct Territorio *)malloc(numTerritorios * sizeof(struct Territorio));

  // Se a memória não veio, o apocalipse é agora. Fim do programa.
  if (mapa == NULL)
  {
    printf("Erro critico: Falha ao alocar memoria. O programa sera encerrado.\n");
    return 1;
  }

  // Chamando as funções para fazer o trabalho sujo.
  cadastrarTerritorios(mapa, numTerritorios);
  exibirTerritorios(mapa, numTerritorios);

  // --- Lógica de Ataque ---
  int idAtacante, idDefensor;
  printf("\n--- Fase de Ataque ---\n");
  printf("Escolha o territorio atacante (pelo indice de 0 a %d): ", numTerritorios - 1);
  scanf("%d", &idAtacante);
  printf("Escolha o territorio defensor (pelo indice de 0 a %d): ", numTerritorios - 1);
  scanf("%d", &idDefensor);

  // Validação básica para evitar o caos absoluto.
  if (idAtacante >= 0 && idAtacante < numTerritorios && idDefensor >= 0 && idDefensor < numTerritorios)
  {
    // Verificação para impedir que você ataque a si mesmo.
    // A autodestruição é um tema para a filosofia, não para este programa.
    if (strcmp(mapa[idAtacante].cor, mapa[idDefensor].cor) == 0)
    {
      printf("\nAtaque invalido: Nao se pode atacar um territorio da mesma cor.\n");
    }
    else
    {
      printf("\n--- Resolvendo Batalha... ---\n");
      // A mágica acontece aqui. Passamos o endereço de memória dos territórios.
      atacar(&mapa[idAtacante], &mapa[idDefensor]);
      exibirTerritorios(mapa, numTerritorios);
    }
  }
  else
  {
    printf("Indices invalidos. O ataque foi cancelado.\n");
  }

  // --- Liberação de Memória ---
  // Devolvendo o que pegamos emprestado.
  liberarMemoria(mapa);

  return 0; // Sobrevivemos. Por enquanto.
}

/**
 * @brief Coleta os dados de todos os territórios do usuário.
 * @param mapa Ponteiro para o início do vetor de territórios.
 * @param tamanho Quantidade de territórios a serem cadastrados.
 */
void cadastrarTerritorios(struct Territorio *mapa, int tamanho)
{
  printf("\n--- Cadastro de Territorios ---\n");
  for (int i = 0; i < tamanho; i++)
  {
    printf("\n--- Territorio %d ---\n", i);
    printf("Nome: ");
    scanf("%s", (mapa + i)->nome);
    printf("Cor: ");
    scanf("%s", (mapa + i)->cor);
    printf("Tropas: ");
    scanf("%d", &(mapa + i)->tropas);
  }
}

/**
 * @brief Exibe o estado atual de todos os territórios.
 * @param mapa Ponteiro para o vetor de territórios (const para garantir que não será alterado).
 * @param tamanho Quantidade de territórios a serem exibidos.
 */
void exibirTerritorios(const struct Territorio *mapa, int tamanho)
{
  printf("\n\n--- Estado Atual do Mapa ---\n");
  for (int i = 0; i < tamanho; i++)
  {
    printf("----------------------------------------\n");
    printf("Indice: %d\n", i);
    printf("Territorio: %s\n", (mapa + i)->nome);
    printf("Cor do Exercito: %s\n", (mapa + i)->cor);
    printf("Quantidade de Tropas: %d\n", (mapa + i)->tropas);
  }
  printf("----------------------------------------\n");
}

/**
 * @brief Simula um ataque entre dois territórios.
 * @param atacante Ponteiro para o território que está atacando.
 * @param defensor Ponteiro para o território que está defendendo.
 */
void atacar(struct Territorio *atacante, struct Territorio *defensor)
{
  // Simula a rolagem de um dado de 6 faces para cada um.
  int dadoAtaque = rand() % 6 + 1;
  int dadoDefesa = rand() % 6 + 1;

  printf("Dado do Atacante: %d\n", dadoAtaque);
  printf("Dado do Defensor: %d\n", dadoDefesa);

  // Compara os dados para ver quem tem mais sorte. Ou azar.
  if (dadoAtaque > dadoDefesa)
  {
    printf("VITORIA DO ATACANTE! O territorio %s foi conquistado.\n", defensor->nome);
    // O defensor agora pertence ao atacante.
    strcpy(defensor->cor, atacante->cor);
    // O atacante move metade de suas tropas (arredondado para baixo).
    int tropasMovidas = atacante->tropas / 2;
    defensor->tropas = tropasMovidas;
    atacante->tropas -= tropasMovidas;
  }
  else
  {
    printf("VITORIA DO DEFENSOR! O territorio %s resistiu ao ataque.\n", defensor->nome);
    // O atacante perde uma tropa pela ousadia.
    if (atacante->tropas > 1)
    {
      atacante->tropas -= 1;
    }
  }
}

/**
 * @brief Libera a memória alocada para o mapa.
 * @param mapa Ponteiro para a memória que precisa ser liberada.
 */
void liberarMemoria(struct Territorio *mapa)
{
  printf("\nLiberando memoria... Fim do jogo.\n");
  free(mapa);
}