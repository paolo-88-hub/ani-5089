#include <iostream>
#include <iomanip>
using namespace std;

// ================================
// Structure Vecteur 3D
// ================================
struct Vecteur
{
    double x;
    double y;
    double z;
};

// ================================
// Structure Quaternion
// ================================
struct Quaternion
{
    double w;
    double x;
    double y;
    double z;
};

// ================================
// Structure Pose
// ================================
struct Pose
{
    Vecteur position;
    Quaternion rotation;
};

// ================================
// Produit vectoriel
// ================================
Vecteur produitVectoriel(Vecteur a, Vecteur b)
{

    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

// ================================
// Addition de deux vecteurs
// ================================
Vecteur addition(Vecteur a, Vecteur b)
{

    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z};
}

// ================================
// Multiplication vecteur * scalaire
// ================================
Vecteur multiplier(Vecteur v, double s)
{

    return {
        v.x * s,
        v.y * s,
        v.z * s};
}

// ================================
// Rotation d'un point par quaternion
// ================================
Vecteur appliquerRotation(
    Quaternion q,
    Vecteur point)
{

    // Partie vectorielle du quaternion
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
    Vecteur qCrossT = produitVectoriel(qVector, t);

    // point + w*t + qVector x t
    Vecteur resultat = addition(point, wt);
    resultat = addition(resultat, qCrossT);

    return resultat;
}

// ================================
// Application complète de la pose
// ================================
Vecteur appliquerPose(Pose pose, Vecteur point)
{

    // 1. Rotation
    Vecteur pointTourne =
        appliquerRotation(pose.rotation, point);

    // 2. Translation
    Vecteur pointFinal =
        addition(pointTourne, pose.position);

    return pointFinal;
}

// ================================
// Programme principal
// ================================
int main()
{

    Pose pose;

    Vecteur point;

    // Lecture de la position de la pose
    cin >> pose.position.x >> pose.position.y >> pose.position.z;

    // Lecture du quaternion
    cin >> pose.rotation.w >> pose.rotation.x >> pose.rotation.y >> pose.rotation.z;

    // Lecture du point
    cin >> point.x >> point.y >> point.z;

    // Application de la pose
    Vecteur resultat =
        appliquerPose(pose, point);

    // Affichage avec 4 decimales
    cout << fixed << setprecision(4);

    cout << resultat.x << endl;
    cout << resultat.y << endl;
    cout << resultat.z << endl;

    return 0;
}