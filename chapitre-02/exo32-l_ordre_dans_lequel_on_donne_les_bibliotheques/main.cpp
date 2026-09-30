#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    std::string ligne;
    std::getline(std::cin, ligne);
    int n = std::stoi(ligne);
    std::map<std::string, std::set<std::string>> besoins;
    for (int i = 0; i < n; ++i)
    {
        std::getline(std::cin, ligne);
        std::istringstream ss(ligne);
        std::string nom, dep;
        if (!(ss >> nom))
            continue;
        besoins[nom];
        while (ss >> dep)
            besoins[nom].insert(dep);
    }
    std::getline(std::cin, ligne);

    std::set<std::string> liste;
    std::vector<std::string> pile;
    std::string mot;
    while (std::cin >> mot)
    {
        if (liste.insert(mot).second)
            pile.push_back(mot);
    }
    while (!pile.empty())
    {
        std::string courant = pile.back();
        pile.pop_back();
        auto it = besoins.find(courant);
        if (it == besoins.end())
            continue;
        for (const std::string &d : it->second)
            if (liste.insert(d).second)
                pile.push_back(d);
    }

    std::map<std::string, int> compte;
    for (const std::string &m : liste)
        compte[m] = 0;
    for (const std::string &m : liste)
    {
        auto it = besoins.find(m);
        if (it == besoins.end())
            continue;
        for (const std::string &d : it->second)
            ++compte[d];
    }

    std::vector<std::string> ordre;
    std::set<std::string> restants = liste;
    while (!restants.empty())
    {
        std::string choisi;
        bool trouve = false;
        for (const std::string &m : restants)
        { // std::set : déjà trié
            if (compte[m] == 0)
            {
                choisi = m;
                trouve = true;
                break;
            }
        }
        if (!trouve)
        {
            std::cout << "CYCLE\n";
            return 0;
        }
        restants.erase(choisi);
        ordre.push_back(choisi);
        auto it = besoins.find(choisi);
        if (it != besoins.end())
            for (const std::string &d : it->second)
                --compte[d];
    }

    for (const std::string &m : ordre)
        std::cout << m << "\n";
    return 0;
}