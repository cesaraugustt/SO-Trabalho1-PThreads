#define HAVE_STRUCT_TIMESPEC
#pragma comment(lib, "pthreadVC2.lib")

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MATRIZ_ALTURA 10000
#define MATRIZ_LARGURA 10000
#define BLOCO_ALTURA 100
#define BLOCO_LARGURA 100
#define NUM_THREADS 12

int** matriz;
int total_primos_serial = 0;
int total_primos_paralelo = 0;

int proximo_bloco = 0;
int NUM_BLOCOS_TOTAL;

pthread_mutex_t mutex_proximo_bloco;
pthread_mutex_t mutex_total_primos;

int** alocarMatriz(int altura, int largura) {
	printf("Alocando matriz %dx%d\n", altura, largura);
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
	printf("Liberando matriz\n");
	for (int i = 0; i < altura; i++) {
		free(mat[i]);
	}
	free(mat);
	printf("Matriz liberada.\n");
}

void gerarMatrizAleatoria(int** mat, int altura, int largura) {
	printf("Gerando numeros aleatorios para a matriz.\n");
	srand(25); // Semente fixa
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

	for (int i = 2; i * i <= n; i++) {
		// O loop continua enquanto i for menor ou igual a raiz quadrada de n.
		if (n % i == 0) return 0;
	}

	return 1;
}

void buscaSerial() {
	printf("Iniciando busca serial \n");
	total_primos_serial = 0;
	for (int i = 0; i < MATRIZ_ALTURA; i++) {
		for (int j = 0; j < MATRIZ_LARGURA; j++) {
			if (verificarPrimo(matriz[i][j])) total_primos_serial++;
		}
	}
}

int main() {
	matriz = alocarMatriz(MATRIZ_ALTURA, MATRIZ_LARGURA);
	gerarMatrizAleatoria(matriz, MATRIZ_ALTURA, MATRIZ_LARGURA);

	clock_t inicio, fim;
	double tempo_serial;

	inicio = clock();
	buscaSerial();
	fim = clock();
	tempo_serial = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
	printf("Busca Serial finalizada.\n");
	printf("Primos encontrados: %d\n", total_primos_serial);
	printf("Tempo de execucao: %.4f segundos\n", tempo_serial);

	liberarMatriz(matriz, MATRIZ_ALTURA);

	return 0;
}