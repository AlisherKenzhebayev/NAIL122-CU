#include "GridPool.h"
#include "mcts.h"

#include <ncine/Random.h>
#include <ncine/Application.h>
#include <ncine/Sprite.h>


// Square grid
GridPool::GridPool(unsigned int maxSize, nc::Texture *gridTexE, nc::Texture *gridTexB, nc::Texture *gridTexW, nc::Vector2f offset, nc::Vector2f size)
    : size_(0), rows_(0), columns_(0), width_(size.x), height_(size.y),
      gridTexE_(gridTexE), gridTexB_(gridTexB), gridTexW_(gridTexW), cells_(maxSize, gridTexB), offset_(offset)
{
	// The placement is in a square, scaling with app height
	rows_ = static_cast<int>(sqrtf(static_cast<float>(maxSize)));
	columns_ = rows_;
	
	size_ = static_cast<unsigned int>(columns_ * rows_);
	ASSERT(size_ > 0);

	reset();
}

// Draw an empty field
void GridPool::reset()
{
	cells_.reserve(size_);

	float cellSideVer = height_ / rows_;
	float cellSideHor = width_ / columns_;
	for (unsigned int i = 0; i < size_; i++)
	{
		// Offset placement
		nc::Vector2f position = offset_;

		position.x = position.x + cellSideHor * (i % columns_) + 0.5 * cellSideHor;
		position.y = position.y + height_ - cellSideVer * (i / columns_) - 0.5 * cellSideVer;

		cells_[i].setPosition(position);
		cells_[i].setTexture(gridTexE_);
		cells_[i].setSize(cellSideHor, cellSideVer);
		cells_[i].setEnabled(true);
	}

	for (unsigned int i = size_; i < cells_.totalSize(); i++)
		cells_[i].setEnabled(false);
}

// TODO: Add game draw method that takes in the game state, so that I dont draw blindly

void GridPool::draw()
{
	for (unsigned int i = 0; i < cells_.acquiredSize(); i++)
		cells_[i].setEnabled(true);
	for (unsigned int i = cells_.acquiredSize(); i < cells_.totalSize(); i++)
		cells_[i].setEnabled(false);
}

// Remove this s#*t, its unnecessary, as it modifies the visualization stage, and its fucking dumb
void GridPool::drawSingle(Coordinate c, Color stone)
{
	// Search for the coordinate using the indices. Hopefully these do not change.
	for (unsigned int i = 0; i < size_; i++) {
		auto coord = GameState::ConvertToCoordinate(i, size_);
		if (coord.x == c.x && coord.y == c.y) {
			// Have a matching cell, can change it
			if (stone == Color::B) {
				cells_[i].setTexture(gridTexB_);
			}
			else if (stone == Color::W)
			{
				cells_[i].setTexture(gridTexW_);
			}
			else
			{
				cells_[i].setTexture(gridTexW_);
			}
		}
	}
}
