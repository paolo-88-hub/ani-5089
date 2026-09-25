#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

struct PositionSouris
{
    sf::Vector2i position;
    steady_clock::time_point temps;
};

int main()
{

    sf::RenderWindow window(
        sf::VideoMode(1000, 700),
        "Test de retard de la souris");

    window.setFramerateLimit(60);

    // Retard initial
    int retardMs = 0;

    // Historique des positions de la souris
    queue<PositionSouris> historique;

    // Cercle qui suit la souris
    sf::CircleShape cercle(20);
    cercle.setFillColor(sf::Color::Blue);
    cercle.setOrigin(20, 20);

    cout << "========================================" << endl;
    cout << "       TEST DE RETARD DE LA SOURIS" << endl;
    cout << "========================================" << endl;

    cout << endl;
    cout << "Commandes :" << endl;
    cout << "+ : augmenter le retard de 10 ms" << endl;
    cout << "- : diminuer le retard de 10 ms" << endl;
    cout << "R : remettre le retard a zero" << endl;
    cout << "Echap : quitter" << endl;
    cout << endl;

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

                if (retardMs < 200)
                {
                    retardMs += 10;
                }

                cout << "Retard : "
                     << retardMs << " ms" << endl;
            }

            // Diminuer le retard
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Subtract)
            {

                if (retardMs > 0)
                {
                    retardMs -= 10;
                }

                cout << "Retard : "
                     << retardMs << " ms" << endl;
            }

            // Remettre à zéro
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::R)
            {

                retardMs = 0;

                cout << "Retard : 0 ms" << endl;
            }

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
            {

                window.close();
            }
        }

        // Enregistrer la position actuelle de la souris
        sf::Vector2i positionSouris =
            sf::Mouse::getPosition(window);

        historique.push({positionSouris,
                         steady_clock::now()});

        // Rechercher une position datant du retard demandé
        auto maintenant = steady_clock::now();

        while (!historique.empty())
        {

            auto age = duration_cast<milliseconds>(
                           maintenant - historique.front().temps)
                           .count();

            if (age >= retardMs)
            {

                sf::Vector2i anciennePosition =
                    historique.front().position;

                cercle.setPosition(
                    static_cast<float>(anciennePosition.x),
                    static_cast<float>(anciennePosition.y));

                historique.pop();

                break;
            }

            break;
        }

        // Affichage
        window.clear(sf::Color::Black);

        window.draw(cercle);

        window.display();
    }

    return 0;
}