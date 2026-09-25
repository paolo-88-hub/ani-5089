#include <iostream>
#include <iomanip>
using namespace std;

// ======================================
// Structure Vecteur
// ======================================
struct Vecteur
{
    double x;
    double y;
    double z;
};

// ======================================
// Structure Quaternion
// ======================================
struct Quaternion
{
    double w;
    double x;
    double y;
    double z;
};

// ======================================
// Structure Pose
// ======================================
struct Pose
{
    Vecteur position;
    Quaternion rotation;
};

// ======================================
// Produit vectoriel
// ======================================
Vecteur produitVectoriel(Vecteur a, Vecteur b)
{

    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

// ======================================
// Addition de vecteurs
// ======================================
Vecteur addition(Vecteur a, Vecteur b)
{

    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z};
}

// ======================================
// Multiplication par un scalaire
// ======================================
Vecteur multiplier(Vecteur v, double s)
{

    return {
        v.x * s,
        v.y * s,
        v.z * s};
}

// ======================================
// Rotation d'un vecteur par quaternion
// ======================================
Vecteur appliquerRotation(
    Quaternion q,
    Vecteur point)
{

    Vecteur qVector = {
        q.x,
        q.y,
        q.z};

    // t = 2 * (qVector x point)
    Vecteur t = produitVectoriel(qVector, point);
    t = multiplier(t, 2.0);

    // w * t
    Vecteur wt = multiplier(t, q.w);

    // qVector x t
    Vecteur qCrossT =
        produitVectoriel(qVector, t);

    // resultat = point + w*t + qVector x t
    Vecteur resultat =
        addition(point, wt);

    resultat =
        addition(resultat, qCrossT);

    return resultat;
}

// ======================================
// Fonction 1 : rotation puis translation
// ======================================
Vecteur rotationPuisTranslation(
    Pose pose,
    Vecteur point)
{

    // Rotation
    Vecteur resultat =
        appliquerRotation(pose.rotation, point);

    // Translation
    resultat =
        addition(resultat, pose.position);

    return resultat;
}

// ======================================
// Fonction 2 : translation puis rotation
// ======================================
Vecteur translationPuisRotation(
    Pose pose,
    Vecteur point)
{

    // Translation d'abord
    Vecteur resultat =
        addition(point, pose.position);

    // Rotation ensuite
    resultat =
        appliquerRotation(pose.rotation, resultat);

    return resultat;
}

// ======================================
// Affichage d'un vecteur
// ======================================
void afficher(Vecteur v)
{

    cout << "("
         << v.x << ", "
         << v.y << ", "
         << v.z << ")";
}

// ======================================
// Programme principal
// ======================================
int main()
{

    Pose pose;
    Vecteur point;

    // Lecture de la translation
    cin >> pose.position.x >> pose.position.y >> pose.position.z;

    // Lecture du quaternion
    cin >> pose.rotation.w >> pose.rotation.x >> pose.rotation.y >> pose.rotation.z;

    // Lecture du point
    cin >> point.x >> point.y >> point.z;

    // Calcul des deux transformations
    Vecteur resultat1 =
        rotationPuisTranslation(pose, point);

    Vecteur resultat2 =
        translationPuisRotation(pose, point);

    cout << fixed << setprecision(4);

    cout << "Rotation puis translation : ";
    afficher(resultat1);
    cout << endl;

    cout << "Translation puis rotation : ";
    afficher(resultat2);
    cout << endl;

    return 0;
}