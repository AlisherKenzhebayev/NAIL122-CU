#ifndef MCTS_H
#define MCTS_H

#include <queue>
#include <vector>

using namespace std;

// 1. Write a game functionality
// 2. Expand to a MCTS model pipeline

class Coordinate
{
	public:
	Coordinate(int c1, int c2) {
		x = c1;
		y = c2;
	}

	int x;
	int y;
};

enum class PlayerSide
{
	BLACK = 0,
	WHITE = 1
};

enum class Color
{
	E = 0,
	W = 1,
	B = 2,
};

class GameState
{
public:

	GameState() {
		N = 9;
		board = vector<Color>(N * N, Color::E);
		terminal = false;
		p1Score = 0;
		p2Score = 0;
		pTurn = PlayerSide::BLACK;
	}

	GameState(int n) {
		N = n;
		board = vector<Color>(N * N, Color::E);
		terminal = false;
		p1Score = 0;
		p2Score = 0;
		pTurn = PlayerSide::BLACK;
	}

	int ConvertToArray(int x, int y) {
		return x * N + y;
	}

	Coordinate ConvertToCoordinate(int i) {
		return Coordinate(i / N, i % N);
	}

	bool IsTerminal() {
		return terminal;
	}

	Color inverseColor(Color c) {
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

	void ResetBoard() {
		for (int i = 0; i < board.size(); i++) {
			board[i] = Color::E;
		}
	}

	void PlayStone(Coordinate c, Color stone) {
		//TODO: Add all the rules here, etc.
		board[ConvertToArray(c.x, c.y)] = stone;
	}

  private:
	int N;
	bool terminal;
	double p1Score, p2Score;
	std::vector<Color> board;
	PlayerSide pTurn;
	//unsigned int numberOfSimulations;
	//queue<GameMove *> *untriedActions;
};

/// <summary>
/// Supposed to keep track of current turns, current GameState, is the game over or not
/// </summary>
class GameStatus
{
  public:
	GameStatus(int n) {
		N = n;
		actionsCount = 0;
		currentTurn = PlayerSide::BLACK;
		gameState = GameState(n);
		finished = false;
	}

	PlayerSide CurrentTurn() {
		return currentTurn;
	}

	GameState CurrentState() {
		return gameState;
	}

	void PlayTurn() {
		if (currentTurn == PlayerSide::BLACK)
		{
			currentTurn = PlayerSide::WHITE;
		}
		else
		{
			currentTurn = PlayerSide::BLACK;
		}
	}

	void ResetGame() {
		actionsCount = 0;
		currentTurn = PlayerSide::BLACK; // Black side moves first
		gameState.ResetBoard();
		finished = false;
	}

	bool IsFinished() {
		return finished;
	}

  private:
	int N;
	int actionsCount;
	PlayerSide currentTurn;
	GameState gameState;
	bool finished;
};

#endif 
