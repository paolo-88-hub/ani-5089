#include <iostream>
#include <iomanip>

struct Dimensions
{
    double largeur;
    double longueur;
    double hauteur;
};

struct Mobilier
{
    Dimensions chaise;
    Dimensions table;
};

struct Salle
{
    Dimensions piece;
    Mobilier meuble;
};

Salle multiplierSalle(const Salle &s, double facteur)
{
    Salle resultat;

    resultat.piece.largeur = s.piece.largeur * facteur;
    resultat.piece.longueur = s.piece.longueur * facteur;
    resultat.piece.hauteur = s.piece.hauteur * facteur;

    resultat.meuble.chaise.largeur = s.meuble.chaise.largeur * facteur;
    resultat.meuble.chaise.longueur = s.meuble.chaise.longueur * facteur;
    resultat.meuble.chaise.hauteur = s.meuble.chaise.hauteur * facteur;

    resultat.meuble.table.largeur = s.meuble.table.largeur * facteur;
    resultat.meuble.table.longueur = s.meuble.table.longueur * facteur;
    resultat.meuble.table.hauteur = s.meuble.table.hauteur * facteur;

    return resultat;
}

void afficherDimensions(const Salle &s)
{
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "--- Piece ---\n";
    std::cout << "Largeur  : " << s.piece.largeur << " m\n";
    std::cout << "Longueur : " << s.piece.longueur << " m\n";
    std::cout << "Hauteur  : " << s.piece.hauteur << " m\n\n";

    std::cout << "--- Mobilier : Chaise ---\n";
    std::cout << "Largeur  : " << s.meuble.chaise.largeur << " m\n";
    std::cout << "Longueur : " << s.meuble.chaise.longueur << " m\n";
    std::cout << "Hauteur  : " << s.meuble.chaise.hauteur << " m\n\n";

    std::cout << "--- Mobilier : Table ---\n";
    std::cout << "Largeur  : " << s.meuble.table.largeur << " m\n";
    std::cout << "Longueur : " << s.meuble.table.longueur << " m\n";
    std::cout << "Hauteur  : " << s.meuble.table.hauteur << " m\n";
}

int main()
{
    Salle salleOrigine = {
        {4.0, 5.0, 2.8},
        {{0.5, 0.5, 0.9},
         {1.2, 0.8, 0.45}}};

    double facteur = 1.0;
    std::cout << "Entrez le facteur de multiplication : ";
    if (!(std::cin >> facteur))
    {
        return 1;
    }

    Salle salleEchelle = multiplierSalle(salleOrigine, facteur);

    std::cout << "\n====================================\n";
    std::cout << "DIMENSIONS APRES FACTEUR (" << facteur << ")\n";
    std::cout << "====================================\n";
    afficherDimensions(salleEchelle);

    return 0;
}