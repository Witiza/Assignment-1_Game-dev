#ifndef  _MODULE_SCENECHANGE_
#define _MODULE_SCENECHANGE_

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

enum ListOfMapNames;

class j1SceneChange : public j1Module
{
public:
	j1SceneChange();
	virtual ~j1SceneChange();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool ChangeScene(int newMap, float time);

	bool IsFading() const;

private:
	
public:

	bool fading = false;

private:
	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	
	int nextMap;
	uint start_time = 0;
	uint total_time = 0;
	SDL_Rect screen;


};
#endif // ! _MODULE_SCENECHANGE_
