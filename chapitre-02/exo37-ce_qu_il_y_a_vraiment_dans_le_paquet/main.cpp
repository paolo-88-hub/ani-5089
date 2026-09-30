#include <iostream>
#include <string>

static bool commence(const std::string &s, const std::string &p)
{
    return s.compare(0, p.size(), p) == 0;
}
static bool finit(const std::string &s, const std::string &e)
{
    return s.size() >= e.size() && s.compare(s.size() - e.size(), e.size(), e) == 0;
}

int main()
{
    std::string arch;
    int f;
    std::cin >> arch >> f;
    long long total = 0;
    bool signe = false, abi = false;
    int inutile = 0;
    const std::string voulu = "lib/" + arch + "/";
    for (int i = 0; i < f; ++i)
    {
        std::string chemin;
        long long taille;
        std::cin >> chemin >> taille;
        total += taille;
        if (commence(chemin, "META-INF/") &&
            (finit(chemin, ".RSA") || finit(chemin, ".DSA") || finit(chemin, ".EC")))
            signe = true;
        if (commence(chemin, voulu))
            abi = true;
        else if (commence(chemin, "lib/"))
            ++inutile;
    }
    std::cout << total << "\n"
              << (signe ? "SIGNE" : "NON SIGNE") << "\n"
              << (abi ? "ABI OUI" : "ABI NON") << "\n"
              << "INUTILE " << inutile << "\n";
    return 0;
}