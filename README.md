

---
### **Roteiro para o Trabalho 1: Programação Multithread com PThreads**

**Objetivo Geral:** Contabilizar números primos em uma matriz de números naturais aleatórios, comparando o desempenho da busca serial e paralela usando PThreads.

---
### **I. Fase de Configuração Inicial do Ambiente**
1.  **Instalação do Visual Studio 2022 Community:**
    * Baixe e execute o instalador do Visual Studio 2022 Community no site da Microsoft.
    * No instalador, certifique-se de marcar o item **"Desenvolvimento para desktop com C++"**. Instale apenas os componentes necessários, pois o VS é um software "gigantesco".
    * Escolha a opção **"Instalar durante o download"** se sua internet for rápida, ou "Baixar tudo, depois instalar" para conexões lentas.
    * Após a instalação, reinicie o computador.
2.  **Criação do Projeto Básico:**
    * Inicie o VS 2022. Se for a primeira vez, pode ignorar o login/criação de conta e a escolha de esquema de cores.
    * Na tela inicial, selecione **"Continuar sem código"**.
    * Crie um novo projeto indo em **Arquivo > Novo > Projeto**.
    * Escolha a opção **"Projeto Vazio"** e clique em "Próximo".
    * Dê um nome ao seu Projeto e Solução (podem ser o mesmo) e escolha o local para salvar.
    * No "Gerenciador de Soluções" (geralmente à direita da janela), clique com o botão direito na pasta **"Arquivos de Origem"** e selecione **Adicionar > Novo Item...**.
    * Selecione Visual C++ > Código e marque a opção **"Arquivo do C++ (.cpp)"**. Importante: Dê um nome ao arquivo e adicione manualmente a extensão **.c** ao final (ex: `main.c`) para que o compilador do VS o trate como C puro, não C++.
3.  **Configuração do PThreads no Visual Studio 2022:**
    * Vá em **Ferramentas > Gerenciador de pacotes do NuGet > Console do Gerenciador de Pacotes**.
    * No console que aparecerá (com `PM>`), digite o comando `NuGet\Install-Package pthreads -Version 2.9.1.4` e pressione Enter. O pacote não é mais listado automaticamente, mas a instalação manual é simples.
    * O pacote NuGet não incorpora a biblioteca necessária, o arquivo `pthreadvc2.lib`.
    * Feche o VS 2022.
    * Localize o pacote "PThreadFilesVS2022" fornecido. Copie `PThreadFilesVS2022 > lib > x86 > pthreadVC2.lib` para `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\xx.xx.xxxxx\lib\x86\` (onde `xx.xx.xxxxx` é a versão do MSVC).
    * Copie `PThreadFilesVS2022 > lib > x64 > pthreadVC2.lib` para `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\xx.xx.xxxxx\lib\x64\`.
    * Abra novamente o VS 2022 no seu projeto. Se o VS reclamar do pacote, ignore.
    * Copie os arquivos `.dll` para a pasta de saída do seu projeto. Para 32 bits, copie a DLL x86 para a pasta `Debug`; para 64 bits, copie a DLL x64 para `x64\Debug`.
    * Para que as PThreads funcionem, adicione as seguintes linhas no início do seu arquivo `.c`:
        ```c
        #define HAVE_STRUCT_TIMESPEC
        #pragma comment(lib, "pthreadVC2.lib")
        #include <pthread.h>
        #include <stdio.h>
        #include <stdlib.h>
        ```
4.  **Configuração para 64 bits (x64):**
    * Altere a configuração de compilação do seu projeto para **x64**. Isso é um ajuste simples na barra de ferramentas do VS e é crucial para lidar com matrizes muito grandes (superiores a 2 GB de memória, limite para processos de 32 bits no Windows).

---
### **II. Fase de Implementação do Código**
1.  **Parametrização da Matriz:**
    * Utilize diretivas `#define` para parametrizar a largura e altura da matriz principal.
    * A matriz deve ser consideravelmente grande para que as medidas de desempenho sejam consistentes (ex: `10000x10000` é um bom começo). Monitore o consumo de memória para evitar o uso significativo de "Memória Virtual" (armazenamento secundário), o que falsearia as medições de tempo.
2.  **Variáveis Globais (Compartilhadas):**
    * A matriz de números naturais aleatórios e a variável que contabiliza o número de primos encontrados na matriz devem ser **globais** e **únicas**.
    * A variável que controla quais macroblocos estão livres/alocados também deverá ser **global (compartilhada)**.
3.  **Alocação Dinâmica da Matriz:**
    * A matriz deverá ser alocada dinamicamente.
    * Utilize a abordagem de alocação dinâmica de matrizes em C, como explicado na **página 113** da "Apostila Programação em C". O professor será "rígido" quanto à forma correta de fazer isso.
    * Lembre-se de verificar se a alocação foi bem-sucedida (se o ponteiro retornado não é `NULL`). Caso contrário, saia do programa usando `exit(1)`.
4.  **Geração da Matriz de Números Aleatórios:**
    * Utilize as funções `srand()` e `rand()` para gerar números naturais aleatórios no intervalo de 0 a 31999.
    * Use uma semente pré-definida em `srand()` para garantir a mesma matriz para todos os testes (reprodutibilidade).
5.  **Função de Verificação de Primo (`ehPrimo`):**
    * Implemente a função `int ehPrimo(int n)`. Ela deve retornar `1` (true) se `n` for primo, e `0` (false) caso contrário.
    * **Otimização:** Teste divisores até a raiz quadrada do número (`sqrt(n)`). No primeiro divisor encontrado, conclua que não é primo e retorne. Se nenhum divisor for encontrado até a raiz quadrada, é primo. Não otimize excessivamente, pois o objetivo é ter uma carga de trabalho considerável para testar a paralelização.
    * Lembre-se de incluir `<math.h>` para usar `sqrt()`.
6.  **Busca Serial:**
    * Implemente uma função (ex: `BuscaSerial()`) que percorra a matriz inteira de forma sequencial, chamando `ehPrimo` para cada número e contabilizando os primos.
    * Meça o tempo decorrido para essa busca. Este será o seu tempo de referência.
    * Exiba a quantidade de números primos encontrados e o tempo decorrido.
    * Para medição de tempo, você pode usar `clock()` do `<time.h>`.
7.  **Busca Paralela com PThreads:**
    * **Função da Thread (`runner`):** Crie uma função (ex: `void *runner(void *param)`) que será o ponto de entrada para cada thread.
    * **Gerenciamento de Macroblocos (Carga de Trabalho):**
        * Cada thread deve processar "macroblocos" (submatrizes) da matriz principal.
        * Evite criar uma estrutura de dados separada para armazenar as coordenadas de início/fim dos macroblocos, especialmente para blocos muito pequenos, pois isso pode consumir muita RAM e inviabilizar o teste em alguns computadores. Calcule as coordenadas na hora de atribuir um macrobloco a uma thread.
        * Implemente um mecanismo onde as threads buscam dinamicamente o próximo macrobloco "livre" para processar.
        * A variável que controla a alocação dos macroblocos deve ser global e protegida por um mutex.
    * **Acesso a Dados Compartilhados (Seções Críticas) e Mutexes:**
        * Tanto a variável global que armazena o número total de primos quanto a variável que controla a alocação de macroblocos são dados compartilhados e devem ter seu acesso controlado por **mutexes** para garantir exclusão mútua.
        * **Declaração e Inicialização:** Declare uma variável `pthread_mutex_t` global (ex: `pthread_mutex_t count_mutex;`, `pthread_mutex_t block_mutex;`). Inicialize-as com `pthread_mutex_init(&mutex_variable, NULL)` antes de criar as threads.
        * **Travamento e Liberação:** Use `pthread_mutex_lock(&mutex_variable)` antes de acessar a seção crítica. Use `pthread_mutex_unlock(&mutex_variable)` imediatamente após a seção crítica.
        * **Desalocação:** Use `pthread_mutex_destroy(&mutex_variable)` no final do `main` para liberar os recursos do mutex.
        * Cada thread deve usar uma variável temporária para contar os primos dentro do seu macrobloco atual e, após terminar a busca, somar esse valor à variável global de total de primos, protegendo essa soma com um mutex.
    * **Criação e Espera de Threads:**
        * Determine o número de threads a serem criadas com base no número de núcleos físicos do processador (`N ≥ 2`). Considere também testar com o número de núcleos lógicos/virtuais (se houver SMT/HT).
        * Crie as threads utilizando `pthread_create(&thread, &attr, start_routine, arg)` em um loop. Passe a função `runner` como `start_routine`.
        * A thread principal (`main`) deve esperar que todas as threads worker (`runner`) terminem, usando `pthread_join(tid, NULL)` em um loop.
        * Permita que as threads `runner` terminem naturalmente quando não houver mais macroblocos para processar.
    * **Medição de Tempo:** Meça o tempo decorrido para a busca paralela e exiba-o.
    * **Fluxo de Teste e Interação:**
        * Forneça uma forma prática e fácil de testar o código em modo serial, multi-thread, ou ambos. Um menu simples no código ou chamadas de função claras (ex: `BuscaSerial();`, `BuscaParalela();`) que possam ser facilmente comentadas/descomentadas são aceitáveis.
        * Permita que os testes multi e single aconteçam numa única "rodada" do programa.
        * Ao final de cada busca (serial e paralela), exiba o **speedup** (Lei de Amdahl).
---
### **III. Fase de Testes e Análise (para o Relatório)**
1.  **Influência do Tamanho dos Macroblocos:**
    * Fixe o número de threads igual à quantidade de núcleos físicos do seu processador.
    * Realize múltiplos testes variando o tamanho dos macroblocos: desde `1x1` até as dimensões da matriz completa. Inclua tamanhos muito pequenos, muito grandes, e alguns intermediários onde o tempo de execução não varie muito.
    * Elabore um gráfico correlacionando o tamanho dos macroblocos com o tempo de execução multithread.
2.  **Influência do Número de Threads:**
    * Utilize um tamanho de macrobloco considerado adequado (nem muito grande, nem muito pequeno, determinado pelo teste anterior).
    * Mensure o speedup rodando com diferentes números de threads (ex: 2, 4, 8, 16 threads, adaptando à sua CPU).
    * Analise se os resultados são coerentes com a Lei de Amdahl. Destaque a diferença entre núcleos físicos e lógicos (virtuais).
    * Calcule e identifique claramente o speedup no relatório.
    * Realize testes em pelo menos dois computadores "bem diferentes" e compare o desempenho.
    * Aumente o número de threads para centenas ou mais para observar o "overhead" (custo de gerenciamento de threads) e analise os resultados, mantendo o tamanho do macrobloco fixo.
3.  **Impacto da Remoção de Mutexes:**
    * Temporariamente remova os mutexes que protegem as regiões críticas.
    * Execute o programa com macroblocos pequenos e grandes e observe os resultados. Descreva as consequências (ex: resultados inconsistentes, erros de corrida).
---
### **IV. Observações Gerais e Dicas**
* **Modularização:** Neste trabalho específico, coloque todo o código em um **único arquivo .c**, mesmo com a modularização lógica.
* **Otimização de Código:** Evite colocar cálculos complexos dentro de loops se eles não variarem a cada iteração.
* **Warnings do Compilador:** Preste atenção aos warnings do Visual Studio e tente corrigi-los, especialmente aqueles relacionados a alocação dinâmica (`ponteiro NULL`) e indireção múltipla.
* **Depuração:** Utilize a ferramenta de debug do Visual Studio em vez de `printf("passou aqui")`.
* **Conteúdo do Relatório:** O relatório deve focar nos testes e análises, com tabelas e gráficos. Não é necessário explicar o código no relatório; use comentários detalhados no próprio código fonte.
* **Configurações do Computador:** Inclua no relatório as configurações básicas dos computadores usados nos testes (CPU e RAM).
* **Plágio e IA:** O uso de trabalhos anteriores ou ferramentas de IA de forma indiscriminada é proibido. O professor convocará para uma entrevista presencial em caso de suspeita, podendo resultar em nota zero.

--- 