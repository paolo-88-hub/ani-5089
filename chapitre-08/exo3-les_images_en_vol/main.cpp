#include <iostream>
#include <string>

int main() {
    int limiteVol = 0;
    int nombreCommandes = 0;
    if (!(std::cin >> limiteVol >> nombreCommandes)
        || limiteVol < 0 || nombreCommandes < 0) {
        return 0;
    }

    int imagesEnVol = 0;
    int premierBlocage = 0;

    for (int tour = 1; tour <= nombreCommandes; ++tour) {
        std::string commande;
        if (!(std::cin >> commande)) {
            return 0;
        }

        bool attend = false;
        if (commande == "SOUMET") {
            if (imagesEnVol >= limiteVol) {
                attend = true;
                if (premierBlocage == 0) {
                    premierBlocage = tour;
                }
            } else {
                ++imagesEnVol;
            }
        } else if (commande == "FINI") {
            if (imagesEnVol > 0) {
                --imagesEnVol;
            }
        }

        std::cout << tour << ' ' << commande << ' ' << imagesEnVol << ' '
                  << (attend ? "ATTEND" : "PASSE") << '\n';
    }

    std::cout << "BLOQUE " << premierBlocage << '\n';
    return 0;
}
