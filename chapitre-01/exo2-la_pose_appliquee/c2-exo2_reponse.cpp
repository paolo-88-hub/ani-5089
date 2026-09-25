#include <iostream>
#include <iomanip>
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

Vecteur produitVectoriel(Vecteur a, Vecteur b)
{

    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

Vecteur addition(Vecteur a, Vecteur b)
{

    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z};
}

Vecteur multiplier(Vecteur v, double s)
{

    return {
        v.x * s,
        v.y * s,
        v.z * s};
}

Vecteur appliquerRotation(
    Quaternion q,
    Vecteur point)
{

    Vecteur qVector = {
        q.x,
        q.y,
        q.z};

    Vecteur t = produitVectoriel(qVector, point);
    t = multiplier(t, 2.0);

    Vecteur wt = multiplier(t, q.w);

    Vecteur qCrossT = produitVectoriel(qVector, t);

    Vecteur resultat = addition(point, wt);
    resultat = addition(resultat, qCrossT);

    return resultat;
}

Vecteur appliquerPose(Pose pose, Vecteur point)
{

    Vecteur pointTourne =
        appliquerRotation(pose.rotation, point);

    Vecteur pointFinal =
        addition(pointTourne, pose.position);

    return pointFinal;
}

int main()
{

    Pose pose;

    Vecteur point;

    cin >> pose.position.x >> pose.position.y >> pose.position.z;

    cin >> pose.rotation.w >> pose.rotation.x >> pose.rotation.y >> pose.rotation.z;

    cin >> point.x >> point.y >> point.z;

    Vecteur resultat =
        appliquerPose(pose, point);

    cout << fixed << setprecision(4);

    cout << resultat.x << endl;
    cout << resultat.y << endl;
    cout << resultat.z << endl;

    return 0;
}