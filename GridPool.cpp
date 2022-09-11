#include "GridPool.h"
#include "mcts.h"

#include <ncine/Random.h>
#include <ncine/Application.h>
#include <ncine/Sprite.h>


// Square grid
GridPool::GridPool(unsigned int maxSize, nc::Texture *gridTexB, nc::Texture *gridTexW)
    : size_(0), height_(0), width_(0), xMin_(0.0f), xMax_(0.0f), yMin_(0.0f), yMax_(0.0f),
      gridTexB_(gridTexB), gridTexW_(gridTexW), cells_(maxSize, gridTexB)
	// TODO: expand to 3
{
	// The size is square, scaling with app height
	height_ = static_cast<int>(sqrtf(static_cast<float>(maxSize)));
	width_ = height_;
	
	size_ = static_cast<unsigned int>(width_ * height_);
	ASSERT(size_ > 0);

	reset();
}

void GridPool::reset()
{
	cells_.reserve(size_);

	float paddingX = (nc::theApplication().height()) / height_;
	for (unsigned int i = 0; i < size_; i++)
	{
		nc::Vector2f position;
		position.x = paddingX * (i % width_) + 0.5 * paddingX;
		position.y = nc::theApplication().height() - paddingX * (i / width_) - 0.5 * paddingX;
		cells_[i].setPosition(position);
		cells_[i].setTexture((i % 2) ? gridTexB_ : gridTexW_);
		cells_[i].setSize(paddingX, paddingX);
		cells_[i].setEnabled(true);
	}

	for (unsigned int i = size_; i < cells_.totalSize(); i++)
		cells_[i].setEnabled(false);
}

void GridPool::draw()
{
	for (unsigned int i = 0; i < cells_.acquiredSize(); i++)
		cells_[i].setEnabled(true);
	for (unsigned int i = cells_.acquiredSize(); i < cells_.totalSize(); i++)
		cells_[i].setEnabled(false);
}

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
