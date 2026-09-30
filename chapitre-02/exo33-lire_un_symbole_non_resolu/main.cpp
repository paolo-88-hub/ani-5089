#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

int main()
{
    std::string ligne;
    std::getline(std::cin, ligne);
    int p = std::stoi(ligne);
    std::vector<std::pair<std::string, std::string>> prefixes;
    for (int i = 0; i < p; ++i)
    {
        std::getline(std::cin, ligne);
        std::istringstream ss(ligne);
        std::string pre, mod;
        if (ss >> pre >> mod)
            prefixes.push_back({pre, mod});
    }
    std::getline(std::cin, ligne);
    int l = std::stoi(ligne);

    const std::string marque = "undefined reference to '";
    std::set<std::string> modules;
    int inconnus = 0;

    for (int i = 0; i < l; ++i)
    {
        if (!std::getline(std::cin, ligne))
            break;
        std::size_t pos = ligne.find(marque);
        while (pos != std::string::npos)
        {
            std::size_t debut = pos + marque.size();
            std::size_t fin = ligne.find('\'', debut);
            if (fin == std::string::npos)
                break;
            std::string symbole = ligne.substr(debut, fin - debut);

            std::size_t meilleur = 0;
            std::string module;
            bool trouve = false;
            for (const auto &pm : prefixes)
            {
                if (symbole.compare(0, pm.first.size(), pm.first) == 0 &&
                    pm.first.size() > meilleur)
                {
                    meilleur = pm.first.size();
                    module = pm.second;
                    trouve = true;
                }
            }
            if (trouve)
                modules.insert(module);
            else
                ++inconnus;
            pos = ligne.find(marque, fin);
        }
    }
    for (const std::string &m : modules)
        std::cout << m << "\n";
    if (inconnus > 0)
        std::cout << "INCONNU " << inconnus << "\n";
    return 0;
}