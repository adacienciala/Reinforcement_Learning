#include "snake.h"

Snake::Snake(unsigned int map_width, unsigned int map_height)
{
    this->width = map_width;
    this->height = map_height;
    qsrand(QTime::currentTime().msec());
    restart();
    int x = (int)qrand()%this->width;
    int y = (int)qrand()%this->height;
    this->snake.push_front(std::make_pair(x, y)); //chyba potrzebuje poczatkowych wartosci no nie, lel
}

void Snake::change_direction(enum Direction d)
{
    this->dir = d;
}

int Snake::get_points() const
{
    return this->points;
}

void Snake::restart()
{
    this->snake.clear();
    this->points = 0;
    this->food.first = (int)qrand()%this->width;
    this->food.second = (int)qrand()%this->height;
}

void Snake::move_snake()
{
    int new_x = this->snake.front().first;
    int new_y = this->snake.front().second;
    switch(this->dir)
    {
    case LEFT:
        new_x -= 1;
        if (new_x<0) new_x = (int)this->width-1;
        break;
    case RIGHT:
        new_x += 1;
        if (new_x>=(int)this->width) new_x = 0;
        break;
    case UP:
        new_y -= 1;
        if (new_y<0) new_y = (int)this->height-1;
        break;
    case DOWN:
        new_y += 1;
        if (new_y>=(int)this->height) new_y = 0;
    }
    this->snake.push_front(std::make_pair(new_x, new_y));
    if (new_x == this->food.first && new_y == this->food.second)
    {
        this->points += 1;
        draw_food();
    }
    else
    {
        this->snake.pop_back();
    }
}

bool Snake::check_uroboros_state() const
{
    for (auto pair = this->snake.begin()+1; pair != this->snake.end(); ++pair)
    {
        if (this->snake.front() == *pair) return true;
    }
    return false;
}

const std::deque<std::pair<int, int>>& Snake::get_snake() const
{
    return this->snake;
}

std::pair<int, int> Snake::get_food_position() const
{
    return this->food;
}

void Snake::draw_food()
{
    int flag = 1, new_x=0, new_y=0;
    while (flag)
    {
        flag = 0;
        new_x = (int)qrand()%this->width;
        new_y = (int)qrand()%this->height;
        for (const auto& pair : this->snake)
        {
            if (this->snake.front().first == new_x && this->snake.front().second == new_y)
            {
                flag = 1;
                break;
            }
        }
    }
    this->food.first = new_x;
    this->food.second = new_y;
}
