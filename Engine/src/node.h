#pragma once

struct Node // A* Node
{
	int gCost; // Distance from starting node.
	int hCost; // Distance form end node.
	int fCost; // Final cost combining gCost and hCost.
	bool isObstacle; // Is this node obstacle?
};

inline bool operator< (const Node& left, const Node& right)
{
	return left.fCost < right.fCost;
}