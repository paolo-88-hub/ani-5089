#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

struct App
{
    std::string nom;
    std::string genre;
    double vection;        // locomotion visuelle continue (0-10)
    double acceleration;   // accélérations/rotations visuelles sans mouvement réel (0-10)
    double rotationForcee; // rotations imposées de la vue (0-10)
    double perteControle;  // absence de contrôle du mouvement (0-10)
    double repereFixe;     // présence d'un repère stable: cockpit, sol, décor fixe (0-10)
};

// Score de risque de cybersickness : plus il est haut, plus l'app rend malade
double score(const App &a)
{
    const double wVection = 0.35;
    const double wAcc = 0.30;
    const double wRot = 0.20;
    const double wCtrl = 0.15;
    const double wRepere = 0.05; // facteur protecteur

    double s = wVection * a.vection + wAcc * a.acceleration + wRot * a.rotationForcee + wCtrl * a.perteControle - wRepere * a.repereFixe;

    return std::max(0.0, s);
}

int main()
{
    std::vector<App> apps = {
        {"Beat Saber", "Rythme", 0, 0, 0, 0, 8},
        {"Half-Life: Alyx", "Action/Aventure", 4, 2, 2, 1, 3},
        {"Epic Roller Coasters", "Simulation passive", 10, 10, 8, 10, 2}};

    // Tri croissant du score : du moins au plus nauséogène
    std::sort(apps.begin(), apps.end(),
              [](const App &a, const App &b)
              { return score(a) < score(b); });

    std::cout << "Classement (du moins au plus susceptible de rendre malade)\n";
    std::cout << std::string(60, '-') << "\n";
    int rang = 1;
    for (const auto &a : apps)
    {
        std::cout << rang++ << ". " << std::left << std::setw(22) << a.nom
                  << std::setw(20) << a.genre
                  << "score = " << std::fixed << std::setprecision(2) << score(a) << "\n";
    }
    return 0;
}