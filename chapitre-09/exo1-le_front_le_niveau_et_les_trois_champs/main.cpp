#include <iostream>

int main() {
    int nombreImages = 0;
    if (!(std::cin >> nombreImages) || nombreImages < 0) {
        return 0;
    }

    bool courantPrecedent = false;
    int nombreFronts = 0;
    int nombreNiveaux = 0;

    for (int image = 1; image <= nombreImages; ++image) {
        int presse = 0;
        int lie = 0;
        if (!(std::cin >> presse >> lie)) {
            return 0;
        }

        const bool actif = (lie == 1);
        const bool courant = actif && (presse == 1);
        const bool change = (courant != courantPrecedent);

        if (courant && change) {
            ++nombreFronts;
        }
        if (courant) {
            ++nombreNiveaux;
        }

        std::cout << image << ' ' << courant << ' ' << change << ' ' << actif << '\n';
        courantPrecedent = courant;
    }

    std::cout << "FRONTS " << nombreFronts << '\n';
    std::cout << "NIVEAUX " << nombreNiveaux << '\n';
    return 0;
}
