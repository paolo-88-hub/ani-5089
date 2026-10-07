#include <iostream>
#include <string>

int main()
{
    int n;
    std::cin >> n;

    long long totalX = 0, totalY = 0;
    long long moteurX = 0, moteurY = 0;

    for (int i = 0; i < n; ++i)
    {
        std::string commande;
        std::cin >> commande;

        if (commande == "bouge")
        {
            long long dx, dy;
            std::cin >> dx >> dy;

            totalX += dx;
            totalY += dy;

            moteurX = dx; // ecrase au lieu d'ajouter
            moteurY = dy;
        }
        else if (commande == "image")
        {
            std::cout << totalX << " " << totalY << " " << moteurX << " " << moteurY << "\n";
            totalX = 0;
            totalY = 0;
        }
    }

    return 0;
}