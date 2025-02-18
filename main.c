#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define NUM_CARACTERES_LINHA 1000
#define NUM_CARACTERES_VERTICE NUM_CARACTERES_LINHA

#define BRANCO 0
#define CINZA 1
#define PRETO 2

typedef struct
{
	int f;
	int indice;
} Pair;

typedef struct __adj__
{
	struct __adj__ * prox;
	char * nomeVerticeAdjacente;
	int vertice;
} Adj;

typedef struct
{
	Adj * primeiro;
} Lista;

typedef struct
{
	char ** nomeVertice;
	Lista ** adj;
	int V;
	int A;
	int * cor;
	int * d;
	int * f;
	int * pai;
} Grafo;

Lista * inicializaLista()
{
	Lista * adj = (Lista *) malloc(sizeof(Lista));
	adj->primeiro = NULL;

	return adj;
}

int encontraVertice(Grafo * grafo, char nomeVerticeAdjacente[NUM_CARACTERES_VERTICE])
{
	for (int i = 0 ; i < grafo->V; i++)
  {
		if (!strcmp(grafo->nomeVertice[i], nomeVerticeAdjacente))
    {
			return i;
		}
	}
	return -1;
}

void insereLista(Lista * adj, char nomeVerticeAdjacente[NUM_CARACTERES_VERTICE])
{
	Adj * novo = (Adj *) malloc(sizeof(Adj));
	novo->nomeVerticeAdjacente = (char *) malloc(sizeof(char)* NUM_CARACTERES_VERTICE);
	strcpy(novo->nomeVerticeAdjacente, nomeVerticeAdjacente);
	novo->prox = adj->primeiro;
	adj->primeiro = novo;
}

void lerLinhaVertice (char linha[NUM_CARACTERES_LINHA], Grafo * grafo, int vertice)
{
	int offset, numCaracteresLidos;

	grafo->nomeVertice[vertice] = (char *) malloc(sizeof(char) * NUM_CARACTERES_VERTICE);
	grafo->adj[vertice] = inicializaLista();

	sscanf(linha, "%[^:]:%n", grafo->nomeVertice[vertice], &numCaracteresLidos);

	offset = numCaracteresLidos;
		
	char nomeVerticeAdjacente[NUM_CARACTERES_VERTICE];

	while(sscanf(linha + offset, " %[^;];%n", nomeVerticeAdjacente, &numCaracteresLidos) == 1)
  {
		insereLista(grafo->adj[vertice], nomeVerticeAdjacente);
		grafo->A++;
		offset += numCaracteresLidos;
	}
}

Grafo * inicializaGrafo(int n)
{
	Grafo * grafo = (Grafo *) malloc(sizeof(Grafo));

	grafo->nomeVertice = (char **) malloc(sizeof(char *) * n);
	grafo->adj = (Lista **) malloc(sizeof(Lista *) * n);
	grafo->A = 0;
	grafo->V = n;
	grafo->d = (int *) malloc(sizeof(int)* n);
	grafo->f = (int *) malloc(sizeof(int)* n);
	grafo->pai = (int *) malloc(sizeof(int)* n);
	grafo->cor = (int *) malloc(sizeof(int)* n);

	return grafo;
}

void ajustaVerticesAdjacentes(Grafo * grafo)
{
	for (int i = 0; i < grafo->V; i++)
  {
		Adj * aux = grafo->adj[i]->primeiro;

		while (aux)
    {
			aux->vertice = encontraVertice(grafo, aux->nomeVerticeAdjacente);
			aux = aux->prox;
		}

	}
}

int tempo;

void DFSvisit (Grafo * grafo, Lista ** adj, int u, int pai)
{
	grafo->cor[u] = CINZA;
	tempo++;
	grafo->d[u] = tempo;

	Adj * aux = adj[u]->primeiro;
	while (aux)
  {
		if(grafo->cor[aux->vertice] == BRANCO)
    {
			grafo->pai[aux->vertice] = pai;
			DFSvisit(grafo, adj, aux->vertice, pai);
		}
		aux = aux->prox;
	}
	grafo->cor[u] = PRETO;
	tempo++;
	grafo->f[u] = tempo;
}

void DFS (Grafo * grafo, Lista ** adj)
{
	for (int u = 0; u < grafo->V; u++)
  {
		grafo->cor[u] = BRANCO;
		grafo->pai[u] = u;
	}
	tempo = 0;
	for (int u = 0; u < grafo->V; u++)
  {
		if (grafo->cor[u] == BRANCO)
			DFSvisit(grafo, adj, u, u);
	}
}

int compare(const void *l, const void *r)
{
	return (((Pair*)r)->f - ((Pair *)l)->f);
}

void ordenacaoTopologica (Grafo * grafo)
{
	Pair * pair = (Pair *) malloc(sizeof(Pair) * grafo->V);

	for (int i = 0; i < grafo->V; i++)
  {
		pair[i].f = grafo->f[i];
		pair[i].indice = i;
	}
	qsort(pair, grafo->V, sizeof(Pair), compare);

	Grafo * novo = inicializaGrafo(grafo->V);

	for (int i = 0; i < grafo->V; i++)
  {
		novo->adj[i] = grafo->adj[pair[i].indice];
		novo->cor[i] = grafo->cor[pair[i].indice];
		novo->d[i] = grafo->d[pair[i].indice];
		novo->f[i] = grafo->f[pair[i].indice];
		novo->pai[i] = grafo->pai[pair[i].indice];
		novo->nomeVertice[i] = (char *) malloc(sizeof(char) * NUM_CARACTERES_VERTICE);
		strcpy(novo->nomeVertice[i], grafo->nomeVertice[pair[i].indice]);
	}

	novo->A = grafo->A;
	novo->V = grafo->V;

	ajustaVerticesAdjacentes(novo);

	*grafo = *novo;
}

void insereListaTransposta(Lista * listaArestas, int indiceAdjacente, char nomeVerticeAdjacente[NUM_CARACTERES_VERTICE])
{
	insereLista(listaArestas, nomeVerticeAdjacente);
	
	listaArestas->primeiro->vertice = indiceAdjacente;
}

Lista ** tranposicaoArestas(Grafo * grafo)
{
	Lista ** arestasTranspostas = (Lista **) malloc(sizeof(Lista *) * grafo->V);

	for (int i = 0; i < grafo->V; i++)
  {
		arestasTranspostas[i] = (Lista *) malloc(sizeof(Lista));
	}

	for (int i = 0; i < grafo->V; i++)
  {
		Adj * aux = grafo->adj[i]->primeiro;
		while (aux)
    {
			insereListaTransposta(arestasTranspostas[aux->vertice], i, grafo->nomeVertice[i]);
			aux = aux->prox;
		}
	}

	return arestasTranspostas;
}

int contaArvores(Grafo * grafo)
{
	int soma = 0;
	for (int i = 0; i < grafo->V; i++)
  {
		if(grafo->pai[i] == i) soma++;
	}

	return soma;
}

Grafo * kosaraju (Grafo * grafo)
{
	Grafo * sccs = inicializaGrafo(contaArvores(grafo));

	for (int i = 0; i < sccs->V; i++)
  {
		sccs->nomeVertice[i] = (char *) malloc(sizeof(char) * NUM_CARACTERES_VERTICE);
		sccs->adj[i] = inicializaLista();
	}

	int * localVertices = (int *) malloc(sizeof(int) * grafo->V);

	for (int i = 0; i < grafo->V; i++)
  {
		localVertices[i] = -1;
	}

	int livre = 0;

	for (int i = 0; i < grafo->V; i++)
  {
		if (localVertices[grafo->pai[i]] == -1)
    {
			localVertices[grafo->pai[i]] = livre++;
		}
		strcat(sccs->nomeVertice[localVertices[grafo->pai[i]]], grafo->nomeVertice[i]);
	}


	for (int i = 0; i <grafo->V; i++)
  {
		Adj * aux = grafo->adj[i]->primeiro;
			while (aux){
				int j = aux->vertice;
				if (grafo->pai[i] != grafo->pai[j])
        {
					for (Adj * auxiliar = sccs->adj[localVertices[grafo->pai[i]]]->primeiro; auxiliar; auxiliar = auxiliar->prox)
          {
						if (auxiliar->vertice == localVertices[grafo->pai[j]]) //condição de dois iguais
							goto oi;
					}
					insereListaTransposta(sccs->adj[localVertices[grafo->pai[i]]], localVertices[grafo->pai[j]], sccs->nomeVertice[localVertices[grafo->pai[j]]]);		
				}
			oi:
			aux = aux->prox;
		}
	}

	DFS(sccs, sccs->adj);
	ordenacaoTopologica(sccs);


	return sccs;

}
Grafo * SCCs1(Grafo * grafo)
{
	DFS(grafo, tranposicaoArestas(grafo));
	ordenacaoTopologica(grafo);
	DFS(grafo, grafo->adj);

	return kosaraju (grafo);
}

Grafo * SCCs2 (Grafo * grafo)
{
	DFS(grafo, grafo->adj);
	ordenacaoTopologica(grafo);
	DFS(grafo, tranposicaoArestas(grafo));
	
	return kosaraju(grafo);

}



void imprimeSaida (Grafo * sccs)
{
	printf (sccs->V == 1 ? "\nSim\n" : "\nNão\n");
	printf ("%d\n", sccs->V);
	for (int i = 0; i < sccs->V; i++)
  {
		printf("%s ", sccs->nomeVertice[i]);
	}
	puts("");
	for (int i = 0; i <sccs->V; i++)
  {
		printf ("%s: ", sccs->nomeVertice[i]);
		for (Adj * aux = sccs->adj[i]->primeiro; aux; aux = aux->prox)
    {
			printf ("%s; ", aux->nomeVerticeAdjacente);
		}
	printf ("\n");
	}
}

Grafo * grafoaleatorio (int V, int A)
{
  double prob = (double) A / (V * (V - 1));
  
  Grafo *G = inicializaGrafo(V);

  for(int i = 0; i < V; i++)
    {
      printf ("\n nomes\n");
     /* sprintf(G->nomeVertice[i], "%d", i);*/
      G->nomeVertice[i] = "1";
    }
  
  srand(time(NULL));
  
  for(int a = 0; a < V; a++)
    {
      for(int b = 0; b < V; b++)
        {
          if(a != b)
            {
              if(rand() < prob*(RAND_MAX+1.0))
                {
                  printf ("\n VerticeOrigem %i\n", a);
                  printf ("\n VerticeDestino %i\n", b);
                  /*insereListaTransposta(G->adj[a], a, G->nomeVertice[b]);*/
                  /*insereLista(G->adj[a], G->nomeVertice[b]);*/
                  /*ajustaVerticesAdjacentes(G);*/
                }
            }
        }
    }
  return G;
}


int main(int argc, char **argv)
{
  if(argc > 1)
  {
    int numVertices;

	scanf("%d", &numVertices);

	Grafo * grafo = inicializaGrafo(numVertices);

	for (int i = 0; i < numVertices; i++)
  {
		char linha[NUM_CARACTERES_LINHA];
		
		scanf("\n%[^\n]", linha);

		lerLinhaVertice(linha, grafo, i);
	}

	ajustaVerticesAdjacentes(grafo);

	int algoritmo;
	scanf("%d", &algoritmo);

	Grafo * sccs = (algoritmo == 1 ? SCCs1(grafo) : SCCs2(grafo));
  
  imprimeSaida(sccs);

    return 0;
  }
  else
  {
    int numVertices;
    int numArestas;
    int algoritmo = 1;
    scanf("%d", &numVertices);
    scanf("%d", &numArestas);

    Grafo * grafo = grafoaleatorio(numVertices, numArestas);
    printf ("\nkosarayu\n");
    Grafo * sccs = SCCs1(grafo);
    printf ("\nkosarayu funfou?\n");
    imprimeSaida(sccs);

    return 0;
  }
}
