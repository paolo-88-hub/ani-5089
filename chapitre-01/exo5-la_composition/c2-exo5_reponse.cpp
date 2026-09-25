#include <iostream>
#include <cmath>

struct Point
{
    double x;
    double y;
};

struct Pose
{
    double x;
    double y;
    double theta;
};

Pose composerPoses(const Pose &p1, const Pose &p2)
{
    Pose result;
    result.x = p1.x + p2.x * std::cos(p1.theta) - p2.y * std::sin(p1.theta);
    result.y = p1.y + p2.x * std::sin(p1.theta) + p2.y * std::cos(p1.theta);
    result.theta = p1.theta + p2.theta;
    return result;
}

Point appliquerPose(const Pose &pose, const Point &pt)
{
    Point result;
    result.x = pose.x + pt.x * std::cos(pose.theta) - pt.y * std::sin(pose.theta);
    result.y = pose.y + pt.x * std::sin(pose.theta) + pt.y * std::cos(pose.theta);
    return result;
}

double calculerEcart(const Point &pt1, const Point &pt2)
{
    return std::sqrt(std::pow(pt1.x - pt2.x, 2) + std::pow(pt1.y - pt2.y, 2));
}

int main()
{
    Pose P1 = {1.0, 2.0, 0.7853981633974483};
    Pose P2 = {3.0, -1.0, 0.5235987755982988};
    Point p = {2.0, 5.0};

    Point p_temp = appliquerPose(P1, p);
    Point p_a = appliquerPose(P2, p_temp);

    Pose P12 = composerPoses(P1, P2);
    Point p_b = appliquerPose(P12, p);

    double ecart = calculerEcart(p_a, p_b);

    std::cout << "Point obtenu (application successive) : (" << p_a.x << ", " << p_a.y << ")\n";
    std::cout << "Point obtenu (pose composee)          : (" << p_b.x << ", " << p_b.y << ")\n";
    std::cout << "Ecart : " << ecart << "\n";

    return 0;
}