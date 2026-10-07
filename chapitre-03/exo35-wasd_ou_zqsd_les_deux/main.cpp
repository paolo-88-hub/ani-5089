#include <iostream>
#include <sstream>
#include <string>

int main()
{
    std::string ligne;
    std::getline(std::cin, ligne);
    int n = std::stoi(ligne);
    for (int i = 0; i < n; ++i)
    {
        std::getline(std::cin, ligne);
        std::istringstream flux(ligne);

        bool w = false, z = false, s = false, a = false, q = false, d = false;
        std::string touche;
        while (flux >> touche)
        {
            if (touche == "W")
                w = true;
            else if (touche == "Z")
                z = true;
            else if (touche == "S")
                s = true;
            else if (touche == "A")
                a = true;
            else if (touche == "Q")
                q = true;
            else if (touche == "D")
                d = true;
        }
        int avance = 0;
        int cote = 0;
        if (w || z)
            avance += 1;
        if (s)
            avance -= 1;
        if (a || q)
            cote -= 1;
        if (d)
            cote += 1;
        std::cout << avance << ' ' << cote << '\n';
    }
    return 0;
}