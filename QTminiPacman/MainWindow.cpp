#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	this->grid = {
		{{0, 0}, 0 }, {{1, 0}, 0 }, {{2, 0}, 0 }, {{3, 0}, 0 },
		{{0, 1}, 0 }, {{1, 1}, -100 }, {{2, 1}, -100 }, {{3, 1}, 0 },
		{{0, 2}, 0 }, {{1, 2}, -100 }, {{2, 2}, 0 }, {{3, 2}, 0 }, 
		{{0, 3}, 0 }, {{1, 3}, -100 }, {{2, 3}, -100 }, {{3, 3}, 0 },
		{{0, 4}, 0 }, {{1, 4}, 0 }, {{2, 4}, 0 }, {{3, 4}, 0 } };

	this->labels = {
		{{0, 0}, ui->label_00}, {{1, 0}, ui->label_10}, {{2, 0}, ui->label_20}, {{3, 0}, ui->label_30},
		{{0, 1}, ui->label_01}, {{1, 1}, ui->label_11}, {{2, 1}, ui->label_21}, {{3, 1}, ui->label_31},
		{{0, 2}, ui->label_02}, {{1, 2}, ui->label_12}, {{2, 2}, ui->label_22}, {{3, 2}, ui->label_32},
		{{0, 3}, ui->label_03}, {{1, 3}, ui->label_13}, {{2, 3}, ui->label_23}, {{3, 3}, ui->label_33},
		{{0, 4}, ui->label_04}, {{1, 4}, ui->label_14}, {{2, 4}, ui->label_24}, {{3, 4}, ui->label_34}};


	for (const auto& [pos, label] : this->labels)
	{
		label->setFixedSize(150, 100);
	}

	this->cur_state = { { 0, 4 }, { 3, 0 }, { 2, 2 } };
	this->rlTest = new rl(grid, 4, 5);
	this->rlTest->clearState_Values();

	this->rlTest->runValueIteration();
	this->rlTest->runPolicyIteration();

	display_board(this->cur_state);

	//this->rlTest->test(); 
}

MainWindow::~MainWindow()
{
    delete ui;
	delete this->rlTest;
}

void MainWindow::display_board(const state_t& state) const
{
	QPalette palette = this->ui->label_00->palette();
	for (const auto& [pos, label] : this->labels)
	{
		if (this->grid.at(pos) == -100)
		{
			palette.setColor(label->backgroundRole(), QColor(0, 0, 102, 255));
			label->setPalette(palette);
		}
		else
		{
			palette.setColor(label->backgroundRole(), Qt::black);
			label->setPalette(palette);
		}

		label->setPixmap(QPixmap());

		if (pos == state.coin)
		{
			QString filename = "coin.png";
			QPixmap pix;
			if (pix.load(filename))
			{
				pix.scaled(label->size(), Qt::KeepAspectRatio);
				label->setPixmap(pix);
			}
		}
		if (pos == state.player)
		{
			QString filename = "pacman.png";
			QPixmap pix;
			if (pix.load(filename))
			{
				pix.scaled(label->size(), Qt::KeepAspectRatio);
				label->setPixmap(pix);
			}
		}
		if (pos == state.ghost)
		{
			QString filename = "ghost.png";
			QPixmap pix;
			if (pix.load(filename))
			{
				pix.scaled(label->size(), Qt::KeepAspectRatio);
				label->setPixmap(pix);
			}
		}
	}
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	std::string action;
	switch (event->key())
	{
	case Qt::Key_Down:
		{
			if (this->rlTest->isAvailable(cur_state.ghost, "south")) cur_state.ghost.second += 1;
			break;
		}
	case Qt::Key_Up:
		{
			if (this->rlTest->isAvailable(cur_state.ghost, "north")) cur_state.ghost.second -= 1;
			break;
		}
	case Qt::Key_Left:
		{
			if (this->rlTest->isAvailable(cur_state.ghost, "west")) cur_state.ghost.first -= 1;
			break;
		}
	case Qt::Key_Right:
		{
			if (this->rlTest->isAvailable(cur_state.ghost, "east")) cur_state.ghost.first += 1;
		}
	}
	display_board(this->cur_state);
	action = this->rlTest->getBestPolicy(cur_state);
	if (action == "stop") return;
	if (action == "north") cur_state.player.second -= 1;
	if (action == "south") cur_state.player.second += 1;
	if (action == "east") cur_state.player.first += 1;
	if (action == "west") cur_state.player.first -= 1;
}