#include <iostream>
#include <iomanip>

using namespace std;

// Structure représentant un vecteur 3D
struct Vecteur
{
    double x;
    double y;
    double z;
};

// Convention du module
Vecteur Avant()
{
    return {0.0, 0.0, 1.0};
}

Vecteur Haut()
{
    return {0.0, 1.0, 0.0};
}

Vecteur Droite()
{
    return {1.0, 0.0, 0.0};
}

// Produit scalaire de deux vecteurs
double produitScalaire(Vecteur a, Vecteur b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int main()
{

    // Lecture du point
    double x, y, z;

    cin >> x >> y >> z;

    Vecteur point = {x, y, z};

    // Récupération des trois directions
    Vecteur avant = Avant();
    Vecteur haut = Haut();
    Vecteur droite = Droite();

    // Affichage avec quatre décimales
    cout << fixed << setprecision(4);

    cout << produitScalaire(point, avant) << endl;
    cout << produitScalaire(point, haut) << endl;
    cout << produitScalaire(point, droite) << endl;

    return 0;
}