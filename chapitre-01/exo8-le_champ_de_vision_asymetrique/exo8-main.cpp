#include <iostream>
#include <iomanip>
using namespace std;

int main()
{

    double gauche = 44.70;
    double droite = 36.24;
    double haut = 38.88;
    double bas = 45.23;

    double champHorizontal = gauche + droite;
    double champVertical = haut + bas;

    double surface = champHorizontal * champVertical;

    double angleSymetriqueHorizontal = champHorizontal;
    double angleSymetriqueVertical = surface / angleSymetriqueHorizontal;

    cout << fixed << setprecision(2);

    cout << "========================================" << endl;
    cout << "   CHAMP DE VISION - OEIL GAUCHE" << endl;
    cout << "========================================" << endl;

    cout << "Angle vers la gauche  : " << gauche << " degres" << endl;
    cout << "Angle vers la droite  : " << droite << " degres" << endl;
    cout << "Angle vers le haut    : " << haut << " degres" << endl;
    cout << "Angle vers le bas     : " << bas << " degres" << endl;

    cout << endl;

    cout << "Champ horizontal : "
         << champHorizontal << " degres" << endl;

    cout << "Champ vertical   : "
         << champVertical << " degres" << endl;

    cout << "Surface angulaire approximative : "
         << surface << " degres^2" << endl;

    cout << endl;

    cout << "Champ symetrique de meme surface :" << endl;
    cout << "Horizontal : "
         << angleSymetriqueHorizontal << " degres" << endl;

    cout << "Vertical   : "
         << angleSymetriqueVertical << " degres" << endl;

    cout << endl;

    cout << "Conclusion :" << endl;
    cout << "Un champ symetrique repartirait la vision" << endl;
    cout << "de maniere plus uniforme autour de l'axe central." << endl;

    return 0;
}