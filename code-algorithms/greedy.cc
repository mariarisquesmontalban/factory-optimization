/* ---------------------------------GREEDY-------------------------------------
 Autors: Elena González Rodríguez i Maria Risques Montalban
*/
/*--------------------------------LLIBRERIES------------------------------------
*/
#include <climits>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

/*--------------------------------VARIABLES-------------------------------------
GENERALS:
- M: nombre enter de millores
- C: nombre enter de cotxes
- K: nombre enter de classes
- ce: vector d'enters amb el nombre de cotxes que es poden millorar per cada
    finestra de les millores sense penalitzar
- ne: vector d'enters amb les mides de les finestres de les millores 
- necessita_millora: matriu d'enters de les millores que necessita cada classe.
    Posant 1 si necessita aquella millora i 0 en cas contrari.
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
- i: nombre enter de la posició actual en la qual estem afegint un cotxe
- sol: vector amb la permutació que estem generant
- millores_finestra_x_estacio: vector amb el número de cotxes que necessiten 
    millora per cada finestres d'una millora en un moment donat
- millores_comu: matriu d'enters amb les millores en comú entre dues classes
- millores: vector d'enters amb la quantitat de millores que necessita cada classe

*/

/*-----------------------------------STRUCT------------------------------------
*/
struct dades{
    int C, M, K;
    vector<int> ce;
    vector<int> ne;
    vector<int> num_cotxes_x_classe;
    vector<vector<bool>> necessita_millora;
};

/*---------------------------------FUNCIONS------------------------------------*/

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

    out << penalitzacio_total << " " << float(clock())/CLOCKS_PER_SEC  << endl;
    for (int i = 0; i < gen.size(); ++i) {
        out << gen[i] << ' ';
    }
    out << endl;
    out.close();
}

int penalitzacio(vector<int>& sol, int k, vector<int>& millores_finestra_x_estacio, const dades& dades_entrada){
     /* Calcula i retorna la penalització que suma el cotxe afegit a la
    penalització de la permutació generada fins el moment. */

    int pen_cotxe = 0;
    int num_estacions = dades_entrada.ne.size();
    for (int i = 0; i<num_estacions; ++i){
        int mida_finestra = dades_entrada.ne[i];

        // finestres finals
        if (k==sol.size()-1){
            if (dades_entrada.necessita_millora[sol[k]][i]) ++millores_finestra_x_estacio[i];
            for (int j = 0; j < mida_finestra; ++j){
                if (dades_entrada.necessita_millora[sol[k - mida_finestra + j]][i]) 
                    --millores_finestra_x_estacio[i];
                if (millores_finestra_x_estacio[i]-dades_entrada.ce[i]>0) 
                    pen_cotxe += (millores_finestra_x_estacio[i]-dades_entrada.ce[i]);
            }
        // finestres del principi i mig
        } else {
            if (k>mida_finestra-1 and dades_entrada.necessita_millora[sol[k-mida_finestra]][i]) 
                --millores_finestra_x_estacio[i];
            if (dades_entrada.necessita_millora[sol[k]][i]) 
                ++millores_finestra_x_estacio[i];
            if (millores_finestra_x_estacio[i]-dades_entrada.ce[i]>0) 
                pen_cotxe += (millores_finestra_x_estacio[i]-dades_entrada.ce[i]);
        }
    }
    return pen_cotxe;
}

bool queden_mes_classes(int cotxe_anterior, const dades& dades_entrada){
    /* Retorna "true" si queda alguna classe amb cotxes 
    diferent a l'anterior i "false" altrament */

    for (int k = 0; k<dades_entrada.K; ++k){
        if (cotxe_anterior != k){
            if (dades_entrada.num_cotxes_x_classe[k] != 0){
                return true;
            }
        }
    }
    return false;
}

vector<int> omplir_millores(const dades& dades_entrada){
    /* Retorna un vector d'enters amb el nombre de millores que necessita 
    cada classe. */

    vector<int> millores(dades_entrada.K,0);
    for (int j = 0; j<dades_entrada.K; ++j){
        for (int i = 0; i<dades_entrada.M; ++i){
            if (dades_entrada.necessita_millora[j][i]==1) ++millores[j];  
        }
    }
    return millores;
}

vector<vector<int>> omplir_millores_comu(const dades& dades_entrada) {
    /* Retorna una matriu d'enters amb les millores en comú entre 
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

void classe_cotxe(vector<int>& sol, int i, vector<int>& millores_finestra_x_estacio, 
                int& pen_min, dades& dades_entrada, const vector<int>& millores, 
                const vector<vector<int>>& millores_comu) {
    /* Afegeix al vector "sol" la millor classe per aquella posició i actualitza la 
    penalització. */

    int classe = -1;
    pen_min = INT_MAX;
    for (int j = 0; j < dades_entrada.K; ++j){
        vector<int> millores_finestra_x_estacio_copia = millores_finestra_x_estacio;
        sol[i] = j;
        // si queden cotxes d'aquella classe
        if (dades_entrada.num_cotxes_x_classe[j] > 0) {
            // si és diferent a la classe anterior o és igual però no queden més cotxes d'altres classes
            if ( (j == sol[i-1] and not queden_mes_classes(sol[i-1], dades_entrada)) or j != sol[i-1]){
                int pen_cotxe = penalitzacio(sol, i, millores_finestra_x_estacio_copia, dades_entrada);
                // desempat1: el que tingui penalització mínima
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

void primer_cotxe(vector<int>& sol, dades& dades_entrada){
    /* Calcula i afegeix el primer cotxe. */

    int index_max =  -1;
     int num_cotxe_x_classe_max = 0;
    for (int i = 0; i< dades_entrada.K; ++i){
        if (dades_entrada.num_cotxes_x_classe[i]> num_cotxe_x_classe_max){
            index_max = i;
            num_cotxe_x_classe_max = dades_entrada.num_cotxes_x_classe[i];
        }
    }
    sol[0] = index_max;
    --dades_entrada.num_cotxes_x_classe[sol[0]];
}

void greedy(char **argv, vector<int>& sol, vector<int>& millores_finestra_x_estacio,
             dades& dades_entrada) {
    /* FUNCIÓ GREEDY: crea la permutació amb la millor distribució de les classes, 
    per tal d'obtenir la menor penalització. */

    vector<int> millores = omplir_millores(dades_entrada);
    vector<vector<int>> millores_comu = omplir_millores_comu(dades_entrada);

    primer_cotxe(sol, dades_entrada);

    int pen = penalitzacio(sol, 0, millores_finestra_x_estacio, dades_entrada);
    int pen_min;
    for (int i = 1; i < dades_entrada.C; ++i) {
        classe_cotxe(sol, i, millores_finestra_x_estacio, pen_min, dades_entrada, millores, millores_comu);
        pen+=pen_min;
    }

    escriure(sol, argv, pen);
}

int main(int argc, char** argv) {
    dades dades_entrada = lectura_dades(argc, argv);
    vector<int> sol(dades_entrada.C);
    vector<int> millores_finestra_x_estacio(dades_entrada.M,0);
    greedy(argv, sol, millores_finestra_x_estacio, dades_entrada);
}