#include <iostream>

int main() {
    int nombreImages = 0;
    if (!(std::cin >> nombreImages) || nombreImages < 0) {
        return 0;
    }

    bool pressePrecedent = false;
    int ballesFront = 0;
    int ballesNiveau = 0;
    int tenuNiveau = 0;
    int tenuFront = 0;

    for (int image = 0; image < nombreImages; ++image) {
        int presseEntree = 0;
        if (!(std::cin >> presseEntree)) {
            return 0;
        }

        const bool presse = (presseEntree == 1);
        const bool frontMontant = presse && !pressePrecedent;

        if (frontMontant) {
            ++ballesFront;
            ++tenuFront;
        }
        if (presse) {
            ++ballesNiveau;
            ++tenuNiveau;
        }

        pressePrecedent = presse;
    }

    std::cout << "BALLES FRONT " << ballesFront << '\n';
    std::cout << "BALLES NIVEAU " << ballesNiveau << '\n';
    std::cout << "TENU NIVEAU " << tenuNiveau << '\n';
    std::cout << "TENU FRONT " << tenuFront << '\n';
    return 0;
}
