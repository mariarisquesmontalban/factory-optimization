#include <algorithm>
#include <climits>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

/*--------------------------------VARIABLES-------------------------------------
GENERALS:
- M: nombre enter de millores
- C: nombre enter de cotxes
- K: nombre enter de classes
- ce: vector d'enters amb el nombre de cotxes que es poden millorar per cada
    finestra de les millores sense penalitzar
- ne: vector d'enters amb les mides de les finestres de les millores 
- necessita_millora: matriu d'enters de les millores que necessita cada classe
    Posant 1 si necessita aquella millora i 0 en cas contrari
- num_cotxes_x_classe: vector d'enters amb el nombre de cotxes per classe
- dades_entrada: struct de tipus dades amb totes les dades d'entrada


LOCALS:
- pen_cotxe: nombre enter amb la penalització que suposa afegir 
    una classe concreta
- pen_min: nombre enter amb la penalització que suposa afegir un cotxe 
    de la millor classe per afegir en aquell moment
- pen: nombre enter amb la penalització de la permutació generada fins
    el moment
- classe: nombre enter amb la millor classe que es pot afegir per aquella 
    posició
- classes: vector d'enters amb les millores classes candidates a posar
- i: nombre enter de la posició actual en la qual estem afegint un cotxe
- sol: vector amb la permutació que estem generant
- millores_finestra_x_estacio: vector amb el número de cotxes que necessiten 
    millora per cada finestres d'una millora en un moment donat
- millores_comu: matriu d'enters amb les millores en comú entre dues classes
- millores: vector d'enters amb la quantitat de millores que necessita cada classe
- p: double amb probabilitat amb la qual agafarem agafarem una solució pitjor
    de la que teniem
- T: nombre double, fixat a 100 experimental, que representa la temperatura
- alpha: nombre double, fixada a 0.7, amb el qual anirem modificant la 
    temperatura de forma experimental
- penalitzacio_greedy_inicial: nombre enter amb la penalització del greedy 
    randomitzat inicialment
- penalitzacio_local_min: nombre enter amb la menor penalització del veinat 
    d'una permutació
- penalitzacio_greedy: nombre enter amb la penalització mínima que obtindrem 
    començant per un greedy random concret
- penalitzacio_millor: nombre enter amb la millor penalització trobada
- actual: vector d'enters sobre el qual farem els "swaps"
- millor_solucio_particular: vector d'enters amb la millor permutació del veïnat
    de la permutació actual
- millor_solucio: vector d'enters amb la millor solució que obtindrem començant 
    per un greedy randomitzat concret
- millor_solucio_final: vector d'enters amb la millor solució trobada

*/

/*-------------------------------------------------STRUCT-----------------------------------------------
*/
struct dades {
    int C, M, K;
    vector<int> ce;
    vector<int> ne;
    vector<int> num_cotxes_x_classe;
    vector<vector<bool>> necessita_millora;
};

/*------------------------------------------------FUNCIONS----------------------------------------------*/

dades lectura_dades(int argc, char** argv){

    if (argc != 3) {
        cout << "Syntax: " << argv[0] << " input_file1 outputfile" << endl;
        exit(1);
    }
    ifstream fitxer_entrada(argv[1]);
    int C, M, K;
    fitxer_entrada >> C >> M >> K;
    vector<int> ce(M);
    for (int i = 0; i < M; ++i) {
        fitxer_entrada >> ce[i];
    }
    vector<int> ne(M);
    for (int i = 0; i < M; ++i) {
        fitxer_entrada >> ne[i];
    }
    vector<int> num_cotxes_x_classe(K);
    vector<vector<bool>> necessita_millora(K, vector<bool>(M));
    for (int i = 0; i < K; ++i) {
        fitxer_entrada >> i >> num_cotxes_x_classe[i];

        for (int j = 0; j < M; ++j) {
            int bolea;
            fitxer_entrada >> bolea;
            if (bolea == 0)
                necessita_millora[i][j] = false;
            else
                necessita_millora[i][j] = true;
        }
    }
    fitxer_entrada.close();

    // assignació dels valors a les variables de la struct
    dades dades_entrada;
    dades_entrada.C = C;
    dades_entrada.M = M;
    dades_entrada.K = K;
    dades_entrada.ce = ce;
    dades_entrada.ne = ne;
    dades_entrada.num_cotxes_x_classe = num_cotxes_x_classe;
    dades_entrada.necessita_millora = necessita_millora;

    return dades_entrada;
}

void escriure(const vector<int>& gen, char** argv, int penalitzacio_total){

    ofstream out(argv[2]);

    // precisió d'un decimal
    out.setf(ios::fixed);
    out.precision(1);
    
    out << penalitzacio_total << " " << float(clock()) / CLOCKS_PER_SEC << endl;
    for (int i = 0; i < gen.size(); ++i) {
        out << gen[i] << ' ';
    }
    out << endl;
    out.close();
}

int penalitzacio(vector<int>& sol, int k, vector<int>& millores_finestra_x_estacio, const dades& dades_entrada) {

    /* Calcula i retorna la penalització que suma el cotxe afegit a la
    penalització de la permutació generada fins el moment. */

    int pen_cotxe = 0;
    int num_estacions = dades_entrada.ne.size();
    for (int i = 0; i < num_estacions; ++i) {
        int mida_finestra = dades_entrada.ne[i];
        // finestres finals
        if (k == sol.size() - 1) {
            if (dades_entrada.necessita_millora[sol[k]][i])
                ++millores_finestra_x_estacio[i];
            for (int j = 0; j < mida_finestra; ++j) {
                if (dades_entrada.necessita_millora[sol[k - mida_finestra + j]][i])
                    --millores_finestra_x_estacio[i];
                if (millores_finestra_x_estacio[i] - dades_entrada.ce[i] > 0)
                    pen_cotxe += (millores_finestra_x_estacio[i] - dades_entrada.ce[i]);
            }
            // finestres del principi i mig
        } else {
            if (k > mida_finestra - 1 and dades_entrada.necessita_millora[sol[k - mida_finestra]][i])
                --millores_finestra_x_estacio[i];
            if (dades_entrada.necessita_millora[sol[k]][i])
                ++millores_finestra_x_estacio[i];
            if (millores_finestra_x_estacio[i] - dades_entrada.ce[i] > 0)
                pen_cotxe += (millores_finestra_x_estacio[i] - dades_entrada.ce[i]);
        }
    }
    return pen_cotxe;
}

vector<int> omplir_millores(const dades& dades_entrada){

    /* Retorna un vector d'enters amb el nombre de millores que necessita
    cada classe. */

    vector<int> millores(dades_entrada.K, 0);
    for (int j = 0; j < dades_entrada.K; ++j) {
        for (int i = 0; i < dades_entrada.M; ++i) {
            if (dades_entrada.necessita_millora[j][i] == 1)
                ++millores[j];
        }
    }
    return millores;
}

vector<vector<int>> omplir_millores_comu(const dades& dades_entrada){

    /* Retorna una matriu d'enters amb les millores amb comú entre
    dues classes. */

    vector<vector<int>> millores_comu(dades_entrada.K, vector<int>(dades_entrada.K));
    for (int i = 0; i < dades_entrada.K; i++) {
        for (int j = 0; j < dades_entrada.K; j++) {
            for (int k = 0; k < dades_entrada.M; ++k)
                if (dades_entrada.necessita_millora[i][k] == dades_entrada.necessita_millora[j][k])
                    ++millores_comu[i][j];
        }
    }
    return millores_comu;
}

void primer_cotxe(vector<int>& sol, dades& dades_entrada){

    /* Calcula i afegeix el primer cotxe. */

    int index_max = -1;
    int num_cotxe_x_classe_max = 0;
    for (int i = 0; i < dades_entrada.K; ++i) {
        if (dades_entrada.num_cotxes_x_classe[i] > num_cotxe_x_classe_max) {
            index_max = i;
            num_cotxe_x_classe_max = dades_entrada.num_cotxes_x_classe[i];
        }
    }
    sol[0] = index_max;
    --dades_entrada.num_cotxes_x_classe[sol[0]];
}

bool queden_mes_classes(int cotxe_anterior, const dades& dades_entrada){

    /* Retorna "true" si queda alguna classe amb cotxes
    diferent a l'anterior i "false" altrament */

    for (int k = 0; k < dades_entrada.K; ++k) {
        if (cotxe_anterior != k) {
            if (dades_entrada.num_cotxes_x_classe[k] != 0) {
                return true;
            }
        }
    }
    return false;
}

void classe_cotxe1(vector<int>& sol, int i, vector<int>& millores_finestra_x_estacio, 
                int& pen_min, dades& dades_entrada, const vector<int>& millores, 
                const vector<vector<int>>& millores_comu) {
    
    /* Afegeix al vector sol la millor classe per aquella posició i actualitza la 
    penalització. NO n'escull ninguna aleatoriament. */

    int classe = -1;
    pen_min = INT_MAX;
    for (int j = 0; j < dades_entrada.K; ++j){
        vector<int> millores_finestra_x_estacio_copia = millores_finestra_x_estacio;
        sol[i] = j;
        // si queden cotxes d'aquella classe
        if (dades_entrada.num_cotxes_x_classe[j] > 0) {
            // si és diferent a la classe anterior o és igual però no queden més cotxes d'altres classes.
            if ( (j == sol[i-1] and not queden_mes_classes(sol[i-1], dades_entrada)) or j != sol[i-1]){
                int pen_cotxe = penalitzacio(sol, i, millores_finestra_x_estacio_copia, dades_entrada);
                // desempat1: el que tingui penalització mínima.
                if (pen_cotxe < pen_min) {
                    classe = j;
                    pen_min = pen_cotxe;
                }
                else if (pen_cotxe == pen_min) {
                    // desempat2: el que tingui més cotxes 
                    if (dades_entrada.num_cotxes_x_classe[j] > dades_entrada.num_cotxes_x_classe[classe]) {
                        classe = j;
                    } else if (dades_entrada.num_cotxes_x_classe[j] == dades_entrada.num_cotxes_x_classe[classe]) {
                        // desempat3: el que tingui més nombre de millores
                        if (millores[j] > millores[classe]) {
                            classe = j;
                        } else if (millores[j] == millores[classe]){
                            // desempat4: el que tingui més millores en comú amb la classe del cotxe anterior
                            if (millores_comu[sol[i-1]][j]>millores_comu[sol[i-1]][classe]){
                                classe = j;
                            }
                        }
                    }
                }
            }
        }
    }
    --dades_entrada.num_cotxes_x_classe[classe];

    // afegim i actualitzem la penalització de la classe que escollim
    sol[i] = classe;
    pen_min = penalitzacio(sol, i, millores_finestra_x_estacio, dades_entrada);
}


void greedy(char **argv, vector<int>& sol, vector<int>& millores_finestra_x_estacio,
             dades& dades_entrada, int& pen) {
    
    /* FUNCIÓ GREEDY: crea la permutació amb la millor distribució de les classes, 
    per tal d'obtenir la menor penalització. L'utilitzarem en la primera iteració.  */

    vector<int> millores = omplir_millores(dades_entrada);
    vector<vector<int>> millores_comu = omplir_millores_comu(dades_entrada);

    primer_cotxe(sol, dades_entrada);
    pen = penalitzacio(sol, 0, millores_finestra_x_estacio, dades_entrada);
    
    int pen_min;
    for (int i = 1; i < dades_entrada.C; ++i) {
        classe_cotxe1(sol, i, millores_finestra_x_estacio, pen_min, dades_entrada, millores, millores_comu);
        pen+=pen_min;
    }
    escriure(sol, argv, pen);
}


void classe_cotxe2(vector<int>& sol, int i, vector<int>& millores_finestra_x_estacio,
    int& pen_min, dades& dades_entrada){

    /* Afegeix al vector sol un classe escollida a l'atzar entre les millors classes per 
    aquella posició i actualitza la penalització. */

    vector<int> classes;
    pen_min = INT_MAX;
    for (int j = 0; j < dades_entrada.K; ++j) {
        vector<int> millores_finestra_x_estacio_copia = millores_finestra_x_estacio;
        sol[i] = j;
        // si queden cotxes d'aquella classe
        if (dades_entrada.num_cotxes_x_classe[j] > 0) {
            // si és diferent a la classe anterior o és igual però no queden més cotxes d'altres classes.
            if ((j == sol[i - 1] and not queden_mes_classes(sol[i - 1], dades_entrada)) or j != sol[i - 1]) {
                int pen_cotxe = penalitzacio(sol, i, millores_finestra_x_estacio_copia, dades_entrada);
                // desempat1: el que tingui penalització mínima.
                if (pen_cotxe < pen_min) {
                    classes.clear();
                    classes.push_back(j);
                    pen_min = pen_cotxe;
                } else if (pen_cotxe == pen_min) {
                    // desempat2: el que tingui més cotxes
                    if (dades_entrada.num_cotxes_x_classe[j] > dades_entrada.num_cotxes_x_classe[classes[0]]) {
                        classes.clear();
                        classes.push_back(j);
                    } else if (dades_entrada.num_cotxes_x_classe[j] == dades_entrada.num_cotxes_x_classe[classes[0]]) {
                        classes.push_back(j);
                    }
                }
            }
        }
    }

    int classe;
    if (classes.size() == 1) {
    // si la mida del vector classe és 1 vol dir que només hi ha una classe
    // candidata i per tant ens quedem amb aquella
        classe = classes[0];
    } else {
    // altrament, escollim una de les classes candidates a l'atzar
        int size = classes.size();
        int id_rand = rand() % size;
        classe = classes[id_rand];
    }

    // col·loquem la classe i calculem la penalització
    sol[i] = classe;
    pen_min = penalitzacio(sol, i, millores_finestra_x_estacio, dades_entrada);
    --dades_entrada.num_cotxes_x_classe[classe];
}


vector<int> greedy_randomized(char** argv, vector<int>& sol, vector<int>& millores_finestra_x_estacio,
                            dades& dades_entrada, int& pen){

    /* FUNCIÓ GREEDY RANDOMIZED: crea la permutació una mica aleatoritzada amb la millor 
    distribució de les classes, per tal d'obtenir la menor penalització. */
   
    primer_cotxe(sol, dades_entrada);
    pen = penalitzacio(sol, 0, millores_finestra_x_estacio, dades_entrada);
    
    int pen_min;
    for (int i = 1; i < dades_entrada.C; ++i) {
        classe_cotxe2(sol, i, millores_finestra_x_estacio, pen_min, dades_entrada);
        pen += pen_min;
    }

    return sol;
}

vector<int> swap_dos_cotxes(vector<int>& greedy_local, int& penalitzacio_local, int i, 
                        int j, const dades& dades_entrada, vector<int>& millores_finestra_x_estacio) {
    
    /* Intercanvia dos cotxes de posició i calcula quina penalització suposa 
    aquella permutació*/

    vector<int> millores_finestra_x_estacio_copia_2 = millores_finestra_x_estacio;
    
    swap(greedy_local[i], greedy_local[j]);
    
    for (int k = 0; k < dades_entrada.C; ++k) {
        penalitzacio_local += penalitzacio(greedy_local, k, millores_finestra_x_estacio_copia_2, dades_entrada);
    }
    return greedy_local;
}

double prob(double T, int penalitzacio_greedy, int penalitzacio_greedy_inicial) {

    /* Calcula la probabilitat amb la fórmula de la "Distribució Boltzmann" */

    double p = (penalitzacio_greedy - penalitzacio_greedy_inicial) / T;
    double e = exp(p);
    return e;
}

void updateT(double& T) {

    /* Actualitza la T */

    double alpha = 0.7;
    T *= alpha;
}

void local_search(int& penalitzacio_greedy, vector<int>& millor_solucio, dades& dades_entrada, 
                vector<int>& millores_finestra_x_estacio_copia, int& penalitzacio){
    
    /* LOCAL SEARCH (SIMULATED ANNEALING): per cada permutació mirem el canvi que ens 
    farà tenir la penalització mínima (del veïnat) i ens el quedem. Si al provar tots 
    els possibles intercanvis la penalització és més petita que la penalització mínima 
    ens quedem amb aquella permutació. Altrament, tenim dos opcions, quedar-nos-la o 
    no, i ho farem amb una certa probailitat, que anirà decreixent durant la cerca. */

    int penalitzacio_greedy_inicial = penalitzacio_greedy;
    vector<int> actual = millor_solucio;
    vector<int> greedy_local(dades_entrada.C);
    
    double T = 100;
    double p = INT_MAX;

    // 0.1 trobat experimentalment
    while (p > 0.1) {
            int penalitzacio_local_min = INT_MAX;
            vector<int> millor_solucio_particular(dades_entrada.C);
            for (int i = 0; i < dades_entrada.C; ++i) {
                for (int j = 0; j < dades_entrada.C; ++j) {
                    if (actual[j] != actual[i]) {
                        penalitzacio = 0;
                        greedy_local = swap_dos_cotxes(actual, penalitzacio, i, j, dades_entrada, millores_finestra_x_estacio_copia);
                        if (penalitzacio < penalitzacio_local_min) {
                            penalitzacio_local_min = penalitzacio;
                            millor_solucio_particular = greedy_local;
                        }
                    }
                }
            }
            if (penalitzacio_local_min < penalitzacio_greedy) {
                actual = millor_solucio_particular;
                penalitzacio_greedy = penalitzacio_local_min;
                millor_solucio = millor_solucio_particular;
            } else if (rand() % 100 < prob(T, penalitzacio_local_min, penalitzacio_greedy_inicial)) {
                actual = millor_solucio_particular;
            }
            p = prob(T, penalitzacio_greedy, penalitzacio_greedy_inicial);
            updateT(T);
        }
}

void grasp(char** argv, vector<int>& sol, vector<int>& millores_finestra_x_estacio, dades& dades_entrada, int& penalitzacio, int& penalitzacio_millor, vector<int>& millor_solucio_final, const vector<int>& sol_greedy, bool& primer, int penalitzacio_greedy)
{
    /*GRASP: va generant greedies randomized amb els quals realitza
    una local search fins que la probabiltat de millorar la solució es
    prou petita com per passar al següent greedy */

    vector<int> millores_finestra_x_estacio_copia = millores_finestra_x_estacio;
    vector<int> millor_solucio(sol.size());
    
    if (primer) {
        // si es la primera iteració que provi amb el millor greedy
        millor_solucio = sol_greedy;
        primer = false;
    } else {
        // si no estem en la primera iteració que provi amb un greedy randomized
        millor_solucio = greedy_randomized(argv, sol, millores_finestra_x_estacio, dades_entrada, penalitzacio);
        penalitzacio_greedy = penalitzacio;
    }
    
    local_search(penalitzacio_greedy, millor_solucio, dades_entrada, millores_finestra_x_estacio_copia, penalitzacio);

    if (penalitzacio_greedy < penalitzacio_millor) {
        penalitzacio_millor = penalitzacio_greedy;
        millor_solucio_final = millor_solucio;
        escriure(millor_solucio_final, argv, penalitzacio_greedy);
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));
   
    dades dades_entrada = lectura_dades(argc, argv);
    dades dades_entrada_copia = dades_entrada;
    
    vector<int> sol(dades_entrada.C);
    vector<int> sol_greedy(dades_entrada.C);
    vector<int> millor_solucio_final(dades_entrada.C);

    vector<int> millores_finestra_x_estacio(dades_entrada.M, 0);
    vector<int> millores_finestra_x_estacio_copia = millores_finestra_x_estacio;
    
    int penalitzacio_millor = INT_MAX;
    int penalitzacio_greedy = 0;

    greedy(argv, sol_greedy, millores_finestra_x_estacio_copia, dades_entrada_copia, penalitzacio_greedy);
    
    bool primer = true;
    
    // Que vagi provant infinitament
    while (true) {
        millores_finestra_x_estacio_copia = millores_finestra_x_estacio;
        dades_entrada_copia = dades_entrada;
        int penalitzacio = 0;
        grasp(argv, sol, millores_finestra_x_estacio_copia, dades_entrada_copia, penalitzacio, penalitzacio_millor, millor_solucio_final, sol_greedy, primer, penalitzacio_greedy);
    }
}