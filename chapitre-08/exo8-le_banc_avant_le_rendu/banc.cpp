#include "chaine.h"

#include <iostream>

int main() {
    ChaineEchange chaine(3);
    std::size_t index = 0;

    std::cout << "Anneau de trois images :\n";
    for (int tour = 1; tour <= 3; ++tour) {
        if (chaine.AcquireImage(index)) {
            std::cout << "AcquireImage " << index << '\n';
        } else {
            std::cout << "AcquireImage REFUSE\n";
        }

        if (chaine.ReleaseImage()) {
            std::cout << "ReleaseImage OK\n";
        } else {
            std::cout << "ReleaseImage REFUSE\n";
        }
    }

    if (chaine.AcquireImage(index)) {
        std::cout << "AcquireImage " << index << '\n';
    } else {
        std::cout << "AcquireImage REFUSE\n";
    }
    chaine.ReleaseImage();

    std::cout << "Refus : double emprunt\n";
    if (chaine.AcquireImage(index)) {
        std::cout << "AcquireImage " << index << '\n';
    }
    if (!chaine.AcquireImage(index)) {
        std::cout << "AcquireImage REFUSE\n";
    }
    chaine.ReleaseImage();

    std::cout << "Refus : rendu sans emprunt\n";
    if (!chaine.ReleaseImage()) {
        std::cout << "ReleaseImage REFUSE\n";
    }

    std::cout << "Refus : chaîne trop grande\n";
    ChaineEchange tropGrande(ChaineEchange::NombreImagesMaximum + 1);
    if (!tropGrande.AcquireImage(index)) {
        std::cout << "AcquireImage REFUSE\n";
    }

    return 0;
}
