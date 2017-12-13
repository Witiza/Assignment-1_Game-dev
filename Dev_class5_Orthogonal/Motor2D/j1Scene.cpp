#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "Window.h"
#include "j1Map.h"
#include "InheritedLabel.h"
#include "j1Scene.h"
#include "j1SceneChange.h"
#include "j1Pathfinding.h"
#include "j1EntityController.h"
#include "j1Gui.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");

	fade_time = config.child("fade_time").attribute("value").as_float();

	for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		p2SString* data = new p2SString;

		data->create(map.attribute("name").as_string());
		map_names.add(data);
	}
	
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{	
	pugi::xml_document	config_file;
	pugi::xml_node		config;

	config = App->LoadConfig(config_file);

	SDL_Rect temp;
	temp.x = 250;
	temp.y = 5;
	temp.w = 0;
	temp.h = 0;

	p2SString tmp("Score: %i",App->gui->scoreNumber);
	App->gui->currentScore = App->gui->AddLabel(temp, { 0,0 }, "fonts/Old School Adventures.ttf", { 255,255,255,255 }, tmp.GetString());

	temp.x = 550;
	p2SString tmp2("Time: 0");
	App->gui->timeLabel = App->gui->AddLabel(temp, { 0,0 }, "fonts/Old School Adventures.ttf", { 255,255,255,255 }, tmp2.GetString(), 24);


	to_end = false;
	bool ret = App->map->Load_map(map_names.start->data->GetString());
	App->audio->PlayMusic(App->map->data.musicFile.GetString());
	LOG("Boi: %s", map_names.start->data->GetString());

	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
		App->pathfinding->SetMap(w, h, data);

	debug_tex = App->tex->Load("maps/Navigable.png");

	currentMap = 0;
	Entity* player = App->entitycontroller->AddEntity(Entity::entityType::PLAYER, { 0,0 });
	player->Awake(config.child(App->entitycontroller->name.GetString()));

	player->Start();

	SpawnEnemies();

	pugi::xml_document	Gui_config_file;
	pugi::xml_node		guiconfig;

	guiconfig = App->LoadConfig(Gui_config_file, "Gui_config.xml");

	guiconfig = guiconfig.child("scene");

	App->gui->Load_UIElements(guiconfig, this);
	App->gui->Load_SceneWindows(guiconfig, this);

	transcurredTime.Start();

	return true;

}

// Called each loop iteration
bool j1Scene::PreUpdate()
{


	p2SString temp("Time: %i", (int)transcurredTime.ReadSec());
	if(pastFrameTime != (int)transcurredTime.ReadSec())
		App->gui->timeLabel->ChangeText(&temp);

	pastFrameTime = (int)transcurredTime.ReadSec();

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene update", Profiler::Color::Black);
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		Load_lvl(0) , currentMap = 0;

	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->entitycontroller->Restart();

	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		Load_lvl(1) ,  currentMap = 1;

	else if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->LoadGame();

	else if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		App->SaveGame();

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		App->map->debug = !App->map->debug;

	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		App->audio->LowerMusicVolume();

	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		App->audio->RaiseMusicVolume();

	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		App->audio->LowerSFXVolume();

	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->audio->RaiseSFXVolume();

	else if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		slowing = true;
	
	else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		App->entitycontroller->godmode = !App->entitycontroller->godmode;



	//else if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
		//App->pathfinding->CreatePath({test.x + 20, test.y + 5}, { test.x, test.y });

	//if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN && !App->scenechange->IsFading())
		//App->scenechange->ChangeScene(map_names[currentMap], 1.0f);

	//if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && !App->scenechange->IsFading())
		//App->scenechange->ChangeScene(map_names[OuterWorld], 1.0f);

	App->map->Draw();
	App->entitycontroller->Draw();
	App->gui->Draw();

	

	//int x, y;
	//App->input->GetMousePosition(x, y);
	//iPoint p = App->render->ScreenToWorld(x, y);
	//p = App->map->WorldToMap(p.x, p.y);
	//p2SString title("%i %i", p.x, p.y);

	//p = App->map->MapToWorld(p.x, p.y);

	//App->render->Blit(debug_tex, p.x, p.y);

	//const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	//for (uint i = 0; i < path->Count(); ++i)
	//{
	//	iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
	//	App->render->Blit(debug_tex, pos.x, pos.y, 0, SDL_FLIP_NONE, 0);
	//}

	//App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (to_end && !App->scenechange->IsChanging())
	{
		App->entitycontroller->ChangeMapEnemies();
		
		if(currentMap < map_names.count() - 1)
			ret = App->scenechange->ChangeMap(++currentMap, fade_time);
		else
			currentMap = 0, ret = App->scenechange->ChangeMap(currentMap, fade_time);


		to_end = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		pause = !pause;
		if (pause)
			OpenPauseMenu();
		else
			ClosePauseMenu();
	}
	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool j1Scene::OnEvent(UIElement* element, int eventType)
{
	bool ret = true;

	element->HandleAnimation(eventType);

	if (eventType == EventTypes::PRESSED_ENTER && element->type == InteractiveType::CLOSE_WINDOW)
	{
		ret = element->OnEvent();
		pause = !pause;
	}

	if (element->type == InteractiveType::CLOSE_WINDOW && eventType == EventTypes::LEFT_MOUSE_PRESSED)
		pause = false, ClosePauseMenu();

	return ret;
}

bool j1Scene::Load(pugi::xml_node& data)
{

	if (currentMap != data.child("currentMap").attribute("num").as_int())
	{
		LOG("Calling switch maps");
		currentMap = data.child("currentMap").attribute("num").as_int();
		App->map->SwitchMaps(map_names[data.child("currentMap").attribute("num").as_int()]);

	}
	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const
{
	data.append_child("currentMap").append_attribute("num") = currentMap;
	return true;
}

bool j1Scene::Load_lvl(int time)
{
	App->map->SwitchMaps(map_names[time]);
	App->entitycontroller->ChangeMapEnemies();
	App->entitycontroller->Restart();
	App->scene->SpawnEnemies();
	return true;
}

void j1Scene::OpenPauseMenu()
{
	if(sceneMenu)
	sceneMenu->active = true;
}

void j1Scene::ClosePauseMenu()
{
	if (sceneMenu)
	sceneMenu->active = false;
}

void j1Scene::SpawnEnemies()
{
	for (p2List_item<ObjectsGroup*>* obj = App->map->data.objLayers.start; obj; obj = obj->next)
	{
		if (obj->data->name == ("Enemies"))
		{
			for (p2List_item<ObjectsData*>* objdata = obj->data->objects.start; objdata; objdata = objdata->next)
			{
				if (objdata->data->name == 7)
				{
					App->entitycontroller->AddEntity(Entity::entityType::FLYING_ENEMY, { objdata->data->x,objdata->data->y });
				}

				else if (objdata->data->name == 8)
				{
					App->entitycontroller->AddEntity(Entity::entityType::LAND_ENEMY, { objdata->data->x,objdata->data->y });
				}

				else if (objdata->data->name == 9)
				{
					App->entitycontroller->AddEntity(Entity::entityType::PICKUP, { objdata->data->x,objdata->data->y });
				}
				
			}
		}
	}
}

