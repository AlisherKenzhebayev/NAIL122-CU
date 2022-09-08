#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <ncine/Sprite.h>
#include <ncine/Texture.h>
#include <nctl/UniquePtr.h>

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
	//GameState state_;
	//nctl::String screenString_;
	//PlayerMovement playerMovement_;

	// Board draw
	nctl::UniquePtr<nc::Texture> gridTexture_;
	nctl::UniquePtr<nc::Texture> whiteTexture_;
	nctl::UniquePtr<nc::Texture> blackTexture_;
	nctl::UniquePtr<nc::Texture> emptyTexture_;
	nctl::UniquePtr<nc::Texture> boardTexture_;
	
	nctl::UniquePtr<nc::Sprite> boardSprite_;
	//nctl::UniquePtr<GridPool> gridPool_;
	//nctl::UniquePtr<StonePool> stonePool_;

  public:
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
};

#endif
