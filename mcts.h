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
	GameState();
	GameState(int n);
	int ConvertToArray(int x, int y);
	static Coordinate ConvertToCoordinate(int i, int N_);
	Color inverseColor(Color c);
	void PlayStone(Coordinate c, Color stone);
	void ResetBoard();

	std::vector<Color> GetBoardState()
	{
		return board_;
	}

	bool isActionValid(Coordinate c, PlayerSide side)
	{
		//TODO:
		return true;
	}

	bool IsTerminal() {
		return terminal_;
	}

private:
	int N_;
	bool terminal_;
	double p1Score_, p2Score_;
	std::vector<Color> board_;
	PlayerSide pSide_;
	//unsigned int numberOfSimulations;
	//queue<GameMove *> *untriedActions;
};

/// <summary>
/// Supposed to keep track of current turns, current GameState, is the game over or not
/// </summary>
class GameStatus
{
  public:
	GameStatus();
	GameStatus(GameState state);

	void PlayTurn(Coordinate c);
	void ResetGame();

	PlayerSide CurrentTurn() {
		return currentTurn_;
	}

	GameState CurrentState() {
		return gameState_;
	}

	bool IsFinished() {
		return finished_;
	}

  private:
	int actionsCount_;
	PlayerSide currentTurn_;
	GameState gameState_;
	bool finished_;
};

#endif 
