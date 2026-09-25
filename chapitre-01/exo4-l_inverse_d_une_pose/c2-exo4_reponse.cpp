#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

struct Vecteur
{
    double x;
    double y;
    double z;
};

struct Quaternion
{
    double w;
    double x;
    double y;
    double z;
};

struct Pose
{
    Vecteur position;
    Quaternion rotation;
};

Vecteur addition(Vecteur a, Vecteur b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z};
}

Vecteur soustraction(Vecteur a, Vecteur b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z};
}

Vecteur multiplier(Vecteur v, double s)
{
    return {
        v.x * s,
        v.y * s,
        v.z * s};
}

Vecteur produitVectoriel(Vecteur a, Vecteur b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

Quaternion conjugue(Quaternion q)
{
    return {
        q.w,
        -q.x,
        -q.y,
        -q.z};
}

Vecteur appliquerRotation(
    Quaternion q,
    Vecteur v)
{

    Vecteur qVector = {
        q.x,
        q.y,
        q.z};

    Vecteur t = produitVectoriel(qVector, v);
    t = multiplier(t, 2.0);

    Vecteur resultat = addition(
        v,
        multiplier(t, q.w));

    resultat = addition(
        resultat,
        produitVectoriel(qVector, t));

    return resultat;
}

Vecteur appliquerPose(
    Pose pose,
    Vecteur point)
{

    Vecteur tourne =
        appliquerRotation(
            pose.rotation,
            point);

    return addition(
        tourne,
        pose.position);
}

Pose Inverser(Pose pose)
{

    Quaternion qInverse =
        conjugue(pose.rotation);

    Vecteur positionOpposee = {
        -pose.position.x,
        -pose.position.y,
        -pose.position.z};

    Vecteur positionInverse =
        appliquerRotation(
            qInverse,
            positionOpposee);

    Pose inverse;

    inverse.position = positionInverse;
    inverse.rotation = qInverse;

    return inverse;
}

double norme(Vecteur v)
{
    return sqrt(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z);
}

int main()
{

    Pose pose;
    Vecteur point;

    cin >> pose.position.x >> pose.position.y >> pose.position.z;

    cin >> pose.rotation.w >> pose.rotation.x >> pose.rotation.y >> pose.rotation.z;

    cin >> point.x >> point.y >> point.z;

    Vecteur transforme =
        appliquerPose(pose, point);

    Pose inverse =
        Inverser(pose);

    Vecteur retour =
        appliquerPose(inverse, transforme);

    Vecteur erreur =
        soustraction(retour, point);

    double ecart =
        norme(erreur);

    cout << fixed << setprecision(10);

    cout << "Point de depart : "
         << point.x << " "
         << point.y << " "
         << point.z << endl;

    cout << "Apres la pose : "
         << transforme.x << " "
         << transforme.y << " "
         << transforme.z << endl;

    cout << "Apres la pose inverse : "
         << retour.x << " "
         << retour.y << " "
         << retour.z << endl;

    cout << "Ecart au point de depart : "
         << ecart << endl;

    return 0;
}