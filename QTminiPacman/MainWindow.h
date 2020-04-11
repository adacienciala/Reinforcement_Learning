#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QkeyEvent>
#include <QTimer>
#include <QTime>
#include "rl.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT;

private:

	QPixmap pixGhost;
	QPixmap pixPlayer;
	QPixmap pixCoin;

    Ui::MainWindow *ui;
	std::map<std::pair<int, int>, int> grid;
	std::map<std::pair<int, int>, QLabel *> labels;
	state_t starting_state;
	state_t cur_state;
	QTimer* myTimer;
	int timerSpeed;
	bool boost;
	bool reset;

public:

	rl* rlObject;
	mdp* mdpObject;

	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	void display_board(const state_t& state) const;
	void keyPressEvent(QKeyEvent* event);
	std::pair<int, int> randomMoveGhost(action_t action);

private slots:

	void loopValue();
	void loopQLearning();
	void loopSarsa();
	void loopFA();

    void on_VPButton_clicked(bool checked);
    void on_QLButton_clicked(bool checked);
    void on_SarsaButton_clicked(bool checked);

    void on_ItSpinBox_valueChanged(int value);
    void on_RunButton_clicked(bool checked);

    void on_ResetButton_clicked();
    void on_SpeedSpinBox_valueChanged(int value);

    void on_progressBar_valueChanged(int value);

    void on_BoostButton_clicked(bool checked);
    void on_FAButton_clicked(bool checked);
};
#endif // MAINWINDOW_H
