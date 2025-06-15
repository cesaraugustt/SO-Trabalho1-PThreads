#define HAVE_STRUCT_TIMESPEC
#pragma comment(lib, "pthreadVC2.lib")

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define MATRIZ_ALTURA 10000
#define MATRIZ_LARGURA 10000

#define BLOCO_ALTURA 100
#define BLOCO_LARGURA 100

#define NUM_THREADS 12
#define SEMENTE 25

int **matriz;
int total_primos_serial = 0;
int total_primos_paralelo = 0;
int proximo_bloco = 0;
int NUM_BLOCOS_TOTAL;

pthread_mutex_t mutex_proximo_bloco;
pthread_mutex_t mutex_total_primos;

int** alocarMatriz(int altura, int largura) {
	printf("Alocando matriz %dx%d...\n", altura, largura);
	int** mat = (int**)malloc(altura * sizeof(int*));

	if (mat == NULL) {
		printf("Erro: Falha na alocação da matriz. \n");
		exit(1);
	}

	for (int i = 0; i < altura; i++) {
		mat[i] = (int*)malloc(largura * sizeof(int));
		if (mat[i] == NULL) {
			printf("Erro: Falha na alocação da matriz. \n");
			exit(1);
		}
	}

	printf("Matriz alocada com sucesso.\n");
	return mat;
}

void liberarMatriz(int** mat, int altura) {
	printf("Liberando matriz...\n");
	for (int i = 0; i < altura; i++) {
		free(mat[i]);
	}
	free(mat);
	printf("Matriz liberada.\n");
}

void gerarMatrizAleatoria(int** mat, int altura, int largura, int semente) {
	printf("Gerando numeros aleatorios para a matriz...\n");
	srand(semente); // Semente fixa
	for (int i = 0; i < altura; i++) {
		for (int j = 0; j < largura; j++) {
			mat[i][j] = rand() % 32000; // Limita o resultado a um valor entre 0 e 31999.
		}
	}
	printf("Matriz preenchida.\n");
}

int verificarPrimo(int n) {
	if (n <= 1) return 0;
	if (n <= 3) return 1;

	double limite = sqrt(n);
	for (int i = 2; i <= limite; i++) {
		// O loop continua enquanto i for menor ou igual a raiz quadrada de n.
		if (n % i == 0) return 0;
	}

	return 1;
}

void buscaSerial() {
	total_primos_serial = 0;
	for (int i = 0; i < MATRIZ_ALTURA; i++) {
		for (int j = 0; j < MATRIZ_LARGURA; j++) {
			if (verificarPrimo(matriz[i][j])) total_primos_serial++;
		}
	}
}

void* runner() {
	int primos_locais = 0;
	int bloco_autal;
	int blocos_por_linhas = MATRIZ_LARGURA / BLOCO_LARGURA;

	while (1) {
		pthread_mutex_lock(&mutex_proximo_bloco);
		bloco_autal = proximo_bloco++;
		pthread_mutex_unlock(&mutex_proximo_bloco);

		if (bloco_autal >= NUM_BLOCOS_TOTAL) break;

		int linha_inicio = (bloco_autal / blocos_por_linhas) * BLOCO_ALTURA;
		int coluna_inicio = (bloco_autal % blocos_por_linhas) * BLOCO_LARGURA;
		int linha_fim = linha_inicio + BLOCO_ALTURA;
		int coluna_fim = coluna_inicio + BLOCO_LARGURA;

		if (linha_fim > MATRIZ_ALTURA) linha_fim = MATRIZ_ALTURA;
		if (coluna_fim > MATRIZ_LARGURA) coluna_fim = MATRIZ_LARGURA;

		for (int i = linha_inicio; i < linha_fim; i++) {
			for (int j = coluna_inicio; j < coluna_fim; j++) {
				if (verificarPrimo(matriz[i][j])) primos_locais++;
			}
		}
	}

	pthread_mutex_lock(&mutex_total_primos);
	total_primos_paralelo += primos_locais;
	pthread_mutex_unlock(&mutex_total_primos);

	return NULL;
}

void buscaParalela() {
	total_primos_paralelo = 0;
	proximo_bloco = 0;

	if (pthread_mutex_init(&mutex_proximo_bloco, NULL) != 0) {
		printf("Erro ao inicializar mutex_proximo_bloco");
		exit(1);
	}
	if (pthread_mutex_init(&mutex_total_primos, NULL) != 0) {
		printf("Erro ao inicializar mutex_total_primos");
		exit(1);
	}

	pthread_t threads[NUM_THREADS];

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, runner, NULL);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex_proximo_bloco);
	pthread_mutex_destroy(&mutex_total_primos);
}

double executarTesteSerial() {
	printf("Iniciando busca serial...\n");
	clock_t inicio, fim;

	inicio = clock();
	buscaSerial();
	fim = clock();

	double tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

	printf("\n--- Resultado da Busca Serial ---\n");
	printf("Primos encontrados: %d\n", total_primos_serial);
	printf("Tempo de execucao: %.4f segundos\n", tempo_gasto);

	return tempo_gasto;
}

double executarTesteParalelo() {
	printf("Iniciando busca paralela com %d threads...\n", NUM_THREADS);
	clock_t inicio, fim;

	inicio = clock();
	buscaParalela();
	fim = clock();

	double tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

	printf("\n--- Resultado da Busca Paralela ---\n");
	printf("Primos encontrados: %d\n", total_primos_paralelo);
	printf("Tempo de execucao: %.4f segundos\n", tempo_gasto);

	return tempo_gasto;
}

void exibirMenu() {
	printf("\n=== MENU DE TESTES ===\n");
	printf("1. Busca Serial\n");
	printf("2. Busca Paralela\n");
	printf("3. Executar ambos os testes\n");
	printf("4. Sair\n");
	printf("Escolha uma opção: ");
}

int main() {
	system("cls");
	printf("=== SISTEMA DE BUSCA PARALELA DE NÚMEROS PRIMOS ===\n");
	printf("Configuração:\n");
	printf("- Matriz: %dx%d\n", MATRIZ_ALTURA, MATRIZ_LARGURA);
	printf("- Macrobloco: %dx%d\n", BLOCO_ALTURA, BLOCO_LARGURA);
	printf("- Threads: %d\n", NUM_THREADS);

	matriz = alocarMatriz(MATRIZ_ALTURA, MATRIZ_LARGURA);
	gerarMatrizAleatoria(matriz, MATRIZ_ALTURA, MATRIZ_LARGURA, SEMENTE);

	NUM_BLOCOS_TOTAL = (MATRIZ_ALTURA / BLOCO_ALTURA) * (MATRIZ_LARGURA / BLOCO_LARGURA);

	clock_t inicio, fim;
	double tempo_serial, tempo_paralelo;

	int opcao;
	do {
		exibirMenu();

		char buffer_entrada[10];
		if (fgets(buffer_entrada, sizeof(buffer_entrada), stdin) != NULL) {
			opcao = atoi(buffer_entrada);
		}
		else {
			opcao = 0;
		}

		switch (opcao) {
			case 1:
				executarTesteSerial();
				break;
			case 2:
				executarTesteParalelo();
				break;
			case 3:
				tempo_serial = executarTesteSerial();
				tempo_paralelo = executarTesteParalelo();

				// Análise de desempenho
				printf("\n--- Analise de Desempenho ---\n");
				if (total_primos_paralelo != total_primos_serial) {
					printf("AVISO: A contagem de primos serial e paralela divergem!\n");
				}
				if (tempo_paralelo > 0) {
					double speedup = tempo_serial / tempo_paralelo;
					printf("Speedup: %.4fx\n", speedup);
				}
				else {
					printf("Speedup: N/A (tempo paralelo foi zero)\n");
				}
				break;
			case 4:
				printf("Encerrando programa...\n");
				break;
			default:
				printf("Opção inválida!\n");
		}
	} while (opcao != 4);

	liberarMatriz(matriz, MATRIZ_ALTURA);

	return 0;
}