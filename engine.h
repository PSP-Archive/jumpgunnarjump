// Engine.h -- Game engine

#ifdef _cpp
extern "C" {
#endif

#define TILEWIDTH 16
#define TILEHEIGHT 16
#define TILESACROSS 16
#define TILESDOWN 24
#define SCREENWIDTH 30
#define SCREENHEIGHT 17

#define MAXNPC 256
#define MAXEFX 256

struct Character {
	int type;	// See T_ defines below
	int x,y;
    int x_residue;  // Subpixel resolution for the x position
	int y_residue;  // Subpixel resolution for the y position
	int x_velocity;
	int y_velocity;
	int health;
	int delay;
	struct TileProp *prop;	// Currently playing animation
	int frame;
	int homex,homey;
	int facing;	// Which direction the character is facing
    int invincible;  // Normally 0; counts down after the player gets hurt
    int gravity;  // The amount of gravitational acceleration
};

struct BoardEffect {
	int type;
	struct TileProp *tileProp;
	int x,y;	// Tile of top left corner
	int delay;
};

struct Game {
	short *map;
	int w,h,t,l;	// Width, height, top, left (of visible) measured in tiles
	int available;	// Fruit available
	int collected;	// Fruit collected
	int bomb;		// number of bombs on hand.
	int checkpointX,checkpointY;
	int level;
	int score;
	int bonus[3];
	int key[4];	// YELLOW, GREEN, BLUE, RED
	int teleporterX[8],teleporterY[8]; // YELLOW {1,2}, GREEN {1,2}, BLUE {1,2}, RED {1,2}
	int activeTeleport;
	int lives;
	int joystick;	// Last seen direction
	struct Character player;
	struct Character npc[MAXNPC];
	int npcCount;
	int invalidate;
	struct BoardEffect effect[MAXEFX];
	int efxCount;
	int mode;
};
extern struct Game game;

extern struct TileProp {
	int ch;
	int id;         // Tile id of top left corner
	int gfxw;
	int gfxh;
	int frames;     // For animated tiles
	int flags;
    int offsetx,offsety,w,h;   // Core position in pixels
} tileProp[];

#define M_WALKRIGHT1 0
#define M_WALKLEFT1 48
#define M_FIRELEFT 144
#define M_FIRERIGHT 147
#define M_CLIMBRIGHT1 259
#define M_DEAD 150
#define M_SHOOTUPLEFT 384
#define M_SHOOTDOWNLEFT 390

#define M_YELLOWDOOR1 96
#define M_GREENDOOR1 100
#define M_BLUEDOOR1 104
#define M_REDDOOR1 108
#define M_FRUIT1 15
#define M_CHECKPOINT 79
#define M_BIRDRIGHT1 153
#define M_SHOOTERRIGHT1 156
#define M_TRAMPOLINE1 192
#define M_TRAMPOLINE2 208
#define M_CHEST1 224
#define M_CHICKRIGHT1 196
#define M_YINGYANG 198
#define M_SHIELD 199
#define M_BOMB 214
#define M_FULLHEALTH 215
#define M_SLOPELEFT 200
#define M_SLOPERIGHT 202
#define M_PLATFORMTL 216
#define M_SLIPPERYL 219
#define M_SLIPPERYR 235
#define M_SLIPPERYT 251
#define M_SOLIDGROUND 220
#define M_SECRETAREA 222
#define M_POUNDPLATFORM 155
#define M_ROCKETPLATFORM 171
#define M_EXIT 185
#define M_RING 256
#define M_EXPLODINGGROUND 304
#define M_DECORATION1 265
#define M_FANCYSOLIDGROUND 268
#define M_FANCYSKY 270
#define M_DECORATION2 313
#define M_TREE 316
#define M_FENCE 318
#define M_HEROSHOT 320
#define M_ENEMYSHOT 321
#define M_BOMBABLEWALL 322
#define M_STEEPSLOPELEFT 336
#define M_STEEPSLOPERIGHT 337
#define M_45SLOPELEFT 338
#define M_45SLOPERIGHT 354
#define M_GENTLESLOPELEFT 368
#define M_GENTLESLOPERIGHT 371
#define M_SLIPPERY45LEFT 374
#define M_SLIPPERY45RIGHT 375
#define M_MINIHEALTH 376
#define M_TIMEBONUS 361
#define M_LIFEBONUS 377
#define M_HEALTHMETER 322
#define M_WALLCLIMBERLEFT 364
#define M_WALLCLIMBERRIGHT 366
#define M_EXPLOSION1 396
#define M_EXPLOSION2 460
#define M_EVILDUCKLEFT1 432
#define M_EVILDUCKRIGHT1 438
#define M_YELLOWTELEPORTER 464
#define M_GREENTELEPORTER 467
#define M_BLUETELEPORTER 470
#define M_REDTELEPORTER 473
#define M_TELEPORTATION 259
#define M_INVISIBLEPLATFORM 355
#define M_YELLOWKEY 357
#define M_GREENKEY 358
#define M_BLUEKEY 359
#define M_REDKEY 360


#define S_FRUIT 0
#define S_JUMP 1
#define S_TRAMPOLINE 2
#define S_SHOOT 3
#define S_SPINATTACK 4
#define S_LEVELSTART 5
#define S_POWERUP 6
#define S_CHECKPOINT 7
#define S_LEVELEND 8
#define S_DIE 9
#define S_BOMB 10
#define S_BEEP 11
#define S_TELEPORT 12
#define S_LOCKED 13
#define S_KEY 14
#define S_DOOR 15
#define S_LEVELSONG 16

#define D_NONE -1
#define D_LEFT 0
#define D_UP 1
#define D_RIGHT 2
#define D_DOWN 3

#define T_PLAYER 0
#define T_YANG 1
#define T_BIRD 2
#define T_CHICK 3
#define T_BUG 4
#define T_SHOOTER 5
#define T_TRAMPOLINE 6
#define T_SHOT 7
#define T_SPECIALEFX 8
#define T_CHEST 9
#define T_GROUND 10
#define T_EVILDUCK 11
#define T_FRUIT 12


#define MODE_RUNNING 0
#define MODE_MENU 1
#define MODE_GETREADY 2
#define MODE_LEVELOVER 3
#define MODE_GAMEOVER 4

extern struct Game game;
// Engine implemented
void new_game();
void new_level(int level);
void load_level(const char *fname);
void save_level(const char *fname);
void next_frame(int elapsed);
void draw_map();
void set_player_direction(int direction);
void do_jump();
void release_jump();
void do_bomb();
void do_attack();
void do_special();

void construct_level(int w,int h);
void set_tiles(int x,int y,int tile);
struct TileProp *tile_to_prop(int tile);
int ch_to_tile(int ch);
extern int editMode;

// SDL implemented
void draw_tile(int id,int x,int y);
void draw_tile_rect(int id,int x,int y,int w,int h);
void play_sound(int id);
void draw_number(int id,int x,int y);
void draw_message(int size,int x,int y,const char *msg);

#ifdef _cpp
}
#endif
