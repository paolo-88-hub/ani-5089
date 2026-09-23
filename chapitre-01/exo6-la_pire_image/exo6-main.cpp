#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iomanip>

using namespace std;
using namespace chrono;

int main()
{

    const int NOMBRE_IMAGES = 1000;
    const double SEUIL_MS = 11.0;

    double plusLongueImage = 0.0;
    int nombreImagesDepassant11ms = 0;

    cout << "Mesure de 1000 images..." << endl;

    for (int i = 0; i < NOMBRE_IMAGES; i++)
    {

        // Debut de la mesure
        auto debut = high_resolution_clock::now();

        // Effacement de l'ecran
        system("cls");

        // Petit dessin
        cout << "Image : " << i + 1 << endl;

        for (int j = 0; j < 20; j++)
        {
            cout << "                    *" << endl;
        }

        // Fin de la mesure
        auto fin = high_resolution_clock::now();

        // Calcul de la duree en millisecondes
        duration<double, milli> duree = fin - debut;

        double dureeMs = duree.count();

        // Recherche de l'image la plus longue
        if (dureeMs > plusLongueImage)
        {
            plusLongueImage = dureeMs;
        }

        // Comptage des images depassant 11 ms
        if (dureeMs > SEUIL_MS)
        {
            nombreImagesDepassant11ms++;
        }
    }

    cout << fixed << setprecision(2);

    cout << endl;
    cout << "========================================" << endl;
    cout << "          RESULTATS DE LA MESURE" << endl;
    cout << "========================================" << endl;

    cout << "Nombre total d'images : "
         << NOMBRE_IMAGES << endl;

    cout << "Duree de la plus longue image : "
         << plusLongueImage << " ms" << endl;

    cout << "Nombre d'images depassant 11 ms : "
         << nombreImagesDepassant11ms << endl;

    cout << "========================================" << endl;

    // Conclusion automatique
    if (plusLongueImage <= 11.0 &&
        nombreImagesDepassant11ms == 0)
    {

        cout << "Conclusion : le programme respecte le seuil"
             << endl;
        cout << "de 11 ms sur cette mesure." << endl;
    }
    else
    {

        cout << "Conclusion : certaines images depassent"
             << endl;
        cout << "le seuil de 11 ms." << endl;
    }

    return 0;
}