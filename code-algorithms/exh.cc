/* -----------------------------CERCA EXHAUSTIVA--------------------------------
 Autors: Elena González Rodríguez i Maria Risques Montalban
*/
/*--------------------------------LLIBRERIES------------------------------------
*/
#include <algorithm>
#include <climits>
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
    finestra de cada millora sense penalitzar
- ne: vector d'enters amb les mides de les finestres de cada millora
- necessita_millora: matriu d'enters de les millores que necessita cada classe.
    Posant 1 si necessita aquella millora i 0 en cas contrari.
- num_cotxes_x_classe: vector d'enters amb el nombre de cotxes per classe
- dades_entrada: struct de tipus dades amb totes les dades d'entrada


LOCALS:
- penalitzacio: nombre enter amb la penalització fins el moment de la permutació
- penalitzacio_min: nombre enter amb la penalització de la millor permutació
    fins el moment
- k: nombre enter de la posició actual en la qual estem afegint un cotxe
- gen: vector amb les permutacions
- millores_finestra_x_estacio: vector amb el numero de cotxes que necessiten 
    millora per cada finestres d'una millora en un moment donat
- cotxes_a_millorar: nombre enter de cotxes que necessiten millora en una estació
- num_finestres_no_solapen: nombre enter de finestres que no se solapen en
    la nostra permutació

*/
/*----------------------------------STRUCT--------------------------------------
*/
struct dades {
    int C, M, K;
    vector<int> ce;
    vector<int> ne;
    vector<int> num_cotxes_x_classe;
    vector<vector<int>> necessita_millora;
};

/*
-----------------------------------FUNCIONS-------------------------------------
*/
dades lectura_dades(int argc, char** argv)
{

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
    vector<vector<int>> necessita_millora(K, vector<int>(M));
    for (int i = 0; i < K; ++i) {
        fitxer_entrada >> i >> num_cotxes_x_classe[i];

        for (int j = 0; j < M; ++j) {
            fitxer_entrada >> necessita_millora[i][j];
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

void escriure(const vector<int>& gen, char** argv, int penalitzacio)
{
    ofstream out(argv[2]);

    // precisió d'un decimal
    out.setf(ios::fixed);
    out.precision(1);

    out << penalitzacio << " " << double(clock()) / CLOCKS_PER_SEC << endl;
    for (int i = 0; i < gen.size(); ++i) {
        out << gen[i] << " ";
    }
    out << endl;
    out.close();
}

int penalitzacio_cotxe_afegit(vector<int>& gen, int k, const dades& dades_entrada,
    vector<int>& millores_finestra_x_estacio)
{
    /* Calcula i retorna la penalització que suma el cotxe afegit a la
    penalització total de la permutació generada fins el moment. */

    int penalitzacio_cotxe_afegit = 0;
    int num_estacions = dades_entrada.ne.size();

    for (int i = 0; i < num_estacions; ++i) {
        int mida_finestra = dades_entrada.ne[i];

        // finestres finals
        if (k == gen.size() - 1) {
            if (dades_entrada.necessita_millora[gen[k]][i] == 1)
                ++millores_finestra_x_estacio[i];
            for (int j = 0; j < mida_finestra; ++j) {
                if (dades_entrada.necessita_millora[gen[k - mida_finestra + j]][i] == 1)
                    --millores_finestra_x_estacio[i];
                if (millores_finestra_x_estacio[i] - dades_entrada.ce[i] > 0)
                    penalitzacio_cotxe_afegit += (millores_finestra_x_estacio[i] - dades_entrada.ce[i]);
            }
        // finestres del principi i mig
        } else {
            if (k > mida_finestra - 1 and dades_entrada.necessita_millora[gen[k - mida_finestra]][i] == 1)
                --millores_finestra_x_estacio[i];
            if (dades_entrada.necessita_millora[gen[k]][i] == 1)
                ++millores_finestra_x_estacio[i];
            if (millores_finestra_x_estacio[i] - dades_entrada.ce[i] > 0)
                penalitzacio_cotxe_afegit += (millores_finestra_x_estacio[i] - dades_entrada.ce[i]);
        }
    }
    return penalitzacio_cotxe_afegit;
}

void desfer_canvis_millores(vector<int>& gen, int k, const dades& dades_entrada,
    vector<int>& millores_finestra_x_estacio)
{
    /* Desfà els canvis fets en la funció "penalitzacio_cotxe_afegit" per
    deixar-ho preparat per a la següent permutació. */

    int num_estacions = dades_entrada.ne.size();
    for (int i = 0; i < num_estacions; ++i) {
        int mida_finestra = dades_entrada.ne[i];
        if (k == gen.size() - 1) {
            if (dades_entrada.necessita_millora[gen[k]][i] == 1)
                --millores_finestra_x_estacio[i];
            for (int j = 0; j < mida_finestra; ++j) {
                if (dades_entrada.necessita_millora[gen[k - mida_finestra + j]][i] == 1)
                    ++millores_finestra_x_estacio[i];
            }
        } else {
            if (k > mida_finestra - 1 and dades_entrada.necessita_millora[gen[k - mida_finestra]][i] == 1)
                ++millores_finestra_x_estacio[i];
            if (dades_entrada.necessita_millora[gen[k]][i] == 1)
                --millores_finestra_x_estacio[i];
        }
    }
}

int prediccio_penalitzacio(const dades& dades_entrada, int k)
{
    /* Calcula i retorna una predicció de la mínima penalització que es
    necessitarà per acabar la permutació. */

    int penalitzacio = 0;
    for (int j = 0; j < dades_entrada.M; ++j) {
        int cotxes_a_millorar = 0;
        for (int i = 0; i < dades_entrada.K; ++i) {
            // càlcul del nombre de cotxes que necessiten millora en una estació
            cotxes_a_millorar += dades_entrada.necessita_millora[i][j] * dades_entrada.num_cotxes_x_classe[i];
        }
        // càlcul del nombre de finestres que no se solapen arrodonint cap a dalt
        int num_finestres_no_solapen = ((dades_entrada.C - k + 1) % dades_entrada.ne[j] == 0 ? (dades_entrada.C - k + 1) / dades_entrada.ne[j] : (dades_entrada.C - k + 1) / dades_entrada.ne[j] + 1);

        // càlcul del nombre màxim de cotxes que es poden afegir sense penalització
        // (nombre de finestres que no se solapen pel nombre de cotxes que es poden
        // tenir en una finestra sense penalitzar)
        int num_max_cotxes_sense_penalitzacio = num_finestres_no_solapen * dades_entrada.ce[j];

        // si passa això tindrem penalització, per tant, la calculem
        if (cotxes_a_millorar > num_max_cotxes_sense_penalitzacio) {
            int cotxes_que_penalitzen = cotxes_a_millorar - num_max_cotxes_sense_penalitzacio;
            penalitzacio = cotxes_que_penalitzen * dades_entrada.ne[j];
        }
    }
    return penalitzacio;
}

void cerca_exaustiva(char** argv, int k, vector<int>& gen, dades& dades_entrada,
    vector<int>& millores_finestra_x_estacio, int& penalitzacio_min,
    int penalitzacio, int classe)
{
    /* Genera totes les permutacions possibles amb les restriccions demanades i
    troba la solució amb la menor penalització. */

    // si la penalització fins el moment és superior a la mínima trobada
    if (penalitzacio >= penalitzacio_min)
        return;

    // si la penalització més la predicció de la penalització per acabar la
    // permutació és superior a la penalització mínima trobada fins al moment
    if (penalitzacio + prediccio_penalitzacio(dades_entrada, k) > penalitzacio_min)
        return;

    if (k == dades_entrada.C) {
        escriure(gen, argv, penalitzacio);
        penalitzacio_min = penalitzacio;
        return;
    } else {
        for (int i = 0; i < dades_entrada.K; ++i) {
            // comencem a buscar la solució per la classe amb el major nombre de cotxes
            // i explorem l'arbre de cerca de la manera més alternada possible
            if (dades_entrada.num_cotxes_x_classe[(classe + k + i) % dades_entrada.K] > 0) {
                gen[k] = (i + k + classe) % dades_entrada.K;
                --dades_entrada.num_cotxes_x_classe[gen[k]];
                cerca_exaustiva(argv, k + 1, gen, dades_entrada,
                    millores_finestra_x_estacio, penalitzacio_min,
                    penalitzacio + penalitzacio_cotxe_afegit(gen, k, 
                    dades_entrada, millores_finestra_x_estacio), classe);
                desfer_canvis_millores(gen, k, dades_entrada, millores_finestra_x_estacio);
                ++dades_entrada.num_cotxes_x_classe[gen[k]];
            }
        }
    }
}

int classe_amb_mes_cotxes(const dades& dades_entrada){
    /* Calcula la classe amb més cotxes */
    
    int classe = 0;
    for (int j = 1; j < dades_entrada.K; ++j) {
        if (dades_entrada.num_cotxes_x_classe[j] > dades_entrada.num_cotxes_x_classe[classe])
            classe = j;
    }
    return classe;
}

int main(int argc, char** argv)
{
    dades dades_entrada = lectura_dades(argc, argv);

    vector<int> gen(dades_entrada.C);
    vector<int> millores_finestra_x_estacio(dades_entrada.ce.size(), 0);
    int penalitzacio_min = INT_MAX;
    
    // calculem la classe per la qual començarem
    int classe = classe_amb_mes_cotxes(dades_entrada);
    
    cerca_exaustiva(argv, 0, gen, dades_entrada, millores_finestra_x_estacio,
        penalitzacio_min, 0, classe);
}
