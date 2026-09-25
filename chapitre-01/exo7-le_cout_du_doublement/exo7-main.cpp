#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstdlib>

using namespace std;
using namespace chrono;

void rendu()
{

    system("cls");

    for (int i = 0; i < 20; i++)
    {
        cout << "                    *" << endl;
    }
}

int main()
{

    const int NOMBRE_MESURES = 1000;
    const double LIMITE_MS = 11.0;

    double somme = 0.0;

    cout << "Mesure du temps de rendu..." << endl;

    for (int i = 0; i < NOMBRE_MESURES; i++)
    {

        auto debut = high_resolution_clock::now();

        rendu();

        auto fin = high_resolution_clock::now();

        duration<double, milli> duree = fin - debut;

        somme += duree.count();
    }

    double tempsRendu = somme / NOMBRE_MESURES;

    double renduDeuxFois = tempsRendu * 2.0;

    double tempsRestant = LIMITE_MS - renduDeuxFois;

    cout << fixed << setprecision(2);

    cout << endl;
    cout << "========================================" << endl;
    cout << "             RESULTATS" << endl;
    cout << "========================================" << endl;

    cout << "Nombre de mesures : "
         << NOMBRE_MESURES << endl;

    cout << "Temps moyen d'un rendu : "
         << tempsRendu << " ms" << endl;

    cout << "Estimation du rendu deux fois : "
         << renduDeuxFois << " ms" << endl;

    cout << "Temps restant sur 11 ms : "
         << tempsRestant << " ms" << endl;

    cout << "========================================" << endl;

    if (tempsRestant > 0)
    {

        cout << "Conclusion : il reste "
             << tempsRestant
             << " ms pour le reste du programme."
             << endl;
    }
    else
    {

        cout << "Conclusion : le double rendu depasse "
             << "la limite de 11 ms."
             << endl;

        cout << "Il faut reduire le cout du rendu."
             << endl;
    }

    return 0;
}