#include <stdio.hd>
#include <pthread.h>

#define N 5 //tamanho do buffer
#define M 10 //tamanho de cada bloco do buffer
int **buffer, **blocoLido; //buffer que armazena os blocos, e matriz que guarda a informação de quais blocos ja foram lidos pelas 3 threads consumidoras
int proxt0=0,proxt1=0,proxt2=0,proxleitor=0; // marca o próximo bloco a ser lido pelas 3 threads consumidoras e a thread leitora
int numerosRestantes; // quantidade de números a serem lidos do array
pthread_cond_t produtor, consumidores; //variáveis de condição
pthread_mutex_t mutex; //exclusão mútua


//thread responsável pela leitura do arquivo
void * leituraArquivo(void * arg)
{
	FILE *arquivo = (FILE *) arg;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		//bloqueia a thread caso a próxima casa do buffer a ser reescrita ainda não tenha sido lida por todas as threads
		while(blocoLido[proxleitor][0]==0 ||
			blocoLido[proxleitor][1] == 0 ||
			blocoLido[proxleitor][2] == 0) 
			pthread_cond_wait(&produtor, &mutex);

		pthread_mutex_unlock(&mutex);
		for (int i = 0; i < M; ++i)
		{
			//lê um número do arquivo e armazena no bloco atual do buffer
			fread(&buffer[proxleitor][i], sizeof(int), 1, arquivo);
			numerosRestantes--;

			if(numerosRestantes == 0) // se não houverem mais números a serem lidos
			{
				while(i<M)
				{
					buffer[proxleitor][i] = -1;
					i++;
				}
				pthread_exit(NULL);
			}
		}
		pthread_mutex_lock(&mutex);
		blocoLido[proxleitor][0] = 0;
		blocoLido[proxleitor][1] = 0;
		blocoLido[proxleitor][2] = 0;
		pthread_mutex_unlock(&mutex);
		proxleitor = (proxleitor+1) % N;
		pthread_cond_broadcast(&consumidores);
	}
}





void * MaiorSequencia()
{
	int numAtual = -1, seqAtual= 0,posSequenciaAtual = 0, maiorSequencia = 0, numMaiorSequencia = -1, posMaiorSequencia = 0;
	int *vetor;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		while(blocoLido[proxt0][0] == 1) //se o próximo bloco a ser lido ainda não foi reescrito pelo produtor
			pthread_cond_wait(&consumidores, &mutex);

		pthread_mutex_unlock(&mutex);

		//pega um vetor(bloco) de inteiros de tamanho M do buffer para ler
		vetor = buffer[proxt0]; // apenas para facilitar a compreensão do código, variável que aponta pro bloco sendo lido atualmente
		for(int i = 0; i < M; i++)
		{
			if(vetor[i] == -1)//acabaram os números a serem lidos
			{
				pthread_mutex_lock(&mutex);
				printf("Maior Sequencia de valores idênticos: %d %d %d\n", posMaiorSequencia, maiorSequencia, numMaiorSequencia);
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}	
			if(vetor[i] == numAtual)
				seqAtual++;
			else{
				if(seqAtual > maiorSequencia)
				{
					maiorSequencia = seqAtual;
					numMaiorSequencia = numAtual;
					posMaiorSequencia = posSequenciaAtual;
				}
				numAtual = vetor[i];
				seqAtual = 1;
				posSequenciaAtual = i;
			}
		}

		pthread_mutex_lock(&mutex);
		blocoLido[proxt0][0] = 1; //assinala que leu o bloco do vetor
		pthread_mutex_unlock(&mutex);
		proxt0 = (proxt0 + 1) % N; //marca a próxima posição do buffer a ser consumida
		pthread_cond_signal(&produtor); //libera a thread leitora do arquivo caso esteja bloqueada
	} 
}





void * SequenciaDe3()
{
	int cont = 0, sequencias = 0, numAtual = -1;
	int *vetor;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		while(blocoLido[proxt1][1] == 1) //se o próximo bloco a ser lido ainda não foi reescrito pelo produtor
			pthread_cond_wait(&consumidores, &mutex);

		pthread_mutex_unlock(&mutex);

		//pega um vetor(bloco) de inteiros de tamanho M do buffer para ler
		vetor = buffer[proxt1]; // apenas para facilitar a compreensão do código, variável que aponta pro bloco sendo lido atualmente

		for(int i = 0; i < M; i++)
		{
			if(vetor[i] == -1)//acabaram os números a serem lidos
			{
				pthread_mutex_lock(&mutex);
				printf("Quantidade de triplas: %d\n", &sequencias);
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}	


			if(vetor[i] == numAtual)
			{
				cont++;
				if(cont == 3)
				{
					sequencias++;
					cont = 0;
				}
			}
			else{
				numAtual = vetor[i];
				cont = 1;
			}
		}

		pthread_mutex_lock(&mutex);
		blocoLido[proxt1][1] = 1; //assinala que leu o bloco do vetor
		pthread_mutex_unlock(&mutex);
		proxt1 = (proxt1 + 1) % N; //marca a próxima posição do buffer a ser consumida
		pthread_cond_signal(&produtor); //libera a thread leitora do arquivo caso esteja bloqueada
	}
}


void * SequenciaCrescente()
{
	int numAtual = -1, sequencias = 0;
	int *vetor;
	while(1)
	{

		pthread_mutex_lock(&mutex);
		while(blocoLido[proxt2][2] == 1) //se o próximo bloco a ser lido ainda não foi reescrito pelo produtor
			pthread_cond_wait(&consumidores, &mutex);

		pthread_mutex_unlock(&mutex);


		//pega um vetor(bloco) de inteiros de tamanho M do buffer para ler
		vetor = buffer[proxt2]; // apenas para facilitar a compreensão do código, variável que aponta pro bloco sendo lido atualmente
		for(int i = 0; i < M; i++)
		{

			if(vetor[i] == -1)//acabaram os números a serem lidos
			{
				pthread_mutex_lock(&mutex);
				printf("Quantidade de ocorrências da sequência <012345>: %d\n", &sequencias);
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}	


			if(vetor[i] == numAtual + 1)
			{
				numAtual++;
				if(numAtual == 5)
					sequencias++;
			}
			else numAtual = -1;
		}

		pthread_mutex_lock(&mutex);
		blocoLido[proxt2][2] = 1; //assinala que leu o bloco do vetor
		pthread_mutex_unlock(&mutex);
		proxt2 = (proxt2 + 1) % N; //marca a próxima posição do buffer a ser consumida
		pthread_cond_signal(&produtor); //libera a thread leitora do arquivo caso esteja bloqueada
	}
}



int main(int argc, char *argv[])
{
	//variáveis iniciais
	pthread_t tid[4]; //vetor de id das threads
	FILE *arq;

	//inicialização das variáveos de condição e exclusão mútua
	pthread_mutex_init(&mutex, 0, 1);
    pthread_cond_init(&produtor, NULL);
    pthread_cond_init(&consumidores,NULL);

	if ( argc != 2 ) 
        printf( "Favor digitar o nome do arquivo a ser executado e o arquivo de entrada dos números");
	
   FILE *arq = fopen( argv[1], "rb" );

    if ( file == NULL )
        printf( "Erro na abertura do arquivo\n" );

    fread(&numerosRestantes,sizeof(int),1,arq);

    //inicializa o buffer e a matriz que verifica os blocos lidos
    buffer = (int **) malloc (N * sizeof(int *));
    blocoLido = (int **) malloc (N * sizeof(int *));

    for (int i = 0; i < N; ++i)
    {
    	buffer[i] = (int *) malloc (M*sizeof(int));
    	blocoLido[i] = (int *) malloc (3*sizeof(int));

    	for (int j = 0; j < M; ++j)
    		blocoLido[i][j] = 1; //no ínicio do programa, marca todos os blocos do buffer como "lidos", para que a thread produtora possa reescrevê-los
    }


    //cria as threads
    pthread_create(&tid[0], NULL, leituraArquivo, (void *) &id[i+L]);
    pthread_create(&tid[1], NULL, MaiorSequencia, (void *) &id[i+L]);
    pthread_create(&tid[2], NULL, SequenciaDe3, (void *) &id[i+L]);
    pthread_create(&tid[3], NULL, SequenciaCrescente, (void *) &id[i+L]);
    
    //permite as threads trabalhando terminarem
    pthread_exit(NULL);


	return 0;
}