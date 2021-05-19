#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

struct node{
    node* dup;
    int id;
    bool final;         //han de ser inicialitzats a true, sino el dfs de components connexes fallara
    bool inicialitzat;  //inicialment fals, a true quan fem percolacio arestes
};

using LListaAdjacencia = vector<node>;
using Graf = vector<LListaAdjacencia>;

/*  Representacio:
 *
 *  0: 1(1->0) -> 2(2->0) -> 3(3->0)
 *  1: 0 -> 2 -> 3
 *  2: 0 -> 3
 *  3: 0
 *
 * */

//Cost: lineal en O(n+m)
//Explicació: per cada aresta del graf, s'eliminia aquesta aresta amb una probabilitat 'q'
void percolacioArestes (Graf& G,double q) {

    for(int u = 0; u < G.size(); ++u){
        for(node& v:G[u]){
            if(!v.inicialitzat) {
                if (rand() / double(RAND_MAX) < q) {
                    v.final = false; //aresta de u->v false
                    v.dup->final = false; //aresta de v->u false
                    //cout << "Eliminem aresta de " << v.id << " a " << v.dup->id <<"." << endl;
                } else {
                    v.final = true;
                    v.dup->final = true;
                    //cout << "Ens quedem aresta de " << v.id << " a " << v.dup->id <<"." << endl;
                }
                v.inicialitzat=true;
                v.dup->inicialitzat=true;
            }
        }
    }
}

//Cost: lineal en O(n)
//Explicació: per cada node del graf, s'elimina aquest node amb probabilitat 'q'
vector<bool> percolacioNodes (const Graf& G, double q){
    vector<bool> vertexFinals (G.size());
    for(int i=0; i<G.size();i++){
        if(rand()/double(RAND_MAX) < q){
            vertexFinals[i]= false;
        }
        else vertexFinals[i] = true;
    }
    return vertexFinals;
}

//Cost: lineal en nodes -> O(n)
//Explicació: s'imprimeix la graella nxn (visible per percolacio de nodes)
void imprimirGraella(Graf& G, vector<bool>& vtxFinals){
    cout << "Graella "<<G.size()<<"x"<<G.size()<<" de G:"<<endl;
    for(int i=0; i<sqrt(G.size());i++){
        for(int j=0;j<sqrt(G.size());j++){
            if(vtxFinals[i*sqrt(G.size())+j] )cout<<"O";
            else cout<<".";
        }
        cout<<endl;
    }
    cout << endl;
}

//Cost: lineal en O(n+m)
//Explicació: s'imprimeix el graf en forma de llistes d'adjacencia
void imprimirG(Graf& G, vector<bool>& vtxFinals){
    cout << "Graf G:"<<endl;
    for(int i=0; i<G.size();i++){
        if(vtxFinals[i]){
            cout<<i<<":";
            for(node v:G[i]){
                if(v.final and vtxFinals[v.id])cout<<" "<<v.id;
            }
            cout<<endl;
        }
    }
    cout<<endl;
}

//Cost: lineal en les arestes de u -> Cas pitjor: O(n)
//Explicació: dfs recursiu on es marquen els nodes visitats
void dfsCC(Graf& G, vector<bool>& vtxFinals, vector<bool>& visitat, int& u){

    visitat[u] = true;
    for(auto v:G[u]){
        if(v.final and vtxFinals[v.id] and not visitat[v.id]){
            dfsCC(G, vtxFinals, visitat, v.id);
        }
    }
}

//Cost: lineal en O(n+m)
//Explicació: troba el numero de components connexes del graf
int nombreCC(Graf& G, vector<bool>& vtxFinals){
    int cont = 0;
    vector<bool> visitat(G.size(),false);
    for(int u = 0; u < G.size(); u++){
        if(vtxFinals[u] and not visitat[u]){
            dfsCC(G, vtxFinals, visitat, u);
            cont++;
        }
    }
    return cont;
}

//Cost: en cas pitjor O(nm+m^2) -> O(nm) on n = |V|
//Explicació: assignar els punter 'dup' per cadascun dels nodes. Per cada aresta, s'assigna el punter al node de l'altre costat de l'adjacencia
void enllacaNodes(Graf& G){
    int n = G.size();
    for(int u = 0; u < n; ++u){
        for(int v=0; v < G[u].size(); v++){
            int nodeV = G[u][v].id;
            if(G[u][v].dup == nullptr) {
                for (int w = 0; w < G[nodeV].size(); w++) {
                    if (G[nodeV][w].id == u) {
                        G[u][v].dup = &G[nodeV][w];
                        G[nodeV][w].dup = &G[u][v];
                    }
                }
            }
        }
    }
}

//Cost: O(n+m) per la part d'introduir els nodes + la part de enllacarNodes
//Explicació: serveix per entrar un graf manualment
Graf entrarGraf(){
    int n;
    cin>>n;
    Graf G(n);
    for(int u = 0; u < n; ++u){
        int adj;
        cin >> adj;
        LListaAdjacencia llist(adj);
        for(int i = 0; i < adj; i++){
            int v;
            cin >> v;
            node node;
            node.inicialitzat = false;
            node.final = true;
            node.id = v;
            node.dup = nullptr;
            llist[i] = node;
        }
        G[u] = llist;
    }

    enllacaNodes(G);

    return G;
}

//Cost:
//  En funcio de n -> n^2 per omplir les adjacencies + enllacaNodes
//  En funcio de nodes (n*n) -> O(|v|) per omplir adjacencies + enllacaNodes
//Explicació: crea el graf en forma de graella NxN
Graf crearGraella(const int& n) {
    Graf graella(n * n);
    for (int u = 0; u < n * n; u++) {
        LListaAdjacencia llista;
        if (u % n != n - 1) {
            node node;
            node.dup = nullptr;
            node.id = u + 1;
            node.final = true;
            node.inicialitzat = false;
            llista.push_back(node);
        }
        if (u % n != 0) {
            node node;
            node.dup = nullptr;
            node.id = u - 1;
            node.final = true;
            node.inicialitzat = false;
            llista.push_back(node);
        }
        if (u >= n) {
            node node;
            node.dup = nullptr;
            node.id = u - n;
            node.final = true;
            node.inicialitzat = false;
            llista.push_back(node);
        }
        if (n * n - u > n) {
            node node;
            node.dup = nullptr;
            node.id = u + n;
            node.final = true;
            node.inicialitzat = false;
            llista.push_back(node);
        }
        graella[u] = llista;
    }
    enllacaNodes(graella);
    return graella;
}

//Cost: constant O(1)
//Explicació: retorna la distancia entre dos punts (x,y)
double dist(pair<double, double>& p1, pair<double, double>& p2){
    return sqrt(pow(p1.first - p2.first, 2)+pow(p1.second-p2.second, 2));
}

//Cost: lineal en n per assignar posicions + quadratic en n per crear el graf + enllacaNodes --> O(n^2)
//Explicació: genera un graf geometric aleatori a partir d'anar generant posicions en el rang dels punts (0,0)
//fins (1,1) on dos vertexs son adjacents si la distancia entre ells dos es menor o igual que la r donada
Graf generaGrafGeomAleatori(int n, double r){
    vector<pair<double, double>> nodes(n);
    for(int i = 0; i < n; ++i){
        nodes[i].first = rand()/double(RAND_MAX);
        nodes[i].second = rand()/double(RAND_MAX);
    }
    Graf G(n);
    for(int u = 0; u < n; ++u){
        LListaAdjacencia llista;
        for(int v = 0; v < n; ++v){
            if(u!=v and dist(nodes[u],nodes[v]) <= r) {
                node node;
                node.dup = nullptr;
                node.id = v;
                node.final = true;
                node.inicialitzat = false;
                llista.push_back(node);
            }
        }
        G[u] = llista;
    }
    enllacaNodes(G);
    return G;
}

void canviaPuntsPerComes(string& s){
    for(int i = 0; i<s.size(); ++i){
        if(s[i] == '.') s[i] = ',';
    }
}

int main() {
    int n, nGrafs;
    double q = 0, iteracions = 20, r = 0;
    string percolacio, tipusGraf;

    cout<<"Entra la mida n:"<<endl;
    cin>>n;

    cout<<"Entra el nombre de grafs: "<<endl;
    cin>>nGrafs;

    cout<<"Percolacio arestes (A) o Percolacio Nodes(N)? "<<endl;
    cin>>percolacio;
    while(percolacio != "A" and percolacio != "N"){
        cout<<"Error en input.\nPercolacio arestes (A) o Percolacio Nodes(N)? "<<endl;
        cin>>percolacio;
    }

    cout<<"Graf Geometric Aleatori (A) o Graella Quadrada (Q)?"<<endl;
    cin>>tipusGraf;
    while(tipusGraf != "A" and tipusGraf != "Q"){
        cout<<"Error en input.\nGraf Geometric Aleatori (A) o Graella Quadrada (Q)?"<<endl;
        cin>>tipusGraf;
    }
    //Generem graf geometric aleatori, fem que vagi per totes les q amb salts de 2/iteracions i per cada q fa totes les r amb salts de 1/iteracions
    if(tipusGraf=="A"){
        for(int iter = 0; iter <= int(iteracions)/2; ++iter){
            cout << "Per q = " << q << ":" << endl;
            r=0;
            for(int it = 0; it <= int(iteracions); ++it){
                double totalCC = 0;
                for(int i = 0; i < nGrafs; i++){
                    Graf G = generaGrafGeomAleatori(n, r);
                    vector<bool> vtxFinals(n*n,true);

                    if (percolacio == "A") percolacioArestes(G,q);
                    else vtxFinals = percolacioNodes(G,q);

                    int ncc = nombreCC(G,vtxFinals);
                    totalCC += ncc;
                }
                double res = (totalCC/nGrafs)/n;
                string text = to_string(res);
                canviaPuntsPerComes(text);
                cout << text << endl;

                r += double(1/iteracions);
            }
            q += double(2/iteracions);
        }
    }
    //Fem l'experiment de les graelles NxN, analitzem per cada q amb salts de 1/iteracions
    else{
            for(int it = 0; it <= int(iteracions); ++it){
                double totalCC = 0;
                for(int i = 0; i < nGrafs; i++){
                    Graf G = crearGraella(n);
                    vector<bool> vtxFinals(n*n,true);

                    if (percolacio == "A") percolacioArestes(G,q);
                    else vtxFinals = percolacioNodes(G,q);

                    int ncc = nombreCC(G,vtxFinals);
                    totalCC += ncc;
                }
                double res = (totalCC/nGrafs);
                string text = to_string(res);
                canviaPuntsPerComes(text);
                cout << text << endl;

                q += double(1/iteracions);
            }
    }
}
