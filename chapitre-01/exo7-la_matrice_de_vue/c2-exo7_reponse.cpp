#include <iostream>
#include <cmath>
#include <array>
#include <iomanip>

using Matrix4x4 = std::array<std::array<double, 4>, 4>;

Matrix4x4 inverserGenerale(const Matrix4x4 &m, bool &succes)
{
    Matrix4x4 a = m;
    Matrix4x4 inv = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};

    for (int i = 0; i < 4; ++i)
    {
        int pivot = i;
        for (int j = i + 1; j < 4; ++j)
        {
            if (std::abs(a[j][i]) > std::abs(a[pivot][i]))
                pivot = j;
        }

        if (std::abs(a[pivot][i]) < 1e-9)
        {
            succes = false;
            return inv;
        }

        std::swap(a[i], a[pivot]);
        std::swap(inv[i], inv[pivot]);

        double div = a[i][i];
        for (int j = 0; j < 4; ++j)
        {
            a[i][j] /= div;
            inv[i][j] /= div;
        }

        for (int k = 0; k < 4; ++k)
        {
            if (k != i)
            {
                double factor = a[k][i];
                for (int j = 0; j < 4; ++j)
                {
                    a[k][j] -= factor * a[i][j];
                    inv[k][j] -= factor * inv[i][j];
                }
            }
        }
    }
    succes = true;
    return inv;
}

Matrix4x4 inverserDirecte(const Matrix4x4 &m)
{
    Matrix4x4 inv = {{{0}}};
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            inv[i][j] = m[j][i];
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < 3; ++j)
        {
            sum -= inv[i][j] * m[j][3];
        }
        inv[i][3] = sum;
    }
    inv[3][3] = 1.0;
    return inv;
}

void afficherMatrice(const Matrix4x4 &m)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::cout << std::setw(10) << std::setprecision(4) << m[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int main()
{
    double theta = 0.7853981633974483;
    Matrix4x4 poseValide{};
    poseValide[0] = {std::cos(theta), -std::sin(theta), 0.0, 5.0};
    poseValide[1] = {std::sin(theta), std::cos(theta), 0.0, 3.0};
    poseValide[2] = {0.0, 0.0, 1.0, -2.0};
    poseValide[3] = {0.0, 0.0, 0.0, 1.0};

    bool ok = false;
    Matrix4x4 invGen = inverserGenerale(poseValide, ok);
    Matrix4x4 invDir = inverserDirecte(poseValide);

    std::cout << "--- Comparaison des 16 coefficients (Pose Valide) ---\n";
    double maxDiff = 0.0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            double diff = std::abs(invGen[i][j] - invDir[i][j]);
            if (diff > maxDiff)
                maxDiff = diff;
            std::cout << "Coeff [" << i << "][" << j << "] -> Gen: "
                      << invGen[i][j] << " | Dir: " << invDir[i][j] << "\n";
        }
    }
    std::cout << "Ecart maximal sur les 16 coefficients : " << maxDiff << "\n\n";

    Matrix4x4 poseDegeneree{};
    poseDegeneree[0] = {0.0, 0.0, 0.0, 5.0};
    poseDegeneree[1] = {0.0, 0.0, 0.0, 3.0};
    poseDegeneree[2] = {0.0, 0.0, 0.0, -2.0};
    poseDegeneree[3] = {0.0, 0.0, 0.0, 1.0};

    std::cout << "--- Test sur Pose Degeneree (Inversion Generale) ---\n";
    Matrix4x4 invDegenere = inverserGenerale(poseDegeneree, ok);
    if (!ok)
    {
        std::cout << "Echec de l'inversion generale : Matrice singuliere (non inversible).\n";
    }
    else
    {
        afficherMatrice(invDegenere);
    }

    return 0;
}