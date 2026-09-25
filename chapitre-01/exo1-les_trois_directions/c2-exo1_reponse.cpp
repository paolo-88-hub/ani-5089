#include <iostream>
#include <iomanip>

using namespace std;

struct Vecteur
{
    double x;
    double y;
    double z;
};

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

double produitScalaire(Vecteur a, Vecteur b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int main()
{

    double x, y, z;

    cin >> x >> y >> z;

    Vecteur point = {x, y, z};

    Vecteur avant = Avant();
    Vecteur haut = Haut();
    Vecteur droite = Droite();

    cout << fixed << setprecision(4);

    cout << produitScalaire(point, avant) << endl;
    cout << produitScalaire(point, haut) << endl;
    cout << produitScalaire(point, droite) << endl;

    return 0;
}