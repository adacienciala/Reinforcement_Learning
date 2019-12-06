#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <deque>
#include <QTime>

enum Direction
{
    LEFT,
    RIGHT,
    DOWN=3,
    UP
};

class Snake
{

public:

    Snake(unsigned int map_width, unsigned int map_height);
    void change_direction(enum Direction d);
    int get_points() const;
    void restart();
    void move_snake();
    bool check_uroboros_state() const;
    const std::deque<std::pair<int, int>>& get_snake() const;
    std::pair<int, int> get_food_position() const;

private:

    enum Direction dir;
    std::deque<std::pair<int, int>> snake;
    std::pair<int, int> food;
    int points;
    unsigned int width;
    unsigned int height;

    void draw_food();

};

#endif // SNAKE_H
