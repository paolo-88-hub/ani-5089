#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Appareil
{
    std::string serie, etat;
};

int main()
{
    std::string ligne;
    std::getline(std::cin, ligne);
    int d = std::stoi(ligne);
    std::vector<Appareil> liste;
    for (int i = 0; i < d; ++i)
    {
        std::getline(std::cin, ligne);
        std::istringstream ss(ligne);
        Appareil a;
        if (ss >> a.serie >> a.etat)
            liste.push_back(a);
    }
    std::string cible;
    std::getline(std::cin, cible);
    while (!cible.empty() && (cible.back() == '\r' || cible.back() == ' '))
        cible.pop_back();

    if (cible != "-")
    {
        for (const Appareil &a : liste)
        {
            if (a.serie == cible)
            {
                if (a.etat == "device")
                    std::cout << a.serie << "\n";
                else
                    std::cout << "ERREUR " << a.serie << " est " << a.etat << "\n";
                return 0;
            }
        }
        std::cout << "ERREUR cible introuvable\n";
        return 0;
    }

    std::vector<std::string> prets;
    for (const Appareil &a : liste)
        if (a.etat == "device")
            prets.push_back(a.serie);
    std::sort(prets.begin(), prets.end());

    if (prets.empty())
        std::cout << "ERREUR aucun appareil\n";
    else if (prets.size() == 1)
        std::cout << prets[0] << "\n";
    else
    {
        std::cout << "ERREUR plusieurs appareils\n";
        for (const std::string &s : prets)
            std::cout << s << "\n";
    }
    return 0;
}