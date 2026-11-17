#include <iostream>
#include <string>

int main() {
    int nombreObjets = 0;
    if (!(std::cin >> nombreObjets) || nombreObjets < 0) {
        return 0;
    }

    int nombreFautes = 0;

    for (int i = 0; i < nombreObjets; ++i) {
        std::string nom;
        std::string intention;
        std::string pose;
        if (!(std::cin >> nom >> intention >> pose)) {
            return 0;
        }

        std::string resultat = "OK";
        if (intention == "TENIR" && pose == "AIM") {
            resultat = "FLOTTE";
            ++nombreFautes;
        } else if (intention == "POINTER" && pose == "GRIP") {
            resultat = "PART DE TRAVERS";
            ++nombreFautes;
        }

        std::cout << nom << ' ' << resultat << '\n';
    }

    std::cout << "FAUTES " << nombreFautes << '\n';
    return 0;
}
