#include <cmath>
#include <iostream>

struct EtatAction {
    bool current = false;
    bool changed = false;
    bool active = false;
};

struct Pose {
    double x = 0.0;

    Pose Inverted() const {
        return {-x};
    }

    Pose operator*(const Pose& autre) const {
        return {x + autre.x};
    }
};

Pose PosePaumeDroite(int image) {
    return {static_cast<double>(image) * 0.1};
}

int main() {
    bool pressionPrecedente = false;
    bool objetTenuAuFront = false;
    bool objetTenuAuNiveau = false;
    Pose attacheFront;
    Pose attacheNiveau;

    for (int image = 1; image <= 6; ++image) {
        const bool pression = image >= 2 && image <= 4;
        const EtatAction saisir{pression, pression != pressionPrecedente, true};
        const Pose posePaume = PosePaumeDroite(image);
        const Pose poseCubeAuDepart{posePaume.x + 0.2};

        // VERSION FAUTIVE : current && changed ne dure qu'une image.
        // L'objet n'est donc rattache qu'au front d'appui, puis tombe.
        if (saisir.active && saisir.current && saisir.changed) {
            attacheFront = posePaume.Inverted() * poseCubeAuDepart;
            objetTenuAuFront = true;
        } else {
            objetTenuAuFront = false;
        }

        // VERSION CORRECTE : memoriser la pose relative une seule fois,
        // au front de saisie, puis suivre la paume pendant tout le niveau.
        if (saisir.active && saisir.current && !objetTenuAuNiveau) {
            attacheNiveau = posePaume.Inverted() * poseCubeAuDepart;
            objetTenuAuNiveau = true;
        }
        if (objetTenuAuNiveau) {
            const Pose poseObjet = posePaume * attacheNiveau;
            (void)poseObjet;
        }
        if (objetTenuAuFront) {
            const Pose poseObjet = posePaume * attacheFront;
            (void)poseObjet;
        }
        if (saisir.active && !saisir.current && saisir.changed) {
            objetTenuAuNiveau = false;
        }

        std::cout << "image " << image
                  << " current=" << saisir.current
                  << " changed=" << saisir.changed
                  << " active=" << saisir.active
                  << " front_tient=" << objetTenuAuFront
                  << " niveau_tient=" << objetTenuAuNiveau << '\n';

        pressionPrecedente = saisir.current;
    }
}
