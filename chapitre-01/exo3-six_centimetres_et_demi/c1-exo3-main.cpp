#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <cmath>

int main()
{
    std::vector<double> ipd_mesures = {62.0, 60.5, 65.0, 63.5, 58.0, 67.0};
    const double ipd_chapitre = 63.0;

    double min_val = *std::min_element(ipd_mesures.begin(), ipd_mesures.end());
    double max_val = *std::max_element(ipd_mesures.begin(), ipd_mesures.end());

    double somme = std::accumulate(ipd_mesures.begin(), ipd_mesures.end(), 0.0);
    double moyenne = somme / ipd_mesures.size();

    double ecart_min_max = max_val - min_val;
    double diff_chapitre = moyenne - ipd_chapitre;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "--- Releve des 6 mesures IPD (en mm) ---\n";
    std::cout << "Mesure personnelle (Moi) : " << ipd_mesures[0] << " mm\n";
    for (size_t i = 1; i < ipd_mesures.size(); ++i)
    {
        std::cout << "Personne " << i << "               : " << ipd_mesures[i] << " mm\n";
    }

    std::cout << "\n--- Resultats Statistiques ---\n";
    std::cout << "Valeur minimale              : " << min_val << " mm\n";
    std::cout << "Valeur maximale              : " << max_val << " mm\n";
    std::cout << "Moyenne de l'echantillon     : " << moyenne << " mm\n";
    std::cout << "Ecart (Max - Min)            : " << ecart_min_max << " mm\n";

    std::cout << "\n--- Comparaison avec le Chapitre ---\n";
    std::cout << "Valeur moyenne du chapitre   : " << ipd_chapitre << " mm\n";
    std::cout << "Difference (Moyenne - Chap)  : " << diff_chapitre << " mm\n";

    return 0;
}