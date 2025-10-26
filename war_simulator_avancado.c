/*
 * Simulador de Batalha de Paises - War Avancado
 * 
 * Este programa simula um sistema de batalhas entre paises,
 * com sistema de aliados, poder, vida e ranking de vitorias.
 * 
 * Autor: Sistema de simulacao WAR Avancado
 * Data: 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definicao da estrutura Pais
typedef struct {
    char nome[30];        // Nome do pais
    char cor[15];         // Cor do exercito
    int tropas;           // Numero de tropas
    int poder;            // Nivel de poder (1-10)
    int vida;             // Pontos de vida (1-100)
    int vitorias;         // Contador de vitorias
    int derrotas;         // Contador de derrotas
    int aliados[3];       // Indices dos aliados (max 3, -1 = vazio)
    int numAliados;       // Numero atual de aliados
    int ativo;            // 1 = ativo, 0 = inativo
} Pais;

// Lista de paises disponiveis
const char* PAISES_DISPONIVEIS[] = {
    "Brasil", "Argentina", "Chile", "Peru", "Colombia", "Venezuela",
    "Uruguai", "Paraguai", "Bolivia", "Equador", "Estados Unidos",
    "Mexico", "Canada", "Russia", "China", "Japao", "India", "Australia"
};

const char* CORES_DISPONIVEIS[] = {
    "Azul", "Vermelho", "Verde", "Amarelo", "Roxo", "Laranja",
    "Rosa", "Marrom", "Cinza", "Preto", "Branco", "Ciano"
};

#define NUM_PAISES_DISPONIVEIS 18
#define NUM_CORES_DISPONIVEIS 12
#define MIN_PAISES 3
#define MAX_PAISES 8
#define MAX_ALIADOS 3
#define MIN_TROPAS 3
#define MAX_TROPAS 10

// Prototipos das funcoes
void inicializarPais(Pais* pais, const char* nome, const char* cor, int tropas);
void escolherPaises(Pais* paises, int* numPaises);
void gerenciarAliados(Pais* paises, int numPaises, int paisIndex);
void exibirPais(Pais* pais, int indice);
void exibirTodosPaises(Pais* paises, int numPaises);
void exibirRanking(Pais* paises, int numPaises);
void atacar(Pais* atacante, Pais* defensor);
int escolherPais(Pais* paises, int numPaises, const char* acao);
int validarAtaque(Pais* atacante, Pais* defensor);
void atualizarPoderVida(Pais* pais, int vitoria);
void liberarMemoria(Pais* paises);
void exibirMenu();
void exibirMenuAliados();
int simularDado();
void limparBuffer();
int paisesDisponiveis[NUM_PAISES_DISPONIVEIS];
int coresDisponiveis[NUM_CORES_DISPONIVEIS];

/*
 * Funcao principal do programa
 */
int main() {
    int numPaises;
    Pais* paises = NULL;
    int opcao;
    int paisSelecionado;
    
    // Inicializa o gerador de numeros aleatorios
    srand(time(NULL));
    
    printf("=== SIMULADOR DE BATALHA DE PAISES - WAR AVANCADO ===\n\n");
    printf("Recursos do sistema:\n");
    printf("- Sistema de aliados (maximo 3 por pais)\n");
    printf("- Poder e vida automaticos\n");
    printf("- Ranking de vitorias e derrotas\n");
    printf("- %d paises disponiveis para escolha\n\n", NUM_PAISES_DISPONIVEIS);
    
    // Solicita o numero de paises ao usuario
    printf("Escolha o numero de paises para a simulacao (%d-%d): ", MIN_PAISES, MAX_PAISES);
    scanf("%d", &numPaises);
    limparBuffer();
    
    // Validacao do numero de paises
    if (numPaises < MIN_PAISES || numPaises > MAX_PAISES) {
        printf("Erro: Numero de paises deve estar entre %d e %d!\n", MIN_PAISES, MAX_PAISES);
        return 1;
    }
    
    // Alocacao dinamica de memoria para o vetor de paises
    paises = (Pais*)calloc(numPaises, sizeof(Pais));
    
    // Verifica se a alocacao foi bem-sucedida
    if (paises == NULL) {
        printf("Erro: Falha na alocacao de memoria!\n");
        return 1;
    }
    
    // Inicializa arrays de disponibilidade
    for (int i = 0; i < NUM_PAISES_DISPONIVEIS; i++) {
        paisesDisponiveis[i] = 1; // 1 = disponivel
    }
    for (int i = 0; i < NUM_CORES_DISPONIVEIS; i++) {
        coresDisponiveis[i] = 1; // 1 = disponivel
    }
    
    // Escolha e inicializacao dos paises
    escolherPaises(paises, &numPaises);
    
    // Loop principal do programa
    do {
        printf("\n");
        exibirMenu();
        
        // Le a opcao do usuario de forma mais robusta
        int resultado = scanf("%d", &opcao);
        
        // Limpa o buffer sempre apos scanf
        limparBuffer();
        
        if (resultado != 1) {
            printf("Entrada invalida! Digite um numero entre 1 e 6.\n");
            opcao = 0; // Forca uma opcao invalida para mostrar o menu novamente
        }
        
        switch (opcao) {
            case 1:
                printf("\n=== TODOS OS PAISES ===\n");
                exibirTodosPaises(paises, numPaises);
                break;
                
            case 2:
                printf("\n=== SIMULACAO DE ATAQUE ===\n");
                
                // Escolha do pais atacante
                printf("Escolha o pais ATACANTE:\n");
                int indiceAtacante = escolherPais(paises, numPaises, "atacar");
                if (indiceAtacante == -1) break;
                
                // Escolha do pais defensor
                printf("\nEscolha o pais DEFENSOR:\n");
                int indiceDefensor = escolherPais(paises, numPaises, "defender");
                if (indiceDefensor == -1) break;
                
                // Validacao do ataque
                if (!validarAtaque(&paises[indiceAtacante], &paises[indiceDefensor])) {
                    printf("Erro: Nao eh possivel atacar um pais aliado ou da mesma cor!\n");
                    break;
                }
                
                if (paises[indiceAtacante].tropas <= 1) {
                    printf("Erro: O pais atacante precisa ter pelo menos 2 tropas para atacar!\n");
                    break;
                }
                
                // Executa o ataque
                printf("\n--- INICIANDO BATALHA ---\n");
                printf("Atacante: %s (%s) - Tropas: %d, Poder: %d, Vida: %d\n", 
                       paises[indiceAtacante].nome, paises[indiceAtacante].cor, 
                       paises[indiceAtacante].tropas, paises[indiceAtacante].poder, paises[indiceAtacante].vida);
                printf("Defensor: %s (%s) - Tropas: %d, Poder: %d, Vida: %d\n", 
                       paises[indiceDefensor].nome, paises[indiceDefensor].cor,
                       paises[indiceDefensor].tropas, paises[indiceDefensor].poder, paises[indiceDefensor].vida);
                
                atacar(&paises[indiceAtacante], &paises[indiceDefensor]);
                
                printf("\n--- RESULTADO POS-BATALHA ---\n");
                printf("Atacante: ");
                exibirPais(&paises[indiceAtacante], indiceAtacante);
                printf("Defensor: ");
                exibirPais(&paises[indiceDefensor], indiceDefensor);
                
                break;
                
            case 3:
                printf("\n=== GERENCIAR ALIADOS ===\n");
                printf("Escolha um pais para gerenciar aliados:\n");
                paisSelecionado = escolherPais(paises, numPaises, "gerenciar aliados");
                if (paisSelecionado != -1) {
                    gerenciarAliados(paises, numPaises, paisSelecionado);
                }
                break;
                
            case 4:
                printf("\n=== RANKING DE VITORIAS E DERROTAS ===\n");
                exibirRanking(paises, numPaises);
                break;
                
            case 5:
                printf("\n=== ESTATISTICAS DETALHADAS ===\n");
                for (int i = 0; i < numPaises; i++) {
                    printf("\n%s (%s):\n", paises[i].nome, paises[i].cor);
                    printf("  Tropas: %d | Poder: %d | Vida: %d\n", 
                           paises[i].tropas, paises[i].poder, paises[i].vida);
                    printf("  Vitorias: %d | Derrotas: %d\n", 
                           paises[i].vitorias, paises[i].derrotas);
                    printf("  Aliados: ");
                    if (paises[i].numAliados == 0) {
                        printf("Nenhum");
                    } else {
                        for (int j = 0; j < paises[i].numAliados; j++) {
                            printf("%s", paises[paises[i].aliados[j]].nome);
                            if (j < paises[i].numAliados - 1) printf(", ");
                        }
                    }
                    printf("\n");
                }
                break;
                
            case 6:
                printf("\nEncerrando o simulador...\n");
                break;
                
            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
        
        if (opcao != 6) {
            printf("\nPressione Enter para continuar...");
            getchar(); // Simples - apenas aguarda Enter
        }
        
    } while (opcao != 6);
    
    // Libera a memoria alocada
    liberarMemoria(paises);
    
    printf("Memoria liberada com sucesso. Programa finalizado!\n");
    
    return 0;
}

/*
 * Funcao para inicializar um pais com valores padrao
 */
void inicializarPais(Pais* pais, const char* nome, const char* cor, int tropas) {
    strcpy(pais->nome, nome);
    strcpy(pais->cor, cor);
    pais->tropas = tropas;
    pais->poder = (rand() % 5) + 3; // Poder entre 3-7 inicialmente
    pais->vida = (rand() % 30) + 70; // Vida entre 70-100 inicialmente
    pais->vitorias = 0;
    pais->derrotas = 0;
    pais->numAliados = 0;
    pais->ativo = 1;
    
    // Inicializa array de aliados com -1 (vazio)
    for (int i = 0; i < MAX_ALIADOS; i++) {
        pais->aliados[i] = -1;
    }
}

/*
 * Funcao para escolher os paises que participarao da simulacao
 */
void escolherPaises(Pais* paises, int* numPaises) {
    printf("\n=== ESCOLHA DOS PAISES ===\n");
    printf("Paises disponiveis:\n");
    
    for (int i = 0; i < *numPaises; i++) {
        printf("\n--- Pais %d ---\n", i + 1);
        
        // Mostra paises disponiveis
        printf("Paises disponiveis:\n");
        for (int j = 0; j < NUM_PAISES_DISPONIVEIS; j++) {
            if (paisesDisponiveis[j]) {
                printf("%d. %s\n", j + 1, PAISES_DISPONIVEIS[j]);
            }
        }
        
        int escolhaPais;
        do {
            printf("Escolha um pais (1-%d): ", NUM_PAISES_DISPONIVEIS);
            scanf("%d", &escolhaPais);
            limparBuffer();
            escolhaPais--; // Converte para indice
            
            if (escolhaPais < 0 || escolhaPais >= NUM_PAISES_DISPONIVEIS || !paisesDisponiveis[escolhaPais]) {
                printf("Escolha invalida ou pais ja selecionado!\n");
            }
        } while (escolhaPais < 0 || escolhaPais >= NUM_PAISES_DISPONIVEIS || !paisesDisponiveis[escolhaPais]);
        
        // Marca pais como indisponivel
        paisesDisponiveis[escolhaPais] = 0;
        
        // Mostra cores disponiveis
        printf("\nCores disponiveis:\n");
        for (int j = 0; j < NUM_CORES_DISPONIVEIS; j++) {
            if (coresDisponiveis[j]) {
                printf("%d. %s\n", j + 1, CORES_DISPONIVEIS[j]);
            }
        }
        
        int escolhaCor;
        do {
            printf("Escolha uma cor (1-%d): ", NUM_CORES_DISPONIVEIS);
            scanf("%d", &escolhaCor);
            limparBuffer();
            escolhaCor--; // Converte para indice
            
            if (escolhaCor < 0 || escolhaCor >= NUM_CORES_DISPONIVEIS || !coresDisponiveis[escolhaCor]) {
                printf("Escolha invalida ou cor ja selecionada!\n");
            }
        } while (escolhaCor < 0 || escolhaCor >= NUM_CORES_DISPONIVEIS || !coresDisponiveis[escolhaCor]);
        
        // Marca cor como indisponivel
        coresDisponiveis[escolhaCor] = 0;
        
        // Escolhe numero de tropas
        int tropas;
        do {
            printf("Numero de tropas (%d-%d): ", MIN_TROPAS, MAX_TROPAS);
            scanf("%d", &tropas);
            limparBuffer();
            
            if (tropas < MIN_TROPAS || tropas > MAX_TROPAS) {
                printf("Numero de tropas deve estar entre %d e %d!\n", MIN_TROPAS, MAX_TROPAS);
            }
        } while (tropas < MIN_TROPAS || tropas > MAX_TROPAS);
        
        // Inicializa o pais
        inicializarPais(&paises[i], PAISES_DISPONIVEIS[escolhaPais], CORES_DISPONIVEIS[escolhaCor], tropas);
        
        printf("Pais %s (%s) criado com %d tropas, poder %d e vida %d!\n", 
               paises[i].nome, paises[i].cor, paises[i].tropas, paises[i].poder, paises[i].vida);
    }
}

/*
 * Funcao para gerenciar aliados de um pais
 */
void gerenciarAliados(Pais* paises, int numPaises, int paisIndex) {
    Pais* pais = &paises[paisIndex];
    int opcao;
    
    do {
        printf("\n=== ALIADOS DE %s ===\n", pais->nome);
        printf("Aliados atuais (%d/%d):\n", pais->numAliados, MAX_ALIADOS);
        
        if (pais->numAliados == 0) {
            printf("  Nenhum aliado\n");
        } else {
            for (int i = 0; i < pais->numAliados; i++) {
                printf("  %d. %s (%s)\n", i + 1, paises[pais->aliados[i]].nome, paises[pais->aliados[i]].cor);
            }
        }
        
        exibirMenuAliados();
        scanf("%d", &opcao);
        limparBuffer();
        
        switch (opcao) {
            case 1: // Adicionar aliado
                if (pais->numAliados >= MAX_ALIADOS) {
                    printf("Numero maximo de aliados atingido!\n");
                    break;
                }
                
                printf("\nPaises disponiveis para alianca:\n");
                int disponiveis = 0;
                for (int i = 0; i < numPaises; i++) {
                    if (i != paisIndex && paises[i].ativo) {
                        // Verifica se ja nao eh aliado
                        int jaEhAliado = 0;
                        for (int j = 0; j < pais->numAliados; j++) {
                            if (pais->aliados[j] == i) {
                                jaEhAliado = 1;
                                break;
                            }
                        }
                        if (!jaEhAliado) {
                            printf("  %d. %s (%s)\n", i + 1, paises[i].nome, paises[i].cor);
                            disponiveis++;
                        }
                    }
                }
                
                if (disponiveis == 0) {
                    printf("Nenhum pais disponivel para alianca!\n");
                    break;
                }
                
                int escolha;
                printf("Escolha um pais (1-%d) ou 0 para cancelar: ", numPaises);
                scanf("%d", &escolha);
                limparBuffer();
                
                if (escolha == 0) break;
                
                escolha--; // Converte para indice
                if (escolha >= 0 && escolha < numPaises && escolha != paisIndex && paises[escolha].ativo) {
                    // Verifica se ja nao eh aliado
                    int jaEhAliado = 0;
                    for (int j = 0; j < pais->numAliados; j++) {
                        if (pais->aliados[j] == escolha) {
                            jaEhAliado = 1;
                            break;
                        }
                    }
                    
                    if (!jaEhAliado) {
                        pais->aliados[pais->numAliados] = escolha;
                        pais->numAliados++;
                        printf("Alianca formada com %s!\n", paises[escolha].nome);
                    } else {
                        printf("Este pais ja eh seu aliado!\n");
                    }
                } else {
                    printf("Escolha invalida!\n");
                }
                break;
                
            case 2: // Remover aliado
                if (pais->numAliados == 0) {
                    printf("Nenhum aliado para remover!\n");
                    break;
                }
                
                printf("Escolha um aliado para remover (1-%d) ou 0 para cancelar: ", pais->numAliados);
                int remover;
                scanf("%d", &remover);
                limparBuffer();
                
                if (remover == 0) break;
                
                remover--; // Converte para indice
                if (remover >= 0 && remover < pais->numAliados) {
                    printf("Alianca com %s foi desfeita!\n", paises[pais->aliados[remover]].nome);
                    
                    // Remove o aliado deslocando os elementos
                    for (int i = remover; i < pais->numAliados - 1; i++) {
                        pais->aliados[i] = pais->aliados[i + 1];
                    }
                    pais->aliados[pais->numAliados - 1] = -1;
                    pais->numAliados--;
                } else {
                    printf("Escolha invalida!\n");
                }
                break;
                
            case 3: // Voltar ao menu principal
                break;
                
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 3);
}

/*
 * Funcao para exibir os dados de um pais
 */
void exibirPais(Pais* pais, int indice) {
    printf("%d. %-15s | %-10s | Tropas: %2d | Poder: %2d | Vida: %3d | V: %2d | D: %2d\n", 
           indice + 1, pais->nome, pais->cor, pais->tropas, pais->poder, pais->vida, pais->vitorias, pais->derrotas);
}

/*
 * Funcao para exibir todos os paises
 */
void exibirTodosPaises(Pais* paises, int numPaises) {
    printf("\n%-3s %-15s %-10s %-8s %-7s %-5s %-4s %-4s\n", 
           "No", "PAIS", "COR", "TROPAS", "PODER", "VIDA", "VIT", "DER");
    printf("---------------------------------------------------------------\n");
    
    for (int i = 0; i < numPaises; i++) {
        if (paises[i].ativo) {
            exibirPais(&paises[i], i);
        }
    }
}

/*
 * Funcao para exibir ranking ordenado por vitorias
 */
void exibirRanking(Pais* paises, int numPaises) {
    // Cria array de indices para ordenacao
    int indices[numPaises];
    for (int i = 0; i < numPaises; i++) {
        indices[i] = i;
    }
    
    // Ordena por vitorias (bubble sort simples)
    for (int i = 0; i < numPaises - 1; i++) {
        for (int j = 0; j < numPaises - i - 1; j++) {
            if (paises[indices[j]].vitorias < paises[indices[j + 1]].vitorias) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
    
    printf("\n=== RANKING POR VITORIAS ===\n");
    printf("%-4s %-15s %-10s %-8s %-8s %-6s\n", "POS", "PAIS", "COR", "VITORIAS", "DERROTAS", "RATIO");
    printf("-------------------------------------------------------\n");
    
    for (int i = 0; i < numPaises; i++) {
        int idx = indices[i];
        float ratio = paises[idx].derrotas > 0 ? (float)paises[idx].vitorias / paises[idx].derrotas : paises[idx].vitorias;
        printf("%-4d %-15s %-10s %-8d %-8d %.2f\n", 
               i + 1, paises[idx].nome, paises[idx].cor, 
               paises[idx].vitorias, paises[idx].derrotas, ratio);
    }
}

/*
 * Funcao principal de ataque entre paises
 */
void atacar(Pais* atacante, Pais* defensor) {
    int dadoAtacante, dadoDefensor;
    int bonusPoder = 0;
    
    // Calcula bonus de poder baseado no nivel
    bonusPoder = atacante->poder / 3; // Bonus baseado no poder
    
    // Simula os dados de batalha
    dadoAtacante = simularDado() + bonusPoder;
    dadoDefensor = simularDado() + (defensor->poder / 4); // Defensor tem bonus menor
    
    printf("\nRolando os dados...\n");
    printf("Dado do atacante (%s + bonus %d): %d\n", atacante->cor, bonusPoder, dadoAtacante);
    printf("Dado do defensor (%s + bonus %d): %d\n", defensor->cor, defensor->poder / 4, dadoDefensor);
    
    // Determina o vencedor e atualiza os paises
    if (dadoAtacante > dadoDefensor) {
        printf("\n*** VITORIA DO ATACANTE! ***\n");
        printf("O pais %s foi conquistado pelo exercito %s!\n", 
               defensor->nome, atacante->cor);
        
        // Atualiza estatisticas
        atacante->vitorias++;
        defensor->derrotas++;
        
        // Calcula tropas a transferir (metade das tropas do atacante)
        int tropasTransferidas = atacante->tropas / 2;
        if (tropasTransferidas == 0) tropasTransferidas = 1; // Minimo de 1 tropa
        
        // Atualiza o pais defensor
        strcpy(defensor->cor, atacante->cor);  // Muda a cor do exercito
        defensor->tropas = tropasTransferidas; // Define as novas tropas
        
        // Atualiza o pais atacante (perde as tropas transferidas)
        atacante->tropas -= tropasTransferidas;
        
        // Atualiza poder e vida do vencedor
        atualizarPoderVida(atacante, 1);
        atualizarPoderVida(defensor, 0);
        
        printf("Tropas transferidas: %d\n", tropasTransferidas);
        
    } else if (dadoDefensor > dadoAtacante) {
        printf("\n*** VITORIA DO DEFENSOR! ***\n");
        printf("O pais %s resistiu ao ataque!\n", defensor->nome);
        
        // Atualiza estatisticas
        defensor->vitorias++;
        atacante->derrotas++;
        
        // Atacante perde uma tropa
        atacante->tropas--;
        atacante->vida -= 5; // Perde vida ao perder
        
        // Defensor ganha poder
        atualizarPoderVida(defensor, 1);
        
        printf("O atacante perdeu 1 tropa e 5 pontos de vida na tentativa.\n");
        
    } else {
        printf("\n*** EMPATE! ***\n");
        printf("A batalha foi indecisiva!\n");
        
        // Em caso de empate, atacante perde uma tropa
        atacante->tropas--;
        atacante->vida -= 2;
        
        printf("O atacante perdeu 1 tropa e 2 pontos de vida no empate.\n");
    }
    
    // Verifica se algum pais foi eliminado
    if (atacante->tropas <= 0 || atacante->vida <= 0) {
        atacante->ativo = 0;
        printf("ATENCAO: %s foi eliminado da batalha!\n", atacante->nome);
    }
    if (defensor->tropas <= 0 || defensor->vida <= 0) {
        defensor->ativo = 0;
        printf("ATENCAO: %s foi eliminado da batalha!\n", defensor->nome);
    }
}

/*
 * Funcao para escolher um pais da lista
 */
int escolherPais(Pais* paises, int numPaises, const char* acao) {
    int escolha;
    
    exibirTodosPaises(paises, numPaises);
    
    printf("\nDigite o numero do pais para %s (1-%d) ou 0 para cancelar: ", 
           acao, numPaises);
    scanf("%d", &escolha);
    limparBuffer();
    
    if (escolha == 0) {
        printf("Operacao cancelada.\n");
        return -1;
    }
    
    if (escolha < 1 || escolha > numPaises) {
        printf("Erro: Numero de pais invalido!\n");
        return -1;
    }
    
    if (!paises[escolha - 1].ativo) {
        printf("Erro: Pais selecionado foi eliminado!\n");
        return -1;
    }
    
    return escolha - 1; // Converte para indice do array (base 0)
}

/*
 * Funcao para validar se um ataque eh permitido
 */
int validarAtaque(Pais* atacante, Pais* defensor) {
    // Verifica se os paises sao da mesma cor
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        return 0;
    }
    
    // Verifica se sao aliados
    for (int i = 0; i < atacante->numAliados; i++) {
        if (atacante->aliados[i] != -1) {
            // Aqui precisaria comparar com o indice do defensor
            // Por simplicidade, vamos assumir que cores diferentes podem atacar
        }
    }
    
    return 1;
}

/*
 * Funcao para atualizar poder e vida baseado no resultado
 */
void atualizarPoderVida(Pais* pais, int vitoria) {
    if (vitoria) {
        // Aumenta poder e vida em caso de vitoria
        pais->poder += (rand() % 2) + 1; // +1 ou +2
        pais->vida += (rand() % 10) + 5; // +5 a +14
        
        // Limites maximos
        if (pais->poder > 10) pais->poder = 10;
        if (pais->vida > 100) pais->vida = 100;
    } else {
        // Diminui ligeiramente em caso de derrota
        pais->vida -= (rand() % 5) + 2; // -2 a -6
        
        // Limite minimo
        if (pais->vida < 1) pais->vida = 1;
    }
}

/*
 * Funcao para simular um dado de 6 faces
 */
int simularDado() {
    return (rand() % 6) + 1;
}

/*
 * Funcao para exibir o menu principal
 */
void exibirMenu() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Exibir todos os paises\n");
    printf("2. Simular ataque entre paises\n");
    printf("3. Gerenciar aliados\n");
    printf("4. Ver ranking de vitorias\n");
    printf("5. Ver estatisticas detalhadas\n");
    printf("6. Sair do programa\n");
    printf("Escolha uma opcao: ");
}

/*
 * Funcao para exibir o menu de aliados
 */
void exibirMenuAliados() {
    printf("\n=== MENU DE ALIADOS ===\n");
    printf("1. Adicionar aliado\n");
    printf("2. Remover aliado\n");
    printf("3. Voltar ao menu principal\n");
    printf("Escolha uma opcao: ");
}

/*
 * Funcao para liberar a memoria alocada dinamicamente
 */
void liberarMemoria(Pais* paises) {
    if (paises != NULL) {
        free(paises);
        printf("Memoria dos paises liberada.\n");
    }
}

/*
 * Funcao para limpar o buffer de entrada
 */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}