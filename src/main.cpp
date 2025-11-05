#define _USE_MATH_DEFINES
#define MAX_VEL 50.0f

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <vector>
#include <iostream>
#include <algorithm>


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
            setColour(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));
        }

        if (position.x <= radius) {
            position.x = radius;
            direction.x = -direction.x;
            setColour(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));
        }

        if (position.y >= window.getSize().y - radius) {
            position.y = window.getSize().y - radius;
            direction.y = -direction.y;
            setColour(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));
        }

        if (position.y <= radius) {
            position.y = radius;
            direction.y = -direction.y;
            setColour(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));
        }

        position += velocity * direction;
        shape.setPosition(position);
    }

    void setColour(sf::Color colour) {
        shape.setFillColor(colour);
    }

    sf::Vector2f getDirection() {
        return direction;
    }

    void setDirection(sf::Vector2f direction) {
        this->direction = direction.normalized();
    }

    float getVelocity() {
        return velocity;
    }


    void setVelocity(float velocity) {
        this->velocity = std::min(velocity, MAX_VEL);
    }

    sf::Vector2f getPosition() {
        return position;
    }

    void setPosition(sf::Vector2f position) {
        this->position = position;
    }

    float getRadius() {
        return radius;
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
        int randAngle = std::rand() % 360;
        return sf::Vector2f{ cos(randAngle * float(M_PI) / 180) , sin(randAngle * float(M_PI) / 180) };
    }
    
    float randomVelocity() {
        return static_cast<float>(std::rand() % 4 + 1);
    }

    float randomRadius() {
        return static_cast<float>(std::rand() % 100 + 1);
    }

    sf::Color randomColour() {
        return sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256);
    }

};

class BallManager {
private:
    std::vector<Ball> balls;

    void calculateCollisions() {
        for (int i = 0; i < balls.size(); i++) {
            for (int j = i + 1; j < balls.size(); j++) {
                Ball& a = balls.at(i);
                Ball& b = balls.at(j);
                if ( isColliding( a, b ) ) {
                    // Vector formed from the centers of the two balls
                    sf::Vector2f v_ab = a.getPosition() - b.getPosition();

                    // Force vectors of movement of A and B
                    sf::Vector2f F_a = a.getVelocity() * a.getDirection();
                    sf::Vector2f F_b = b.getVelocity() * b.getDirection();

                    // Ball A
                    // Orthogonal projection
                    sf::Vector2f force_b_on_a = F_b.projectedOnto(v_ab);
                    sf::Vector2f resultant_a = F_a + force_b_on_a;
                    a.setVelocity(resultant_a.length());
                    a.setDirection(resultant_a.normalized());

                    // Ball B
                    // Orthogonal projection
                    sf::Vector2f force_a_on_b = F_a.projectedOnto(v_ab);
                    sf::Vector2f resultant_b = F_b + force_a_on_b;
                    b.setVelocity(resultant_b.length());
                    b.setDirection(resultant_b.normalized());


                }
            }
        }
    }

    bool isColliding(Ball& a, Ball& b) {
        // Vector between two centers
        sf::Vector2f v = b.getPosition() - a.getPosition();

        float minDistance = a.getRadius() + b.getRadius();

        if (v.length() < minDistance) {
            b.setPosition(b.getPosition() + v.normalized() * (minDistance - v.length()));
            return true;
        }
        return false;
    }

public:
    void addBall(Ball& ball) {
        balls.push_back(ball);
    }

    void removeBall() {
        balls.pop_back();
    }

    bool empty() {
        return balls.empty();
    }

    void increaseBallsVelocity(float amount) {
        for (Ball& ball : balls) {
            ball.setVelocity(ball.getVelocity() * amount);
        }
    }

    void decreaseBallsVelocity(float amount) {
        for (Ball& ball : balls) {
            ball.setVelocity(ball.getVelocity() * amount);
        }
    }

    void randomiseBallsDirection() {
        for (Ball& ball : balls) {
            int randAngle = std::rand() % 360;
            ball.setDirection(sf::Vector2f{ cos(randAngle * float(M_PI) / 180) , sin(randAngle * float(M_PI) / 180) });
        }
    }

    void updateBalls(sf::RenderWindow& window) {

        calculateCollisions();

        for (Ball& ball : balls) {
            ball.update(window);
        }

    }

    void drawBalls(sf::RenderWindow& window) {
        for (Ball& ball : balls) {
            ball.draw(window);
        }
    }


};


int main()
{
    srand(time(NULL));

    auto window = sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Bouncy Ball");
    window.setFramerateLimit(144);

    BallManager ballManager;
    ballManager.addBall(RandomBall(sf::Vector2f{ window.getSize() / 2u }));

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Backspace)) {
            if (!backspacePressed) {
                if (!ballManager.empty()) {
                    ballManager.removeBall();
                }
                backspacePressed = true;
            }
        }
        else {
            backspacePressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            if (!enterPressed) {
                ballManager.addBall(RandomBall(sf::Vector2f{ window.getSize() / 2u }));
                enterPressed = true;
            }
        }
        else {
            enterPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Equal)) {
            if (!plusPressed) {
                ballManager.increaseBallsVelocity(1.25f);
                plusPressed = true;
            }
        }
        else {
            plusPressed = false;
        }
        

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Hyphen)) {
            if (!minusPressed) {
                ballManager.decreaseBallsVelocity(0.8f);
                minusPressed = true;
            }
        }
        else {
            minusPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) {
            if (!cPressed) {
                ballManager.randomiseBallsDirection();
                cPressed = true;
            }
        }
        else {
            cPressed = false;
        }


        ballManager.updateBalls(window);

        window.clear();

        // Draw stuff here
        
        ballManager.drawBalls(window);

        window.display();
    }
}
