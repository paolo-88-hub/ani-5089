#include <iostream>
#include <string>

bool estUsageConnu(const std::string& usage) {
    return usage == "SELECT" || usage == "GRAB" || usage == "MENU" ||
           usage == "MOVE" || usage == "AIM_POSE" || usage == "GRIP_POSE" ||
           usage == "HAPTIC" || usage == "SELECT_LEFT" || usage == "GRAB_LEFT" ||
           usage == "AIM_POSE_LEFT" || usage == "GRIP_POSE_LEFT" ||
           usage == "HAPTIC_LEFT" || usage == "MOVE_RIGHT";
}

bool estUsageGauche(const std::string& usage) {
    return usage == "MENU" || usage == "MOVE" ||
           (usage.size() >= 5 && usage.compare(usage.size() - 5, 5, "_LEFT") == 0);
}

int main() {
    int nombreApplications = 0;
    if (!(std::cin >> nombreApplications) || nombreApplications < 0) {
        return 0;
    }

    int nombreApplicationsDeuxMains = 0;

    for (int i = 0; i < nombreApplications; ++i) {
        std::string nomApplication;
        if (!(std::cin >> nomApplication)) {
            return 0;
        }

        int nombreUsagesDroite = 0;
        int nombreUsagesGauche = 0;
        std::string usage;
        while (std::cin.peek() == ' ' || std::cin.peek() == '\t') {
            std::cin.get();
        }

        while (std::cin.peek() != '\n' && std::cin.peek() != std::char_traits<char>::eof()) {
            if (!(std::cin >> usage)) {
                return 0;
            }
            if (estUsageConnu(usage)) {
                if (estUsageGauche(usage)) {
                    ++nombreUsagesGauche;
                } else {
                    ++nombreUsagesDroite;
                }
            }
            while (std::cin.peek() == ' ' || std::cin.peek() == '\t') {
                std::cin.get();
            }
        }
        if (std::cin.peek() == '\n') {
            std::cin.get();
        }

        const bool deuxMains = nombreUsagesDroite > 0 && nombreUsagesGauche > 0;
        if (deuxMains) {
            ++nombreApplicationsDeuxMains;
        }

        std::cout << nomApplication << ' ' << nombreUsagesDroite << ' '
                  << nombreUsagesGauche << ' '
                  << (deuxMains ? "DEUX MAINS" : "UNE MAIN") << '\n';
    }

    std::cout << "DEUX MAINS " << nombreApplicationsDeuxMains << '\n';
    return 0;
}
