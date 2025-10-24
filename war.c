// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CODIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o codigo em funcoes especializadas.
// - Implementar um sistema de missoes para um jogador.
// - Criar uma funcao para verificar se a missao foi cumprida.
// - Utilizar passagem por referencia (ponteiros) para modificar dados e
//   passagem por valor/referencia constante (const) para apenas ler.
// - Foco em: Design de software, modularizacao, const correctness, logica de jogo.
//
// ============================================================================

// Inclusao das bibliotecas padrao necessarias para entrada/saida, alocacao de memoria, manipulacao de strings e tempo.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Constantes Globais ---
// Definem valores fixos para o numero de territorios, missoes e tamanho maximo de strings, facilitando a manutencao.
#define NUM_TERRITORIOS 42
#define NUM_MISSOES 5
#define NUM_CORES 6
#define MAX_NOME 30
#define MAX_COR 10
#define MAX_VIDA 1000
#define MAX_TROPAS_ATAQUE 100

// --- Estrutura de Dados ---
// Define a estrutura para um territorio, contendo seu nome, a cor do exercito que o domina e o numero de tropas.
typedef struct {
    char nome[MAX_NOME];
    char corExercito[MAX_COR];
    int numTropas;
    int vida;           // Vida do territorio (0-1000)
    int poder;          // Poder de ataque do territorio (0-1000)
} Territorio;

// Estrutura para dados do jogador
typedef struct {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    char paisOrigem[MAX_NOME];
    int vida;
    int poder;
    int pontos;
    int territoriosConquistados;
    int batalhasVencidas;
    int batalhasPerdidas;
} Jogador;

// Estrutura para estatisticas dos exercitos
typedef struct {
    char cor[MAX_COR];
    int territorios;
    int tropasTotal;
    int vidaTotal;
    int poderTotal;
    int pontos;
} EstatisticaExercito;

// --- Prototipos das Funcoes ---
// Declaracoes antecipadas de todas as funcoes que serao usadas no programa, organizadas por categoria.

// Funcoes de setup e gerenciamento de memoria:
Territorio* alocarMapa(void);
void inicializarTerritorios(Territorio* mapa, const Jogador* jogador);
void liberarMemoria(Territorio* mapa);

// Funcoes de configuracao do jogador:
void configurarJogador(Jogador* jogador, const Territorio* mapa);
void exibirCoresDisponiveis(void);
void exibirPaisesDisponiveis(const Territorio* mapa);
void exibirStatusJogador(const Jogador* jogador);

// Funcoes de estatisticas e ranking:
void calcularEstatisticas(const Territorio* mapa, EstatisticaExercito estatisticas[]);
void exibirRanking(const EstatisticaExercito estatisticas[], const Jogador* jogador);
void exibirStatusGeral(const Territorio* mapa, const Jogador* jogador);
void atualizarPontuacao(Jogador* jogador, int tipoAcao, int valor);
void exibirResultadoFinal(const Territorio* mapa, const Jogador* jogador, int vitoria);
void exibirInimigosEAliados(const Territorio* mapa, const Jogador* jogador);
void analisarRelacoesDiplomaticas(const Territorio* mapa, const Jogador* jogador);

// Funcoes de interface com o usuario:
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio* mapa);
void exibirMapaComStatus(const Territorio* mapa, const Jogador* jogador);
void exibirMissao(int idMissao);

// Funcoes de logica principal do jogo:
void faseDeAtaque(Territorio* mapa, Jogador* jogador);
void simularAtaque(Territorio* territorioOrigem, Territorio* territorioDestino, Jogador* jogador, int tropasAtaque);
int sortearMissao(void);
int verificarVitoria(const Territorio* mapa, int idMissao, const char* corJogador);

// Funcao utilitaria:
void limparBufferEntrada(void);

// --- Funcao Principal (main) ---
// Funcao principal que orquestra o fluxo do jogo, chamando as outras funcoes em ordem.
int main() {
    // 1. Configuracao Inicial (Setup):
    // Removido setlocale para evitar problemas com caracteres especiais
    srand((unsigned int)time(NULL));
    
    Territorio* mapa = alocarMapa();
    if (mapa == NULL) {
        printf("Erro: Nao foi possivel alocar memoria para o mapa!\n");
        return 1;
    }
    
    // Configuracao do jogador
    Jogador jogador = {0}; // Inicializa com zeros
    printf("=== BEM-VINDO AO WAR ESTRUTURADO ===\n\n");
    
    // Inicializacao temporaria para permitir configuracao
    inicializarTerritorios(mapa, NULL);
    configurarJogador(&jogador, mapa);
    
    // Reinicializa com dados do jogador
    inicializarTerritorios(mapa, &jogador);
    
    int missaoJogador = sortearMissao();
    int opcao;
    int jogoAtivo = 1;
    
    printf("\nSua missao foi sorteada!\n\n");
    
    // 2. Laco Principal do Jogo (Game Loop):
    do {
        exibirStatusGeral(mapa, &jogador);
        exibirMissao(missaoJogador);
        exibirMenuPrincipal();
        
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limparBufferEntrada();
        
        switch (opcao) {
            case 1:
                // Opcao 1: Inicia a fase de ataque.
                faseDeAtaque(mapa, &jogador);
                break;
                
            case 2:
                // Opcao 2: Verifica se a condicao de vitoria foi alcancada e informa o jogador.
                if (verificarVitoria(mapa, missaoJogador, jogador.cor)) {
                    printf("\n*** PARABENS! VOCE VENCEU O JOGO! ***\n");
                    exibirResultadoFinal(mapa, &jogador, 1);
                    jogoAtivo = 0;
                } else {
                    printf("\nMissao ainda nao foi cumprida. Continue jogando!\n");
                }
                break;
                
            case 3:
                // Opcao 3: Exibir ranking e estatisticas detalhadas
                {
                    EstatisticaExercito estatisticas[NUM_CORES];
                    calcularEstatisticas(mapa, estatisticas);
                    exibirRanking(estatisticas, &jogador);
                }
                break;
                
            case 4:
                // Opcao 4: Analisar inimigos e aliados
                analisarRelacoesDiplomaticas(mapa, &jogador);
                break;
                
            case 0:
                // Opcao 0: Encerra o jogo.
                printf("Encerrando o jogo...\n");
                exibirResultadoFinal(mapa, &jogador, 0);
                jogoAtivo = 0;
                break;
                
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
        
        if (jogoAtivo) {
            printf("\nPressione Enter para continuar...");
            getchar();
            system("cls"); // Para Windows, use "clear" para Linux/Mac
        }
        
    } while (jogoAtivo);
    
    // 3. Limpeza:
    liberarMemoria(mapa);
    return 0;
}

// --- Implementacao das Funcoes ---

// alocarMapa():
// Aloca dinamicamente a memoria para o vetor de territorios usando calloc.
// Retorna um ponteiro para a memoria alocada ou NULL em caso de falha.
Territorio* alocarMapa(void) {
    return (Territorio*)calloc(NUM_TERRITORIOS, sizeof(Territorio));
}

// inicializarTerritorios():
// Preenche os dados iniciais de cada territorio no mapa (nome, cor do exercito, numero de tropas).
// Esta funcao modifica o mapa passado por referencia (ponteiro).
void inicializarTerritorios(Territorio* mapa, const Jogador* jogador) {
    const char* nomes[NUM_TERRITORIOS] = {
        "Brasil", "Argentina", "Peru", "Venezuela", "Uruguai", "Chile",
        "Mexico", "Estados Unidos", "Canada", "Groenlandia", "Alasca", "Mackenzie",
        "Franca", "Alemanha", "Inglaterra", "Suecia", "Polonia", "Espanha",
        "Egito", "Argelia", "Sudao", "Congo", "Africa do Sul", "Madagascar",
        "Oriente Medio", "Arabia", "India", "China", "Mongolia", "Japao",
        "Vietna", "Coreia", "Siberia", "Tchita", "Ural", "Omsk",
        "Australia", "Nova Guine", "Borneo", "Sumatra", "Nova Zelandia", "Filipinas"
    };
    
    const char* cores[NUM_CORES] = {"Vermelho", "Verde", "Amarelo", "Preto", "Branco", "Rosa"};
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        strcpy(mapa[i].nome, nomes[i]);
        
        // Se o jogador foi configurado e este eh o pais de origem, usar a cor do jogador
        if (jogador != NULL && strcmp(nomes[i], jogador->paisOrigem) == 0) {
            strcpy(mapa[i].corExercito, jogador->cor);
            mapa[i].vida = jogador->vida;
            mapa[i].poder = jogador->poder;
            mapa[i].numTropas = 10; // Pais de origem comeca com mais tropas
        } else {
            strcpy(mapa[i].corExercito, cores[rand() % NUM_CORES]);
            mapa[i].vida = rand() % 301 + 200; // 200 a 500 de vida
            mapa[i].poder = rand() % 301 + 200; // 200 a 500 de poder
            mapa[i].numTropas = rand() % 5 + 1; // 1 a 5 tropas
        }
    }
}

// liberarMemoria():
// Libera a memoria previamente alocada para o mapa usando free.
void liberarMemoria(Territorio* mapa) {
    if (mapa != NULL) {
        free(mapa);
    }
}

// configurarJogador():
// Permite ao jogador escolher seu nome, cor, pais de origem e atributos.
void configurarJogador(Jogador* jogador, const Territorio* mapa) {
    printf("=== CONFIGURACAO DO JOGADOR ===\n");
    
    // Nome do jogador
    printf("Digite seu nome: ");
    fgets(jogador->nome, MAX_NOME, stdin);
    jogador->nome[strcspn(jogador->nome, "\n")] = 0; // Remove newline
    
    // Escolha da cor
    exibirCoresDisponiveis();
    printf("Escolha sua cor (1-6): ");
    int escolhaCor;
    scanf("%d", &escolhaCor);
    limparBufferEntrada();
    
    const char* cores[NUM_CORES] = {"Vermelho", "Verde", "Amarelo", "Preto", "Branco", "Rosa"};
    if (escolhaCor >= 1 && escolhaCor <= NUM_CORES) {
        strcpy(jogador->cor, cores[escolhaCor - 1]);
    } else {
        strcpy(jogador->cor, "Azul"); // Cor padrao
    }
    
    // Escolha do pais de origem
    exibirPaisesDisponiveis(mapa);
    printf("Escolha seu pais de origem (1-%d): ", NUM_TERRITORIOS);
    int escolhaPais;
    scanf("%d", &escolhaPais);
    limparBufferEntrada();
    
    if (escolhaPais >= 1 && escolhaPais <= NUM_TERRITORIOS) {
        strcpy(jogador->paisOrigem, mapa[escolhaPais - 1].nome);
    } else {
        strcpy(jogador->paisOrigem, "Brasil"); // Pais padrao
    }
    
    // Distribuicao de atributos
    printf("\n=== DISTRIBUICAO DE ATRIBUTOS ===\n");
    printf("Voce tem 1000 pontos para distribuir entre Vida e Poder.\n");
    
    printf("Digite pontos para VIDA (0-1000): ");
    scanf("%d", &jogador->vida);
    limparBufferEntrada();
    
    if (jogador->vida < 0) jogador->vida = 0;
    if (jogador->vida > MAX_VIDA) jogador->vida = MAX_VIDA;
    
    jogador->poder = MAX_VIDA - jogador->vida;
    
    // Inicializa estatisticas
    jogador->pontos = 100; // Pontos iniciais
    jogador->territoriosConquistados = 0;
    jogador->batalhasVencidas = 0;
    jogador->batalhasPerdidas = 0;
    
    printf("Pontos de PODER: %d\n", jogador->poder);
    printf("================================\n");
}

// exibirCoresDisponiveis():
// Mostra as cores disponiveis para o jogador escolher.
void exibirCoresDisponiveis(void) {
    printf("\n=== CORES DISPONIVEIS ===\n");
    printf("1. Vermelho\n");
    printf("2. Verde\n");
    printf("3. Amarelo\n");
    printf("4. Preto\n");
    printf("5. Branco\n");
    printf("6. Rosa\n");
    printf("========================\n");
}

// exibirPaisesDisponiveis():
// Mostra todos os paises disponiveis para escolha.
void exibirPaisesDisponiveis(const Territorio* mapa) {
    printf("\n=== PAISES DISPONIVEIS ===\n");
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("%2d. %s\n", i + 1, mapa[i].nome);
        if ((i + 1) % 3 == 0) printf("\n"); // Quebra de linha a cada 3 paises
    }
    printf("==========================\n");
}

// exibirStatusJogador():
// Mostra as informacoes atuais do jogador.
void exibirStatusJogador(const Jogador* jogador) {
    printf("\n=== STATUS DO JOGADOR ===\n");
    printf("Nome: %s\n", jogador->nome);
    printf("Cor: %s\n", jogador->cor);
    printf("Pais de Origem: %s\n", jogador->paisOrigem);
    printf("Vida: %d | Poder: %d\n", jogador->vida, jogador->poder);
    printf("=========================\n");
}

// exibirMenuPrincipal():
// Imprime na tela o menu de acoes disponiveis para o jogador.
void exibirMenuPrincipal(void) {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Atacar territorio\n");
    printf("2. Verificar vitoria\n");
    printf("3. Ver ranking e estatisticas\n");
    printf("4. Analisar inimigos e aliados\n");
    printf("0. Sair do jogo\n");
    printf("=====================\n");
}

// exibirMapa():
// Mostra o estado atual de todos os territorios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a funcao apenas leia os dados do mapa, sem modifica-los.
void exibirMapa(const Territorio* mapa) {
    printf("\n=== MAPA DO MUNDO ===\n");
    printf("%-3s %-20s %-10s %-6s %-6s %-6s %s\n", "ID", "TERRITORIO", "COR", "TROPAS", "VIDA", "PODER", "STATUS");
    printf("=======================================================================\n");
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("%-3d %-20s %-10s %-6d %-6d %-6d %s\n", 
               i + 1, 
               mapa[i].nome, 
               mapa[i].corExercito, 
               mapa[i].numTropas,
               mapa[i].vida,
               mapa[i].poder,
               "---");
    }
    printf("=======================================================================\n");
}

// exibirMissao():
// Exibe a descricao da missao atual do jogador com base no ID da missao sorteada.
void exibirMissao(int idMissao) {
    printf("\n=== SUA MISSAO ===\n");
    
    switch (idMissao) {
        case 1:
            printf("Destruir completamente o exercito VERMELHO.\n");
            break;
        case 2:
            printf("Destruir completamente o exercito VERDE.\n");
            break;
        case 3:
            printf("Conquistar 18 territorios a sua escolha.\n");
            break;
        case 4:
            printf("Conquistar 24 territorios a sua escolha.\n");
            break;
        case 5:
            printf("Conquistar a America do Sul e a Africa inteiras.\n");
            break;
        default:
            printf("Missao desconhecida.\n");
            break;
    }
    printf("==================\n");
}

// faseDeAtaque():
// Gerencia a interface para a acao de ataque, solicitando ao jogador os territorios de origem e destino.
// Chama a funcao simularAtaque() para executar a logica da batalha.
void faseDeAtaque(Territorio* mapa, Jogador* jogador) {
    int origem, destino, tropasBatalha;
    
    printf("\n=== FASE DE ATAQUE ===\n");
    printf("Digite o ID do territorio de origem (1-%d): ", NUM_TERRITORIOS);
    scanf("%d", &origem);
    
    printf("Digite o ID do territorio de destino (1-%d): ", NUM_TERRITORIOS);
    scanf("%d", &destino);
    
    // Validacao basica dos IDs
    if (origem < 1 || origem > NUM_TERRITORIOS || destino < 1 || destino > NUM_TERRITORIOS) {
        printf("IDs de territorios invalidos!\n");
        limparBufferEntrada();
        return;
    }
    
    if (origem == destino) {
        printf("Nao e possivel atacar o proprio territorio!\n");
        limparBufferEntrada();
        return;
    }
    
    // Converte para indices do array (base 0)
    origem--;
    destino--;
    
    // Verificacao se o territorio de origem pertence ao jogador
    if (strcmp(mapa[origem].corExercito, jogador->cor) != 0) {
        printf("Voce nao pode atacar com um territorio que nao e seu!\n");
        limparBufferEntrada();
        return;
    }
    
    // Solicita numero de tropas para a batalha
    printf("Digite quantas tropas usar no ataque (1-%d): ", MAX_TROPAS_ATAQUE);
    scanf("%d", &tropasBatalha);
    limparBufferEntrada();
    
    if (tropasBatalha < 1 || tropasBatalha > MAX_TROPAS_ATAQUE) {
        printf("Numero de tropas invalido!\n");
        return;
    }
    
    printf("\nAtacando %s com %s usando %d tropas...\n", 
           mapa[destino].nome, mapa[origem].nome, tropasBatalha);
    simularAtaque(&mapa[origem], &mapa[destino], jogador, tropasBatalha);
}

// simularAtaque():
// Executa a logica de uma batalha entre dois territorios.
// Realiza validacoes, rola os dados, compara os resultados e atualiza o numero de tropas.
// Se um territorio for conquistado, atualiza seu dono e move uma tropa.
void simularAtaque(Territorio* territorioOrigem, Territorio* territorioDestino, Jogador* jogador, int tropasAtaque) {
    // Validacoes
    if (territorioOrigem->numTropas <= 1) {
        printf("Voce precisa de pelo menos 2 tropas para atacar!\n");
        return;
    }
    
    if (strcmp(territorioDestino->corExercito, jogador->cor) == 0) {
        printf("Voce nao pode atacar seu proprio territorio!\n");
        return;
    }
    
    // Simulacao da batalha considerando vida, poder e tropas
    int forcaAtacante = (rand() % 6 + 1) + (territorioOrigem->poder / 100) + tropasAtaque;
    int forcaDefensor = (rand() % 6 + 1) + (territorioDestino->poder / 100) + territorioDestino->numTropas;
    
    printf("\nResultados da batalha:\n");
    printf("Atacante (%s): Forca %d (Dados + Poder + Tropas)\n", territorioOrigem->nome, forcaAtacante);
    printf("Defensor (%s): Forca %d (Dados + Poder + Tropas)\n", territorioDestino->nome, forcaDefensor);
    
    if (forcaAtacante > forcaDefensor) {
        // Atacante vence
        int dano = tropasAtaque * 10; // Dano baseado nas tropas usadas
        territorioDestino->vida -= dano;
        territorioDestino->numTropas = (territorioDestino->numTropas > 1) ? territorioDestino->numTropas - 1 : 1;
        
        printf("Atacante venceu! %s perdeu %d de vida e 1 tropa.\n", territorioDestino->nome, dano);
        
        // Atualiza estatisticas do jogador
        jogador->batalhasVencidas++;
        atualizarPontuacao(jogador, 1, 10); // 10 pontos por vitoria
        
        if (territorioDestino->vida <= 0) {
            // Territorio conquistado
            strcpy(territorioDestino->corExercito, jogador->cor);
            territorioDestino->vida = territorioOrigem->vida / 2; // Metade da vida do atacante
            territorioDestino->poder = territorioOrigem->poder / 2; // Metade do poder do atacante
            territorioDestino->numTropas = tropasAtaque;
            territorioOrigem->numTropas -= tropasAtaque;
            
            // Atualiza estatisticas
            jogador->territoriosConquistados++;
            atualizarPontuacao(jogador, 2, 50); // 50 pontos por conquista
            
            printf("*** TERRITORIO CONQUISTADO! ***\n");
            printf("%s agora pertence ao exercito %s!\n", territorioDestino->nome, jogador->cor);
        }
    } else {
        // Defensor vence
        territorioOrigem->numTropas = (territorioOrigem->numTropas > tropasAtaque) ? 
                                      territorioOrigem->numTropas - tropasAtaque : 1;
        
        // Atualiza estatisticas do jogador
        jogador->batalhasPerdidas++;
        atualizarPontuacao(jogador, 3, -5); // -5 pontos por derrota
        
        printf("Defensor venceu! %s perdeu %d tropas.\n", territorioOrigem->nome, tropasAtaque);
    }
}

// sortearMissao():
// Sorteia e retorna um ID de missao aleatorio para o jogador.
int sortearMissao(void) {
    return rand() % NUM_MISSOES + 1;
}

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missao atual.
// Implementa a logica para cada tipo de missao (destruir um exercito ou conquistar um numero de territorios).
// Retorna 1 (verdadeiro) se a missao foi cumprida, e 0 (falso) caso contrario.
int verificarVitoria(const Territorio* mapa, int idMissao, const char* corJogador) {
    int territoriosJogador = 0;
    int temVermelho = 0, temVerde = 0;
    int americaSul = 0, africa = 0;
    
    // Contabiliza territorios e verifica presenca de cores
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        if (strcmp(mapa[i].corExercito, corJogador) == 0) {
            territoriosJogador++;
        }
        
        if (strcmp(mapa[i].corExercito, "Vermelho") == 0) {
            temVermelho = 1;
        }
        
        if (strcmp(mapa[i].corExercito, "Verde") == 0) {
            temVerde = 1;
        }
        
        // Verifica America do Sul (territorios 0-5) e Africa (territorios 18-23)
        if ((i >= 0 && i <= 5) && strcmp(mapa[i].corExercito, corJogador) == 0) {
            americaSul++;
        }
        
        if ((i >= 18 && i <= 23) && strcmp(mapa[i].corExercito, corJogador) == 0) {
            africa++;
        }
    }
    
    switch (idMissao) {
        case 1:
            return !temVermelho; // Verdadeiro se nao ha mais exercito vermelho
        case 2:
            return !temVerde; // Verdadeiro se nao ha mais exercito verde
        case 3:
            return territoriosJogador >= 18;
        case 4:
            return territoriosJogador >= 24;
        case 5:
            return (americaSul == 6 && africa == 6); // America do Sul e Africa completas
        default:
            return 0;
    }
}

// limparBufferEntrada():
// Funcao utilitaria para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// calcularEstatisticas():
// Calcula estatisticas para todos os exercitos no mapa.
void calcularEstatisticas(const Territorio* mapa, EstatisticaExercito estatisticas[]) {
    const char* cores[NUM_CORES] = {"Vermelho", "Verde", "Amarelo", "Preto", "Branco", "Rosa"};
    
    // Inicializa estatisticas
    for (int i = 0; i < NUM_CORES; i++) {
        strcpy(estatisticas[i].cor, cores[i]);
        estatisticas[i].territorios = 0;
        estatisticas[i].tropasTotal = 0;
        estatisticas[i].vidaTotal = 0;
        estatisticas[i].poderTotal = 0;
        estatisticas[i].pontos = 0;
    }
    
    // Calcula estatisticas para cada territorio
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        for (int j = 0; j < NUM_CORES; j++) {
            if (strcmp(mapa[i].corExercito, cores[j]) == 0) {
                estatisticas[j].territorios++;
                estatisticas[j].tropasTotal += mapa[i].numTropas;
                estatisticas[j].vidaTotal += mapa[i].vida;
                estatisticas[j].poderTotal += mapa[i].poder;
                // Pontos baseados em territorios, tropas e atributos
                estatisticas[j].pontos = estatisticas[j].territorios * 10 + 
                                       estatisticas[j].tropasTotal * 2 + 
                                       (estatisticas[j].vidaTotal + estatisticas[j].poderTotal) / 100;
                break;
            }
        }
    }
}

// exibirRanking():
// Exibe tabela de ranking ordenada por pontuacao.
void exibirRanking(const EstatisticaExercito estatisticas[], const Jogador* jogador) {
    printf("\n=== TABELA DE RANKING ===\n");
    printf("%-10s %-5s %-7s %-8s %-8s %-6s\n", "COR", "TERR", "TROPAS", "VIDA", "PODER", "PONTOS");
    printf("=======================================================\n");
    
    // Cria copia para ordenacao
    EstatisticaExercito ranking[NUM_CORES];
    for (int i = 0; i < NUM_CORES; i++) {
        ranking[i] = estatisticas[i];
    }
    
    // Ordenacao simples por pontos (bubble sort)
    for (int i = 0; i < NUM_CORES - 1; i++) {
        for (int j = 0; j < NUM_CORES - 1 - i; j++) {
            if (ranking[j].pontos < ranking[j + 1].pontos) {
                EstatisticaExercito temp = ranking[j];
                ranking[j] = ranking[j + 1];
                ranking[j + 1] = temp;
            }
        }
    }
    
    // Exibe ranking
    for (int i = 0; i < NUM_CORES; i++) {
        char indicador = ' ';
        if (strcmp(ranking[i].cor, jogador->cor) == 0) {
            indicador = '*'; // Marca o jogador
        }
        
        printf("%-10s %-5d %-7d %-8d %-8d %-6d %c\n", 
               ranking[i].cor, 
               ranking[i].territorios,
               ranking[i].tropasTotal,
               ranking[i].vidaTotal,
               ranking[i].poderTotal,
               ranking[i].pontos,
               indicador);
    }
    printf("=======================================================\n");
    printf("* = Sua posicao atual\n");
    
    // Exibe status do jogador
    printf("\n=== SUAS ESTATISTICAS ===\n");
    printf("Territorios conquistados: %d\n", jogador->territoriosConquistados);
    printf("Batalhas vencidas: %d\n", jogador->batalhasVencidas);
    printf("Batalhas perdidas: %d\n", jogador->batalhasPerdidas);
    printf("Taxa de vitoria: %.1f%%\n", 
           (jogador->batalhasVencidas + jogador->batalhasPerdidas > 0) ? 
           (float)jogador->batalhasVencidas / (jogador->batalhasVencidas + jogador->batalhasPerdidas) * 100 : 0);
    printf("Pontos totais: %d\n", jogador->pontos);
    printf("=========================\n");
}

// exibirStatusGeral():
// Exibe status geral do jogo incluindo mapa e estatisticas basicas.
void exibirStatusGeral(const Territorio* mapa, const Jogador* jogador) {
    exibirStatusJogador(jogador);
    exibirMapaComStatus(mapa, jogador);
    
    // Calcula e exibe estatisticas rapidas
    EstatisticaExercito estatisticas[NUM_CORES];
    calcularEstatisticas(mapa, estatisticas);
    
    printf("\n=== STATUS RAPIDO ===\n");
    int melhorPontuacao = 0;
    char lider[MAX_COR] = "";
    
    for (int i = 0; i < NUM_CORES; i++) {
        if (estatisticas[i].pontos > melhorPontuacao) {
            melhorPontuacao = estatisticas[i].pontos;
            strcpy(lider, estatisticas[i].cor);
        }
    }
    
    printf("Lider atual: %s (%d pontos)\n", lider, melhorPontuacao);
    printf("Sua posicao: %d pontos\n", jogador->pontos);
    printf("Diferenca: %d pontos\n", melhorPontuacao - jogador->pontos);
    printf("=====================\n");
}

// atualizarPontuacao():
// Atualiza pontuacao do jogador baseada em acoes.
void atualizarPontuacao(Jogador* jogador, int tipoAcao, int valor) {
    jogador->pontos += valor;
    if (jogador->pontos < 0) jogador->pontos = 0; // Nao permite pontos negativos
}

// exibirResultadoFinal():
// Exibe resultado final do jogo com estatisticas completas.
void exibirResultadoFinal(const Territorio* mapa, const Jogador* jogador, int vitoria) {
    printf("\n");
    printf("===============================================\n");
    printf("           RESULTADO FINAL DO JOGO\n");
    printf("===============================================\n");
    
    if (vitoria) {
        printf("*** PARABENS %s! VOCE VENCEU! ***\n", jogador->nome);
        printf("Missao cumprida com sucesso!\n");
    } else {
        printf("Jogo encerrado. Obrigado por jogar!\n");
    }
    
    printf("\n=== ESTATISTICAS FINAIS ===\n");
    printf("Jogador: %s\n", jogador->nome);
    printf("Cor: %s\n", jogador->cor);
    printf("Pais de origem: %s\n", jogador->paisOrigem);
    printf("Territorios conquistados: %d\n", jogador->territoriosConquistados);
    printf("Batalhas vencidas: %d\n", jogador->batalhasVencidas);
    printf("Batalhas perdidas: %d\n", jogador->batalhasPerdidas);
    printf("Total de batalhas: %d\n", jogador->batalhasVencidas + jogador->batalhasPerdidas);
    printf("Taxa de vitoria: %.1f%%\n", 
           (jogador->batalhasVencidas + jogador->batalhasPerdidas > 0) ? 
           (float)jogador->batalhasVencidas / (jogador->batalhasVencidas + jogador->batalhasPerdidas) * 100 : 0);
    printf("Pontuacao final: %d pontos\n", jogador->pontos);
    
    // Calcula ranking final
    EstatisticaExercito estatisticas[NUM_CORES];
    calcularEstatisticas(mapa, estatisticas);
    
    int posicao = 1;
    for (int i = 0; i < NUM_CORES; i++) {
        if (estatisticas[i].pontos > jogador->pontos && strcmp(estatisticas[i].cor, jogador->cor) != 0) {
            posicao++;
        }
    }
    
    printf("Posicao final no ranking: %do lugar\n", posicao);
    printf("============================\n");
    
    // Exibe ranking final completo
    printf("\n=== RANKING FINAL ===\n");
    exibirRanking(estatisticas, jogador);
    
    printf("\n===============================================\n");
    printf("        Obrigado por jogar WAR Estruturado!\n");
    printf("===============================================\n");
}

// exibirMapaComStatus():
// Exibe o mapa com indicacao de territorios aliados/inimigos.
void exibirMapaComStatus(const Territorio* mapa, const Jogador* jogador) {
    printf("\n=== MAPA DO MUNDO ===\n");
    printf("%-3s %-20s %-10s %-6s %-6s %-6s %s\n", "ID", "TERRITORIO", "COR", "TROPAS", "VIDA", "PODER", "STATUS");
    printf("=======================================================================\n");
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        char status[10] = "";
        
        // Determina se e aliado, inimigo ou neutro
        if (strcmp(mapa[i].corExercito, jogador->cor) == 0) {
            strcpy(status, "SEU");
        } else {
            strcpy(status, "INIMIGO");
        }
        
        printf("%-3d %-20s %-10s %-6d %-6d %-6d %s\n", 
               i + 1, 
               mapa[i].nome, 
               mapa[i].corExercito, 
               mapa[i].numTropas,
               mapa[i].vida,
               mapa[i].poder,
               status);
    }
    printf("=======================================================================\n");
    printf("Legenda: SEU = Seus territorios | INIMIGO = Territorios inimigos\n");
}

// analisarRelacoesDiplomaticas():
// Analisa e exibe informacoes detalhadas sobre inimigos e aliados.
void analisarRelacoesDiplomaticas(const Territorio* mapa, const Jogador* jogador) {
    printf("\n=== ANALISE DIPLOMATICA ===\n");
    
    // Conta territorios por cor
    int territoriosPorCor[NUM_CORES] = {0};
    int tropasPorCor[NUM_CORES] = {0};
    int vidaPorCor[NUM_CORES] = {0};
    int poderPorCor[NUM_CORES] = {0};
    
    const char* cores[NUM_CORES] = {"Vermelho", "Verde", "Amarelo", "Preto", "Branco", "Rosa"};
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        for (int j = 0; j < NUM_CORES; j++) {
            if (strcmp(mapa[i].corExercito, cores[j]) == 0) {
                territoriosPorCor[j]++;
                tropasPorCor[j] += mapa[i].numTropas;
                vidaPorCor[j] += mapa[i].vida;
                poderPorCor[j] += mapa[i].poder;
                break;
            }
        }
    }
    
    // Identifica a cor do jogador
    int indiceJogador = -1;
    for (int i = 0; i < NUM_CORES; i++) {
        if (strcmp(cores[i], jogador->cor) == 0) {
            indiceJogador = i;
            break;
        }
    }
    
    printf("\n=== SEUS TERRITORIOS ===\n");
    if (indiceJogador >= 0) {
        printf("Cor: %s\n", cores[indiceJogador]);
        printf("Territorios: %d\n", territoriosPorCor[indiceJogador]);
        printf("Tropas totais: %d\n", tropasPorCor[indiceJogador]);
        printf("Vida total: %d\n", vidaPorCor[indiceJogador]);
        printf("Poder total: %d\n", poderPorCor[indiceJogador]);
    }
    
    printf("\n=== FORCAS INIMIGAS ===\n");
    printf("%-10s %-5s %-7s %-8s %-8s %s\n", "COR", "TERR", "TROPAS", "VIDA", "PODER", "AMEACA");
    printf("====================================================\n");
    
    for (int i = 0; i < NUM_CORES; i++) {
        if (i != indiceJogador && territoriosPorCor[i] > 0) {
            // Calcula nivel de ameaca
            int forcaTotal = tropasPorCor[i] + (vidaPorCor[i] + poderPorCor[i]) / 100;
            char ameaca[10];
            
            if (forcaTotal > 100) strcpy(ameaca, "ALTA");
            else if (forcaTotal > 50) strcpy(ameaca, "MEDIA");
            else strcpy(ameaca, "BAIXA");
            
            printf("%-10s %-5d %-7d %-8d %-8d %s\n", 
                   cores[i], 
                   territoriosPorCor[i],
                   tropasPorCor[i],
                   vidaPorCor[i],
                   poderPorCor[i],
                   ameaca);
        }
    }
    
    printf("====================================================\n");
    
    // Recomendacoes estrategicas
    printf("\n=== RECOMENDACOES ESTRATEGICAS ===\n");
    
    // Encontra o inimigo mais fraco
    int menorForca = 999999;
    char inimigoMaisfraco[MAX_COR] = "";
    
    for (int i = 0; i < NUM_CORES; i++) {
        if (i != indiceJogador && territoriosPorCor[i] > 0) {
            int forca = tropasPorCor[i] + (vidaPorCor[i] + poderPorCor[i]) / 100;
            if (forca < menorForca) {
                menorForca = forca;
                strcpy(inimigoMaisfraco, cores[i]);
            }
        }
    }
    
    // Encontra o inimigo mais forte
    int maiorForca = 0;
    char inimigoMaisForte[MAX_COR] = "";
    
    for (int i = 0; i < NUM_CORES; i++) {
        if (i != indiceJogador && territoriosPorCor[i] > 0) {
            int forca = tropasPorCor[i] + (vidaPorCor[i] + poderPorCor[i]) / 100;
            if (forca > maiorForca) {
                maiorForca = forca;
                strcpy(inimigoMaisForte, cores[i]);
            }
        }
    }
    
    if (strlen(inimigoMaisfraco) > 0) {
        printf("• ALVO RECOMENDADO: %s (mais fraco)\n", inimigoMaisfraco);
    }
    
    if (strlen(inimigoMaisForte) > 0) {
        printf("• MAIOR AMEACA: %s (mais forte)\n", inimigoMaisForte);
    }
    
    printf("• Foque em territorios com poucas tropas\n");
    printf("• Evite atacar territorios com vida muito alta\n");
    printf("• Use mais tropas contra inimigos com muito poder\n");
    
    printf("==================================\n");
    
    // Lista de territorios atacaveis
    printf("\n=== TERRITORIOS ATACAVEIS ===\n");
    printf("%-3s %-20s %-10s %-6s %s\n", "ID", "TERRITORIO", "COR", "TROPAS", "DIFICULDADE");
    printf("=================================================\n");
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        if (strcmp(mapa[i].corExercito, jogador->cor) != 0) {
            char dificuldade[10];
            int resistencia = mapa[i].numTropas + mapa[i].vida / 100;
            
            if (resistencia > 15) strcpy(dificuldade, "DIFICIL");
            else if (resistencia > 8) strcpy(dificuldade, "MEDIO");
            else strcpy(dificuldade, "FACIL");
            
            printf("%-3d %-20s %-10s %-6d %s\n", 
                   i + 1,
                   mapa[i].nome, 
                   mapa[i].corExercito, 
                   mapa[i].numTropas,
                   dificuldade);
        }
    }
    
    printf("=================================================\n");
}