#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "QPainter"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pos_y = 0;
    myTimer = new QTimer(this);
    connect(myTimer, &QTimer::timeout, this, &MainWindow::loop);
    myTimer->start(100);
    qsrand(QTime::currentTime().msec());
    this->snake = new Snake(this->size().width()/10, this->size().height()/10);

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    paint.setBrush(Qt::blue);
    for (const auto& pair: this->snake->get_snake())
    {
        paint.drawRect(pair.first*10, pair.second*10, 10, 10);
    }
    paint.setBrush(Qt::green);
    paint.drawRect(this->snake->get_food_position().first*10, this->snake->get_food_position().second*10, 10, 10);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Down:
    {
        this->snake->Snake::change_direction(DOWN);
        break;
    }
    case Qt::Key_Up:
    {
        this->snake->Snake::change_direction(UP);
        break;
    }
    case Qt::Key_Left:
    {
        this->snake->Snake::change_direction(LEFT);
        break;
    }
    case Qt::Key_Right:
    {
        this->snake->Snake::change_direction(RIGHT);
    }
    }
}

void MainWindow::loop()
{
    this->snake->Snake::move_snake();
    if (this->snake->Snake::check_uroboros_state()) this->myTimer->stop();
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete myTimer;
}

