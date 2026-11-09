#include <iostream>
#include <string>

int main() {
    int largeurFenetre = 0;
    int hauteurFenetre = 0;
    int nombreCibles = 0;
    if (!(std::cin >> largeurFenetre >> hauteurFenetre >> nombreCibles)
        || nombreCibles < 0) {
        return 0;
    }

    int nombreBloquantes = 0;
    for (int i = 0; i < nombreCibles; ++i) {
        std::string nom;
        int cibleReliee = 0;
        int tailleForcee = 0;
        int largeurCible = 0;
        int hauteurCible = 0;
        if (!(std::cin >> nom >> cibleReliee >> tailleForcee
              >> largeurCible >> hauteurCible)) {
            return 0;
        }

        int largeurUtilisee = largeurFenetre;
        int hauteurUtilisee = hauteurFenetre;
        std::string verdict;

        if (cibleReliee == 0) {
            verdict = "PAS RELIEE";
        } else if (tailleForcee == 1) {
            largeurUtilisee = largeurCible;
            hauteurUtilisee = hauteurCible;
            verdict = "OK";
        } else if (largeurFenetre != largeurCible
                   || hauteurFenetre != hauteurCible) {
            verdict = "BLOQUE";
            ++nombreBloquantes;
        } else {
            verdict = "SANS EFFET";
        }

        std::cout << nom << ' ' << largeurUtilisee << ' ' << hauteurUtilisee
                  << ' ' << verdict << '\n';
    }

    std::cout << "BLOQUANTES " << nombreBloquantes << '\n';
    return 0;
}
