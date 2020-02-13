#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <Windows.h>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	srand(time(NULL));

	this->grid = {
		{{0, 0}, 0 }, {{1, 0}, 0 }, {{2, 0}, 0 }, {{3, 0}, 0 },
		{{0, 1}, 0 }, {{1, 1}, -100 }, {{2, 1}, -100 }, {{3, 1}, 0 },
		{{0, 2}, 0 }, {{1, 2}, -100 }, {{2, 2}, 0 }, {{3, 2}, 0 }, 
		{{0, 3}, 0 }, {{1, 3}, -100 }, {{2, 3}, -100 }, {{3, 3}, 0 },
		{{0, 4}, 0 }, {{1, 4}, 0 }, {{2, 4}, 0 }, {{3, 4}, 0 } };

	int width = 4, height = 5;

	/*this->grid = {
		{{0, 0}, 0 }, {{1, 0}, 0 },
		{{0, 1}, 0 }, {{1, 1}, 0 } };

	int width = 2, height = 2;*/

	//automatyczne przypisanie labelow by bylo fajne kiedys V:
	/*for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			
		}
	}*/

	this->labels = {
		{{0, 0}, ui->label_00}, {{1, 0}, ui->label_10}, {{2, 0}, ui->label_20}, {{3, 0}, ui->label_30},
		{{0, 1}, ui->label_01}, {{1, 1}, ui->label_11}, {{2, 1}, ui->label_21}, {{3, 1}, ui->label_31},
		{{0, 2}, ui->label_02}, {{1, 2}, ui->label_12}, {{2, 2}, ui->label_22}, {{3, 2}, ui->label_32},
		{{0, 3}, ui->label_03}, {{1, 3}, ui->label_13}, {{2, 3}, ui->label_23}, {{3, 3}, ui->label_33},
		{{0, 4}, ui->label_04}, {{1, 4}, ui->label_14}, {{2, 4}, ui->label_24}, {{3, 4}, ui->label_34}};


	for (const auto& [pos, label] : this->labels)
	{
		label->setGeometry(QRect(x(), y(), 100, 100));
	}

	this->cur_state = { { 0, 4 }, { 3, 0 }, {2, 2} };
	//this->cur_state = { { 0, 0 }, { 0, 1 }, {1, 1} };

	this->mdpObject = new mdp(grid, width, height);
	this->rlObject = new rl(this->mdpObject);
	

	if (false)
	{
		qDebug() << "VALUE ON\n";
		this->rlObject->runValueIteration();
		qDebug() << "VALUE DONE\n";
		this->rlObject->runPolicyIteration();

		myTimer = new QTimer(this);
		connect(myTimer, &QTimer::timeout, this, &MainWindow::loopValue);
		myTimer->start(1);
		qsrand(QTime::currentTime().msec());

		display_board(this->cur_state);
	}
	else
	{
		myTimer = new QTimer(this);
		connect(myTimer, &QTimer::timeout, this, &MainWindow::loopQLearning);
		myTimer->start(1);
		qsrand(QTime::currentTime().msec());

		display_board(this->cur_state);
	}
}

MainWindow::~MainWindow()
{
    delete ui;
	delete this->rlObject;
	delete this->mdpObject;
	delete this->myTimer;
}

void MainWindow::display_board(const state_t& state) const
{
	QPalette palette = this->ui->centralwidget->palette();
	palette.setColor(this->ui->centralwidget->backgroundRole(), Qt::black);
	this->ui->centralwidget->setPalette(palette);
	
	for (const auto& [pos, label] : this->labels)
	{
		label->setPixmap(QPixmap());
		
		if (pos == state.coin)
		{
			QString filename = "coin.png";
			QPixmap pix;
			if (pix.load(filename))
			{
				label->setPixmap(pix.scaled(label->size(), Qt::KeepAspectRatio));
			}
		}
		if (pos == state.player)
		{
			QString filename = "pacman.png";
			QPixmap pix;
			if (pix.load(filename))
			{
				label->setPixmap(pix.scaled(label->size(), Qt::KeepAspectRatio));
			}
		}
		if (pos == state.ghost)
		{
			QString filename = "ghost.png";
			QPixmap pix;
			if (pix.load(filename))
			{
				label->setPixmap(pix.scaled(label->size(), Qt::KeepAspectRatio));
			}
		}
	}
}

void MainWindow::loopValue()
{
	action_t action = this->rlObject->getBestPolicy(this->cur_state);
	const auto& newGhost = this->randomMoveGhost(action);
	switch (action)
	{
	case NORTH:
		this->cur_state.player.second -= 1;
		break;
	case SOUTH:
		this->cur_state.player.second += 1;
		break;
	case EAST:
		this->cur_state.player.first += 1;
		break;
	case WEST:
		this->cur_state.player.first -= 1;
	}
	this->cur_state.ghost = newGhost;
	display_board(this->cur_state);

	if (this->mdpObject->isTerminal(this->cur_state))
	{
		qDebug() << "TERMINAL\n";
		if (this->cur_state.player == this->cur_state.coin)
		{
			qDebug() << "ZJADLO COINA\n";
			int free_coords = 0;
			for (const auto& [pos, val] : this->grid)
			{
				if (val != -100) free_coords += 1;
			}
			int random = rand() % free_coords;

			std::pair<int, int> new_coin;
			for (const auto& [pos, val] : this->grid)
			{
				if (random == 0 && val != -100)
				{
					new_coin = pos;
					break;
				}
				if (val != -100) random -= 1;
			}

			this->cur_state.coin = new_coin;
			qDebug() << "NEW COIN\n";
		}
		else
		{
			qDebug() << "ZJADLO GO\n";
			QApplication::quit();
		}
	}
}

void MainWindow::loopQLearning()
{
	static int episode = 0;
	static bool nauka = true;
	static state_t starting_state = this->cur_state;
	this->display_board(this->cur_state);

	if (episode < this->rlObject->episodes)
	{
		bool is_terminal = this->rlObject->stepQLearning(this->cur_state);
		this->display_board(this->cur_state);

		/*for (const auto& state : this->rlObject->state_QValues)
		{
			printf("* player: (%d, %d), ghost: (%d, %d), coin: (%d, %d):\n", state.first.player.first, state.first.player.second, state.first.ghost.first, state.first.ghost.second, state.first.coin.first, state.first.coin.second);
			for (const auto& action : state.second)
			{
				printf("\t-%d. %f\n", action.first, action.second);
			}
		}*/

		if (is_terminal == true)
		{
			++episode;
			qDebug() << "TERMINAL -> EP." << episode;
			this->cur_state = starting_state;
			if (episode == this->rlObject->episodes) myTimer->start(250);
			return;
		}
	}
	else
	{
		qDebug() << "KONIEC NAUKI\n";

		FILE* fp;
		fp = fopen("Qvalues.txt", "w");
		for (const auto& state : this->rlObject->state_QValues)
		{
			fprintf(fp, "* player: (%d, %d), ghost: (%d, %d), coin: (%d, %d):\n", state.first.player.first, state.first.player.second, state.first.ghost.first, state.first.ghost.second, state.first.coin.first, state.first.coin.second);
			for (const auto& action : state.second)
			{
				fprintf(fp, "\t-%d. %f\n", action.first, action.second);
			}
		}
		fclose(fp);

		if (nauka == false) QApplication::quit();
		else
		{
			this->rlObject->epsilon = -1;
			this->rlObject->alpha = 1;
			nauka = false;
			episode = 0;
		}
	}
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	action_t action = this->rlObject->getBestPolicy(this->cur_state);
	const auto& newGhost = this->randomMoveGhost(action);
	switch (action)
	{
	case NORTH:
		this->cur_state.player.second -= 1;
		break;
	case SOUTH:
		this->cur_state.player.second += 1;
		break;
	case EAST:
		this->cur_state.player.first += 1;
		break;
	case WEST:
		this->cur_state.player.first -= 1;
	}

	display_board(this->cur_state);

	if (this->mdpObject->isTerminal(this->cur_state)) QApplication::quit();

	switch (event->key())
	{
	case Qt::Key_Down:
		{
			if (this->mdpObject->isAvailable(this->cur_state.ghost, SOUTH)) this->cur_state.ghost.second += 1;
			break;
		}
	case Qt::Key_Up:
		{
			if (this->mdpObject->isAvailable(this->cur_state.ghost, NORTH)) this->cur_state.ghost.second -= 1;
			break;
		}
	case Qt::Key_Left:
		{
			if (this->mdpObject->isAvailable(this->cur_state.ghost, WEST)) this->cur_state.ghost.first -= 1;
			break;
		}
	case Qt::Key_Right:
		{
			if (this->mdpObject->isAvailable(this->cur_state.ghost, EAST)) this->cur_state.ghost.first += 1;
		}
	default:
		this->cur_state.ghost = newGhost;
	}

	display_board(this->cur_state);
}

std::pair<int, int> MainWindow::randomMoveGhost(action_t action)
{
	const auto& transitions = this->mdpObject->getTransitions(this->cur_state, action);

	float random = rand() % 1000 / 1000.0;
	std::pair<int, int> new_coords = this->cur_state.ghost;

	for (const auto& [pair, prob] : transitions)
	{
		if (random < prob)
		{
			new_coords = pair.second.ghost;
			break;
		}
		else random -= prob;
	}

	return new_coords;
}