#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;

int main()
{
    // Création de la fenêtre
    sf::RenderWindow window(
        sf::VideoMode(1000, 700),
        "Test de latence de la souris");

    window.setFramerateLimit(60);

    // Retard initial
    int retard = 0;

    // Position réelle de la souris
    sf::Vector2i positionReelle;

    // Position affichée après application du retard
    sf::Vector2f positionAffichee(500, 350);

    // Cercle représentant le curseur
    sf::CircleShape curseur(15);
    curseur.setFillColor(sf::Color::Red);
    curseur.setOrigin(15, 15);

    cout << "=====================================\n";
    cout << "       TEST DE LATENCE SOURIS\n";
    cout << "=====================================\n\n";

    cout << "Le retard peut etre regle entre 0 et 200 ms.\n";
    cout << "Utilisez :\n";
    cout << "  + : augmenter le retard de 5 ms\n";
    cout << "  - : diminuer le retard de 5 ms\n";
    cout << "  Echap : quitter\n\n";

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // Augmenter le retard
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Add)
            {
                retard = min(200, retard + 5);

                cout << "Retard : "
                     << retard
                     << " ms\n";
            }

            // Diminuer le retard
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Subtract)
            {
                retard = max(0, retard - 5);

                cout << "Retard : "
                     << retard
                     << " ms\n";
            }

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }
        }

        // Récupération de la position réelle de la souris
        positionReelle = sf::Mouse::getPosition(window);

        /*
         * Simulation du retard.
         *
         * On attend le nombre de millisecondes
         * choisi avant d'afficher la nouvelle position.
         */
        this_thread::sleep_for(
            chrono::milliseconds(retard));

        positionAffichee.x = static_cast<float>(
            positionReelle.x);

        positionAffichee.y = static_cast<float>(
            positionReelle.y);

        curseur.setPosition(positionAffichee);

        // Affichage
        window.clear(sf::Color::White);

        window.draw(curseur);

        window.display();
    }

    return 0;
}