#include <cstdint>
#include <iostream>
#include <vector>

int main() {
    int images = 0;
    int nombreCommandes = 0;
    if (!(std::cin >> images >> nombreCommandes)
        || images < 0 || nombreCommandes < 0) {
        return 0;
    }

    std::vector<std::uint64_t> poignees(static_cast<std::size_t>(images));
    std::vector<bool> valeurPresente(static_cast<std::size_t>(images), false);
    int nombreIndexPoses = 0;

    for (int tour = 0; tour < nombreCommandes; ++tour) {
        std::string commande;
        if (!(std::cin >> commande)) {
            return 0;
        }

        if (commande == "POSE") {
            int index = 0;
            std::uint64_t valeur = 0;
            if (!(std::cin >> index >> valeur)) {
                return 0;
            }

            if (index < 0 || index >= images) {
                std::cout << "HORS ANNEAU\n";
                continue;
            }

            const std::size_t position = static_cast<std::size_t>(index);
            if (!valeurPresente[position]) {
                valeurPresente[position] = true;
                ++nombreIndexPoses;
            }
            poignees[position] = valeur;
        } else if (commande == "LIT") {
            int index = 0;
            if (!(std::cin >> index)) {
                return 0;
            }

            if (index < 0 || index >= images) {
                std::cout << "VIDE\n";
                continue;
            }

            const std::size_t position = static_cast<std::size_t>(index);
            if (valeurPresente[position]) {
                std::cout << poignees[position] << '\n';
            } else {
                std::cout << "VIDE\n";
            }
        }
    }

    std::cout << "POSEES " << nombreIndexPoses << '\n';
    return 0;
}

