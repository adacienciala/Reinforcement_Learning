#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	this->rewards = {
		{{0, 0}, 0 }, {{1, 0}, 0 }, {{2, 0}, 0 }, {{3, 0}, -10 },
		{{0, 1}, 0 }, {{1, 1}, -100 }, {{2, 1}, -100 }, {{3, 1}, 0 },
		{{0, 2}, 0 }, {{1, 2}, -100 }, {{2, 2}, 10 }, {{3, 2}, 0 }, 
		{{0, 3}, 0 }, {{1, 3}, -100 }, {{2, 3}, -100 }, {{3, 3}, 0 },
		{{0, 4}, 0 }, {{1, 4}, 0 }, {{2, 4}, 0 }, {{3, 4}, 0 } };

	this->rlTest = new rl(rewards, 4, 5);
	this->rlTest->clearState_Values();
	displayValues();

	//this->rlTest->test(); 
}

MainWindow::~MainWindow()
{
    delete ui;
	delete this->rlTest;
}

void MainWindow::displayValues()
{
}

