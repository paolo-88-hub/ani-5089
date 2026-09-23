#include <iostream>
#include <iomanip>

int main()
{
    double frequences[] = {72, 90, 120};
    const double tempsSysteme = 8.0;

    for (double frequence : frequences)
    {
        double dureeImage = 1000.0 / frequence;
        double tempsCode = dureeImage - tempsSysteme;

        std::cout << std::fixed << std::setprecision(1);
        std::cout << frequence << " Hz : "
                  << dureeImage << " ms par image, "
                  << tempsCode << " ms pour le code\n";
    }

    return 0;
}