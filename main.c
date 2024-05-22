#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
//#include <SDL.h>
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include "engine.h"

/*
 PLEASE NOTE: the program will require SDL.dll which is located in
              dev-c++'s dll directory. You have to copy it to you
			  program's home directory or the path.
 */

/* The screen surface */

#ifdef PSP
#include <pspkernel.h>
/* Define the module info section */
PSP_MODULE_INFO("jumpgunnarjump", 0, 1, 1);

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#endif
SDL_Surface *screen = NULL;
SDL_Surface *compo=NULL;
SDL_Surface *title=NULL;
SDL_Surface *gfx=NULL;
SDL_Surface *bg=NULL;
SDL_Surface *numbers=NULL;

TTF_Font *prompt,*headline,*body,*hint;

#define MAXPLAYLIST 5
Mix_Chunk *sfx[S_LEVELSONG];
Mix_Music *musicfx;
Mix_Music *playlist[MAXPLAYLIST];
int playlistCount;
#define MAXTIP 64
char *tips[MAXTIP];
int tipsCount;

char *sfxpath[]={
	"fruit.wav",
	"jump.wav",
	"trampoline.wav",
	"shoot.wav",
	"spin.wav",
	"start.wav",
	"powerup.wav",
	"checkpoint.wav",
	"end.wav",
	"die.wav",
	"bomb.wav",
	"empty.wav",	// Out of bombs
	"teleport.wav",
	"locked.wav",
	"key.wav",
	"door.wav"
};


void draw_text_line(int size,int x,int y,const char *msg)
{
	SDL_Surface *surf;
	TTF_Font *font=prompt;
	SDL_Rect rect;
	int bgskin=0;

	if(size==18) font=headline;
	if(size==12) font=body;
	if(size==10) font=hint;

	SDL_Color fg={0xff,0xff,0xff,0xff};
	SDL_Color bg={0x00,0x7f,0x7f,0xff};
	if(bgskin && (size==8 || size>18)) {
		bg.r=0; bg.g=0; bg.b=0;
	} else if(bgskin) {
		bg.r=255; bg.g=255;	bg.b=255;
	}

	if(size==18) { fg.r=0xd0; fg.g=0xd0; fg.b=0xd0; }
	if(size==10) { fg.r=0xc0; fg.g=0xc0; fg.b=0xff; }
	if(size==8) fg.b=0x80;

	if(bgskin && size==12) { fg.r=0; fg.b=0; fg.g=0; }
	if(bgskin && size==18) { fg.r=0x60; fg.g=0x60; fg.b=0x60; }

	surf=TTF_RenderText_Shaded(font,msg,fg,bg);
	rect.x=x;
	rect.y=y;
	rect.w=0;
	rect.h=0;
	SDL_BlitSurface(surf,NULL,screen,&rect);
	SDL_FreeSurface(surf);
}

// Adds in word wrapping
void draw_text(int size,int x,int y,const char *msg)
{
	TTF_Font *font=prompt;
	int w,h;
	int x1;
	int offset=0;
	char *line=(char *)malloc(strlen(msg)+1);
	char *buf=(char *)malloc(strlen(msg)+1);
	int i=0;

	if(size==18) font=headline;
	if(size==12) font=body;
	if(size==10) font=hint;

	offset=0;
	do { 
		int len=strlen(msg);
		// Add words until too long.
		while(i<len && msg[i]!=' ') {
			buf[i]=msg[i];
			i++;
		}
		w=0;
		h=0;
		while(msg[i]==' ') {
			buf[i]=msg[i];
			i++;
		}
		buf[i]=0;
		TTF_SizeText(font,buf,&w,&h);
		int edge=(size==8?480:480-27);
		if(x+w>=edge || i==len) {
			strcpy(line,msg);
			if(i<len) line[offset]=0;
			draw_text_line(size,x,y,line);
			y+=h;
			msg+=offset;
			if(i==len) msg+=i-offset;
			i=0;
			offset=0;
		} else {
			offset=i;
		}
	} while(strlen(msg)>0);

	free(line);
	free(buf);
}

void draw_status()
{
	char buf[512];
	sprintf(buf,"Level %d Score %05d Lives %d Health %3d%% Ammo %d Fruit %d%% %c%c%c %s ",
	game.level,game.score,game.lives,game.player.health,game.bomb,
	100*game.collected/game.available,
	game.bonus[0]?'O':'?',game.bonus[1]?'O':'?',game.bonus[2]?'O':'?',
	game.lives<=0?"game over":(2*game.collected>=game.available?"Find exit  ":"Gather fruit"));
	draw_text(8,0,0,buf);
}

/* This function draws to the screen; replace this with your own code! */
static void draw()
{
	// Clear the screen each frame for now.
	Uint32 color;

	/* Create a black background */
	color = SDL_MapRGB (screen->format, 0x88, 0xcc, 0xff);
	if(!bg) {
		SDL_FillRect(screen,0,color);
	} else {
        int tmp, parallax_ratio, parallax_xmin, parallax_ymin;
        SDL_Rect srcrect = {0,0,SCREENWIDTH*TILEWIDTH,SCREENHEIGHT*TILEHEIGHT};

        parallax_ratio = ((game.w-SCREENWIDTH)*TILEWIDTH +
            (bg->w-SCREENWIDTH*TILEWIDTH - 1)) /
            (bg->w-SCREENWIDTH*TILEWIDTH);
        tmp = ((game.h-SCREENHEIGHT)*TILEHEIGHT +
            (bg->h-SCREENHEIGHT*TILEHEIGHT - 1)) /
            (bg->h-SCREENHEIGHT*TILEHEIGHT);
        if( parallax_ratio < tmp) parallax_ratio = tmp;
        if( parallax_ratio < 3) parallax_ratio = 3;

        parallax_xmin = (bg->w - SCREENWIDTH*TILEWIDTH -
            ((game.w-SCREENWIDTH)*TILEWIDTH) / parallax_ratio) / 2;
        parallax_ymin = (bg->h - SCREENHEIGHT*TILEHEIGHT -
            ((game.h-SCREENHEIGHT)*TILEHEIGHT) / parallax_ratio) / 2;

        srcrect.x = parallax_xmin + game.l / parallax_ratio;
        srcrect.y = parallax_ymin + game.t / parallax_ratio;

		SDL_BlitSurface(bg,&srcrect,screen,0);
	}

	draw_map();

	draw_status();
    /* Make sure everything is displayed on screen */
    SDL_Flip (screen);
    /* Don't run too fast */
    SDL_Delay (1);
}

void draw_tile(int id,int x,int y)
{
    SDL_Rect tileRect;
    SDL_Rect rect;

	if(id<0) return;
	
	rect.x=x;
	rect.y=y;
	rect.w=TILEWIDTH;
	rect.h=TILEHEIGHT;
	if(gfx) {
		tileRect.x=(id%TILESACROSS)*TILEWIDTH;
		tileRect.y=(id/TILESACROSS)*TILEHEIGHT;
		tileRect.w=TILEWIDTH;
		tileRect.h=TILEHEIGHT;
		SDL_BlitSurface(gfx,&tileRect,screen,&rect);
	} else {
	    Uint32 color;
	
	    /* Create a black background */
	    color = SDL_MapRGB (screen->format, id&4?255:0, id&2?255:0, id&1?255:0);
		SDL_FillRect(screen,&rect,color);
	}
}

void draw_tile_rect(int id,int x,int y,int w,int h)
{
    SDL_Rect tileRect;
    SDL_Rect rect;
	
	if(id<0) return;

	rect.x=x;
	rect.y=y;
	rect.w=TILEWIDTH*w;
	rect.h=TILEHEIGHT*h;
	if(gfx) {
		tileRect.x=(id%TILESACROSS)*TILEWIDTH;
		tileRect.y=(id/TILESACROSS)*TILEHEIGHT;
		tileRect.w=TILEWIDTH*w;
		tileRect.h=TILEHEIGHT*h;
		SDL_BlitSurface(gfx,&tileRect,screen,&rect);
	} else {
	    Uint32 color;
	
	    /* Create a black background */
	    color = SDL_MapRGB (screen->format, id&4?255:0, id&2?255:0, id&1?255:0);
		SDL_FillRect(screen,&rect,color);
	}
}


void draw_number(int id,int x,int y)
{
    SDL_Rect tileRect;
    SDL_Rect rect;
	
	rect.x=x;
	rect.y=y;
	rect.w=TILEWIDTH;
	rect.h=TILEHEIGHT;
	if(numbers) {
		tileRect.x=id*TILEWIDTH;
		tileRect.y=0;
		tileRect.w=TILEWIDTH;
		tileRect.h=TILEHEIGHT;
		SDL_BlitSurface(numbers,&tileRect,screen,&rect);
	} else {
	    Uint32 color;
	
	    /* Create a black background */
	    color = SDL_MapRGB (screen->format, id&4?255:0, id&2?255:0, id&1?255:0);
		SDL_FillRect(screen,&rect,color);
	}
}

int lastLocked=0;

void play_sound(int id)
{
	static int lastmusic=0;
	
	switch(id) {
	case S_JUMP:
	case S_TRAMPOLINE:
	case S_SPINATTACK:
	case S_SHOOT:
	case S_BOMB:
	case S_POWERUP:
	case S_CHECKPOINT:
	case S_BEEP:
	case S_KEY:
	case S_DOOR:
	case S_TELEPORT:
		Mix_PlayChannel(1,sfx[id],0);
		break;
	case S_LOCKED:
		if(Mix_Playing(3)==0) {
			Mix_PlayChannel(3,sfx[id],0);
		}
		break;
	case S_FRUIT:
		Mix_PlayChannel(2,sfx[id],0);
		break;
	case S_LEVELSTART:
	case S_LEVELEND:
	case S_DIE:
		Mix_PauseMusic();
		Mix_PlayChannel(3,sfx[id],0);
		//SDL_Delay(500);
		Mix_ResumeMusic();
		break;
	case S_LEVELSONG:
		printf("Starting level song as requested\n",playlistCount);
		if(playlistCount>0) {
			printf("Starting level song %d of %d\n",game.level%playlistCount,playlistCount);
			Mix_PlayMusic(playlist[game.level%playlistCount],-1);
		} else {
			Mix_PlayMusic(musicfx,1);
		}
		break;
	}
}

void load_playlist()
{
	int i=0;
	FILE *file;
	char fname[512];
	
	file=fopen("playlist.txt","r");
	if(!file) return;
	fname[511]=0;
	while( fgets(fname,511,file)) {
		if(strstr(fname,"\n")) {
			strstr(fname,"\n")[0]=0;
		}
		if(strstr(fname,"\r")) {
			strstr(fname,"\r")[0]=0;
		}
		printf("Attempting to load music '%s'\n",fname);
		playlist[i]=Mix_LoadMUS(fname);
		if(playlist[i]) {
			printf("loaded music '%s' okay.\n",fname);
			i++;
		} else {
			printf("could not load music '%s'\n",fname);
		}
		if(i>=MAXPLAYLIST) break;
	}
	i=playlistCount;
	fclose(file);
}

void load_tips()
{
	int i=0;
	FILE *file;
	char str[512];
	
	file=fopen("tips.txt","r");
	if(!file) return;
	str[511]=0;
	while( fgets(str,511,file)) {
		if(strstr(str,"\n")) {
			strstr(str,"\n")[0]=0;
		}
		if(strstr(str,"\r")) {
			strstr(str,"\r")[0]=0;
		}
		tips[i]=strdup(str);
		i++;
		if(i>=MAXTIP) break;
	}
	tipsCount=i;
	fclose(file);
}

int wait_button()
{
	SDL_Event event;
	int start=0;
	int done=0;
    done = 0;
	start=0;	
    /* Check for events */
//    SDL_Delay(500);
	while (SDL_PollEvent (&event)) {
		if(event.type==SDL_QUIT) done=1;
	}
    while (start==0 && SDL_WaitEvent (&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
        case SDL_JOYBUTTONDOWN:
			start=1;
			break;
        case SDL_KEYUP:
        case SDL_JOYBUTTONUP:
			start=2;
            break;
        case SDL_QUIT:
            done = 1;
            break;
        default:
            break;
        }
        if(start==2 || done) break;
    }
	return done;		
}

int activeTip=0;

void draw_bg()
{
	Uint32 color;
	SDL_Rect rect;
	char msg[256];

	/* Create a black background */
	color = SDL_MapRGB (screen->format, 0x88, 0xcc, 0xff);
	SDL_FillRect(screen,0,color);

	rect.x=25;
	rect.y=25;
	rect.w=480-25*2;
	rect.h=272-25*2;
	color = SDL_MapRGB (screen->format, 0x0, 0x7f, 0x7f);
	SDL_FillRect(screen,&rect,color);

	draw_status();
}

void draw_tip()
{
	draw_text(10,27,155,"Tip:");
	if(activeTip<tipsCount) {
		char *s=tips[activeTip];
		int w=0,h=1;
		if(s[0]=='\'') {
			int id=ch_to_tile(s[1]);
			struct TileProp *tileProp=tile_to_prop(id);
			w=1;
			if(tileProp) { w=tileProp->gfxw; h=tileProp->gfxh; }
			// Draw the thing we're thinking of.
			draw_tile_rect(id,27,185,w,h);
			
			s+=3;
		}
		draw_text(10,w*TILEWIDTH+27,185,s);
	}
	activeTip++;
	if(activeTip>tipsCount) activeTip=0;

}

int show_getready()
{
	char msg[256];
	
	// Clear the screen each frame for now.
	draw_bg();
	draw_text(18,57,27,"Get Ready...");
	sprintf(msg,"%d of %d fruit collected.",game.collected,game.available);
	draw_text(12,57,77,msg);
	sprintf(msg,"%d of 3 rings collected.",game.bonus[0]+game.bonus[1]+game.bonus[2]);
	draw_text(12,57,107,msg);

	draw_tip();	
    /* Make sure everything is displayed on screen */
    SDL_Flip (screen);

    /* Don't run too fast */
    game.mode=MODE_RUNNING;
    return wait_button();
}

int show_levelover()
{
	draw_bg();
	draw_text(18,57,57,"Level Completed!");
	draw_tip();
    /* Make sure everything is displayed on screen */
    SDL_Flip (screen);

    game.mode=MODE_GETREADY;
    return wait_button();
}

int show_gameover()
{
	draw_bg();
	draw_text(18,57,57,"Game Over");
	draw_tip();	
    /* Make sure everything is displayed on screen */
    SDL_Flip (screen);

	game.mode=MODE_RUNNING;	
    return wait_button();
}

int
main (int argc, char *argv[])
{
    char msg[256];
    int done;
	int i;
	SDL_Joystick *joy;
    SDL_Rect rect;
	int start=0;
	SDL_Event event;
	int oldTime,newTime;
	int dir;
    int oldDir=-1;
    
    /* Initialize SDL */
    if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_AUDIO) < 0)
    {
#ifdef _WIN32
        sprintf (msg, "Couldn't initialize SDL: %s\n", SDL_GetError ());
        MessageBox (0, msg, "Error", MB_ICONHAND); 
        //free (msg);
#endif
        exit (1);
    }
    atexit (SDL_Quit);

    /* Set 640x480 16-bits video mode */
    screen = SDL_SetVideoMode (480, 272, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL)
    {
#ifdef _WIN32
        sprintf (msg, "Couldn't set 480x272x32 video mode: %s\n",
          SDL_GetError ());
        MessageBox (0, msg, "Error", MB_ICONHAND); 
        //free (msg);
#endif
        exit (2);
    }
    SDL_WM_SetCaption ("Jump Gunnar, Jump SDL by PSPSMM", NULL);

    //gfx=SDL_LoadBMP("gfx.bmp");
    gfx=IMG_Load("gfx.png");
    bg=IMG_Load("bg.png");
    //if(gfx) SDL_SetColorKey(gfx, SDL_SRCCOLORKEY, SDL_MapRGB(gfx->format, 0, 255, 0));
    numbers=SDL_LoadBMP("numbers.bmp");
    title=IMG_Load("title.png");
    compo=IMG_Load("ps2dev_tfc_psp.png");

	TTF_Init();
	prompt=TTF_OpenFont("BoomBox.ttf",9);
	headline=TTF_OpenFont("Anaconda Regular.ttf",48);
	body=TTF_OpenFont("Arial.ttf",24);
	hint=TTF_OpenFont("Arial.ttf",18);

	joy=0;
	if(SDL_NumJoysticks()>0)
	   joy=SDL_JoystickOpen(0);

    if(Mix_OpenAudio(44100,AUDIO_S16SYS, 2, 2048)<0) {
#ifdef _WIN32
        sprintf(msg,"Warning: Couldn't set 44100 Hz 16-bit audio\r\n"
          "- Reason: %s\n",SDL_GetError());
        MessageBox(0,msg,"Warning",MB_ICONHAND);
#endif
    }
    
	for(i=0;i<S_LEVELSONG;i++) {
		sfx[i]=Mix_LoadWAV(sfxpath[i]);
		if(!sfx[i]) printf("Warning: missing sound file '%s'\n",sfxpath[i]);
	}
    musicfx=Mix_LoadMUS("rush.it");

	rect.x=0;
	rect.y=0;
	rect.w=480;
	rect.h=272;
	SDL_BlitSurface(compo,0,screen,&rect);

	SDL_Flip(screen);
	
    done = wait_button();
//	SDL_Delay(3000);	
    /* Check for events */

	rect.x=0;
	rect.y=0;
	rect.w=480;
	rect.h=272;
	SDL_BlitSurface(title,0,screen,&rect);

	SDL_Flip(screen);
	
	/* Try to load a playlist */
	load_playlist();
	load_tips();

	done=wait_button();

	new_game();
	new_level(1);
	oldTime=SDL_GetTicks();

	dir=-1;
	oldDir=-2;	// Dummy value
    while (!done)
    {
        SDL_Event event;

		if(game.mode!=MODE_RUNNING) {
			switch(game.mode) {
			case MODE_GETREADY:
				show_getready();
				game.mode=MODE_RUNNING;
				break;
			case MODE_LEVELOVER:
				show_levelover();
				game.mode=MODE_RUNNING;
				break;
			case MODE_GAMEOVER:
				show_gameover();
				game.mode=MODE_RUNNING;
				break;				
			}
			game.joystick=D_NONE;
			oldTime=SDL_GetTicks();
		}
        /* Check for events */
        if (SDL_PollEvent (&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE) done=1;
                if(event.key.keysym.sym == SDLK_LSHIFT) do_bomb();
                if(event.key.keysym.sym == SDLK_SPACE) do_jump();
                if(event.key.keysym.sym == SDLK_LALT) do_attack();
                if(event.key.keysym.sym == SDLK_LCTRL) do_special();
				if(event.key.keysym.sym == SDLK_LEFT) dir=0;
				if(event.key.keysym.sym == SDLK_UP) dir=1;
				if(event.key.keysym.sym == SDLK_RIGHT) dir=2;
				if(event.key.keysym.sym == SDLK_DOWN) dir=3;
                if(event.key.keysym.sym == SDLK_RSHIFT) new_level(game.level);
                if(event.key.keysym.sym == SDLK_RCTRL) new_level(game.level+1);
                break;
            case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_LEFT && dir==0) dir=-1;
				if(event.key.keysym.sym == SDLK_UP && dir==1) dir=-1;
				if(event.key.keysym.sym == SDLK_RIGHT && dir==2) dir=-1;
				if(event.key.keysym.sym == SDLK_DOWN && dir==3) dir=-1;
                if(event.key.keysym.sym == SDLK_SPACE) release_jump();
				break;
            case SDL_JOYBUTTONDOWN:
                if(event.jbutton.button == 11 /* START */) done=1;
                if(event.jbutton.button == 10 /* SELECT */) new_level(game.level);
                if(event.jbutton.button == 0 /* TRIANGLE */) do_bomb();
                if(event.jbutton.button == 1 /* CIRCLE */) do_special();
                if(event.jbutton.button == 2 /* CROSS */) do_jump();
                if(event.jbutton.button == 3 /* SQUARE */) do_attack();
				if(event.jbutton.button == 7 /* LEFT */) dir=0;
				if(event.jbutton.button == 8 /* UP */) dir=1;
				if(event.jbutton.button == 9 /* RIGHT */) dir=2;
				if(event.jbutton.button == 6 /* DOWN */) dir=3;
				break;
            case SDL_JOYBUTTONUP:
				if(event.jbutton.button == 7 /* LEFT */ && dir==0) dir=-1;
				if(event.jbutton.button == 8 /* UP */ && dir==1) dir=-1;
				if(event.jbutton.button == 9 /* RIGHT */ && dir==2) dir=-1;
				if(event.jbutton.button == 6 /* DOWN */ && dir==3) dir=-1;
                if(event.jbutton.button == 2 /* CROSS */) release_jump();
				break;
            case SDL_QUIT:
                done = 1;
                break;
            default:
                break;
            }
        }
        if(dir!=oldDir) {
			set_player_direction(dir);
		}
		oldDir=dir;

        /* Draw to screen */
		newTime=SDL_GetTicks();
        next_frame(newTime-oldTime+5);
        oldTime=SDL_GetTicks();
        draw ();
    }

    return 0;
}
