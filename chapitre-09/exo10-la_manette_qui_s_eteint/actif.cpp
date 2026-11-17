#include <iostream>

struct EtatAction {
    bool courant = false;
    bool change = false;
    bool actif = false;
};

int main() {
    const bool pressionPhysique = true;
    bool courantPrecedent = false;
    bool objetAvantTenu = false;
    bool objetApresTenu = false;

    for (int image = 1; image <= 20; ++image) {
        const bool manetteAllumee = image <= 5 || image >= 16;
        const EtatAction etat{
            manetteAllumee && pressionPhysique,
            (manetteAllumee && pressionPhysique) != courantPrecedent,
            manetteAllumee
        };

        // Les deux versions utilisent le niveau pour tenir l'objet.
        if (etat.courant) {
            objetAvantTenu = true;
            objetApresTenu = true;
        }

        // Comportement fautif observe avant correction : current devient faux
        // quand la source s'eteint, et ce faux front est pris pour un lacher.
        if (!etat.courant && etat.change) {
            objetAvantTenu = false;
        }

        // Comportement corrige : une source inactive ne peut pas signaler un
        // lacher. On conserve donc l'objet tenu pendant cette interruption.
        if (etat.actif && !etat.courant && etat.change) {
            objetApresTenu = false;
        }

        std::cout << "image : " << image
                  << ", courant : " << etat.courant
                  << ", change : " << etat.change
                  << ", actif : " << etat.actif << '\n';

        courantPrecedent = etat.courant;
    }

    (void)objetAvantTenu;
    (void)objetApresTenu;
    return 0;
}
