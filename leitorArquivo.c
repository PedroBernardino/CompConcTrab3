

void * maiorSequencia()
{
	int numAtual = -1, seqAtual= 0, maiorSequencia = 0;
	while(1)
	{
		//pega um vetor(bloco) de inteiros de tamanho M do buffer para ler
		for(int i = 0; i < M; i++)
		{
			if(vetor[i] == numAtual)
				seqAtual++;
			else{
				if(seqAtual > maiorSequencia)
					maiorSequencia = seqAtual;
				numAtual = vetor[i];
				seqAtual = 1;
			}
		}
	} 
}





void * SequenciaDe3()
{
	int cont = 0, sequencias = 0, numAtual = -1;
	while(1)
	{
		//pega um vetor(bloco) de inteiros de tamanho M do buffer para ler
		for(int i = 0; i < M; i++)
		{
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
	}
}


void * SequenciaCrescente()
{
	int numAtual = -1, sequencias = 0;
	while(1)
	{
		//pega um vetor(bloco) de inteiros de tamanho M do buffer para ler
		for(int i = 0; i < M; i++)
		{
			if(vetor[i] == numAtual + 1)
			{
				numAtual++;
				if(numAtual == 5)
					sequencias++;
			}
			else numAtual = -1;
		}
	}
}