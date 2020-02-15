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
	myTimer = new QTimer(this);
	qsrand(QTime::currentTime().msec());

	this->pixGhost.load("ghost.png");
	this->pixPlayer.load("pacman.png");
	this->pixCoin.load("coin.png");

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

	this->starting_state = { { 0, 4 }, { 3, 0 }, {2, 2} };
	//this->starting_state = { { 0, 0 }, { 0, 1 }, {1, 1} };
	this->cur_state = this->starting_state;

	this->mdpObject = new mdp(grid, width, height);
	this->rlObject = new rl(this->mdpObject);
	this->reset = false;
	this->timerSpeed = this->ui->SpeedSpinBox->value();
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
			label->setPixmap(pixCoin.scaled(label->size(), Qt::KeepAspectRatio));
		}
		if (pos == state.player)
		{
			label->setPixmap(pixPlayer.scaled(label->size(), Qt::KeepAspectRatio));
		}
		if (pos == state.ghost)
		{
			label->setPixmap(pixGhost.scaled(label->size(), Qt::KeepAspectRatio));
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
			on_VPButton_clicked(false);
		}
	}
}

void MainWindow::loopQLearning()
{
	static int episode = 0;
	static bool nauka = true;
	if (this->reset == true)
	{
		this->reset = false;
		episode = 0;
		nauka = true;
	}
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
			this->cur_state = this->starting_state;
			if (episode == this->rlObject->episodes) this->ui->SpeedSpinBox->setValue(250);
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

void MainWindow::loopSarsa()
{
	static int episode = 0;
	static bool nauka = true;
	if (this->reset == true)
	{
		episode = 0;
		nauka = true;
	}
	this->display_board(this->cur_state);

	if (episode < this->rlObject->episodes)
	{
		bool is_terminal = this->rlObject->stepSarsa(this->cur_state, this->reset);
		if (this->reset == true) this->reset = false;
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
			this->cur_state = this->starting_state;
			if (episode == this->rlObject->episodes) this->ui->SpeedSpinBox->setValue(250);
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

void MainWindow::on_VPButton_clicked(bool checked)
{
	if (checked == true)
	{
		this->ui->ItLabel->setText("ITERATIONS");
		this->rlObject->mode = VPITERATIONS;
		this->ui->Options->setTitle("MODE: Value + Policy");
		this->ui->QLButton->setChecked(false);
		this->ui->SarsaButton->setChecked(false);
	}
	else
	{
		this->rlObject->mode = NOT_SET;
		this->ui->Options->setTitle("MODE:");
	}
}

void MainWindow::on_QLButton_clicked(bool checked)
{
	if (checked == true)
	{
		this->ui->ItLabel->setText("EPISODES");
		this->rlObject->mode = QLEARNING;
		this->ui->Options->setTitle("MODE: Qlearning");
		this->ui->VPButton->setChecked(false);
		this->ui->SarsaButton->setChecked(false);
	}
	else
	{
		this->rlObject->mode = NOT_SET;
		this->ui->Options->setTitle("MODE:");
	}
}

void MainWindow::on_SarsaButton_clicked(bool checked)
{
	if (checked == true)
	{
		this->ui->ItLabel->setText("EPISODES");
		this->rlObject->mode = SARSA;
		this->ui->Options->setTitle("MODE: Sarsa");
		this->ui->VPButton->setChecked(false);
		this->ui->QLButton->setChecked(false);
	}
	else
	{
		this->rlObject->mode = NOT_SET;
		this->ui->Options->setTitle("MODE:");
	}
}

void MainWindow::on_ItSpinBox_valueChanged(int value)
{
	switch (this->rlObject->mode)
	{
	case VPITERATIONS:
		this->rlObject->iterations = value;
		break;
	case QLEARNING:
	case SARSA:
		this->rlObject->episodes = value;
		break;
	}
}

void MainWindow::on_RunButton_clicked(bool checked)
{
	if (checked == true)
	{
		this->ui->RunButton->setText("PAUSE");
		switch (this->rlObject->mode)
		{
		case VPITERATIONS:
			qDebug() << "VALUE ON\n";
			this->rlObject->runValueIteration();
			qDebug() << "POLICY ON\n";
			this->rlObject->runPolicyIteration();
			connect(myTimer, &QTimer::timeout, this, &MainWindow::loopValue);
			break;
		case QLEARNING:
			qDebug() << "QL ON\n";
			connect(myTimer, &QTimer::timeout, this, &MainWindow::loopQLearning);
			break;
		case SARSA:
			qDebug() << "SARSA ON\n";
			connect(myTimer, &QTimer::timeout, this, &MainWindow::loopSarsa);
			break;
		case NOT_SET:
			this->ui->RunButton->setText("RUN");
			this->ui->RunButton->setChecked(false);
			return;
		}
		myTimer->start(this->timerSpeed);
	}
	else
	{
		this->ui->RunButton->setText("RUN");
		switch (this->rlObject->mode)
		{
		case VPITERATIONS:
			qDebug() << "VALUE OFF\n";
			disconnect(myTimer, &QTimer::timeout, this, &MainWindow::loopValue);
			break;
		case QLEARNING:
			qDebug() << "QL OFF";
			disconnect(myTimer, &QTimer::timeout, this, &MainWindow::loopQLearning);
			break;
		case SARSA:
			qDebug() << "SARSA OFF";
			disconnect(myTimer, &QTimer::timeout, this, &MainWindow::loopSarsa);
			break;
		}
	}
}

void MainWindow::on_ResetButton_clicked()
{
	disconnect(myTimer, nullptr, nullptr, nullptr);
	this->timerSpeed = 1;
	this->reset = true;
	this->rlObject->mode = NOT_SET;

	this->ui->RunButton->setText("RUN");
	this->ui->Options->setTitle("MODE:");

	this->ui->SpeedSpinBox->setValue(1);
	this->ui->ItSpinBox->setValue(1000);
	this->ui->QLButton->setChecked(false);
	this->ui->SarsaButton->setChecked(false);
	this->ui->VPButton->setChecked(false);
	this->ui->RunButton->setChecked(false);
	printf("RESET");

	this->cur_state = this->starting_state;
	this->rlObject->clearStateValues();
	this->rlObject->state_QValues.clear();
	this->rlObject->episodes = 1000;

	this->display_board(this->cur_state);
}

void MainWindow::on_SpeedSpinBox_valueChanged(int value)
{
	this->timerSpeed = value;
	myTimer->start(this->timerSpeed);
}
