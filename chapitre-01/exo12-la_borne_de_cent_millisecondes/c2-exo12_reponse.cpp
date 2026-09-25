#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

struct PoseTete
{
    double angle;
};

constexpr double PI = 3.14159265358979323846;

double deg2rad(double deg)
{
    return deg * PI / 180.0;
}

double rad2deg(double rad)
{
    return rad * 180.0 / PI;
}

PoseTete simulerVraiePose(double angleInit, double vitesseAngulaire, double accelerationAngulaire, double t)
{
    double dt_sim = 0.0001;
    double angle = angleInit;
    double w = vitesseAngulaire;

    for (double current_t = 0; current_t < t; current_t += dt_sim)
    {
        w += accelerationAngulaire * dt_sim;
        angle += w * dt_sim;
    }

    return {angle};
}

PoseTete extrapolerPose(double angleInit, double vitesseAngulaire, double t)
{
    return {angleInit + vitesseAngulaire * t};
}

int main()
{
    double angleInit = 0.0;
    double vitesseAngulaire = deg2rad(180.0);
    double accelerationAngulaire = 15.0;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Duree (ms) | Extrapolee (deg) | Vraie Pose (deg) | Erreur (deg)\n";
    std::cout << "---------------------------------------------------------------\n";

    for (int ms = 10; ms <= 1000; ms += (ms < 100 ? 10 : 100))
    {
        double t = ms / 1000.0;

        PoseTete poseExtrapol = extrapolerPose(angleInit, vitesseAngulaire, t);
        PoseTete poseVraie = simulerVraiePose(angleInit, vitesseAngulaire, accelerationAngulaire, t);

        double erreurDeg = std::abs(rad2deg(poseVraie.angle - poseExtrapol.angle));

        std::cout << std::setw(10) << ms << " | "
                  << std::setw(16) << rad2deg(poseExtrapol.angle) << " | "
                  << std::setw(16) << rad2deg(poseVraie.angle) << " | "
                  << std::setw(12) << erreurDeg << "\n";
    }

    return 0;
}