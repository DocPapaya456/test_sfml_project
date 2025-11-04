#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <vector>
#include <iostream>


class Ball {
protected:
    sf::Vector2f position;
    sf::Vector2f direction;
    float velocity;
    float radius;
    sf::CircleShape shape;

public:
    Ball(sf::Vector2f position, sf::Vector2f direction, float velocity, float radius, sf::Color colour) :
    position(position), direction(direction.normalized()), velocity(velocity), radius(radius), shape(sf::CircleShape(radius)){
        shape.setOrigin({ radius, radius });
        shape.setPosition(position);
        shape.setFillColor(colour);
    }

    void update(const sf::RenderWindow& window) {
        if (position.x >= window.getSize().x - radius) {
            position.x = window.getSize().x - radius;
            direction.x = -direction.x;
            setColour(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        }

        if (position.x <= radius) {
            position.x = radius;
            direction.x = -direction.x;
            setColour(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        }

        if (position.y >= window.getSize().y - radius) {
            position.y = window.getSize().y - radius;
            direction.y = -direction.y;
            setColour(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        }

        if (position.y <= radius) {
            position.y = radius;
            direction.y = -direction.y;
            setColour(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        }

        position += velocity * direction;
        shape.setPosition(position);
    }

    void setColour(sf::Color colour) {
        shape.setFillColor(colour);
    }

    void setDirection(sf::Vector2f direction) {
        this->direction = direction.normalized();
    }

    float getVelocity() {
        return velocity;
    }

    void setVelocity(float velocity) {
        this->velocity = velocity;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

class RandomBall : public Ball {
public:
    RandomBall(sf::Vector2f position) : Ball(position, randomDirection(), randomVelocity(), randomRadius(), randomColour()){ }

private:
    sf::Vector2f randomDirection() {
        int randAngle = rand() % 360;
        return sf::Vector2f{ cos(randAngle * float(M_PI) / 180) , sin(randAngle * float(M_PI) / 180) };
    }
    
    float randomVelocity() {
        return static_cast<float>(rand() % 4 + 1);
    }

    float randomRadius() {
        return static_cast<float>(rand() % 100 + 1);
    }

    sf::Color randomColour() {
        return sf::Color(rand() % 256, rand() % 256, rand() % 256);
    }

};


int main()
{
    srand(time(NULL));

    auto window = sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Bouncy Ball");
    window.setFramerateLimit(144);

    std::vector<RandomBall> balls;
    balls.push_back(RandomBall(sf::Vector2f{ window.getSize() / 2u }));

    bool plusPressed = false;
    bool minusPressed = false;
    bool cPressed = false;
    bool enterPressed = false;
    bool backspacePressed = false;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }


        /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            position.x -= speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            position.x += speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            position.y -= speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            position.y += speed;
        }*/

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Backspace)) {
            if (!backspacePressed) {
                if (!balls.empty()) {
                    balls.pop_back();
                }
                backspacePressed = true;
            }
        }
        else {
            backspacePressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            if (!enterPressed) {
                balls.push_back(RandomBall(sf::Vector2f{ window.getSize() / 2u }));
                enterPressed = true;
            }
        }
        else {
            enterPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Equal)) {
            if (!plusPressed) {
                for (RandomBall& ball : balls) {
                    ball.setVelocity(ball.getVelocity() * 1.25);
                }
                plusPressed = true;
            }
        }
        else {
            plusPressed = false;
        }
        

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Hyphen)) {
            if (!minusPressed) {
                for (RandomBall& ball : balls) {
                    ball.setVelocity(ball.getVelocity() * 0.8);
                }
                minusPressed = true;
            }
        }
        else {
            minusPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) {
            if (!cPressed) {
                for (RandomBall& ball : balls) {
                    int randAngle = rand() % 360;
                    ball.setDirection(sf::Vector2f{cos(randAngle * float(M_PI) / 180) , sin(randAngle * float(M_PI) / 180)});
                }
                cPressed = true;
            }
        }
        else {
            cPressed = false;
        }


        for (RandomBall& ball : balls) {
            ball.update(window);
        }

        window.clear();

        // Draw stuff here
        
        for (RandomBall& ball : balls) {
            ball.draw(window);
        }

        window.display();
    }
}
