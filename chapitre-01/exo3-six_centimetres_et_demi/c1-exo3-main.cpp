#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
    double mesures[6] = {6.2, 6.0, 6.4, 6.1, 6.3, 6.2};

    double somme = 0;
    double minimum = mesures[0];
    double maximum = mesures[0];

    for (int i = 0; i < 6; i++)
    {
        somme += mesures[i];

        if (mesures[i] < minimum)
        {
            minimum = mesures[i];
        }

        if (mesures[i] > maximum)
        {
            maximum = mesures[i];
        }
    }

    double moyenne = somme / 6;

    double ecart = maximum - minimum;

    cout << fixed << setprecision(2);

    cout << "=== Mesure de l'ecart entre les pupilles ===" << endl;

    for (int i = 0; i < 6; i++)
    {
        cout << "Personne " << i + 1 << " : "
             << mesures[i] << " cm" << endl;
    }

    cout << endl;
    cout << "Moyenne : " << moyenne << " cm" << endl;
    cout << "Plus petite valeur : " << minimum << " cm" << endl;
    cout << "Plus grande valeur : " << maximum << " cm" << endl;
    cout << "Ecart : " << ecart << " cm" << endl;

    return 0;
}