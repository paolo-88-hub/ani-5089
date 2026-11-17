#include <cstdlib>
#include <iostream>

int main() {
    int seuil = 0;
    int vitesseMaximale = 0;
    if (!(std::cin >> seuil >> vitesseMaximale)) {
        return 0;
    }

    int nombreTours = 0;
    if (!(std::cin >> nombreTours) || nombreTours < 0) {
        return 0;
    }

    int nombreToursMorts = 0;
    const long long courseTotale = 1000LL - seuil;
    const long long denominateur = courseTotale * courseTotale;

    for (int tour = 0; tour < nombreTours; ++tour) {
        int valeurBrute = 0;
        if (!(std::cin >> valeurBrute)) {
            return 0;
        }

        const long long valeurAbsolue = std::llabs(static_cast<long long>(valeurBrute));
        if (valeurAbsolue < seuil) {
            ++nombreToursMorts;
            std::cout << 0 << '\n';
            continue;
        }

        const long long reste = valeurAbsolue - seuil;
        long long vitesse = static_cast<long long>(vitesseMaximale) * reste * reste / denominateur;
        if (valeurBrute < 0) {
            vitesse = -vitesse;
        }

        std::cout << vitesse << '\n';
    }

    std::cout << "MORTS " << nombreToursMorts << '\n';
    return 0;
}
