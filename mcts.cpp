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

void GameStatus::PlayTurn(Coordinate c)
{
	if (currentTurn_ == PlayerSide::BLACK)
	{
		gameState_.PlayStone(c, Color::B);
		currentTurn_ = PlayerSide::WHITE;
	}
	else
	{
		gameState_.PlayStone(c, Color::W);
		currentTurn_ = PlayerSide::BLACK;
	}
}

// TODO: isMoveValid()
// TODO: isKoRuleApplied()
// TODO: cleanUpAreasRule()

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

	InitNeighbors(N_);
}


GameState::GameState(int n)
	: N_(n)
{
	board_ = vector<Color>(N_ * N_, Color::E);
	terminal_ = false;
	p1Score_ = 0;
	p2Score_ = 0;
	pSide_ = PlayerSide::BLACK;

	InitNeighbors(N_);
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
	// with checks, generating next section of the board at once
	// Q: Do I need to recalculate the score of players each generation?

	//TODO: Add suicide rule implementation
	//TODO: Add ko rule implementation (repeat not allowed)
	board_[ConvertToArray(c.x, c.y)] = stone;
}

bool GameState::IsActionValid(Coordinate c, PlayerSide side)
{
	//TODO: define what is valid game action (placement of stone)? according to the rules
	// 1. The players may choose any unoccupied intersection
	// 2. except for those forbidden by the ko
	// 3. and suicide rules (see below). 

	if (IsOccupiedCell(c)) {
		return false;
	}

	return true;
}

bool GameState::IsOccupiedCell(Coordinate c)
{
	if (board_[ConvertToArray(c.x, c.y)] == Color::E)
		return false;
	return true;
}

bool GameState::IsOnBoard(Coordinate c) {
	if (c.x % N_ == c.x && c.y % N_ == c.y) {
		return true;
	}

	return false;
}

vector<Coordinate> GameState::GetValidNeighbors(Coordinate c) {
	vector<Coordinate> retVal;
	int x = c.x;
	int y = c.y;

	Coordinate arrayC[4] = { Coordinate(x + 1, y), Coordinate(x - 1, y), Coordinate(x, y + 1), Coordinate(x, y - 1) };
	for (int i = 0; i < 4; i++) {
		if (IsOnBoard(arrayC[i])) {
			retVal.push_back(arrayC[i]);
		}
	}

	return retVal;
}
