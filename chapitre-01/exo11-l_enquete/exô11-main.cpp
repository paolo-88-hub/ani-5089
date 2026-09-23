#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Personne
{
    string nom;
    string reponse;
    bool gene;
};

int main()
{
    vector<Personne> personnes = {
        {"Personne 1",
         "La première chose dont je me souviens, c’est d’avoir été impressionné par le fait que l’environnement semblait vraiment autour de moi. J’ai ressenti une petite gêne au niveau des yeux après quelques minutes.",
         true},
        {"Personne 2",
         "Je me souviens surtout d’avoir eu l’impression de tomber quand j’ai regardé vers le bas. Ça m’a un peu donné le vertige, mais je n’ai pas eu mal.",
         true},
        {"Personne 3",
         "Ce qui m’a marqué en premier, c’est l’immersion. J’avais vraiment l’impression d’être dans le jeu. Au début, le casque était un peu lourd sur ma tête.",
         true},
        {"Personne 4",
         "Je me souviens principalement des graphismes et du fait que je pouvais regarder partout autour de moi. J’ai ressenti une légère fatigue des yeux après l’utilisation.",
         true},
        {"Personne 5",
         "La première chose que j’ai remarquée, c’est que les mouvements de ma tête étaient reproduits immédiatement dans la réalité virtuelle. Je n’ai pas vraiment ressenti de gêne, à part une légère chaleur avec le casque.",
         false}};

    cout << "===== ENQUETE SUR LA REALITE VIRTUELLE =====\n\n";

    for (const auto &personne : personnes)
    {
        cout << personne.nom << " :\n";
        cout << "\"" << personne.reponse << "\"\n";
        cout << "Gene ressentie : "
             << (personne.gene ? "Oui" : "Non")
             << "\n\n";
    }

    int nombreGene = 0;

    for (const auto &personne : personnes)
    {
        if (personne.gene)
        {
            nombreGene++;
        }
    }

    cout << "===== CE QUE J'EN TIRE =====\n";
    cout << "Nombre de personnes ayant ressenti une gene : "
         << nombreGene << "/5\n";

    cout << "Les principales premieres impressions sont "
         << "l'immersion, le realisme et la possibilite de regarder "
         << "autour de soi.\n";

    cout << "La majorite des personnes ont egalement signale "
         << "un certain inconfort lie aux yeux, au vertige, au poids "
         << "ou a la chaleur du casque.\n";

    return 0;
}