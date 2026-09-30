#include <iostream>
#include <map>
#include <sstream>
#include <string>

int main()
{
    std::string ligne;
    std::getline(std::cin, ligne);
    int v = std::stoi(ligne);
    std::map<std::string, std::string> etat;
    for (int i = 0; i < v; ++i)
    {
        std::getline(std::cin, ligne);
        std::size_t eg = ligne.find('=');
        if (eg == std::string::npos)
            continue;
        etat[ligne.substr(0, eg)] = ligne.substr(eg + 1);
    }
    std::getline(std::cin, ligne);
    int f = std::stoi(ligne);
    for (int i = 0; i < f; ++i)
    {
        std::getline(std::cin, ligne);
        std::istringstream ss(ligne);
        std::string terme;
        bool ok = true;
        while (ss >> terme)
        {
            if (terme == "&&")
                continue;
            bool nie = false;
            if (!terme.empty() && terme[0] == '!')
            {
                nie = true;
                terme = terme.substr(1);
            }
            std::size_t eg = terme.find('=');
            bool vrai = false;
            if (eg != std::string::npos)
            {
                auto it = etat.find(terme.substr(0, eg));
                vrai = (it != etat.end() && it->second == terme.substr(eg + 1));
            }
            if (nie)
                vrai = !vrai;
            if (!vrai)
                ok = false;
        }
        std::cout << (ok ? "OUI" : "NON") << "\n";
    }
    return 0;
}