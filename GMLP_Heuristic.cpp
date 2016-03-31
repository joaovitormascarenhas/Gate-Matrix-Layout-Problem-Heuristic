#include <iostream>
#include <vector>
#include <bitset>
#include <set>
#include <queue> 
#define L 5000
#define MINPORTAS 50

using namespace std;

void leituraDaEntrada (FILE *pf, vector < vector <int> > &M, int lines, int columns, vector < set <int> > &tracksOfGates, vector <int> &NGatesOfTracks, vector < set<int> > &gatesOfTracks){
	int i, j, k;
	for (i=0; i<lines; i++){
		for(j=0; j<columns; j++){
			fscanf(pf,"%d", &k);
			M[i].push_back(k);
			if(k==1){
				NGatesOfTracks[i]++;
				tracksOfGates[j].insert(i);
				gatesOfTracks[i].insert(j);
			}
		}
	}
}

void representarLAdjacencia(vector < vector <int> > M, int lines, int columns, vector < set <int> > &LAdjacency){ 
	int i, j, p1, p2;
	for (j=0; j<columns; j++){
		for(i=0; i<lines; i++){
			if(M[i][j]==1){
				p1=i++;
				while(i<lines && M[i][j]!=1) i++;
				if(i<lines && M[i][j]==1){
					p2=i--;
					LAdjacency[p1].insert(p2);
					LAdjacency[p2++].insert(p1);
					while(p2<lines){
						if(M[p2][j]==1){
							LAdjacency[p1].insert(p2);
							LAdjacency[p2].insert(p1);
						}
						p2++;
					}					
				}
			}
		}
	}
}

void dominancia(vector < vector <int> > &M, int lines, int columns, vector <int> &TPilhas, vector < set <int> > &ListaP, vector < set <int> > &dominadas, vector <int> &portasAtivas, int &npdominadas){
	vector < bitset < L > > P(columns); 
	bitset < L > B(0);
	vector <int> TPilhas2(lines,0);
	int i, j;
	npdominadas=0;
	for (j=0; j<columns; j++){
		for(i=0; i<lines; i++){
			P[j].set(i, M[i][j]);
		}
	}
	for (i=0; i<columns; i++){
		if (portasAtivas[i]==0){
			for(j=0; j<columns; j++){
				B=0;
				B=(P[i] | P[j]);
				if(P[i]==B && i!=j){
					if (portasAtivas[j]==0){
						npdominadas++;
					}
					portasAtivas[j]=1;
					dominadas[i].insert(j);
				}
			}
		}
	}
	for(i=0; i<lines; i++){
		for(j=0; j<columns; j++){
			if ((portasAtivas[j]==0) && (ListaP[j].find(i)!=ListaP[j].end())){
				TPilhas2[i]++;
			}
		}
	}
	TPilhas=TPilhas2;
}

void incluirDomNaOrdem(int columns, vector <int> &orderOfGates, vector <int> activeGates, vector < set <int> > gatesDominated){
	vector< int >::iterator it;
	set< int >::iterator it2;
	vector <int> newOrder(columns, 0);
	newOrder.clear();
	if(orderOfGates.size()<(unsigned int)columns){
		for (it = orderOfGates.begin(); it != orderOfGates.end(); ++it){
			newOrder.push_back(*it);
			for(it2=gatesDominated[(*it)].begin(); it2!=gatesDominated[(*it)].end(); ++it2){
				if(find(newOrder.begin(), newOrder.end(), *it2) == newOrder.end()){
					newOrder.push_back(*it2);
				}
			}
		}
		orderOfGates=newOrder;
	}
}

int evaluate(int lines, vector < set <int> > tracksOfGates, vector <int> NGatesOfTracks, vector <int> orderOfGates, int &cost){
	int cont=0, maior=0, pilhasfechadas;
	set< int >::iterator it;
	vector<int> TContada(lines, 0);//Indica com 1 se a trilha ja foi contabilizada
	vector< int >::iterator ito;

	for(ito=orderOfGates.begin(); ito != orderOfGates.end(); ++ito){
		pilhasfechadas=0;
		for (it = tracksOfGates[*ito].begin() ; it != tracksOfGates[*ito].end(); ++it){
			if(TContada[*it]==0 && NGatesOfTracks[*it]>0){
				cont++;
				TContada[*it]=1;
			}
			NGatesOfTracks[*it]--;
			if (NGatesOfTracks[*it]==0){
				pilhasfechadas++;
			}	
		}
		if(cont>maior){
			maior=cont;
		}
		cont-=pilhasfechadas;
	}
	cost=maior;
	return maior;
}

bool aprimoramentoAdaptativo2(vector < vector <int> > M, int lines, vector <int> &orderOfGates, vector <int> activeGates, vector < set <int> > tracksOfGates, vector <int> NGatesOfTracks, int LAHEAD){
	vector <int> onesPosition, newOrder(orderOfGates.begin(), orderOfGates.end()), currentOrder(orderOfGates.begin(), orderOfGates.end());//, NGatesOfTracks(lines, 0);
	int i, j, originalCost, initialCost, newCost, nOfActiveGates, limitGate, adaptation;
	vector< int >::iterator it, itb;
	bool flag=false, flag2=false;
	initialCost=originalCost=evaluate(lines, tracksOfGates, NGatesOfTracks, newOrder, originalCost);
	nOfActiveGates=orderOfGates.size();
	do{
		flag=false;
		for (i=0; i<lines; i++){//Abrindo mais tarde
			onesPosition.clear();
			
			if(NGatesOfTracks[i]>MINPORTAS) 
				adaptation=(NGatesOfTracks[i]*LAHEAD)/100; 
			else adaptation=NGatesOfTracks[i]-1; 

			for (j=0; j<(nOfActiveGates); j++){//Busca a porta limite e armazena a posição das anteriores
				if(M[i][(currentOrder[j])]==1){
					if (onesPosition.size() < (unsigned int)adaptation){
						onesPosition.push_back(j);
					}
					else{//j é a porta limite
						limitGate=j; break;
					}
				}
			}
			vector< int >::reverse_iterator itr;
			for(itr=onesPosition.rbegin(); itr!=onesPosition.rend(); ++itr){
				currentOrder.insert(currentOrder.begin()+limitGate, (currentOrder[(*itr)]));
				currentOrder.erase(currentOrder.begin()+(*itr));
				if (originalCost>=evaluate(lines, tracksOfGates, NGatesOfTracks, currentOrder, newCost)){
					if(originalCost>newCost){
						flag=true;
					}
					flag2=true;
					newOrder=currentOrder;
					originalCost=newCost;
					limitGate--;
				}
				else{

					currentOrder=newOrder;//Faz com que a melhoria nao seja tudo/nada 
					
					if(itr!=onesPosition.rbegin()){//caso seja o primeiro não tem como acessar *(itr-1)
						currentOrder.insert(currentOrder.begin()+*(itr-1), (currentOrder[(*itr)]));
						currentOrder.erase(currentOrder.begin()+(*itr));
						if (originalCost>=evaluate(lines, tracksOfGates, NGatesOfTracks, currentOrder, newCost)){
							if(originalCost>newCost){
								flag=true;
							}
							flag2=true;
							newOrder=currentOrder;
							originalCost=newCost;

						}
						else{
							currentOrder=newOrder;//Faz com que a melhoria nao seja tudo/nada 
						}
					}
				}
			}
			
		}
	}while(flag);
	
	do{
		flag=false;
		for (i=0; i<lines; i++){//Fechar a pilha mais cedo
			onesPosition.clear();
			if(NGatesOfTracks[i]>MINPORTAS)
				adaptation=(NGatesOfTracks[i]*LAHEAD)/100; 
			else adaptation=NGatesOfTracks[i]-1;
			for (j=nOfActiveGates-1; j>=0; j--){//Busca a porta limite e armazena a posição das anteriores
				if(M[i][(currentOrder[j])]==1){
					if (onesPosition.size() < (unsigned int)adaptation){
						onesPosition.push_back(j);
					}
					else{//j é a porta limite
						limitGate=j; break;
					}
				}
			}
			vector< int >::reverse_iterator itr;
			for(itr=onesPosition.rbegin(); itr!=onesPosition.rend(); ++itr){
				currentOrder.insert(currentOrder.begin()+limitGate+1, (currentOrder[(*itr)]));
				currentOrder.erase(currentOrder.begin()+(*itr)+1);
				if (originalCost>=evaluate(lines, tracksOfGates, NGatesOfTracks, currentOrder, newCost)){
					if(originalCost>newCost){
						flag=true;
					}
					flag2=true;
					originalCost=newCost;
					newOrder=currentOrder;
					limitGate++;
				}
				else{
					currentOrder=newOrder;//Faz com que a melhoria nao seja tudo/nada 
					
					if(itr!=onesPosition.rbegin()){//caso seja o primeiro não tem como acessar *(itr-1)
						currentOrder.insert(currentOrder.begin()+*(itr-1)+1, (currentOrder[(*itr)]));
						currentOrder.erase(currentOrder.begin()+(*itr)+1);
						if (originalCost>=evaluate(lines, tracksOfGates, NGatesOfTracks, currentOrder, newCost)){
							if(originalCost>newCost){
								flag=true;
							}
							flag2=true;
							originalCost=newCost;
							newOrder=currentOrder;

						}
						else{
							currentOrder=newOrder;//Faz com que a melhoria nao seja tudo/nada 
						}
					}
				}
			}						
		}
	}while(flag);
	
	if(flag2) orderOfGates=newOrder;
	return (initialCost>originalCost);
}

/* Heap Sort */
void heapRefaz(vector<int> &v, int left, int right, vector <int> NGatesOfTracks) { 
    int i = left;
    int j = i*2 + 1; 
    int aux = v[i]; 
    while (j <= right) { 
        if (j < right && NGatesOfTracks[v[j]] > NGatesOfTracks[v[j+1]]) 
            j++; 
        if (NGatesOfTracks[aux] <= NGatesOfTracks[v[j]]) 
            break; 
        v[i] = v[j]; 
        i = j; 
        j = i*2 + 1; 
    } 
    v[i] = aux; 
}  
void heapConstroi(vector<int> &v, int n, vector <int> NGatesOfTracks) { 
    int left;
    left = (n/2)-1;
    while (left >= 0) { 
        heapRefaz(v, left, n-1, NGatesOfTracks); 
        left--; 
    } 
} 
void heapSortNPilhas(vector<int> &v, int n, vector <int> NGatesOfTracks) {
    int aux;
    heapConstroi(v, n, NGatesOfTracks); 
    while (n > 1) { 
        aux = v[n-1]; 
        v[n-1] = v[0]; 
        v[0] = aux; 
        n--; 
        heapRefaz(v, 0, n-1, NGatesOfTracks); 
    } 
} 

void BFSPilha(vector<int> &orderBFS, vector < set <int> > LAdjacency, vector<int> NGatesOfTracks, int lines, bitset<L> &vVisited, vector<int> verticesAtivos){
	queue<int> Q;
	vector< int >::iterator ita;
	int i, v, menosPortas;
	menosPortas=INT_MAX;
	for(i=0; i<lines; i++){
		if(menosPortas > NGatesOfTracks[i] && vVisited[i]==0 && verticesAtivos[i]==0){
			menosPortas=NGatesOfTracks[i];
			v=i;
		}			
	}
	Q.push(v);
	vVisited[v]=1; //Marca o vertice
	while(!Q.empty()){
		v=Q.front();
		orderBFS.push_back(v);
		Q.pop();
		vector<int> aux(LAdjacency[v].begin(), LAdjacency[v].end());
		heapSortNPilhas(aux, LAdjacency[v].size(), NGatesOfTracks);
		for(ita=aux.begin(); ita!= aux.end(); ++ita){
			if(vVisited[*ita]==0 && verticesAtivos[*ita]==0){//Vertice nao explorado
				Q.push(*ita);
				vVisited[*ita]=1;
			}
			
		}
	}
	
	for(i=0; i<lines; i++){//Busca por um vertice desconexo ainda não visitado
		if(vVisited[i]==0 && verticesAtivos[i]==0){
			BFSPilha(orderBFS, LAdjacency, NGatesOfTracks, lines, vVisited, verticesAtivos);
		}			
	}
}


void GemeosEntreSi(vector < set <int> > LAdjacency, int lines, vector <int> &verticesAtivos, vector < set <int> > &verticesDominados){
	int i, j;
	vector<int> v(lines);
	vector <int> :: iterator it;
	for(i=0; i<lines; i++){
		for(j=0; j<lines; j++){
			if(i!=j && verticesAtivos[i]==0 && verticesAtivos[j]==0){
				it=set_symmetric_difference (LAdjacency[i].begin(), LAdjacency[i].end(), LAdjacency[j].begin(), LAdjacency[j].end(), v.begin());
				v.resize(it-v.begin());

				if(v.size()==2){
					if((v[0]==i && v[1]==j) || (v[0]==j && v[1]==i)){//Vértices gêmeos/adjacentes entre si
						verticesDominados[i].insert(j);
						verticesAtivos[j]=1;
					}
				}
			}
		}
	}
}


void incluirVerticesDomNaOrdem(int lines, vector <int> &ordem, vector <int> verticesAtivos, vector < set <int> > verticesDominados){
	vector< int >::iterator it;
	set< int >::iterator it2;
	vector <int> novaOrdem(lines);
	novaOrdem.clear();
	if(ordem.size()<(unsigned int)lines){
		for (it = ordem.begin(); it != ordem.end(); ++it){
			novaOrdem.push_back(*it);
			for(it2=verticesDominados[(*it)].begin(); it2!=verticesDominados[(*it)].end(); ++it2){
				if(find(novaOrdem.begin(), novaOrdem.end(), *it2) == novaOrdem.end()){
					novaOrdem.push_back(*it2);
					verticesAtivos[*it2]=0;
				}
			}
		}
		
		for(int i=0; i<lines; i++){
			if(verticesAtivos[i]==1){
				novaOrdem.push_back(i);
				verticesAtivos[i]=0;
			}
		}
		ordem=novaOrdem;		
	}
}

void sortBecceneri(vector<int> ordembfs, vector<int> &orderOfGates, vector < set <int> > ListaP, vector <int> portasAtivas, int npdominadas, int lines, int columns){
	int diferenca, i;
	vector<int>::iterator ita;
	set<int>::iterator itb;
	set<int> trilhas;
	vector<int> v(lines);
	orderOfGates.clear();
	vector< int >::iterator it;
	for (it=ordembfs.begin(); it!=ordembfs.end(); ++it){
		trilhas.insert(*it);	
		for(i=0; i<columns; i++){
			if (portasAtivas[i]==0){
				v.clear();
				ita= set_difference (ListaP[i].begin(), ListaP[i].end(), trilhas.begin(), trilhas.end(), v.begin());
				v.resize(ita-v.begin());
				diferenca=v.size();
				if (diferenca==0){
					orderOfGates.push_back(i);
					portasAtivas[i]=1;
				}
			}
		}
		if(orderOfGates.size()==(unsigned int)columns-npdominadas) break;
	}
}


int main(){ 
	
	FILE *pf, *pr;
	char file[80], resultFile[80];
	int nOfGatesDominated=0, cost=0, lines, columns,   LAhead=70;
	bitset < L > vVisitados;

	printf("Enter file name: ");
	scanf("%s", file);
	if ((pf=fopen(file, "r"))==NULL) cout<<"ERRO\n";
	strcpy(resultFile,"Sol_"); strcat(resultFile, file);
	if ((pr=fopen(resultFile, "w"))==NULL) cout<<"ERRO\n";

	fscanf (pf, "%d %d\n", &lines, &columns);
	vector < vector <int> > M (lines);
	vector < set <int> > tracksOfGates(columns), gatesDominated(columns), LAdjacency(lines), gatesOfTracks(lines), verticesDominados(lines);
	vector <int> NGatesOfTracks(lines, 0), activeGates(columns, 0), orderOfGates(columns, 0), ordemPortasTemp(columns, 0), verticesAtivos(lines, 0);
	vector<int> orderOfVertices;
	
	leituraDaEntrada (pf, M, lines, columns, tracksOfGates, NGatesOfTracks, gatesOfTracks);
	fclose(pf);
	dominancia(M, lines, columns, NGatesOfTracks, tracksOfGates, gatesDominated, activeGates, nOfGatesDominated);	
	representarLAdjacencia(M, lines, columns, LAdjacency);
	GemeosEntreSi(LAdjacency, lines, verticesAtivos, verticesDominados);
	BFSPilha(orderOfVertices, LAdjacency, NGatesOfTracks, lines, vVisitados, verticesAtivos);
	incluirVerticesDomNaOrdem(lines, orderOfVertices, verticesAtivos, verticesDominados);
	sortBecceneri(orderOfVertices, orderOfGates, tracksOfGates, activeGates, nOfGatesDominated, lines, columns);
	while (aprimoramentoAdaptativo2(M, lines, orderOfGates, activeGates, tracksOfGates, NGatesOfTracks, LAhead)){}

	evaluate(lines, tracksOfGates, NGatesOfTracks, orderOfGates, cost);

	fprintf(pr,"Matrix final cost: %d\n\n", cost);
	fprintf(pr,"Sequence of gates:\n");	
	incluirDomNaOrdem(columns, orderOfGates, activeGates, gatesDominated);
	for (vector< int >::iterator it = orderOfGates.begin(); it != orderOfGates.end(); ++it){
		fprintf(pr, "%d ", *it);
	}
	fprintf(pr,"\n\nNumber of dominated gates: %d\n", nOfGatesDominated);
	fprintf(pr,"\nList of dominated gates:");	
	for(int i=0; i<columns; i++){
		fprintf(pr, "\n%d -> ", i);
		for (set< int >::iterator it = gatesDominated[i].begin() ; it != gatesDominated[i].end(); ++it){
			fprintf(pr, "%d ", *it);
		}
	}
	fprintf(pr,"\n\nMatrix of solution:\n");
	for (int i=0; i<lines; i++){
		for (int j=0; j<columns; j++){
			fprintf(pr, "%d\t", M[i][(orderOfGates[j])]);
		}
		fprintf(pr, "\n");
	}
	
	return 0;
}


