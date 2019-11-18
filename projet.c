#include <stdio.h>
#include <stdlib.h>
#define N 8 //nombre de sommets
#define PR 0.6 //proba
#define Approche 2 //si Approche est 1, c'est la melange de Leighton et Brutal
                   //si Approche est 2, c'est la melange de Brelaz et Brutal
#define K 4 //nombre de couleurs, si K>10 on peut calculer le resultat,mais je mets seulment 10 couleurs dans
            // la fonction trace, donc si K>10, on ne peut pas tracer le graphe

//methode Brutale pour savoir si k-coloration est possible
int Brutal(int M[][N], int k, int co[N])
{
	int i, m, o;
	for (i = 0; i < N; i++) co[i] = 0;
	while (co[0] != k)
	{
		//verifier si cette coloration est bien
		int existence = 1;
		for (m = 0; m < N; m++)
			for (o = m+1; o < N; o++)
				if (M[m][o] == 1 && co[m] == co[o]) existence = 0;
		if (existence == 1) return 1;
		//changer la coloration
		co[N - 1] += 1;
		for (i = N - 1; i > 0; i--)
		{
			if (co[i] == k)
			{
				co[i] = co[i] % k;
				co[i - 1] += 1;
			}
		}
	}
	return 0;
}

//methode de Zykov
int Zykov(int M[][N], int n, int co[N]) //n est la vraie taille de M
{
	int i, j, k;
	int T[N][N] = { 0 }, P[N][N] = { 0 }; //T est M+uv, P est M/uv
	for (i = 0; i < n; i++)
	{
		for (j = i + 1; j < n; j++)
		{
			T[i][j] = T[j][i] = M[i][j];
			P[i][j] = P[j][i] = M[i][j];
		}
	}
	for (i = 0; i < n; i++)
	{
		for (j = i+1; j < n; j++)
		{
			if (M[i][j] == 0)
			{
				T[i][j] = T[j][i] = 1; //nouveau T qui est M+uv
				//pour obtenir nouveau P qui est M/uv
				for (k = 0; k < n; k++)
				{
					if (M[k][j] == 1)
					{
							P[i][k] = 1;
						    P[k][i] = 1;
							P[j][k] = 0;
							P[k][j] = 0;
					}
				}
				int l, m;
				for (l = 0; l < n; l++)
				{
					for (m = 0; m < n; m++)
					{
						if (l > j && m > j) P[l - 1][m - 1] = P[l][m];
						else if (l < j && m > j) P[l][m - 1] = P[l][m];
						else if (l > j && m < j) P[l - 1][m] = P[l][m];
					}
				}
				int essai1[N] = { 0 }, essai2[N] = { 0 };//essai1 et essai2 sont les colorations de T et P
				l = Zykov(T, n, essai1);
				m = Zykov(P, n - 1, essai2);
				int o;
				//si l>=m, utiliser la coloration de P
				if (l >= m)
				{
					//colorer M a partir de P qui est M/uv
					for (o = 0; o < j; o++) co[o] = essai2[o];
					for (o = n - 1; o > j; o--) co[o] = essai2[o - 1];
					co[j] = essai2[i];
				    return m;
				}
				else
				{
					//colorer M a partir de T qui est M+uv
					for (o = 0; o < n; o++) co[o] = essai1[o];
					return l;
				}

			}
		}
	}
	for (i = 0; i < n; i++)
		co[i] = i;
	return n;
}

// methode de Leighton
int Leighton(int M[][N], int co[N], int n)
{
	int degre[N] = { 0 };
	int i, j, k;
	//calculer le degre de chaque sommet
	for(i=0;i<n;i++)
		for(j=i+1;j<n;j++)
			if(M[i][j]==1)
			{
				degre[i] += 1; degre[j] += 1;
			}
	//initialiser le max
	int max=-1;
	for (i = 0; i < n; i++)
		if (degre[i] < n - 1)
		{
			max = i;
			break;
		}
	//si tous les sommets sont complets
	if (max == -1)
	{
		for (i = 0; i < n; i++)
			co[i] = i;
		return n;
	}
	//trouver le sommet qui n'est pas complet et qui a le degre maximal
	for (i = 0; i < n; i++)
		if (degre[i] > degre[max] && degre[i] < n-1)
			max = i;
	//
	int P[N][N] = { 0 };
	for (i = 0; i < n; i++)
		for (j = i + 1; j < n; j++)
			P[i][j] = P[j][i] = M[i][j];
	//contracter
	int contract=0;
	for (i = 0; i < n; i++)
		if (M[i][max] == 0 && i != max)
		{
			contract = i;
			break;
		}
	for (k = 0; k < n; k++)
	{
		if (M[k][contract] == 1)
		{
			P[max][k] = 1;
			P[k][max] = 1;
		}
	}
	int l, m;
	for (l = 0; l < n; l++)
	{
		for (m = 0; m < n; m++)
		{
			if (l > contract && m > contract) P[l - 1][m - 1] = P[l][m];
			else if (l < contract && m > contract) P[l][m - 1] = P[l][m];
			else if (l > contract && m < contract) P[l - 1][m] = P[l][m];
		}
	}
	l=Leighton(P, co, n - 1);
	//colorer
	for (i = n - 1; i > contract; i--) co[i] = co[i - 1];
	co[contract] = co[max];
	return l;
}

// methode de Brelaz
int Brelaz(int M[][N], int co[N])
{
	int degre[N] = { 0 }, utilise[N] = { 0 };
	int i, j;
	for (i = 1; i < N; i++)
		co[i] = -1;//initialiser
	//calculer le degre de chaque sommet
	for (i = 0; i < N; i++)
		for (j = i + 1; j < N; j++)
			if (M[i][j] == 1)
			{
				degre[i] += 1; degre[j] += 1;
			}
	while (1)
	{
		//trouver un sommet non colorie
		int maxu=-1;
		for (i = 0; i < N; i++)
		{
			if (co[i] == -1)
			{
				maxu = i; break;
			}
		}
		//verifier si tous les sommets sont colorie
		if (maxu == -1) break;
		//trouver utilise maximal
		for (i = 0; i < N; i++)
		{
			if (utilise[i] > utilise[maxu] && co[i]==-1)
				maxu = i;
			else if (utilise[i] == utilise[maxu] && co[i] == -1 && degre[i]>degre[maxu])
					maxu = i;
		}

		//colorier
		for (i = 0; i < N; i++)
		{
			int colorier=1;
			for (j = 0; j < N; j++)
			{
				if (M[maxu][j] == 1 && co[j] == i)
					colorier = 0;
			}
			if (colorier == 1)
			{
				co[maxu] = i;
				break;
			}
		}
		// changer utilise
		for (i = 0; i < N; i++)
		{
			if (M[maxu][i] == 1) utilise[i] += 1;
		}
	}
	int nom=0;
	for (i = 1; i < N; i++)
		if (co[i] > nom)
			nom = co[i];
	return nom+1;
}

//m¨¦lange de m¨¦thode approch¨¦e et la m¨¦thode brutale
int Melange(int M[][N], int co[N],int n) //si n=1, methode de Leighton, si n=2, methode de Brelaz
{
	int max,i;
	if (n == 1) max = Leighton(M, co, N);
	else if (n == 2) max = Brelaz(M, co);
	for (i = max - 1; i > 0; i--)
	{
		if (Brutal(M, i, co) == 0)
		{
			Brutal(M, i+1, co);
			return i + 1;
		}
	}
}

//pour generer un graphe simple
void genere_GS(int Mat[][N])
{
    int i, j;
    for (i = 0;i < N;i++)
        for (j =i+1;j < N;j++)
            if (i !=j && ((rand()) % 1000) <= PR*1000 )
                {Mat[i][j] = 1;Mat[j][i] = 1;}
            else {Mat[i][j] = 0;Mat[j][i] = 0;}
}

//pour tracer le graphe simple, je mets seulement 10 couleurs dans la fonction
void trace_GS(const int Mat[][N],int co[N],int n)
{
	FILE* mongraphe = fopen("Mon_graphe.txt", "w");
    fprintf(mongraphe,"graph {\n");
    int i, j;
	for (i = 0; i < N - 1; i++)
	{
		switch (co[i])
		{
		    case(0): fprintf(mongraphe, "%d [shape=circle][color=green][style=filled];\n", i); break;
			case(1): fprintf(mongraphe, "%d [shape=circle][color=red][style=filled];\n", i); break;
			case(2): fprintf(mongraphe, "%d [shape=circle][color=yellow][style=filled];\n", i); break;
			case(3): fprintf(mongraphe, "%d [shape=circle][color=blue][style=filled];\n", i); break;
			case(4): fprintf(mongraphe, "%d [shape=circle][color=gray][style=filled];\n", i); break;
			case(5): fprintf(mongraphe, "%d [shape=circle][color=brown][style=filled];\n", i); break;
			case(6): fprintf(mongraphe, "%d [shape=circle][color=cyan][style=filled];\n", i); break;
			case(7): fprintf(mongraphe, "%d [shape=circle][color=white][style=filled];\n", i); break;
			case(8): fprintf(mongraphe, "%d [shape=circle][color=orange][style=filled];\n", i); break;
			case(9): fprintf(mongraphe, "%d [shape=circle][color=magenta][style=filled];\n", i); break;
		}
	}
	switch (co[N-1])
	{
	case(0): fprintf(mongraphe, "%d [shape=circle][color=green][style=filled]\n", N-1); break;
	case(1): fprintf(mongraphe, "%d [shape=circle][color=red][style=filled]\n", N - 1); break;
	case(2): fprintf(mongraphe, "%d [shape=circle][color=yellow][style=filled]\n", N - 1); break;
	case(3): fprintf(mongraphe, "%d [shape=circle][color=blue][style=filled]\n", N - 1); break;
	case(4): fprintf(mongraphe, "%d [shape=circle][color=gray][style=filled]\n", N - 1); break;
	case(5): fprintf(mongraphe, "%d [shape=circle][color=brown][style=filled]\n", N - 1); break;
	case(6): fprintf(mongraphe, "%d [shape=circle][color=cyan][style=filled]\n", N - 1); break;
	case(7): fprintf(mongraphe, "%d [shape=circle][color=white][style=filled]\n", N - 1); break;
	case(8): fprintf(mongraphe, "%d [shape=circle][color=orange][style=filled]\n", N - 1); break;
	case(9): fprintf(mongraphe, "%d [shape=circle][color=magenta][style=filled]\n", N - 1); break;
	}
    for (i = 0;i < N;i++)
        for (j = 0;j < i;j++)
            if (Mat[i][j] != 0)
                fprintf(mongraphe,"%d -- %d \n", i, j);
    fprintf(mongraphe,"}");
    fclose(mongraphe);
    //system("dot -Tpdf Mon_graphe.txt > essai.pdf && Acrobat essai.pdf");
	switch (n)
	{
	       case(1): system("dot -Tpdf Mon_graphe.txt > Brutal.pdf"); break;
	       case(2): system("dot -Tpdf Mon_graphe.txt > Zykov.pdf"); break;
		   case(3): system("dot -Tpdf Mon_graphe.txt > Leighton.pdf"); break;
		   case(4): system("dot -Tpdf Mon_graphe.txt > Brelaz.pdf"); break;
		   case(5): system("dot -Tpdf Mon_graphe.txt > Melange.pdf"); break;
	}
}



int main()
{
    int M[N][N]={0};
	int coloration[N] = { 0 };
    int i, j;
    //genere_GS(M); //generer un graphe
    M[0][1]=M[1][0]=M[0][2]=M[2][0]=M[0][3]=M[3][0]=M[0][4]=M[4][0]=1;
    M[1][2]=M[2][1]=M[1][3]=M[3][1]=M[1][5]=M[5][1]=1;
    M[4][6]=M[6][4]=M[4][7]=M[7][4]=1;
    M[5][6]=M[6][5]=M[5][7]=M[7][5]=1;
    M[6][7]=M[7][6]=1;

	//imprimer la matrice
	printf("La matrice est : \n");
    for (i = 0;i < N;i++)
        {
            for (j =0;j < N;j++)
               printf("%d ",M[i][j]);
            printf("\n");
        }
	/*
	//pour la fonction Brutale
    if(Brutal(M, K, coloration)==1)
    {
		printf("Fonction Brutal : Il existe %d-coloration : ",K);
		for (i=0;i<N;i++) printf("%d ",coloration[i]);
		printf("\n");
        trace_GS(M,coloration,1);
		printf("Ouvrir le fichier Brutal.pdf pour verifier.\n\n");
	}
    else printf("Fonction Brutal : Il n'existe pas.\n\n");
	*/
	//pour la fonction zykov
	printf("Fonction de Zykov : le nombre de chromatique est %d.\n",Zykov(M, N,coloration));
	printf("Une coloration est : ");
	for (i = 0; i < N; i++) printf("%d ", coloration[i]);
	printf("\n");
	trace_GS(M, coloration,2);
	printf("Ouvrir le fichier Zykov.pdf pour verifier.\n\n");

	//pour la fonction Leighton
	printf("Fonction de Leighton : une estimation du nombre de chromatique est %d.\n", Leighton(M, coloration, N));
	printf("Une coloration est : ");
	for (i = 0; i < N; i++) printf("%d ", coloration[i]);
	printf("\n");
	trace_GS(M, coloration, 3);
	printf("Ouvrir le fichier Leighton.pdf pour verifier.\n\n");

	//pour la fonction Brelaz
	printf("Fonction de Brelaz : une estimation du nombre de chromatique est %d.\n", Brelaz(M, coloration));
	printf("Une coloration est : ");
	for (i = 0; i < N; i++) printf("%d ", coloration[i]);
	printf("\n");
	trace_GS(M, coloration, 4);
	printf("Ouvrir le fichier Brelaz.pdf pour verifier.\n\n");
/*
	//pour la fonction Melange
	printf("Fonction de Melange : le nombre de chromatique est %d.\n", Melange(M, coloration,Approche));
	printf("Une coloration est : ");
	for (i = 0; i < N; i++) printf("%d ", coloration[i]);
	printf("\n");
	trace_GS(M, coloration, 5);
	printf("Ouvrir le fichier Melange.pdf pour verifier.\n\n");*/
}
