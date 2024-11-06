#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#define NUM_COLONIAS 2
#define TEMPO_TOTAL 10

pthread_mutex_t recursoA;
pthread_mutex_t recursoB;

void *crescimento_colonia(void *args) {
    int id = *((int *)args);
    for (int t = 0; t < TEMPO_TOTAL; t++) {
        // Ordenação consistente dos recursos
        pthread_mutex_lock(&recursoA);
        pthread_mutex_lock(&recursoB);

        // Simulação do crescimento exponencial
        double P0 = 100.0;  // População inicial
        double r = 0.05;    // Taxa de crescimento
        double Pt = P0 * exp(r * t);
        printf("Colônia %d - Tempo %d: População = %.2f\n", id, t, Pt);

        // Liberação dos recursos
        pthread_mutex_unlock(&recursoB);
        pthread_mutex_unlock(&recursoA);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t colonias[NUM_COLONIAS];
    int ids[NUM_COLONIAS];

    // Inicialização dos mutexes
    pthread_mutex_init(&recursoA, NULL);
    pthread_mutex_init(&recursoB, NULL);

    // Criação das threads
    for (int i = 0; i < NUM_COLONIAS; i++) {
        ids[i] = i + 1;
        pthread_create(&colonias[i], NULL, crescimento_colonia, &ids[i]);
    }

    // Espera pelas threads terminarem
    for (int i = 0; i < NUM_COLONIAS; i++) {
        pthread_join(colonias[i], NULL);
    }

    // Destruição dos mutexes
    pthread_mutex_destroy(&recursoA);
    pthread_mutex_destroy(&recursoB);

    return 0;
}
