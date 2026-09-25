#include <iostream>
#include <cmath>
#include <iomanip>

struct Quaternion
{
    double w, x, y, z;

    Quaternion conjugate() const
    {
        return {w, -x, -y, -z};
    }

    Quaternion operator*(const Quaternion &q) const
    {
        return {
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w};
    }

    double dot(const Quaternion &q) const
    {
        return w * q.w + x * q.x + y * q.y + z * q.z;
    }
};

struct Vector3
{
    double x, y, z;

    double norm() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
};

Vector3 calculerVitesseAngulaire(Quaternion q1, Quaternion q2, double dt, bool forcageCheminCourt)
{
    if (forcageCheminCourt && q1.dot(q2) < 0.0)
    {
        q2.w = -q2.w;
        q2.x = -q2.x;
        q2.y = -q2.y;
        q2.z = -q2.z;
    }

    Quaternion dq = q2 * q1.conjugate();

    double norm_v = std::sqrt(dq.x * dq.x + dq.y * dq.y + dq.z * dq.z);
    double angle = 2.0 * std::atan2(norm_v, dq.w);

    if (std::abs(angle) < 1e-9 || norm_v < 1e-9)
    {
        return {0.0, 0.0, 0.0};
    }

    double factor = (angle / dt) / norm_v;
    return {dq.x * factor, dq.y * factor, dq.z * factor};
}

int main()
{
    double dt = 0.1;

    Quaternion q1 = {1.0, 0.0, 0.0, 0.0};
    Quaternion q2 = {-0.9999, 0.0, 0.0, 0.0141};

    double norm2 = std::sqrt(q2.w * q2.w + q2.x * q2.x + q2.y * q2.y + q2.z * q2.z);
    q2.w /= norm2;
    q2.x /= norm2;
    q2.y /= norm2;
    q2.z /= norm2;

    Vector3 w_avec = calculerVitesseAngulaire(q1, q2, dt, true);
    Vector3 w_sans = calculerVitesseAngulaire(q1, q2, dt, false);

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "--- Resultats du calcul de vitesse angulaire ---\n";
    std::cout << "Avec forcage du chemin court :\n";
    std::cout << "  omega = (" << w_avec.x << ", " << w_avec.y << ", " << w_avec.z << ") rad/s\n";
    std::cout << "  Norme = " << w_avec.norm() << " rad/s\n\n";

    std::cout << "Sans forcage du chemin court (Cas absurde) :\n";
    std::cout << "  omega = (" << w_sans.x << ", " << w_sans.y << ", " << w_sans.z << ") rad/s\n";
    std::cout << "  Norme = " << w_sans.norm() << " rad/s\n";

    return 0;
}