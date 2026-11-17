#include "actions.h"

#include <iostream>

int main() {
    bool touchePremiere = false;
    bool toucheDeuxieme = false;
    bool courantPrecedentPremiere = false;
    bool courantPrecedentDeuxieme = false;
    for (int image = 1; image <= 6; ++image) {
        touchePremiere = (image >= 2 && image <= 4);
        toucheDeuxieme = (image == 5);

        const EtatAction premiere{
            touchePremiere,
            touchePremiere != courantPrecedentPremiere,
            true
        };
        const EtatAction deuxieme{
            toucheDeuxieme,
            toucheDeuxieme != courantPrecedentDeuxieme,
            true
        };
        const EtatAction nonLiee{};

        std::cout << image << ' '
                  << premiere.courant << ' ' << premiere.change << ' ' << premiere.actif << ' '
                  << deuxieme.courant << ' ' << deuxieme.change << ' ' << deuxieme.actif << ' '
                  << nonLiee.courant << ' ' << nonLiee.change << ' ' << nonLiee.actif << '\n';

        courantPrecedentPremiere = premiere.courant;
        courantPrecedentDeuxieme = deuxieme.courant;
    }

    return 0;
}
