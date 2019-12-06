#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QkeyEvent>
#include <QTimer>
#include <QTime>
#include "snake.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);


private:
    Ui::MainWindow *ui;
    int pos_y, pos_x;
    QTimer *myTimer;
    Snake *snake;

private slots:
    void loop();
};
#endif // MAINWINDOW_H
