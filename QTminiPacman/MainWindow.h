#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <rl.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	void displayValues();

private:
    Ui::MainWindow *ui;
	rl* rlTest;
	std::map<std::pair<int, int>, int> rewards;
};
#endif // MAINWINDOW_H
