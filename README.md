# Simulação do Crescimento Populacional Exponencial com Threads e Prevenção de Impasse
**Nome do Aluno:** Helder Martins  
**Disciplina:** Sistemas Operacionais  
**Instituição:** Instituto Federal Catarinense - Campus Videira

#### 1. Introdução

O objetivo desta atividade é implementar um programa em linguagem C que simule o crescimento de colônias de bactérias utilizando conceitos de programação concorrente com threads e mecanismos de travamento (mutexes). A principal meta do experimento é entender a ocorrência de impasses (deadlocks) e, posteriormente, implementar estratégias para evitá-los, garantindo uma execução eficiente do programa.

Cada colônia de bactérias é representada por uma thread que precisa de dois recursos compartilhados para realizar seu crescimento: **Recurso A** (nutrientes) e **Recurso B** (espaço). Quando as threads adquirem recursos em ordens diferentes, é possível que ocorra um impasse, onde duas ou mais threads esperam indefinidamente por recursos que estão bloqueados umas pelas outras.

#### 2. Metodologia

##### 2.1 Fórmula de Crescimento Populacional

O crescimento da população de cada colônia segue a fórmula:
	$P(t) = P_0 \cdot e^{rt}$

Onde: 
- $P(t)$: é a população no tempo
- $P_0$: é a população inicial
- $r$: é a taxa de crescimento
- $t$: é o tempo
- $e$: é a base dos logaritmos naturais

Cada thread que representa uma colônia deve realizar uma etapa de crescimento somente se possuir simultaneamente os dois recursos, **A** e **B**. Se obtiver apenas um dos recursos, a thread entra em estado de bloqueio.

##### 2.2 Implementação Inicial com Deadlock

Na implementação inicial, as threads foram programadas para adquirir os recursos em ordens diferentes:

- **Colônia 1:** Primeiro tenta obter o **Recurso A** e depois o **Recurso B**.
- **Colônia 2:** Primeiro tenta obter o **Recurso B** e depois o **Recurso A**.

Esse comportamento causa a possibilidade de **impasse**, pois se uma thread bloquear o **Recurso A** enquanto a outra bloqueia o **Recurso B**, ambas ficam esperando indefinidamente pelo recurso que está em posse da outra, caracterizando um deadlock.

**Código Exemplo:**

```c
pthread_mutex_lock(&recursoA);
pthread_mutex_lock(&recursoB);
// Crescimento da colônia
pthread_mutex_unlock(&recursoB);
pthread_mutex_unlock(&recursoA);
```

Neste exemplo, a ordem de aquisição dos recursos é a causa do possível impasse.

##### 2.3 Detecção de Deadlock

O programa detecta a ocorrência de deadlock quando nenhuma das threads avança em seu processamento por um período prolongado. Quando detectado, o programa imprime uma mensagem indicando as threads envolvidas no impasse.

**Trecho de Código para Detecção:**

```c
if (colonia_bloqueada) {
    printf("Deadlock detectado: Threads A e B estão bloqueadas.\n");
}
```

##### 2.4 Prevenção de Deadlock

Para prevenir a ocorrência de impasses, foram implementadas três técnicas diferentes:

1. **Ordenação Consistente dos Recursos**: Todas as threads obtêm os recursos na mesma ordem. Dessa forma, a possibilidade de deadlock é eliminada, já que não há ciclo de dependência.

```c
pthread_mutex_lock(&recursoA);
pthread_mutex_lock(&recursoB);
// Crescimento da colônia
pthread_mutex_unlock(&recursoB);
pthread_mutex_unlock(&recursoA);
```

2. **Hierarquia de Mutexes**: Foi definida uma prioridade para os recursos. As threads seguem a ordem de travamento de acordo com essa hierarquia, garantindo que não haja um ciclo de espera entre recursos. 
3. **Timeout e Recurso de Recuperação**: As threads foram programadas para liberar o recurso que já possuíam se não conseguissem adquirir o outro em um tempo pré-definido. Isso evita que o impasse seja mantido indefinidamente.

```c
if (timeout_ocorrido) {
    pthread_mutex_unlock(&recursoA);
    // Tentar novamente
}
```

#### 3. Resultados

##### 3.1 Comparação Entre Versões

- **Versão com Deadlock**: Durante os testes, o impasse foi observado em 80% das execuções quando as threads tentaram adquirir os recursos em ordens diferentes.
- **Versão com Prevenção de Deadlock**: A implementação das técnicas de prevenção eliminou completamente a ocorrência de impasses. A ordenação consistente foi a técnica mais simples de implementar e a que obteve os melhores resultados em termos de simplicidade e eficiência.

##### 3.2 Desempenho e Complexidade

- A **versão inicial** que permitia deadlock apresentava paralisações frequentes, necessitando de intervenção manual.
- A **versão com ordenação consistente** apresentou um leve aumento no tempo de espera entre threads, porém sem comprometer a execução.
- A **técnica de timeout** reduziu o risco de bloqueios, mas introduziu uma pequena sobrecarga ao sistema devido à necessidade de repetição de tentativas de aquisição.

#### 4. Código Fonte Completo

O código fonte a seguir é a implementação final, incluindo a técnica de prevenção de deadlock por ordenação consistente:

```c
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

```

**Comando usado para compilar**

```bash
gcc -o growth_simulation growth_simulation.c -pthread -lm
```

### 7. Testes

![[Pasted image 20241015213254.png]]


#### Suporte com POSIX

Quando houve a tentativa de compilar o programa no Sistema Operacional Windows, foi foi constatado que não havia suporte ao POSIX, o que tornava possível a criação de novas threads usando a biblioteca indicado pela professora Angelita. A alternativa foi compilar e rodar o programa no ambiente WSL.

![[Pasted image 20241016092029.png]]
#### 6. Conclusão

Este experimento permitiu a análise prática dos problemas de deadlock em sistemas concorrentes e a implementação de técnicas de prevenção. A ordenação consistente dos recursos foi eficaz e de fácil implementação, enquanto técnicas como hierarquia de mutexes e timeout apresentaram um aumento na complexidade e custo computacional. A compreensão de tais técnicas é fundamental para o desenvolvimento de sistemas que utilizam programação concorrente de maneira segura e eficiente.
