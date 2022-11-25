//
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>
using namespace std;

//Screen dimension constants
enum ScreenParams
{
	SCREEN_WIDTH = 1500,
	SCREEN_HEIGHT = 1000
};
//Button constants
enum ButtonParams
{
	BUTTON_WIDTH = 494,
	BUTTON_HEIGHT = 107
};
enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = BUTTON_SPRITE_MOUSE_OUT,
	BUTTON_SPRITE_TOTAL = 4
};
enum ButtonNameInMenu
{
	GAME,
	GALLERY,
	EXIT,
	COUNT_OF_BUTTONS = 3
};
enum ButtonInGallery
{
	BACK_GAL,
	NEXT,
	COUNT_BUT_GAL = 2
};
enum LevelSelectionButtonName
{
	LEVEL1,
	LEVEL2,
	LEVEL3,
	BACK,
	COUNT_OF_LEVELSELECTION_BUTTONS = 4
};
enum PuzzlepieceParams
{
	PUZZLEPIECE_WIDTH = 140,
	PUZZLEPIECE_HEIGHT = 140,
	PUZZLEPIECES_HOR = 5, // puzzle pieces horizontal count
	PUZZLEPIECES_VERT = 5, // puzzle pieces vertical count
	MARGIN = 20
};
enum PuzzlepieceSprites
{
	NOT_SELECTED = 0,
	SELECTED = 1,
	PUZZLEPIECE_SPRITE_TOTAL = 2
};
enum Window
{

	MENU_WIND,
	GAME_WIND,
	GALLERY_WIND,
	EXIT_WIND
};
enum KeyPress
{
	KEY_PRESS_DEFAULT,
	KEY_PRESS_UP,
	KEY_PRESS_DOWN,
	KEY_PRESS_LEFT,
	KEY_PRESS_RIGHT,
	KEY_PRESS_LEFT_CTRL,
	KEY_PRESS_Z
};
enum Pictures
{
	FIRST,
	SECOND,
	THIRD,
	COUNT_OF_PIC = 3
};
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;

/// <summary>
/// Texture wrapper class
/// </summary>
/// 
bool EscapeIsPressed(SDL_Event e)
{
	if (e.type == SDL_QUIT)
		return true;
	else if (e.type == SDL_KEYDOWN)
	{
		if (e.key.keysym.sym == SDLK_ESCAPE)
			return true;
	}
	return false;
}
class LTexture
{
private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;

public:
	//Initializes variables
	LTexture()
	{
		//Initialize
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
	//Deallocates memory
	~LTexture()
	{
		//Deallocate
		free();
	}
	//Loads image at specified path
	bool loadFromFile(string path)
	{
		//Get rid of preexisting texture
		free();

		//The final texture
		SDL_Texture* newTexture = NULL;

		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		}
		else
		{
			//Color key image
			SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

			//Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
			if (newTexture == NULL)
			{
				printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}
			else
			{
				//Get image dimensions
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		//Return success
		mTexture = newTexture;
		return mTexture != NULL;
	}
	//Deallocates texture
	void free()
	{
		//Free texture if it exists
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	}

	//Renders texture at given point
	void render(int x = 0, int y = 0, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		//Set clip rendering dimensions
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}
		//Render to screen
		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	}

	//Gets image dimensions
	int getWidth()
	{
		return mWidth;
	}
	int getHeight()
	{
		return mHeight;
	}

};
class Picture : public LTexture
{
private:
	int x;
	int y;
public:
	Picture() :Picture(0, 0)
	{
	}
	Picture(int x, int y)
	{
		setPosition(x, y);
	}
	void setX(int x)
	{
		this->x = x;
	}
	void setY(int y)
	{
		this->y = y;
	}
	void setPosition(int x, int y)
	{
		setX(x);
		setY(y);
	}
	void renderP()
	{
		render(x, y);
	}
};
/// <summary>
/// level selection window
/// </summary>
LTexture sth;
LTexture gallery_window;
/// <summary>
/// The intro images
/// </summary>
LTexture IntroTextures[2];

class LButton
{
private:
	//Top left position
	SDL_Point mPosition;
	//Currently used sprite
	LButtonSprite mCurrentSprite;
	SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
	LTexture gButtonSpriteSheetTexture;
	int height;
	int width;
public:
	//Initializes internal variables
	LButton() :LButton(BUTTON_WIDTH, BUTTON_HEIGHT, 0, 0)
	{
	}
	LButton(int width, int height, int x, int y)
	{
		setHeight(height);
		setWidth(width);
		setPosition(x, y);
		mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
	}

	/// <summary>
	/// Sets top left position
	/// </summary>
	/// <param name="x">X coordinate</param>
	/// <param name="y">Y coordinate</param>

	void SetMCurrentSprite(LButtonSprite sprite)
	{
		if (sprite >= LButtonSprite::BUTTON_SPRITE_MOUSE_OUT && sprite <= LButtonSprite::BUTTON_SPRITE_MOUSE_DOWN)
		{
			mCurrentSprite = sprite;
		}
	}
	void setPosition(int x, int y)
	{
		mPosition.x = x;
		mPosition.y = y;
	}
	void setHeight(int height)
	{
		this->height = height;
	}
	void setWidth(int width)
	{
		this->width = width;
	}
	/// <summary>
	/// Handles mouse event
	/// </summary>
	/// <param name="e">mouse event</param>
	/// <returns>returns button sprite number</returns>
	int handleEvent(SDL_Event* e)
	{
		int button_sprite = BUTTON_SPRITE_MOUSE_OUT;
		//If mouse event happened
		if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP || e->type == SDL_KEYDOWN)
		{
			//Get mouse position
			int x, y;
			SDL_GetMouseState(&x, &y);

			//Check if mouse is in button
			bool inside = true;

			//Mouse is left of the button
			if (x < mPosition.x
				|| x > mPosition.x + width
				|| y < mPosition.y
				|| y > mPosition.y + height)
				inside = false;

			//Mouse is outside button
			if (!inside)
			{
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
				button_sprite = BUTTON_SPRITE_MOUSE_OUT;
			}
			//Mouse is inside button
			else
			{
				//Set mouse over sprite
				switch (e->type)
				{
				case SDL_MOUSEMOTION:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
					button_sprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
					break;

				case SDL_MOUSEBUTTONDOWN:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
					button_sprite = BUTTON_SPRITE_MOUSE_DOWN;
					break;

				case SDL_MOUSEBUTTONUP:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
					button_sprite = BUTTON_SPRITE_MOUSE_UP;
					break;
				}
			}

		}
		return button_sprite;
	}

	//Shows button sprite
	void render()
	{
		//Show current button sprite
		gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
	}
	void free()
	{
		gButtonSpriteSheetTexture.free();
	}
	void Set_gSpriteClips(SDL_Rect sprite, int index)
	{
		gSpriteClips[index] = sprite;
	}
	SDL_Rect* Get_gSpriteClips(int index)
	{
		return &gSpriteClips[index];
	}
	bool loadButton(string link)
	{
		//Loading success flag
		bool success = true;

		//Load sprites
		if (!gButtonSpriteSheetTexture.loadFromFile(link))
		{
			printf("Failed to load button sprite texture!\n");
			success = false;
		}
		else
		{
			//Set sprites
			for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i)
			{
				gSpriteClips[i].x = 0;
				gSpriteClips[i].y = i * height;
				gSpriteClips[i].w = width;
				gSpriteClips[i].h = height;
			}
		}

		return success;
	}


};
class CurrentWindow abstract
{
protected:
	Picture texture_bg;
	int count_of_but;
	vector <LButton> button;
public:

	CurrentWindow()
	{

	}
	CurrentWindow(int x_but, int y_but, int margin, int height_but, int width_but, int count_of_but, bool vert_but = true)
	{

		this->count_of_but = count_of_but;
		for (int i = 0; i < count_of_but; i++)
		{
			LButton butt;
			button.push_back(butt);
		}
		for (int i = 0, y = y_but; i < count_of_but; i++, y += margin)
		{
			if (vert_but)
			{
				button[i].setPosition(x_but, y);
			}
			else
			{
				button[i].setPosition(x_but, y_but);
				x_but += margin;
			}
			button[i].setHeight(height_but);
			button[i].setWidth(width_but);
		}
	}
	//void SetPositionForGorizBut(int x_but, int y_but, int margin)
	//{
	//	for (int i = 0, j = x_but; i < count_of_but; i++, j += margin)
	//	{
	//		button[i].setPosition(j, y_but);
	//	}
	//}

	Picture GetTextureBG()
	{
		return texture_bg;
	}
	LButton* GetButton(int index)
	{
		return &button[index];
	}
	virtual bool LoadButton() = 0;
	virtual bool LoadBg() = 0;
	void Show_window()
	{
		static int count = 0;
		LoadBg();//show bg
		LoadButton();
		texture_bg.renderP();
		for (int i = 0; i < count_of_but; i++)
		{
			button[i].render();
		}
		SDL_RenderPresent(gRenderer);
		count++;
	}
	virtual int SelectedButtonInCurrentWindow(int button) = 0;
	int click_window()
	{
		vector <int> event_buttons;

		for (int i = 0; i < count_of_but; i++)
		{
			event_buttons.push_back(0);
		}
		bool quit = false;
		SDL_Event e;
		Show_window();
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				if (EscapeIsPressed(e))
					return SelectedButtonInCurrentWindow(EXIT_WIND);

				for (int i = 0; i < count_of_but; i++)
				{
					event_buttons[i] = button[i].handleEvent(&e);
				}
			}
			for (int i = 0; i < count_of_but; i++)
			{
				if (event_buttons[i] == LButtonSprite::BUTTON_SPRITE_MOUSE_DOWN)
				{
					for (int i = 0; i < count_of_but; i++)
					{
						button[i].render();
					}
					SDL_RenderPresent(gRenderer);
					SDL_Delay(200);
					//SDL_RenderClear(gRenderer);

					//texture_bg.free();
					//for (int i = 0; i < count_of_but; i++)
					//{
					//	button[i].free();
					//}
					button[i].SetMCurrentSprite(LButtonSprite::BUTTON_SPRITE_MOUSE_OUT);
					return SelectedButtonInCurrentWindow(i);
				}
				else if (event_buttons[i] == LButtonSprite::BUTTON_SPRITE_MOUSE_OVER_MOTION || event_buttons[i] == LButtonSprite::BUTTON_SPRITE_MOUSE_OUT)
				{
					//SDL_RenderClear(gRenderer);
					texture_bg.renderP();

					for (int i = 0; i < count_of_but; i++)
					{
						button[i].render();
					}
				}
			}
			//Update screen
			SDL_RenderPresent(gRenderer);
		}
	}
};
class Menu : public CurrentWindow
{
public:
	Menu(int x_but, int y_but, int margin, int height_but, int width_but, int count_of_but = ButtonNameInMenu::COUNT_OF_BUTTONS) :CurrentWindow(x_but, y_but, margin, height_but, width_but, count_of_but)
	{

	}
	bool LoadButton()override
	{
		bool success = true;
		if (!button[EXIT].loadButton("img\\Exit.png"))
		{
			success = false;
		}
		if (!button[GALLERY].loadButton("img\\Gallery.png"))
		{
			success = false;
		}
		if (!button[GAME].loadButton("img\\Play.png"))
		{
			success = false;
		}
		return success;
	}
	bool LoadBg()override
	{
		bool success = true;
		if (!texture_bg.loadFromFile("img\\Background.png"))
		{
			success = false;
		}
		return success;
	}
	int SelectedButtonInCurrentWindow(int button)override
	{
		switch (button)
		{
		case GAME:
			return GAME_WIND;
			break;
		case GALLERY:
			return GALLERY_WIND;
			break;
		case EXIT:
		case EXIT_WIND:
			return EXIT_WIND;
			break;

		}
	}
};
class LevelSelection : public CurrentWindow
{
public:
	LevelSelection(int x_but, int y_but, int margin, int height_but, int width_but, int count_of_but = COUNT_OF_LEVELSELECTION_BUTTONS) :CurrentWindow(x_but, y_but, margin, height_but, width_but, count_of_but)
	{

	}
	bool LoadButton()override
	{
		bool success = true;
		if (!button[LEVEL1].loadButton("img\\Level\\Level1.png"))
		{
			success = false;
		}
		if (!button[LEVEL2].loadButton("img\\Level\\Level2.png"))
		{
			success = false;
		}
		if (!button[LEVEL3].loadButton("img\\Level\\Level3.png"))
		{
			success = false;
		}
		if (!button[BACK].loadButton("img\\Level\\Back.png"))
		{
			success = false;
		}
		return success;
	}
	bool LoadBg()override
	{
		bool success = true;
		if (!texture_bg.loadFromFile("img\\Background_Gallery.png"))
		{
			success = false;
		}
		return success;
	}
	int SelectedButtonInCurrentWindow(int button)override
	{
		switch (button)
		{
		case EXIT_WIND:
			return MENU_WIND;
			break;
		}
	}
};

class Gallery : public CurrentWindow
{
private:
	Picture pictures[COUNT_OF_PIC];
	int selected_pic = FIRST;
public:
	Gallery(int x_but, int y_but, int margin,
		int height_but, int width_but,
		int count_of_but = COUNT_BUT_GAL) :CurrentWindow(x_but, y_but, margin, height_but, width_but, count_of_but, false)
	{
		for (int i = 0; i < COUNT_OF_PIC; i++)
		{
			pictures[i].setPosition(281, 45);
		}
	}
	bool LoadButton()override
	{
		bool success = true;
		if (!button[NEXT].loadButton("img\\Buttons Gallery\\next.png"))
		{
			success = false;
		}
		if (!button[ButtonInGallery::BACK_GAL].loadButton("img\\Buttons Gallery\\back.png"))
		{
			success = false;
		}
		return success;
	}
	bool LoadBg()override
	{
		bool success = true;
		if (!texture_bg.loadFromFile("img\\Background_Gallery.png"))
		{
			success = false;
		}
		return success;
	}
	bool Load_Pics()
	{
		bool success = true;
		if (!pictures[FIRST].loadFromFile("img\\Gallery\\1.png"))
		{
			success = false;
		}
		if (!pictures[SECOND].loadFromFile("img\\Gallery\\2.png"))
		{
			success = false;
		}
		if (!pictures[THIRD].loadFromFile("img\\Gallery\\2.png"))
		{
			success = false;
		}
		return success;
	}
	int SelectedButtonInCurrentWindow(int button)override
	{
		switch (button)
		{
		case ButtonInGallery::BACK_GAL:
			switch (selected_pic)
			{
			case FIRST:
				selected_pic = FIRST;
				return Window::MENU_WIND;
				break;
			case SECOND:
			case THIRD:
				selected_pic--;
				return GALLERY_WIND;
				break;
			}
			break;
		case NEXT:
			switch (selected_pic)
			{
			case FIRST:
			case SECOND:
				selected_pic++;
				return GALLERY_WIND;
				break;
			case THIRD:
				selected_pic = FIRST;
				return Window::MENU_WIND;
				break;
			}
			break;
		case EXIT_WIND:
			return MENU_WIND;
			break;
		}
	}
	void ShowPics()
	{
		pictures[selected_pic].renderP();
	}
};

//class Menu
//{
//private:
//	Picture menu_texture;
//	LButton menu_button[COUNT_OF_BUTTONS];
//	int event_menu_buttons[COUNT_OF_BUTTONS];
//	int current_over_motion;
//	int count_of_but = COUNT_OF_BUTTONS;
//public:
//	Menu()
//	{
//		for (int i = 0, j = 429; i < COUNT_OF_BUTTONS; i++, j += 124)
//		{
//
//			menu_button[i].setPosition(535, j);
//			menu_button[i].setHeight(105.8);
//			menu_button[i].setWidth(430);
//		}
//	}
//	//LTexture Get_menu_texture()
//	//{
//	//	return menu_texture;
//	//}
//	LButton* Get_menu_button(int index)
//	{
//		return &menu_button[index];
//	}
//	void Set_menu_texture(Picture texture)
//	{
//		menu_texture = texture;
//	}
//	void Set_menu_texture(LButton button, int index)
//	{
//		menu_button[index] = button;
//	}
//	bool Load_menu_button()
//	{
//		bool success = true;
//		if (!menu_button[EXIT].loadButton("img\\Exit.png"))
//		{
//			success = false;
//		}
//		if (!menu_button[GALLERY].loadButton("img\\Gallery.png"))
//		{
//			success = false;
//		}
//		if (!menu_button[GAME].loadButton("img\\Play.png"))
//		{
//			success = false;
//		}
//		return success;
//	}
//	bool Load_Bg()
//	{
//		bool success = true;
//		if (!menu_texture.loadFromFile("img\\Background.png"))
//		{
//			success = false;
//		}
//		return success;
//	}
//	void Show_menu()
//	{
//		Load_Bg();//show bg
//		menu_texture.render(0, 0);
//		SDL_RenderPresent(gRenderer);
//	}
//	int SelectedWindowInMenu(int button)
//	{
//		switch (button)
//		{
//		case GAME:
//			return GAME_WIND;
//			break;
//		case GALLERY:
//			return GALLERY_WIND;
//			break;
//		case EXIT:
//			return EXIT_WIND;
//			break;
//		}
//	}
//	int click_menu()
//	{
//		bool quit = false;
//		SDL_Event e;
//		Show_menu();
//		while (!quit)
//		{
//			//Handle events on queue
//			while (SDL_PollEvent(&e) != 0)
//			{
//				if (EscapeIsPressed(e))
//					return Window::EXIT_WIND;
//
//				for (int i = 0; i < count_of_but; i++)
//				{
//					event_menu_buttons[i] = menu_button[i].handleEvent(&e);
//				}
//			}
//			for (int i = 0; i < count_of_but; i++)
//			{
//				if (event_menu_buttons[i] == LButtonSprite::BUTTON_SPRITE_MOUSE_DOWN)
//				{
//					for (int i = 0; i < count_of_but; i++)
//					{
//						menu_button[i].render();
//					}
//					SDL_RenderPresent(gRenderer);
//					SDL_Delay(300);
//					event_menu_buttons[i] = LButtonSprite::BUTTON_SPRITE_MOUSE_OUT;
//					return SelectedWindowInMenu(i);
//				}
//				else
//				{
//					SDL_RenderClear(gRenderer);
//					menu_texture.renderP();
//
//					for (int i = 0; i < count_of_but; i++)
//					{
//						menu_button[i].render();
//					}
//				}
//			}
//			//Update screen
//			SDL_RenderPresent(gRenderer);
//		}
//	}
//};

//class LevelSelection
//{
//private:
//	Picture levelselection_bgtexture;
//	int count_of_but = COUNT_OF_LEVELSELECTION_BUTTONS;
//	LButton levelselection_button[COUNT_OF_LEVELSELECTION_BUTTONS];
//    int event_levelselection_buttons[COUNT_OF_LEVELSELECTION_BUTTONS];
//public:
//	LevelSelection()
//	{
//		for (int i = 0, j = 429; i < count_of_but; i++, j += 124)
//		{
//
//			levelselection_button[i].setPosition(535, j);
//			levelselection_button[i].setHeight(105.8);
//			levelselection_button[i].setWidth(430);
//		}
//	}
//	/*Picture Get_levelselection_bgtexture()
//	{
//		return levelselection_bgtexture;
//	}*/
//	LButton* Get_levelselection_button(int index)
//	{
//		return &levelselection_button[index];
//	}
//	void Set_levelselection_bgtexture(Picture texture)
//	{
//		levelselection_bgtexture = texture;
//	}
//	void Set_levelselection_button(LButton button, int index)
//	{
//		levelselection_button[index] = button;
//	}
//	bool Load_levelselection_button()
//	{
//		bool success = true;
//		if (!levelselection_button[LEVEL1].loadButton("img\\Level\\Level1.png"))
//		{
//			success = false;
//		}
//		if (!levelselection_button[LEVEL2].loadButton("img\\Level\\Level2.png"))
//		{
//			success = false;
//		}
//		if (!levelselection_button[LEVEL3].loadButton("img\\Level\\Level3.png"))
//		{
//			success = false;
//		}
//		if (!levelselection_button[BACK].loadButton("img\\Level\\Back.png"))
//		{
//			success = false;
//		}
//		return success;
//	}
//	bool Load_levelselection_bg()
//	{
//		bool success = true;
//		if (!levelselection_bgtexture.loadFromFile("img\\Background_Gallery.png"))
//		{
//			success = false;
//		}
//		return success;
//	}
//	int Show_levelselection()
//	{
//		bool back = false;
//		SDL_Event e;
//
//		//show bg
//		Load_levelselection_bg();
//		levelselection_bgtexture.renderP();
//		SDL_RenderPresent(gRenderer);
//
//		while (!back)
//		{
//			//Handle events on queue
//			while (SDL_PollEvent(&e) != 0)
//			{
//				//User requests quit
//				if (EscapeIsPressed(e))
//					return Window::MENU_WIND;
//
//				for (int i = 0; i < count_of_but; i++)
//				{
//					event_levelselection_buttons[i] = levelselection_button[i].handleEvent(&e);
//				}
//				for (int i = 0; i < count_of_but; i++)
//				{
//					if (event_levelselection_buttons[i] == LButtonSprite::BUTTON_SPRITE_MOUSE_DOWN)
//					{
//						for (int i = 0; i < count_of_but; i++)
//						{
//							levelselection_button[i].render();
//						}
//						SDL_RenderPresent(gRenderer);
//						SDL_Delay(300);
//						event_levelselection_buttons[i] = LButtonSprite::BUTTON_SPRITE_MOUSE_OUT;
//						//return SelectedWindowInMenu(i);
//					}
//					else
//					{
//						SDL_RenderClear(gRenderer);
//						levelselection_bgtexture.renderP();
//
//						for (int i = 0; i < count_of_but; i++)
//						{
//							levelselection_button[i].render();
//						}
//					}
//				}
//			}
//			//Update screen
//			SDL_RenderPresent(gRenderer);
//		}
//		return 0;
//	}
//};

//class Gallery
//{
//private:
//	Picture texture_bg;
//	LButton buttons[COUNT_BUT_GAL];
//	int event_gal_buttons[COUNT_BUT_GAL];
//	int current_over_motion;
//	Picture pictures[COUNT_OF_PIC];
//	int selected_pic = FIRST;
//public:
//	Gallery()
//	{
//		for (int i = 0, j = 74; i < COUNT_BUT_GAL; i++, j += 1214)
//		{
//
//			buttons[i].setPosition(j, 374);
//			buttons[i].setHeight(252.5);
//			buttons[i].setWidth(138);
//		}
//		for (int i = 0; i < COUNT_OF_PIC; i++)
//		{
//			pictures[i].setPosition(281, 45);
//		}
//	}
//	Picture Get_menu_texture()
//	{
//		return texture_bg;
//	}
//	LButton* Get_menu_button(int index)
//	{
//		return &buttons[index];
//	}
//	void Set_menu_texture(Picture texture)
//	{
//		texture_bg = texture;
//	}
//	void Set_menu_texture(LButton button, int index)
//	{
//		buttons[index] = button;
//	}
//	bool Load_button()
//	{
//		bool success = true;
//
//		return success;
//	}
//	bool Load_Buttons()
//	{
//		bool success = true;
//		if (!buttons[NEXT].loadButton("img\\Buttons Gallery\\next.png"))
//		{
//			success = false;
//		}
//		if (!buttons[ButtonInGallery::BACK_GAL].loadButton("img\\Buttons Gallery\\back.png"))
//		{
//			success = false;
//		}
//		return success;
//	}
//	bool Load_Bg()
//	{
//		bool success = true;
//		if (!texture_bg.loadFromFile("img\\Background_Gallery.png"))
//		{
//			success = false;
//		}
//		return success;
//	}
//	bool Load_Pics()
//	{
//		bool success = true;
//		if (!pictures[FIRST].loadFromFile("img\\Gallery\\1.png"))
//		{
//			success = false;
//		}
//		if (!pictures[SECOND].loadFromFile("img\\Gallery\\2.png"))
//		{
//			success = false;
//		}
//		if (!pictures[THIRD].loadFromFile("img\\Gallery\\2.png"))
//		{
//			success = false;
//		}
//		return success;
//	}
//	void Show_gallery()
//	{
//		Load_Bg();
//		Load_Buttons();
//		if (!Load_Pics())
//		{
//			printf("Failed to load media!\n");
//		}
//		texture_bg.renderP();
//		pictures[FIRST].renderP();
//		SDL_RenderPresent(gRenderer);
//	}
//	int SelectedGalleryWindow(int button)
//	{
//		switch (button)
//		{
//		case EXIT_WIND:
//			return MENU_WIND;
//			break;
//		case GALLERY:
//			return GALLERY_WIND;
//			break;
//		case EXIT:
//			return EXIT_WIND;
//			break;
//		}
//	}
//	int Click_gallery()
//	{
//		int quit = false;
//		SDL_Event e;
//		Show_gallery();
//		while (!quit)
//		{
//			while (SDL_PollEvent(&e) != 0)
//			{
//				if (EscapeIsPressed(e))
//					return Window::MENU_WIND;
//
//				for (int i = 0; i < COUNT_BUT_GAL; i++)
//				{
//					event_gal_buttons[i] = buttons[i].handleEvent(&e);
//				}
//			}
//			for (int i = 0; i < COUNT_BUT_GAL; i++)
//			{
//				if (event_gal_buttons[i] == LButtonSprite::BUTTON_SPRITE_MOUSE_DOWN)
//				{
//					for (int i = 0; i < COUNT_BUT_GAL; i++)
//					{
//						buttons[i].render();
//					}
//					SDL_RenderPresent(gRenderer);
//					SDL_Delay(300);
//					event_gal_buttons[i] = LButtonSprite::BUTTON_SPRITE_MOUSE_OUT;
//						switch (i)
//						{
//						case ButtonInGallery::BACK_GAL:
//							switch (selected_pic)
//							{
//							case FIRST:
//								selected_pic = FIRST;
//								return Window::MENU_WIND;
//								break;
//							case SECOND:
//							case THIRD:
//								selected_pic--;
//								break;
//							}
//							break;
//						case NEXT:
//							switch (selected_pic)
//							{
//							case FIRST:
//							case SECOND:
//								selected_pic++;
//								break;
//							case THIRD:
//								selected_pic = FIRST;
//								return Window::MENU_WIND;
//								break;
//							}
//							break;
//						}
//						SDL_RenderClear(gRenderer);
//						texture_bg.renderP();
//						pictures[selected_pic].renderP();
//						for (int i = 0; i < COUNT_BUT_GAL; i++)
//						{
//							buttons[i].render();
//						}
//				}
//				else
//				{
//					SDL_RenderClear(gRenderer);
//					texture_bg.render(0, 0);
//					pictures[selected_pic].renderP();
//					for (int i = 0; i < COUNT_BUT_GAL; i++)
//					{
//						buttons[i].render();
//					}
//				}
//			}
//			SDL_RenderPresent(gRenderer);
//		}
//	}
//};
class PuzzlePiece
{
private:
	//Top left position
	SDL_Point mPosition;

	SDL_Rect PuzzlepieceClip;
	LTexture PuzzleSpriteSheetTexture;

	bool selected = false;

public:
	//Initializes internal variables
	PuzzlePiece()
	{
		mPosition.x = 0;
		mPosition.y = 0;
	}

	//Sets top left position
	void setPosition(int x, int y)
	{
		mPosition.x = x;
		mPosition.y = y;
	}

	//Handles mouse event
	/*
	int handleEvent(SDL_Event* e)
	{
		int button_sprite = BUTTON_SPRITE_MOUSE_OUT;
		//If mouse event happened
		if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
		{
			//Get mouse position
			int x, y;
			SDL_GetMouseState(&x, &y);

			//Check if mouse is in button
			bool inside = true;

			//Mouse is left of the button
			if (x < mPosition.x
				|| x > mPosition.x + BUTTON_WIDTH
				|| y < mPosition.y
				|| y > mPosition.y + BUTTON_HEIGHT)
				inside = false;

			//Mouse is outside button
			if (!inside)
			{
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
				button_sprite = BUTTON_SPRITE_MOUSE_OUT;
			}
			//Mouse is inside button
			else
			{
				//Set mouse over sprite
				switch (e->type)
				{
				case SDL_MOUSEMOTION:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
					button_sprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
					break;

				case SDL_MOUSEBUTTONDOWN:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
					button_sprite = BUTTON_SPRITE_MOUSE_DOWN;
					break;

				case SDL_MOUSEBUTTONUP:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
					button_sprite = BUTTON_SPRITE_MOUSE_UP;
					break;
				}
			}

		}
		return button_sprite;
	}
	*/

	//Shows button sprite
	void render()
	{
		//Show current button sprite
		PuzzleSpriteSheetTexture.render(mPosition.x, mPosition.y, &PuzzlepieceClip);
	}

	void Set_PuzzlepieceClip(int x_coor, int y_coor, int width, int height)
	{
		PuzzlepieceClip.x = x_coor;
		PuzzlepieceClip.y = y_coor;
		PuzzlepieceClip.w = width;
		PuzzlepieceClip.h = height;
	}

	bool loadPuzzlepiece(string link)
	{
		//Loading success flag
		bool success = true;

		//Load sprites
		if (!PuzzleSpriteSheetTexture.loadFromFile(link))
		{
			printf("Failed to load button sprite texture!\n");
			success = false;
		}
		return success;
	}

};

class Puzzle
{
private:
	//Top left position
	SDL_Point mPosition;

	PuzzlePiece Puzzlepieces[PUZZLEPIECES_VERT][PUZZLEPIECES_HOR];

public:
	//Initializes internal variables
	Puzzle()
	{
		mPosition.x = 0;
		mPosition.y = 0;
	}

	//Sets top left position
	void setPosition(int x, int y)
	{
		mPosition.x = x;
		mPosition.y = y;
	}

	//Shows button sprite
	void render(int x_coor, int y_coor, int vert, int hor)
	{
		//Show current button sprite
		Puzzlepieces[vert][hor].setPosition(x_coor, y_coor);
		Puzzlepieces[vert][hor].render();
	}

	bool loadPuzzle(string link)
	{
		//Loading success flag
		bool success = true;

		//Load puzzle pieces textures and set sprites
		for (int i = 0; i < PUZZLEPIECES_VERT; i++)
		{
			for (int j = 0; j < PUZZLEPIECES_HOR; j++)
			{
				Puzzlepieces[i][j].loadPuzzlepiece("img/Puzzle1.png");
				Puzzlepieces[i][j].Set_PuzzlepieceClip(j * PUZZLEPIECE_WIDTH, i * PUZZLEPIECE_HEIGHT, PUZZLEPIECE_WIDTH, PUZZLEPIECE_HEIGHT);
			}
		}
		return success;
	}
};

//Starts up SDL and creates window
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();
//LButton gButtons[TOTAL_BUTTONS];
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;
	//Load PNG texture
	if (!IntroTextures[0].loadFromFile("img\\logo1.png"))
	{
		printf("Failed to load intro logo1 texture!\n");
		success = false;
	}

	//Load PNG texture
	if (!IntroTextures[1].loadFromFile("img\\logo2.png"))
	{
		printf("Failed to load intro logo2 texture!\n");
		success = false;
	}
	return success;
}

void close()
{
	//Free loaded images
	//gButtonSpriteSheetTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

DWORD WINAPI Intro(void* param)
{
	//Clear screen
	SDL_RenderClear(gRenderer);

	//Render texture to screen
	IntroTextures[0].render(0, 0);
	//Update screen
	SDL_RenderPresent(gRenderer);

	SDL_Delay(3000);

	//Clear screen
	SDL_RenderClear(gRenderer);
	//Render texture to screen
	IntroTextures[1].render(0, 0);
	//Update screen
	SDL_RenderPresent(gRenderer);
	return 0;
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		Menu m(535, 429, 124, 106, 430);
		LevelSelection l(535, 429, 124, 106, 430);
		Gallery g(74, 374, 1214, 252, 138);
		Puzzle pzz;
		//Load media
		if (!m.LoadButton() ||
			!l.LoadButton() ||
			//!pzz.loadPuzzle("img/Puzzle1.png") || 
			!loadMedia() || !g.Load_Pics())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			// Launch intro
			//CreateThread(0, 0, Intro, 0, 0, 0);
			//SDL_Delay(6000);

			int window_click = Window::MENU_WIND;
			while (window_click != Window::EXIT_WIND)
			{
				switch (window_click)
				{
				case Window::MENU_WIND:
					window_click = m.click_window();
					break;
				case Window::GALLERY_WIND:
					//g.Show_gallery();
					g.ShowPics();
					window_click = g.click_window();
					break;
				case Window::GAME_WIND:
					window_click = l.click_window();
					break;
				}

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
