#include <iostream>
#include <string>
#include <vector>

struct Rappel
{
    int id;
    std::string type;
};

int main()
{
    int n = 0;
    std::cin >> n;
    std::vector<Rappel> registre;
    for (int i = 0; i < n; ++i)
    {
        std::string commande;
        std::cin >> commande;
        if (commande == "poser")
        {
            int id;
            std::string type;
            std::cin >> id >> type;
            for (std::size_t k = 0; k < registre.size(); ++k)
            {
                if (registre[k].id == id)
                {
                    registre.erase(registre.begin() + static_cast<std::ptrdiff_t>(k));
                    break;
                }
            }
            registre.push_back({id, type});
        }
        else if (commande == "retirer")
        {
            int id;
            std::cin >> id;
            for (std::size_t k = 0; k < registre.size(); ++k)
            {
                if (registre[k].id == id)
                {
                    registre.erase(registre.begin() + static_cast<std::ptrdiff_t>(k));
                    break;
                }
            }
        }
        else if (commande == "envoyer")
        {
            std::string type;
            std::cin >> type;
            bool premier = true;
            for (const Rappel &r : registre)
            {
                if (r.type == type)
                {
                    if (!premier)
                        std::cout << ' ';
                    std::cout << r.id;
                    premier = false;
                }
            }
            if (premier)
                std::cout << "AUCUN";
            std::cout << '\n';
        }
    }
    return 0;
}