#include <iostream>
#include <sstream>
#include <string>

int main() {
    int nombreProgrammes = 0;
    if (!(std::cin >> nombreProgrammes) || nombreProgrammes < 0) {
        return 0;
    }
    std::cin.ignore(10000, '\n');

    int survivants = 0;
    for (int i = 0; i < nombreProgrammes; ++i) {
        std::string ligne;
        if (!std::getline(std::cin, ligne)) {
            return 0;
        }

        std::istringstream lecture(ligne);
        std::string nom;
        if (!(lecture >> nom)) {
            return 0;
        }

        bool detruit = false;
        bool recree = false;
        bool reenregistre = false;
        int positionDestruction = -1;
        int positionRecreation = -1;
        int positionEtape = 0;

        std::string etape;
        while (lecture >> etape) {
            if (etape == "DETRUIT") {
                detruit = true;
                positionDestruction = positionEtape;
            } else if (etape == "RECREE") {
                recree = true;
                positionRecreation = positionEtape;
            } else if (etape == "REENREGISTRE") {
                reenregistre = true;
            }
            ++positionEtape;
        }

        std::string verdict;
        if (detruit && recree && positionRecreation < positionDestruction) {
            verdict = "MAUVAIS ORDRE";
        } else if (!detruit || !recree) {
            verdict = "CHAINE PERIMEE";
        } else if (!reenregistre) {
            verdict = "POIGNEES PERDUES";
        } else {
            verdict = "SURVIT";
            ++survivants;
        }

        std::cout << nom << ' ' << verdict << '\n';
    }

    std::cout << "SURVIVANTS " << survivants << '\n';
    return 0;
}
