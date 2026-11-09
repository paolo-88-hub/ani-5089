#include <iostream>
#include <string>

int main() {
    int nombreAppels = 0;
    if (!(std::cin >> nombreAppels) || nombreAppels < 0) {
        return 0;
    }

    bool imageAcquise = false;
    bool imageEnAttenteDeSoumission = false;
    int fautes = 0;

    for (int tour = 1; tour <= nombreAppels; ++tour) {
        std::string appel;
        if (!(std::cin >> appel)) {
            return 0;
        }

        std::string resultat = "OK";
        if (appel == "ACQUIERT") {
            if (imageAcquise) {
                resultat = "DEJA ACQUISE";
            } else {
                imageAcquise = true;
            }
        } else if (appel == "DESSINE") {
            if (!imageAcquise) {
                resultat = "PAS ACQUISE";
            }
        } else if (appel == "REND") {
            if (!imageAcquise) {
                resultat = "RIEN ACQUIS";
            } else {
                imageAcquise = false;
                imageEnAttenteDeSoumission = true;
            }
        } else if (appel == "SOUMET") {
            if (!imageEnAttenteDeSoumission) {
                resultat = "PAS RENDUE";
            } else {
                imageEnAttenteDeSoumission = false;
            }
        }

        if (resultat != "OK") {
            ++fautes;
        }
        std::cout << tour << ' ' << appel << ' ' << resultat << '\n';
    }

    std::cout << "FAUTES " << fautes << '\n';
    return 0;
}
