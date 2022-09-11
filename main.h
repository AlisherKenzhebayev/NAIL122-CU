#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "mcts.h"
#include "GridPool.h"
#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <ncine/Sprite.h>
#include <ncine/SceneNode.h>
#include <ncine/TextNode.h>
#include <ncine/Font.h>
#include <ncine/Texture.h>
#include <nctl/UniquePtr.h>
#include <nctl/String.h>

namespace ncine {

class AppConfiguration;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler :
    public nc::IAppEventHandler,
    public nc::IInputEventHandler
{
  private:
	GameState rootState_;
	GameStatus gameStatus_;
	//PlayerMovement playerMovement_;

	nctl::UniquePtr<nc::SceneNode> dummy_;

	// Board draw
	nctl::UniquePtr<nc::Texture> gridTexture_;
	nctl::UniquePtr<nc::Texture> whiteTexture_;
	nctl::UniquePtr<nc::Texture> blackTexture_;
	nctl::UniquePtr<nc::Texture> emptyTexture_;
	nctl::UniquePtr<nc::Texture> boardTexture_;

	// Functional Sprites
	nctl::UniquePtr<nc::Sprite> boardSprite_;

	// Text
	nctl::String screenString_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> debugText_;

	// SpritePools for a grid (board setup) and stones of different colors
	nctl::UniquePtr<GridPool> gridPool_;
	nctl::UniquePtr<GridPool> stonePool_;

  public:
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
};

#endif
