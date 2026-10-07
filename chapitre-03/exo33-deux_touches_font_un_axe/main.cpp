#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

int main()
{
    int c;
    std::cin >> c;

    std::vector<long long> echelles(c);
    std::vector<long long> seuils(c);

    for (int i = 0; i < c; ++i)
    {
        std::string nom; // le nom n'est pas reutilise pour le calcul, juste lu
        std::cin >> nom >> echelles[i] >> seuils[i];
    }

    int t;
    std::cin >> t;

    for (int tour = 0; tour < t; ++tour)
    {
        long long axe = 0;

        for (int i = 0; i < c; ++i)
        {
            long long brute;
            std::cin >> brute;

            long long contribution = brute * echelles[i] / 1000;

            if (std::llabs(contribution) >= seuils[i])
            {
                axe += contribution;
            }
        }

        std::cout << axe << "\n";
    }

    return 0;
}