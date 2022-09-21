#ifndef MCTS_H
#define MCTS_H

#include <queue>
#include <set>
#include <vector>
#include <stdio.h>
#include <iostream>

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

	bool operator<(const Coordinate &rhs) const noexcept
	{
		if (this->x == rhs.x) {
			return this->y < rhs.y;
		}
		else
		{
			return this->x < rhs.x;
		}
	}
	
	bool operator==(const Coordinate &rhs) const noexcept
	{
		if (this->x == rhs.x && this->y == rhs.y)
		{
			return true;
		}
		return false;
	}

	string to_string() {

	}
};

enum class PlayerSide
{
	BLACK = 0,
	WHITE = 1,
};

enum class Color
{
	E = 0,
	W = 1,
	B = 2,
	Size_ = 3,
};

struct ChainReached
{
	set<Coordinate> Chain;
	set<Coordinate> Reached;
};

class GameState
{
public:
	GameState();
	GameState(int n, Coordinate ko);

	int ConvertToArray(int x, int y);
	static Coordinate ConvertToCoordinate(int i, int N_);
	Color inverseColor(Color c);
	void PlayStone(Coordinate c, PlayerSide side);
	void ResetBoard();
	bool IsActionValid(Coordinate c, PlayerSide side);
	bool IsOccupiedCell(Coordinate c);
	bool IsKoRepeated(Coordinate c);
	bool IsSuicide(Coordinate c, PlayerSide side);
	Coordinate IsThisKoCandidate(Coordinate c, vector<Coordinate> candidates, PlayerSide side);
	bool IsOnBoard(Coordinate c);
	ChainReached FindFloodFill(Coordinate c);
	vector<Coordinate> GetValidNeighbors(Coordinate c);
	pair <int, int> ProcessNeighborStones(Coordinate c, PlayerSide side);
	set<Coordinate> TryCaptureStones(Coordinate c);
	void PlaceSetStones(Color col, set<Coordinate> chain);
	void AddScore(pair<int, int> *score, Color col, int size);
	bool CheckForTerminationCondition(PlayerSide side, int k);	// Used in MCTS to define "early stopping" conditions + enables skipping in MCTS
	vector<Coordinate> GetAllValidActions(PlayerSide side);
	pair<int, int> ScoreCurrentStateFinal();

	std::vector<Color> GetBoardState()
	{
		std::cout << *this << "\n";
	
		return board_;
	}

	bool IsTerminal() {
		return terminal_;
	}

	void GameTerminalStage(bool set)
	{
		terminal_ = set;
	}
	
	GameState &operator=(GameState const &copy)
	{
		N_ = copy.N_;
		terminal_ = copy.terminal_;
		
		// Ensuring a copy, not a reference
		board_ = vector<Color>();
		board_ = copy.board_;

		// This copy is ok, as it only carries over the chached neighbors positions
		neighbors_ = copy.neighbors_;
		lastKo_ = copy.lastKo_;

		return *this;
	}

	Coordinate lastKo_;

	friend ostream &operator<<(std::ostream &s, const GameState &state)
	{
		string out = "";

		for (int i = 0; i < state.board_.size(); i++) {
			if (i % state.N_ == 0 && i != 0) {
				out += "\n";
			}

			if (state.board_[i] == Color::B) {
				out += "X";
			}
			else if (state.board_[i] == Color::W)
			{
				out += "O";
			}
			else
			{
				out += "_";
			}
		}
		return s << out;
	}

  private:
	int N_;
	bool terminal_;
	vector<Color> board_;
	vector<vector<Coordinate>> neighbors_;	// Responsible for caching the neighbors operation only.

	void InitNeighbors(int n)
	{
		if (neighbors_.size() != 0) {
			return;
		}

		for (int i = 0; i < N_ * N_; i++)
		{
			neighbors_.push_back(GetValidNeighbors(ConvertToCoordinate(i, N_)));
		}
	}
};

/// <summary>
/// Supposed to keep track of current turns, current GameState, is the game over or not
/// TODO: Write addtional / turn *this into a MCTS 
/// </summary>
class GameStatus
{
  public:
	GameStatus();
	GameStatus(GameState state);

	pair<int, int> GameStatus::PlayTurn(Coordinate c);
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
