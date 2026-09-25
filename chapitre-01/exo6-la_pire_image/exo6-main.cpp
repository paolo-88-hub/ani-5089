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

        auto debut = high_resolution_clock::now();

        system("cls");

        cout << "Image : " << i + 1 << endl;

        for (int j = 0; j < 20; j++)
        {
            cout << "                    *" << endl;
        }

        auto fin = high_resolution_clock::now();

        duration<double, milli> duree = fin - debut;

        double dureeMs = duree.count();

        if (dureeMs > plusLongueImage)
        {
            plusLongueImage = dureeMs;
        }

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