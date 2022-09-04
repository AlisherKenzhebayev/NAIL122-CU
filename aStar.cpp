#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

#include "aStar.h"
#include "pong.h"

#include "ncine/Vector2.h"
#include "ncine/Rect.h"

using namespace std;

struct HeapNode
{
  public:
	int statesIndex;
	float cost;
	int depth;

	HeapNode &operator =(HeapNode const& copy) {
		statesIndex = copy.statesIndex;
		cost = copy.cost;
		depth = copy.depth;
		return *this;
	}
};

struct HeapNodeMinFirst
{
	bool operator ()(const HeapNode &a, const HeapNode &b) const
	{
		return a.cost > b.cost;
	}
};

struct Node
{
  public:
	gc::GameAction action;
	gamestate1::GameStateDynamic state;
	float cost;
	int depth;
	int stateIndex;
	int previousIndex;

	Node() {
		action = gc::GameAction::NONE;
		cost = 0;
		depth = 0;
		stateIndex = -1;
	}

	Node &operator =(Node const &copy) {
		action = copy.action;
		state = copy.state;
		cost = copy.cost;
		depth = copy.depth;
		stateIndex = copy.stateIndex;
		return *this;
	}

	bool Node::operator ==(const Node & node) const {
		return (((int)(state.ballDirX * 10) == (int)(node.state.ballDirX * 10))
			&& ((int)(state.ballDirY * 10) == (int)(node.state.ballDirY * 10))
			&& ((int)(state.ballX * 10) == (int)(node.state.ballX * 10))
			&& ((int)(state.ballY * 10) == (int)(node.state.ballY * 10))
			&& ((int)(state.p1Y * 10) == (int)(node.state.p1Y * 10))
			&& ((int)(state.p2Y * 10) == (int)(node.state.p2Y * 10))
			&& state.p1Score == node.state.p1Score
			&& state.p2Score == node.state.p2Score);
	}

	bool Node::operator <(const Node & node) const {
		return (((int)(state.ballDirX * 10) < (int)(node.state.ballDirX * 10))
			|| ((int)(state.ballDirY * 10) < (int)(node.state.ballDirY * 10))
			|| ((int)(state.ballX * 10) < (int)(node.state.ballX * 10))
			|| ((int)(state.ballY * 10) < (int)(node.state.ballY * 10))
			|| ((int)(state.p1Y * 10) < (int)(node.state.p1Y * 10))
			|| ((int)(state.p2Y * 10) < (int)(node.state.p2Y * 10))
			|| state.p1Score < node.state.p1Score
			|| state.p2Score < node.state.p2Score);
	}
};

/// <summary>
/// Has both HeapNode as Fringe heap, to take min
/// </summary>
class AStarHeap
{
  public:
	vector<HeapNode> *Fringe;
	vector<Node> *States;

	AStarHeap() {
		Fringe = new vector<HeapNode>();
		States = new vector<Node>();

		make_heap(Fringe->begin(), Fringe->end(), HeapNodeMinFirst());
	}

	virtual ~AStarHeap() {
		delete Fringe;
		delete States;
	}

	void Pop(HeapNode &result) {
		pop_heap(Fringe->begin(), Fringe->end(), HeapNodeMinFirst());
		result = Fringe->back();
		Fringe->pop_back();
	}

	void Push(Node &node) {
		node.stateIndex = States->size();
		States->push_back(node);

		HeapNode heapNode;
		heapNode.statesIndex = node.stateIndex;
		heapNode.cost = node.cost;
		heapNode.depth = node.depth;

		Fringe->push_back(heapNode);
		push_heap(Fringe->begin(), Fringe->end(), HeapNodeMinFirst());
	}

	void ChangeCost(Node &node) {
		for (int i = 0; i < Fringe->size(); i++)
		{
			if (Fringe->at(i).statesIndex == node.stateIndex)
			{
				Fringe->at(i).cost = node.cost;
				Fringe->at(i).depth = node.depth;

				make_heap(Fringe->begin(), Fringe->end(), HeapNodeMinFirst());
				return;
			}
		}
	}
};

namespace ai
{
	gc::GameAction AStar::PlayerThink(ForwardModel &forwardModel, GameStateStatic &s, GameStateDynamic &d){
	    AStarHeap heap;
	    map<Node, int> previous; // map of / hashset of nodes you have touched. Can it store a pointer??

		int maxDepth = 19;

		HeapNode heapNode;
	    Node currState;
	    Node nextState;

		currState.state = d;
	    currState.action = gc::GameAction::NONE;
	    currState.previousIndex = -1;
	    heap.Push(currState);
	    previous.emplace(currState, -1);

		gc::GameAction actions[2] = { gc::GameAction::DOWN, gc::GameAction::UP };

		bool timeout = false;
		while (!heap.Fringe->empty())
	    {
		    timeout = false;
		    heap.Pop(heapNode);
		    currState = heap.States->at(heapNode.statesIndex);

			//If goal reached, track back using stored States. This acts as an exit condition.
			if (currState.state.ballDirX > 0 || currState.state.p1Score > d.p1Score)
		    {
			    //Hit the ball\scored, as changed score for p1.
			    gc::GameAction action = currState.action;
			    while (currState.previousIndex >= 0)
			    {
				    action = currState.action;
				    currState = heap.States->at(currState.previousIndex);
			    }
			    return action;
		    }

			for (int i = 0; i < size(actions); ++i) {
				const gc::GameAction aiAction = actions[i];
				const gc::GameAction opAction = SimplifiedOpponent(s, currState.state);

				forwardModel.Simulate(s, currState.state, aiAction, opAction, nextState.state);

				if (currState.state.p2Score > d.p2Score) {
					// truncate the search
					continue;
				} 

			    if (currState.depth >= maxDepth)
			    {
				    // truncate the search if we are too deep
				    timeout = true;
				    continue;
			    }

				nextState.action = aiAction;
			    nextState.previousIndex = currState.stateIndex;


				// Estimate cost as min of corner distance
				nextState.cost = abs(s.p1X - nextState.state.ballX) +
			                     min(abs(nextState.state.p1Y + s.stickHeight / 2 - nextState.state.ballY),
			                         abs(nextState.state.p1Y - s.stickHeight / 2 - nextState.state.ballY)) +
								//Tries estimating from the inverse of a dist for the simulated opponent. So try to maximize the opponent's final distance
								//	(without accounting for collision and then with)
			                     EstimateOpponentBallDistance(s, nextState.state.p1Y, nextState.state.p2Y,
										nextState.state.ballX, nextState.state.ballY,
										nextState.state.ballDirX, nextState.state.ballDirY);

				nextState.depth = currState.depth + 1;
				auto torrent = previous.find(nextState);
			    if (torrent != previous.end()) {
				    if (torrent->first.cost <= nextState.cost) {
						// truncate the action branch if we are not improving
					    continue;
					}

					nextState.stateIndex = torrent->first.stateIndex;
				    previous.erase(nextState);

				    heap.ChangeCost(nextState);
				    previous.emplace(nextState, currState.stateIndex);
				}
				else
				{
				    heap.Push(nextState);
				    previous.emplace(nextState, currState.stateIndex);
			    }
			}
		}

		// Timeouts
	    if (timeout){
		    Node bestNode;
		    float minCost = numeric_limits<float>::max();

		    vector<Node>::iterator it;
			// Iterate over all recorded nodes, find the most promising one
		    for (auto i = 0; i < size(*heap.States); i++)
		    {
			    Node cNode = heap.States->at(i);
			    if (cNode.cost > 0 && cNode.cost < minCost)
			    {
				    minCost = cNode.cost;
				    bestNode = cNode;
				}
			}

			// Recover if it exists
		    if (bestNode.stateIndex >= 0)
		    {
				currState = heap.States->at(bestNode.stateIndex);

			    gc::GameAction action = currState.action;
			    while (currState.previousIndex >= 0)
			    {
				    action = currState.action;
				    currState = heap.States->at(currState.previousIndex);
			    }
			    return action;
			}
		}

		// Complete failure
	    return gc::GameAction::NONE;
	}

	/// <summary>
	/// Estimate of the ball distance for the opponent after bouncing back to the X level with it.
	/// Very simplified, not accounting for the bounces, etc.
	/// </summary>
    float AStar::EstimateOpponentBallDistance(GameStateStatic &s, float p1Y, float p2Y,
                                                       float ballX, float ballY,
                                                       float ballDirX, float ballDirY)
	{
	    float retVal = 100000;

		// As the player AI is assumed on the left
	    int collisionFramesLeft = abs(ballX - s.p1X) / (ballDirX * s.ballSpeed);
	    float ballMoveLeftY = (ballDirY * s.ballSpeed) * collisionFramesLeft;
		// Assuming perfect bounce
		int collisionFramesRight = abs(s.p2X - s.p1X) / (ballDirX * s.ballSpeed);
	    float ballMoveRightY = (ballDirY * s.ballSpeed) * collisionFramesRight;

		ballY += ballMoveLeftY;
	    ballY += ballMoveRightY;

		return retVal / abs(p2Y - ballY);
	}
	
	gc::GameAction AStar::OpponentThink(ForwardModel &forwardModel, GameStateStatic &s, GameStateDynamic &d)
    {
		// A bit more simplified, with no opponent estimation
	    AStarHeap heap;
	    map<Node, int> previous; // map of / hashset of nodes you have touched. Can it store a pointer??

	    int maxDepth = 19;

	    HeapNode heapNode;
	    Node currState;
	    Node nextState;

	    currState.state = d;
	    currState.action = gc::GameAction::NONE;
	    currState.previousIndex = -1;
	    heap.Push(currState);
	    previous.emplace(currState, -1);

	    gc::GameAction actions[2] = { gc::GameAction::DOWN, gc::GameAction::UP };

	    bool timeout = false;
	    while (!heap.Fringe->empty())
	    {
		    timeout = false;
		    heap.Pop(heapNode);
		    currState = heap.States->at(heapNode.statesIndex);

		    //If goal reached, track back using stored States. This acts as an exit condition.
		    if (currState.state.ballDirX < 0 || currState.state.p2Score > d.p2Score)
		    {
			    //Hit the ball\scored, as changed score for p1.
			    gc::GameAction action = currState.action;
			    while (currState.previousIndex >= 0)
			    {
				    action = currState.action;
				    currState = heap.States->at(currState.previousIndex);
			    }
			    return action;
		    }

		    for (int i = 0; i < size(actions); ++i)
		    {
			    const gc::GameAction aiAction = actions[i];
			    const gc::GameAction opAction = SimplifiedPlayer(s, currState.state);

			    forwardModel.Simulate(s, currState.state, opAction, aiAction, nextState.state);

			    if (currState.state.p1Score > d.p1Score)
			    {
				    // truncate the search
				    continue;
			    }

			    if (currState.depth >= maxDepth)
			    {
				    // truncate the search if we are too deep
				    timeout = true;
				    continue;
			    }

			    nextState.action = aiAction;
			    nextState.previousIndex = currState.stateIndex;

			    // Estimate cost as min of corner distance
			    nextState.cost = abs(s.p2X - nextState.state.ballX) +
			                     min(abs(nextState.state.p2Y + s.stickHeight / 2 - nextState.state.ballY),
			                         abs(nextState.state.p2Y - s.stickHeight / 2 - nextState.state.ballY));
			    nextState.depth = currState.depth + 1;
			    
			    auto torrent = previous.find(nextState);
			    if (torrent != previous.end())
			    {
				    if (torrent->first.cost <= nextState.cost)
				    {
					    // truncate the action branch if we are not improving
					    continue;
				    }

				    nextState.stateIndex = torrent->first.stateIndex;
				    previous.erase(nextState);

				    heap.ChangeCost(nextState);
				    previous.emplace(nextState, currState.stateIndex);
			    }
			    else
			    {
				    heap.Push(nextState);
				    previous.emplace(nextState, currState.stateIndex);
			    }
		    }
	    }

	    // Timeouts
	    if (timeout)
	    {
		    Node bestNode;
		    float minCost = numeric_limits<float>::max();

		    vector<Node>::iterator it;
		    // Iterate over all recorded nodes, find the most promising one
		    for (auto i = 0; i < size(*heap.States); i++)
		    {
			    Node cNode = heap.States->at(i);
			    if (cNode.cost > 0 && cNode.cost < minCost)
			    {
				    minCost = cNode.cost;
				    bestNode = cNode;
			    }
		    }

		    // Recover if it exists
		    if (bestNode.stateIndex >= 0)
		    {
			    currState = heap.States->at(bestNode.stateIndex);

			    gc::GameAction action = currState.action;
			    while (currState.previousIndex >= 0)
			    {
				    action = currState.action;
				    currState = heap.States->at(currState.previousIndex);
			    }
			    return action;
		    }
	    }

	    // Complete failure
	    return gc::GameAction::NONE;
    }

    /// <summary>
    /// Reactive agent, simplified following the ball
    /// </summary>
    gc::GameAction AStar::SimplifiedOpponent(GameStateStatic &s, GameStateDynamic &d)
    {
	    if (d.p2Y > d.ballY)
	    {
		    return gc::GameAction::DOWN;
	    }
	    else
	    {
		    if (d.p2Y < d.ballY)
		    {
			    return gc::GameAction::UP;
		    }
		    return gc::GameAction::NONE;
	    }
    }

    /// <summary>
    /// Reactive agent, simplified following the ball
    /// </summary>
    gc::GameAction AStar::SimplifiedPlayer(GameStateStatic &s, GameStateDynamic &d)
    {
	    if (d.p1Y > d.ballY)
	    {
		    return gc::GameAction::DOWN;
	    }
	    else
	    {
		    if (d.p1Y < d.ballY)
		    {
			    return gc::GameAction::UP;
		    }
		    return gc::GameAction::NONE;
	    }
    }
}
