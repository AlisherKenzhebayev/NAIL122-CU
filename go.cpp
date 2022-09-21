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
    : state_(NULL), turn_(PlayerSide::BLACK), moveCounter_(0), skipCounter_(0), colorCaptures_(pair<int, int>(0, 0))
{
	state_ = new GameState();
}

Go_state::Go_state(const Go_state &other)
    : state_(NULL), turn_(other.turn_), moveCounter_(other.moveCounter_), skipCounter_(other.skipCounter_), colorCaptures_(other.colorCaptures_)
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

bool Go_state::legal_move(const Go_move *move) {
	if (move->skip_) {
		return true;
	}

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

	if (move->skip_) {
		// Increment the skips
		skipCounter_ += 1;
	} else {
		// Comes from the chunk of code in game.cpp, simulating the play on the state_
		pair<int, int> retVal = pair<int, int>(0, 0);

		// Play the stone
		state_->PlayStone(move->c_, move->player_);

		// Remove new captures from the board, including suicides
		retVal = state_->ProcessNeighborStones(move->c_, move->player_);

		// Add capturesScore
		colorCaptures_.first += retVal.first;
		colorCaptures_.second += retVal.second;

		// Move is valid, so reset skip counter
		skipCounter_ = 0;
	}

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
	Go_state *new_state = new Go_state(*this);
	new_state->play_move((const Go_move *)move);
	return new_state;
}


Go_move *pick_random_move(Go_state &s, uniform_real_distribution<double> &dist, default_random_engine &gen)
{
	// In essence, the same process with method actions_to_try()
	PlayerSide turn = s.whose_turn();

	vector<Go_move *> *V = new vector<Go_move *>();

	// All legal moves as generated by the GameState
	vector<Coordinate> list = *s.all_actions();
	for (auto &move : list)
	{
		V->push_back(new Go_move(move, turn));
	}

	// Expand with skip
	if (s.skip_enabled())
	{
		V->push_back(new Go_move(true, turn));
	}

	int i = floor(dist(gen));

	return (*V)[i];
}

bool Go_state::skip_enabled() const {
	if (state_->CheckForTerminationCondition(turn_, SKIP_THRES)) {
		return true;
	}
	return false;
}

vector<Coordinate> *Go_state::all_actions() const {
	return &(state_->GetAllValidActions(turn_));
}

queue<MCTS_move *> *Go_state::actions_to_try() const
{
	// TODO: Branching (pass move) for randomness, as the threshold allows
	queue<MCTS_move *> *Q = new queue<MCTS_move *>();

	// All legal moves as generated by the GameState
	vector<Coordinate> list = *all_actions();
	for (auto &move : list)
	{
		Q->push(new Go_move(move, turn_));
	}

	// Add the skip move IFF
	if (skip_enabled())
	{
		Q->push(new Go_move(true, turn_));
	}

	return Q;
}

// The playout/rollout simulation in MCTS
// The win is returned for a P1 (Blacks)
double Go_state::rollout() const
{
	// TODO
	#define MAXSTEPS 600 
	#define DDEBUG
	
    uniform_real_distribution<double> dist(0.0, 1.0);
	Go_state s(*this); // copy current state (bypasses const restriction and allows to change state)
	bool noerror;
#ifdef DDEBUG
	queue<Go_move*> hist;
#endif
	for (int i = 0; i < MAXSTEPS; i++)
	{
		// first check if terminal state. I approximate it in here
		if (s.is_terminal())
		{
			return (s.check_winner() == 'W') ? 1.0 : 0.0;
		}

		// Keep simulating until we do or reached a certain depth
		Go_move *m = pick_random_move(s, dist, generator);
		if (!s.legal_move(m))
		{
			cout << "Picked illegal move: " << ((m != NULL) ? m->sprint() : "NULL") << " intentionally! Move history:" << endl;
#ifdef DDEBUG
			while (!hist.empty())
			{
				Go_move *prev = hist.front();
				hist.pop();
				cout << prev->sprint() << endl;
			}
#endif
		}
#ifdef DDEBUG
		hist.push(m);
#endif
		noerror = s.play_move(m);
		if (!noerror)
		{
			cerr << "Error: in rollouts" << endl;
			break;
		}
#ifndef DDEBUG
		delete m;
#endif
	}

	auto scoreTerritory = state_->ScoreCurrentStateFinal();
	if (scoreTerritory.first - colorCaptures_.first > scoreTerritory.second - colorCaptures_.second) {
		// Black won
		return 1;
	}
	else if (scoreTerritory.first - colorCaptures_.first < scoreTerritory.second - colorCaptures_.second)
	{
		return 0;
	}
	else
	{
		// Draw
		return 0.5;
	}
}

void Go_state::print() const
{
	// As i already defined the outstream functionality before
	cout << (*state_);
	cout << endl << endl;
}
