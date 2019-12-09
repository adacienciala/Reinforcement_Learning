#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "rl.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	
	this->rewards = {
		{{0, 0}, 0 }, {{1, 0}, 0 }, {{2, 0}, 0 }, {{3, 0}, 1 },
		{{0, 1}, 0 }, {{1, 1}, -10 }, {{2, 1}, 0 }, {{3, 1}, -1 },
		{{0, 2}, 0 }, {{1, 2}, 0 }, {{2, 2}, 0 }, {{3, 2}, 0 } };
	
	this->rlTest = new rl(rewards, 4, 3);
	this->rlTest->clearState_Values();
	displayValues();

	//this->rlTest->test(); 
}

MainWindow::~MainWindow()
{
    delete this->ui;
	delete this->rlTest;
}


void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
	//qDebug() << value;
	 
	this->rlTest->clearState_Values();
	this->rlTest->iterations = value; 
	this->rlTest->runValueIteration();

	displayValues();
}

void MainWindow::displayValues()
{
	std::vector<QLabel *> labels;
	labels.push_back(ui->label_01);
	labels.push_back(ui->label_02);
	labels.push_back(ui->label_03);
	labels.push_back(ui->label_04);
	labels.push_back(ui->label_05);
	labels.push_back(ui->label_06);
	labels.push_back(ui->label_07);
	labels.push_back(ui->label_08);
	labels.push_back(ui->label_09);
	labels.push_back(ui->label_10);
	labels.push_back(ui->label_11);

	//no i to powinno siê samo rysowaæ na podstawie tabeli przejœæ, a nie rêcznie dodawane :V
	this->ui->label_01->setStyleSheet("");
    
	QPalette palette = this->ui->centralwidget->palette();

	palette.setColor(this->ui->centralwidget->backgroundRole(), Qt::darkBlue);
	this->ui->centralwidget->setPalette(palette);

	palette = this->ui->label_nope->palette();
	palette.setColor(this->ui->label_nope->backgroundRole(), Qt::gray);
	palette.setColor(this->ui->label_nope->foregroundRole(), Qt::white);
	this->ui->label_nope->setPalette(palette);
	palette.setColor(this->ui->label_nope->backgroundRole(), Qt::black);
	this->ui->labelText->setPalette(palette);
	this->ui->labelText->setText(QString("VALUES AFTER %1 ITERATIONS").arg(this->rlTest->iterations));

	unsigned long long i = 0;
    for (const auto& state : this->rlTest->state_values)
	{
		if (state.second == -1) palette.setColor(labels[i]->backgroundRole(), QColor(177, 2, 2, 255));
		else if (state.second == 1) palette.setColor(labels[i]->backgroundRole(), Qt::darkGreen);
		else if (state.second == 0.0) palette.setColor(labels[i]->backgroundRole(), Qt::black);
		else if (state.second > 0.6) palette.setColor(labels[i]->backgroundRole(), QColor(58, 150, 58, 255));
		else if (state.second > 0.4) palette.setColor(labels[i]->backgroundRole(), QColor(58, 191, 73, 255));
		else palette.setColor(labels[i]->backgroundRole(), QColor(58, 208, 99, 255));

		labels[i]->setPalette(palette);
        labels[i++]->setText(QString().setNum(state.second, 'f', 2));
	}
} 
