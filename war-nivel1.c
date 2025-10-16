// Bibliotecas necessárias para manipulação de entrada/saída (stdio.h)
// e para manipulação de strings (string.h), embora não seja estritamente usada aqui
// de forma complexa, é uma boa prática.
#include <stdio.h>
#include <string.h>

// --- Criação da Struct ---
// Define a estrutura 'Territorio'. Pense nisto como um molde
// para criar variáveis que guardam múltiplos dados relacionados.
// Neste caso, as informações de um território do jogo.
struct Territorio
{
  char nome[30];
  char cor[10];
  int tropas;
};

int main()
{
  // --- Declaração do Vetor de Structs ---
  // Cria um vetor chamado 'mapa' que pode armazenar 5 estruturas do tipo Territorio.
  // Basicamente, uma lista para guardar seus 5 futuros domínios.
  struct Territorio mapa[5];
  int i; // Variável de controle para os laços. Tão original.

  // --- Cadastro dos Territórios (Entrada de Dados) ---
  // Um laço 'for' para repetir o processo de cadastro 5 vezes.
  // Menos doloroso que preencher formulários do governo, eu suponho.
  printf("--- Cadastro de Territorios do Jogo WAR ---\n");
  for (i = 0; i < 5; i++)
  {
    printf("\n--- Cadastrando Territorio %d ---\n", i + 1);

    // Pede e armazena o nome do território.
    printf("Digite o nome do territorio: ");
    scanf("%s", mapa[i].nome);

    // Pede e armazena a cor do exército.
    printf("Digite a cor do exercito: ");
    scanf("%s", mapa[i].cor);

    // Pede e armazena o número de tropas.
    printf("Digite a quantidade de tropas: ");
    scanf("%d", &mapa[i].tropas);
  }

  // --- Exibição dos Dados ---
  // Outro laço 'for' para percorrer o vetor 'mapa' já preenchido
  // e exibir as informações de cada território. A sua recompensa.
  printf("\n\n--- Relatorio de Territorios Cadastrados ---\n");
  for (i = 0; i < 5; i++)
  {
    printf("----------------------------------------\n");
    printf("Territorio: %s\n", mapa[i].nome);
    printf("Cor do Exercito: %s\n", mapa[i].cor);
    printf("Quantidade de Tropas: %d\n", mapa[i].tropas);
  }
  printf("----------------------------------------\n");

  return 0; // O programa acaba aqui. Felizmente.
}