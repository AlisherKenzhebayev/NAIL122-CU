#include "mcts.h"

GameStatus::GameStatus()
{
	gameState_ = GameState();
	actionsCount_ = 0;
	currentTurn_ = PlayerSide::BLACK;
	finished_ = false;
}


GameStatus::GameStatus(GameState state)
    : gameState_(state)
{
	actionsCount_ = 0;
	currentTurn_ = PlayerSide::BLACK;
	finished_ = false;
}

void GameStatus::PlayTurn()
{
	if (currentTurn_ == PlayerSide::BLACK)
	{
		currentTurn_ = PlayerSide::WHITE;
	}
	else
	{
		currentTurn_ = PlayerSide::BLACK;
	}
}

void GameStatus::ResetGame()
{
	actionsCount_ = 0;
	currentTurn_ = PlayerSide::BLACK; // Black side moves first
	gameState_.ResetBoard();
	finished_ = false;
}

GameState::GameState()
{
	N_ = 9;
	board_ = vector<Color>(N_ * N_, Color::E);
	terminal_ = false;
	p1Score_ = 0;
	p2Score_ = 0;
	pSide_ = PlayerSide::BLACK;
}


GameState::GameState(int n)
	: N_(n)
{
	board_ = vector<Color>(n * n, Color::E);
	terminal_ = false;
	p1Score_ = 0;
	p2Score_ = 0;
	pSide_ = PlayerSide::BLACK;
}

int GameState::ConvertToArray(int x, int y)
{
	return x * N_ + y;
}

Coordinate GameState::ConvertToCoordinate(int i, int N_)
{
	return Coordinate(i / N_, i % N_);
}

void GameState::ResetBoard()
{
	for (int i = 0; i < board_.size(); i++)
	{
		board_[i] = Color::E;
	}
}

Color GameState::inverseColor(Color c)
{
	if (c == Color::B)
	{
		return Color::W;
	}
	else if (c == Color::W)
	{
		return Color::B;
	}
	else
	{
		return c;
	}
}

void GameState::PlayStone(Coordinate c, Color stone)
{
	//TODO: Add all the rules here, etc.
	board_[ConvertToArray(c.x, c.y)] = stone;
}
