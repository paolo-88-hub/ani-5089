#include <iostream>

struct EtatAction {
    bool courant = false;
    bool change = false;
    bool actif = false;
};

int main() {
    bool courantPrecedent = false;
    bool objetTenuAvant = false;
    bool objetTenuApres = false;
    bool objetTombeAvant = false;
    bool objetTombeApres = false;
    bool tombeAvantImage6 = false;
    bool tenuApresImage6 = false;

    // La manette s'eteint aux images 6 a 15, soit dix images.
    // Le bouton reste physiquement tenu jusqu'a l'image 18.
    for (int image = 1; image <= 20; ++image) {
        const bool boutonPhysiquementTenu = image <= 18;
        const bool actif = !(image >= 6 && image <= 15);
        const bool courant = actif && boutonPhysiquementTenu;
        const bool change = courant != courantPrecedent;
        const EtatAction saisir{courant, change, actif};

        // Version fautive : le faux courant provoque un faux relachement
        // lorsque la manette s'eteint, car le code ignore saisir.actif.
        if (!objetTombeAvant && saisir.courant) {
            objetTenuAvant = true;
        }
        if (objetTenuAvant && !saisir.courant && saisir.change) {
            objetTenuAvant = false;
            objetTombeAvant = true;
        }

        // Version corrigee : current tient l'objet; seul un vrai front de
        // relachement d'une action active peut le detacher.
        if (!objetTombeApres && saisir.courant) {
            objetTenuApres = true;
        }
        if (objetTenuApres && saisir.actif &&
            !saisir.courant && saisir.change) {
            objetTenuApres = false;
            objetTombeApres = true;
        }

        if (image == 6) {
            tombeAvantImage6 = objetTombeAvant;
            tenuApresImage6 = objetTenuApres;
        }

        std::cout << "image : " << image
                  << ", courant : " << saisir.courant
                  << ", change : " << saisir.change
                  << ", actif : " << saisir.actif << '\n';

        courantPrecedent = saisir.courant;
    }

    // Verifier que les deux captures correspondent bien a l'image 6 et que
    // le code corrige detachera aussi l'objet au vrai relachement (image 19).
    if (!tombeAvantImage6 || !tenuApresImage6 || !objetTombeApres ||
        objetTenuApres) {
        return 1;
    }
}
