#include <iostream>
#include <string>
using namespace std;

int main()
{

    // Témoignages des trois personnes
    string temoignages[3] = {
        "Apres quelques secondes, j'ai ressenti une legere gene et "
        "une sensation de mouvement. A la fin de la video, j'avais "
        "un peu mal a la tete.",

        "J'ai ressenti une impression de desequilibre et un leger "
        "vertige. Les mouvements rapides de la camera etaient "
        "assez desagreables a regarder.",

        "Au debut, je ne ressentais presque rien, mais apres environ "
        "une minute, j'ai commence a avoir une sensation de fatigue "
        "visuelle et une legere nausee."};

    cout << "============================================" << endl;
    cout << "     EXPERIENCE : VIDEO EN MOUVEMENT" << endl;
    cout << "============================================" << endl;

    cout << endl;
    cout << "Duree de l'experience : 2 minutes" << endl;
    cout << "Conditions : video en plein ecran, regardee de tres pres" << endl;

    cout << endl;

    for (int i = 0; i < 3; i++)
    {
        cout << "Temoignage de la personne " << i + 1 << " :" << endl;
        cout << temoignages[i] << endl;
        cout << endl;
    }

    cout << "============================================" << endl;
    cout << "Conclusion :" << endl;
    cout << "Les participants peuvent ressentir une gene, du vertige," << endl;
    cout << "une fatigue visuelle ou une sensation de mouvement." << endl;
    cout << "============================================" << endl;

    return 0;
}