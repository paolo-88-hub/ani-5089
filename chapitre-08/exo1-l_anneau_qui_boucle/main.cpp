#include <iostream>
#include <string>

int main() {
	int images = 0;
	int nombreCommandes = 0;
	if (!(std::cin >> images >> nombreCommandes) || images <= 0 || nombreCommandes < 0) {
		return 0;
	}

	int suivant = 0;
	int imageEmpruntee = 0;
	int prets = 0;
	bool empruntEnCours = false;

	for (int tour = 1; tour <= nombreCommandes; ++tour) {
		std::string commande;
		if (!(std::cin >> commande)) {
			return 0;
		}

		std::cout << tour << ' ' << commande << ' ';
		if (commande == "EMPRUNTE") {
			if (empruntEnCours) {
				std::cout << "REFUSE\n";
			} else {
				imageEmpruntee = suivant;
				empruntEnCours = true;
				++prets;
				std::cout << imageEmpruntee << '\n';
			}
		} else if (commande == "REND") {
			if (!empruntEnCours) {
				std::cout << "REFUSE\n";
			} else {
				std::cout << imageEmpruntee << '\n';
				empruntEnCours = false;
				suivant = (suivant + 1) % images;
			}
		} else {
			std::cout << "REFUSE\n";
		}
	}

	std::cout << "PRETS " << prets << '\n';
	return 0;
}
