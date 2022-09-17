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
	// TODO: This here needs abstraction through a game model simulation

	// Copy the game state
	GameState copyState = gameState_;

	// Play the stone
	copyState.PlayStone(c, currentTurn_);

	// Remove new captures from the board, including suicides
	copyState.ProcessNeighborStones(c, currentTurn_);

	// Get the state back to the AI node
	gameState_ = copyState;

	// Switch
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
    : GameState(9, Coordinate(-1,-1))
{
}


GameState::GameState(int n, Coordinate ko)
    : N_(n), lastKo_(ko)
{
	board_ = vector<Color>(N_ * N_, Color::E);
	terminal_ = false;
	p1Captures_ = 0;
	p2Captures_ = 0;

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

bool GameState::CheckForTerminalStart() {
	//TODO: implement here a check for a boolean, to see if the state is allowed to move onto "terminal pass" moves

	return false;
}

set<Coordinate> GameState::TryCaptureStones(Coordinate c) {
	set<Coordinate> retVal;

	ChainReached chain = FindFloodFill(c);
	bool isCaptured = true;

	for (auto i = chain.Reached.begin(); i != chain.Reached.end(); i++) {
		if (board_[ConvertToArray((*i).x, (*i).y)] == Color::E)
		{
			isCaptured = false;
		}
	}

	if (isCaptured) {
		PlaceSetStones(Color::E, chain.Chain);
		retVal = chain.Chain;
	}

	return retVal;
}

void GameState::PlaceSetStones(Color col, set<Coordinate> chain) {
	for (auto i = chain.begin(); i != chain.end(); i++)
	{
		board_[ConvertToArray((*i).x, (*i).y)] = col;
	}

	return;
}

/// <summary>
/// Given a state and a "latest move" coordinate with a context of side, process all chains/stones nearby
/// This operation also includes the removal of the suicide.
/// </summary>
void GameState::ProcessNeighborStones(Coordinate c, PlayerSide side) {
	GameState copyState = *this;

	Color opponentColor = side == PlayerSide::BLACK ? Color::W : Color::B;
	vector<Coordinate> oppStones;
	vector<Coordinate> myStones;
	vector<Coordinate> neighbors = neighbors_[ConvertToArray(c.x, c.y)];
	for (int i = 0; i < neighbors.size(); i++) {
		if (copyState.board_[ConvertToArray(neighbors[i].x, neighbors[i].y)] == opponentColor)
		{
			oppStones.push_back(neighbors[i]);
		}
		else if (copyState.board_[ConvertToArray(neighbors[i].x, neighbors[i].y)] != Color::E)
		{
			myStones.push_back(neighbors[i]);
		}
	}

	int opCaptured = 0;
	vector<Coordinate> koCandidates;

	for (int i = 0; i < oppStones.size(); i++)
	{
		set<Coordinate> captures = copyState.TryCaptureStones(oppStones[i]);
		opCaptured += captures.size();

		if (captures.size() == 1) {
			koCandidates.push_back(*captures.begin());
		}
	}

	// Check self-capture/suicide
	copyState.SuicideCapture(c);

	// Check for a Ko situation occuring
	copyState.lastKo_ = Coordinate(-1, -1);
	if (opCaptured == 1)
	{
		Coordinate potentialKo = this->IsThisKoCandidate(c, koCandidates, side);
		// TODO: immediate Ko checks
		if (copyState.IsOnBoard(potentialKo))
		{
			// Ko coordinate is a valid one
			// Backlog - TODO: rewrite in another way
			copyState.lastKo_ = potentialKo;
		}
	}

	*this = copyState;
}

/// <summary>
/// Assumes the current position of the board with a coordinate c to be already in action when evaluating. Only checks and resolves self-capture
/// </summary>
bool GameState::SuicideCapture(Coordinate c) {
	set<Coordinate> captures = TryCaptureStones(c);
	if (captures.size() != 0) {
		return true;
	}

	return false;
}

/// <summary>
/// Only plays the stone on the current state's board
/// </summary>
void GameState::PlayStone(Coordinate c, PlayerSide side)
{
	GameState retVal;

	//TODO: WRONG! - This needs to be in simulation code. The rule itself are in a separate methods
	// Add all the rules here, etc.
	// with checks, generating next section of the board at once
	// Q: Do I need to recalculate the score of players each generation?
	//TODO: Add suicide rule implementation
	//TODO: Add ko rule implementation (repeat not allowed)

	if (side == PlayerSide::BLACK)
		board_[ConvertToArray(c.x, c.y)] = Color::B;
	else 
		board_[ConvertToArray(c.x, c.y)] = Color::W;
}

/// <summary>
/// Given the current state, will this move be considered legal?
/// </summary>
bool GameState::IsActionValid(Coordinate c, PlayerSide side)
{
	//TODO: define what is valid game action (placement of stone)? according to the rules
	// 1. The players may choose any unoccupied intersection
	// 3. except for those forbidden by the suicide rules (see below). 
	// 2. except for those forbidden by the ko

	if (IsOccupiedCell(c)) {
		return false;
	}

	if (IsKoRepeated(c)) {
		return false;
	}

	if (IsSuicide(c, side)) {
		return false;
	}

	return true;
}

/// <summary>
/// Given the state, with a simulation of stone removal, determines if the placement will result in a removal of this particular placement right afterwards 
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
bool GameState::IsSuicide(Coordinate c, PlayerSide side) {
	GameState copyState = *this;
	// 1. Play the stone
	copyState.PlayStone(c, side);
	// 2. Remove the stones, suicide including
	copyState.ProcessNeighborStones(c, side);
	// 3. Check if the cell c is empty after the stones processing

	if (copyState.board_[ConvertToArray(c.x, c.y)] == Color::E) {
		return true;
	}

	return false;
}

/// <summary>
/// Given the state, new stone placement, guaranteed single capture, will this result in a capture of a ONE stone from all similarly colored in that "eye"
/// </summary>
/// <returns> Coordinate (-1, -1) if not </returns>
Coordinate GameState::IsThisKoCandidate(Coordinate c, vector<Coordinate> candidates, PlayerSide side)
{
	GameState copyState = *this;
	int colors[(int)Color::Size_] = {0};

	vector<Coordinate> neighbors = copyState.neighbors_[ConvertToArray(c.x, c.y)];
	for (auto i = 0; i < neighbors.size(); i++)
	{
		// Record the colors count
		Color cell = copyState.board_[ConvertToArray(neighbors[i].x, neighbors[i].y)];
		colors[(int)cell]++;
	}

	int differentColors = 0;
	for (auto i = 0; i < (int)Color::Size_; i++) {
		if (colors[i] > 0) {
			differentColors++;
		}
	}

	// If the N of surrounding the "eye" colors == 1
	if (differentColors == 1) {
		// We have a Ko member
		return candidates[0];
	}

	// Otherwise
	return Coordinate(-1, -1);
}	

/// <summary>
/// Only checks if the move is a repeat of a previously existing Ko
/// </summary>
bool GameState::IsKoRepeated(Coordinate c)
{
	if (lastKo_ == c)
		return true;
	return false;
}

bool GameState::IsOccupiedCell(Coordinate c)
{
	if (board_[ConvertToArray(c.x, c.y)] == Color::E)
		return false;
	return true;
}

bool GameState::IsOnBoard(Coordinate c) {
	if (c.y >= 0 && c.x >= 0 &&
		c.x % N_ == c.x && c.y % N_ == c.y)
	{
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

// Finds the connected chain for the coordinate of one of its stones
ChainReached GameState::FindFloodFill(Coordinate c) {
	ChainReached retVal;
	Color color = board_[ConvertToArray(c.x, c.y)];
	retVal.Chain.insert(c);
	queue<Coordinate> frontier;
	frontier.push(c);
	while (!frontier.empty())
	{
		Coordinate currentC = frontier.front();
		frontier.pop();

		retVal.Chain.insert(currentC);
		vector<Coordinate> neighbors = neighbors_[ConvertToArray(currentC.x, currentC.y)];
		for (auto i = 0; i < neighbors.size(); i++)
		{
			if (board_[ConvertToArray(neighbors[i].x, neighbors[i].y)] == color && retVal.Chain.find(neighbors[i]) == retVal.Chain.end())
			{
				frontier.push(neighbors[i]);
			}
			else if (board_[ConvertToArray(neighbors[i].x, neighbors[i].y)] != color)
			{
				retVal.Reached.insert(neighbors[i]);
			}
		}
	}
	return retVal;
}
