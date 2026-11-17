#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

struct Action {
    std::string nom;
    std::string usage;
};

int main() {
    int nombreActions = 0;
    if (!(std::cin >> nombreActions) || nombreActions < 0) {
        return 0;
    }

    std::vector<Action> actions;
    actions.reserve(static_cast<std::size_t>(nombreActions));

    for (int i = 0; i < nombreActions; ++i) {
        Action action;
        if (!(std::cin >> action.nom >> action.usage)) {
            return 0;
        }
        actions.push_back(action);
    }

    int nombreUsagesFournis = 0;
    if (!(std::cin >> nombreUsagesFournis) || nombreUsagesFournis < 0) {
        return 0;
    }

    std::unordered_set<std::string> usagesFournis;
    for (int i = 0; i < nombreUsagesFournis; ++i) {
        std::string usage;
        if (!(std::cin >> usage)) {
            return 0;
        }
        usagesFournis.insert(usage);
    }

    int nombreActionsMortes = 0;
    for (const Action& action : actions) {
        const bool active = usagesFournis.find(action.usage) != usagesFournis.end();
        std::cout << action.nom << ' ' << (active ? "ACTIVE" : "MORTE") << '\n';
        if (!active) {
            ++nombreActionsMortes;
        }
    }

    std::cout << "MORTES " << nombreActionsMortes << '\n';
    return 0;
}
