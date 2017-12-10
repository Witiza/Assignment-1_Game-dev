#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include "p2Point.h"
#include "j1App.h"
#include "j1Gui.h"

class UIElement
{
public:
	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate() { return true; }
	virtual bool Draw() { return true; }
	virtual bool HandleAnimation(int eventType) { return true; }

	void MoveElement(iPoint difference);

	UIElement();
	UIElement(SDL_Rect & position, bool draggable = false);
	~UIElement();

public:
	UIType UItype = NO_TYPE;
	InteractiveType type = DEFAULT;
	SDL_Rect position;
	bool active = true;
	bool In_window = false;
	bool draggable = false;
	bool being_dragged = false;
	WinElement* winElement = nullptr;
};

#endif