#include <cstdlib>
#include <iostream>
#include <string>

struct ChaineImage {
    int prochaineImage = 0;
    int imageEmpruntee = 0;
    int rendusAcceptes = 0;
    bool empruntEnCours = false;
};

int main() {
    int nombreImages = 0;
    int nombreCommandes = 0;
    if (!(std::cin >> nombreImages >> nombreCommandes)
        || nombreImages <= 0 || nombreCommandes < 0) {
        return 0;
    }

    ChaineImage gauche;
    ChaineImage droite;
    int premierDecalageExcessif = 0;

    for (int tour = 1; tour <= nombreCommandes; ++tour) {
        char oeil = '\0';
        std::string commande;
        if (!(std::cin >> oeil >> commande)) {
            return 0;
        }

        ChaineImage* chaine = nullptr;
        if (oeil == 'G') {
            chaine = &gauche;
        } else if (oeil == 'D') {
            chaine = &droite;
        }

        bool refuse = (chaine == nullptr);
        int index = 0;
        if (chaine != nullptr && commande == "EMPRUNTE") {
            if (chaine->empruntEnCours) {
                refuse = true;
            } else {
                chaine->imageEmpruntee = chaine->prochaineImage;
                chaine->empruntEnCours = true;
                index = chaine->imageEmpruntee;
            }
        } else if (chaine != nullptr && commande == "REND") {
            if (!chaine->empruntEnCours) {
                refuse = true;
            } else {
                index = chaine->imageEmpruntee;
                chaine->empruntEnCours = false;
                chaine->prochaineImage =
                    (chaine->prochaineImage + 1) % nombreImages;
                ++chaine->rendusAcceptes;
            }
        } else {
            refuse = true;
        }

        std::cout << tour << ' ' << oeil << ' ' << commande << ' ';
        if (refuse) {
            std::cout << "REFUSE\n";
        } else {
            std::cout << index << '\n';
        }

        const int decalage = std::abs(gauche.rendusAcceptes - droite.rendusAcceptes);
        if (decalage > 1 && premierDecalageExcessif == 0) {
            premierDecalageExcessif = tour;
        }
    }

    const int decalageFinal =
        std::abs(gauche.rendusAcceptes - droite.rendusAcceptes);
    std::cout << "DECALAGE " << decalageFinal << '\n';
    std::cout << "DESYNC " << premierDecalageExcessif << '\n';
    return 0;
}
