#include <iostream>
#include <cmath>
#include <iomanip>

struct Pose
{
    double x;
    double y;
    double theta;
};

Pose avancerPose(const Pose &p, double v, double omega, double dt)
{
    Pose p_next;
    const double eps = 1e-9;

    if (std::abs(omega) < eps)
    {
        p_next.x = p.x + v * dt * std::cos(p.theta);
        p_next.y = p.y + v * dt * std::sin(p.theta);
        p_next.theta = p.theta;
    }
    else
    {
        p_next.x = p.x + (v / omega) * (std::sin(p.theta + omega * dt) - std::sin(p.theta));
        p_next.y = p.y - (v / omega) * (std::cos(p.theta + omega * dt) - std::cos(p.theta));
        p_next.theta = p.theta + omega * dt;
    }

    return p_next;
}

int main()
{
    Pose p_init;
    double v, omega, dt;

    std::cout << "Entrez la pose initiale (x y theta_rad) : ";
    if (!(std::cin >> p_init.x >> p_init.y >> p_init.theta))
        return 1;

    std::cout << "Entrez la vitesse lineaire (v) et angulaire (omega) : ";
    if (!(std::cin >> v >> omega))
        return 1;

    std::cout << "Entrez la duree (dt) : ";
    if (!(std::cin >> dt))
        return 1;

    Pose p_final = avancerPose(p_init, v, omega, dt);

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n--- Pose Extrapolee ---\n";
    std::cout << "x     : " << p_final.x << "\n";
    std::cout << "y     : " << p_final.y << "\n";
    std::cout << "theta : " << p_final.theta << " rad\n";

    return 0;
}