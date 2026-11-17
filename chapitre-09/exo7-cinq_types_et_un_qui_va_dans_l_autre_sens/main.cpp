#include <iostream>
#include <string>
#include <unordered_map>

int main() {
    int nombreActions = 0;
    if (!(std::cin >> nombreActions) || nombreActions < 0) {
        return 0;
    }

    std::unordered_map<std::string, bool> actionEstSortie;
    int nombreEntrees = 0;
    int nombreSorties = 0;

    for (int i = 0; i < nombreActions; ++i) {
        std::string nom;
        std::string type;
        if (!(std::cin >> nom >> type)) {
            return 0;
        }

        const bool estSortie = (type == "HAPTIC");
        actionEstSortie[nom] = estSortie;
        if (estSortie) {
            ++nombreSorties;
        } else {
            ++nombreEntrees;
        }
    }

    int nombreOperations = 0;
    if (!(std::cin >> nombreOperations) || nombreOperations < 0) {
        return 0;
    }

    for (int i = 0; i < nombreOperations; ++i) {
        std::string nom;
        std::string operation;
        if (!(std::cin >> nom >> operation)) {
            return 0;
        }

        const auto action = actionEstSortie.find(nom);
        if (action == actionEstSortie.end()) {
            continue;
        }

        if (action->second && operation == "LIT") {
            std::cout << nom << " LIT UNE SORTIE\n";
        } else if (!action->second && operation == "ECRIT") {
            std::cout << nom << " ECRIT UNE ENTREE\n";
        }
    }

    std::cout << "ENTREES " << nombreEntrees << '\n';
    std::cout << "SORTIES " << nombreSorties << '\n';
    return 0;
}
