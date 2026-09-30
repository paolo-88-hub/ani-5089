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
    std::map<std::string, std::vector<std::string>> besoins;
    for (int i = 0; i < n; ++i)
    {
        std::getline(std::cin, ligne);
        std::istringstream ss(ligne);
        std::string nom, dep;
        if (!(ss >> nom))
            continue;
        while (ss >> dep)
            besoins[nom].push_back(dep);
    }
    std::getline(std::cin, ligne);
    std::set<std::string> resultat;
    std::vector<std::string> pile;
    std::string mot;
    while (std::cin >> mot)
    {
        if (resultat.insert(mot).second)
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
            if (resultat.insert(d).second)
                pile.push_back(d);
    }
    for (const std::string &m : resultat)
        std::cout << m << "\n";
    return 0;
}