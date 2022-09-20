#ifndef MCTS_QUORIDOR_H
#define MCTS_QUORIDOR_H

#include "state.h"
#include "game.h"
#include <forward_list>
#include <random>
#include <string>

struct Go_move : public MCTS_move
{
	Coordinate c_;
	PlayerSide player_;
	Go_move(Coordinate c, PlayerSide p)
	    : c_(c), player_(p)
	{}

	bool operator==(const MCTS_move &other) const override
	{
		const Go_move &o = (const Go_move &)other;
		return c_ == o.c_ && player_ == o.player_;
	}

	string sprint() const override
	{
		string playerstr = (player_ == PlayerSide::WHITE) ? "White" : "Black";
		return playerstr + " plays stone at - (" + to_string(c_.x) + ", " + to_string(c_.y);
	}
};

class Go_state : public MCTS_state
{
	// As I have the entire game implemented already, I'll just link the game state here, and copy it in the constructor for the Go_state
	GameState* state_;
	// Whose turn is it
	PlayerSide turn_;
	// Skip counter 
	int skipCounter_;
	// Moves played
	unsigned int moveCounter_;
	// randomness for rollouts
	static default_random_engine generator;

	//////////////////////////////////////////
	PlayerSide change_turn()
	{
		turn_ = (turn_ == PlayerSide::BLACK) ? PlayerSide::WHITE : PlayerSide::BLACK;
		return turn_;
	}

  public:
	Go_state();
	Go_state(const Go_state &other);
	~Go_state() override;

	PlayerSide whose_turn() const { return turn_; }
	unsigned int get_number_of_turns() const { return moveCounter_; }

	char check_winner() const;

	bool legal_move(const Go_move *move);
	bool move_skip_once();
	bool play_move(const Go_move *move);
	
	/** Overrides: **/
	bool is_terminal() const override;
	MCTS_state *next_state(const MCTS_move *move) const override;
	queue<MCTS_move *> *actions_to_try() const override;
	// the rollout simulation in MCTS
	double rollout() const override;
	void print() const override;
	// As we are still playing the Go, Blacks are first
	bool player1_turn() const override { return turn_ == PlayerSide::BLACK; }
};

#endif
