#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QkeyEvent>
#include "rl.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT;

private:

    Ui::MainWindow *ui;
	std::map<std::pair<int, int>, int> grid;
	std::map<std::pair<int, int>, QLabel *> labels;
	state_t cur_state;

public:

	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	rl* rlObject;
	mdp* mdpObject;
	void display_board(const state_t& state) const;
	void keyPressEvent(QKeyEvent* event);

};
#endif // MAINWINDOW_H
