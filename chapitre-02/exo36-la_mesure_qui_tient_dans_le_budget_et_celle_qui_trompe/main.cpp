#include <iostream>
#include <string>

int main()
{
    long long budget;
    int s;
    std::cin >> budget >> s;
    int trompe = 0;
    for (int i = 0; i < s; ++i)
    {
        std::string nom;
        long long debug, release;
        std::cin >> nom >> debug >> release;
        long long facteur = (release > 0) ? (debug + release / 2) / release : 0;
        bool tient = release <= budget;
        std::cout << nom << " " << facteur << " " << (tient ? "TIENT" : "DEPASSE") << "\n";
        if (debug > budget && release <= budget)
            ++trompe;
    }
    std::cout << "TROMPE " << trompe << "\n";
    return 0;
}