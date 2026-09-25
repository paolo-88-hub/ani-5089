#include <iostream>
#include <iomanip>
#include <vector>

struct MesureParallaxe
{
    double distanceDoigt;
    double deplacementMetres;
    double deplacementCm;
};

double calculerDeplacementApparent(double b, double d, double D)
{
    return b * (D - d) / d;
}

int main()
{
    const double b = 0.063;
    const double D = 4.0;

    std::vector<double> distances = {0.30, 1.00, 3.00};
    std::vector<MesureParallaxe> resultats;

    for (double d : distances)
    {
        double dx = calculerDeplacementApparent(b, d, D);
        resultats.push_back({d, dx, dx * 100.0});
    }

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "--- Mesures du deplacement apparent (Parallaxe) ---\n";
    std::cout << "Ecart pupillaire (b) : " << b * 100.0 << " cm\n";
    std::cout << "Distance du mur (D)  : " << D << " m\n\n";

    for (size_t i = 0; i < resultats.size(); ++i)
    {
        std::cout << "Mesure " << (i + 1) << " (Distance = " << resultats[i].distanceDoigt << " m) : "
                  << "Deplacement apparent = " << resultats[i].deplacementCm << " cm ("
                  << std::setprecision(3) << resultats[i].deplacementMetres << " m)\n"
                  << std::setprecision(1);
    }

    std::cout << "\n--- Ce que ces mesures annoncent du Chapitre 9 ---\n";
    std::cout << "1. Disparite stereoscopique : Le deplacement apparent mesure directement la disparite binoculaire.\n";
    std::cout << "2. Sensibilite a la profondeur : La precision stéréoscopique decroit rapidement avec la distance (non-lineaire).\n";
    std::cout << "3. Calibration VR/AR : Le chapitre 9 exploite cette geometrie stéréoscopique pour synthetiser des paires d'images 3D.\n";

    return 0;
}