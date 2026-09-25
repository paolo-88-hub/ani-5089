#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

struct App
{
    std::string nom;
    std::string genre;
    double vection;
    double acceleration;
    double rotationForcee;
    double perteControle;
    double repereFixe;
};

double score(const App &a)
{
    const double wVection = 0.35;
    const double wAcc = 0.30;
    const double wRot = 0.20;
    const double wCtrl = 0.15;
    const double wRepere = 0.05;

    double s = wVection * a.vection + wAcc * a.acceleration + wRot * a.rotationForcee + wCtrl * a.perteControle - wRepere * a.repereFixe;

    return std::max(0.0, s);
}

int main()
{
    std::vector<App> apps = {
        {"Beat Saber", "Rythme", 0, 0, 0, 0, 8},
        {"Half-Life: Alyx", "Action/Aventure", 4, 2, 2, 1, 3},
        {"Epic Roller Coasters", "Simulation passive", 10, 10, 8, 10, 2}};

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