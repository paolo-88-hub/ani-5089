#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
    // Distances du doigt par rapport au visage (en cm)
    double distances[3] = {30.0, 100.0, 300.0};

    // Déplacements apparents mesurés (en cm)
    double deplacements[3] = {6.0, 20.0, 60.0};

    cout << fixed << setprecision(2);

    cout << "=== DEPLACEMENT APPARENT DU DOIGT ===" << endl;
    cout << endl;

    for (int i = 0; i < 3; i++)
    {
        cout << "Distance : " << distances[i] << " cm"
             << " -> Deplacement apparent : "
             << deplacements[i] << " cm" << endl;
    }

    // Calcul de la moyenne des déplacements
    double somme = 0;

    for (int i = 0; i < 3; i++)
    {
        somme += deplacements[i];
    }

    double moyenne = somme / 3;

    cout << endl;
    cout << "Moyenne des deplacements : "
         << moyenne << " cm" << endl;

    return 0;
}