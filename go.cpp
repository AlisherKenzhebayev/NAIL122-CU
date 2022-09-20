#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "go.h"

using namespace std;

const int TERM_THRES = 2; // Considers all less as a terminal state / finished game
const int SKIP_THRES = 10; // Allows skips from this point

default_random_engine Go_state::generator = default_random_engine(time(NULL));

Go_state::Go_state()
    : state_(NULL), turn_(PlayerSide::BLACK), moveCounter_(0), skipCounter_(0)
{
	state_ = &GameState();
}

Go_state::Go_state(const Go_state &other)
    : state_(NULL), turn_(other.turn_), moveCounter_(other.moveCounter_), skipCounter_(other.skipCounter_)
{
	GameState t = *other.state_;
	state_ = &t;
}

Go_state::~Go_state()
{
	// TODO: look at mem leaks, if any. Rewrite if needed
	state_ = NULL;
}

char Go_state::check_winner() const
{
	if (state_->CheckForTerminationCondition(turn_, TERM_THRES))
	{
		if (turn_ == PlayerSide::BLACK) {
			return 'W';	
		}
		else
		{
			return 'B';	
		}
	}
	return ' ';
}

bool Go_state::move_skip_once()
{
	// Increment the skips
	skipCounter_ += 1;
	// change turn
	change_turn();
	// add to move counter
	moveCounter_++;
}

bool Go_state::legal_move(const Go_move *move) {
	if (state_->IsActionValid(move->c_, move->player_))
	{
		return true;
	}

	return false;
}

bool Go_state::play_move(const Go_move *move) {
	if (move == NULL || !legal_move(move))
	{
		cout << "Invalid command: Illegal move: " << ((move != NULL) ? move->sprint() : "NULL") << endl << endl;
		return false;
	}

	// Comes from the chunk of code in game.cpp, simulating the play on the state_
	{
		pair<int, int> retVal = pair<int, int>(0, 0);

		// Play the stone
		state_->PlayStone(move->c_, move->player_);

		// Remove new captures from the board, including suicides
		retVal = state_->ProcessNeighborStones(move->c_, move->player_);
	}

	// Move is valid, so reset skip counter
	skipCounter_ = 0;
	// change turn
	change_turn();
	// add to move counter
	moveCounter_++;
	return true;
}

///////////////////////////////////////////////////////////////////////////
bool Go_state::is_terminal() const {
	if (check_winner() != ' ')
	{
		// Win was found
		return true;
	}
	if (skipCounter_ >= 2) {
		// Double skip was done, exiting
		return true;
	}

	return false;
}

MCTS_state *Go_state::next_state(const MCTS_move *move) const
{
	// TODO:
}

queue<MCTS_move *> *Go_state::actions_to_try() const {

	// TODO: Branching for randomness, as the threshold allows
	queue<MCTS_move *> *Q = new queue<MCTS_move *>();
}

// the rollout simulation in MCTS
double Go_state::rollout() const
{
	// TODO:
}

void Go_state::print() const
{
	// As i already defined the outstream functionality before
	cout << (*state_);
	cout << endl << endl;
}
