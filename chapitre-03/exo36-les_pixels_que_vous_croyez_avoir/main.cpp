#include <iostream>

int main()
{
    int n = 0;
    std::cin >> n;
    int lisibles = 0;
    for (int i = 0; i < n; ++i)
    {
        long long largeur, hauteur, echelle, champ;
        std::cin >> largeur >> hauteur >> echelle >> champ;
        long long largeurReelle = largeur * echelle / 100;
        long long hauteurReelle = hauteur * echelle / 100;
        long long pixelsParDegre = 0;
        if (champ > 0)
        {
            pixelsParDegre = (largeurReelle + champ / 2) / champ;
        }
        if (pixelsParDegre >= 15)
            ++lisibles;
        std::cout << largeurReelle << ' ' << hauteurReelle << ' ' << pixelsParDegre << '\n';
    }
    std::cout << "LISIBLE " << lisibles << '\n';
    return 0;
}