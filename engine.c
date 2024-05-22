/* Engine */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include "engine.h"

struct Game game;
static int dir_x[4]={-1,0,1,0},dir_y[4]={0,-1,0,1};
int editMode=0;

#define TP_ANIMATED 1
#define TP_MIRROR_BELOW 2
#define TP_MIRROR_BESIDE 4
#define TP_MAP_MODULUS 8	// Pick subtile with map location in mind
#define TP_NPC 16
#define TP_SUBSTRATE 32	// You can stand on it, can't walk through it
#define TP_SLOPELEFT 64
#define TP_SLOPERIGHT 128
#define TP_SLIPPERY 256

struct TileProp tileProp[]={
	{'!',M_WALKRIGHT1,3,3,5,TP_ANIMATED|TP_MIRROR_BELOW,16,5,16,43},
	{'0',M_FIRELEFT,3,3,1,TP_MIRROR_BESIDE,16,5,16,43},
	{'0',M_SHOOTUPLEFT,3,3,1,TP_MIRROR_BESIDE,16,5,16,43},
	{'0',M_SHOOTDOWNLEFT,3,3,1,TP_MIRROR_BESIDE,16,5,16,43},
	{'0',M_DEAD,3,3,1,0,16,5,16,43},

	{'+',M_FRUIT1,1,4,1,TP_MAP_MODULUS,0,0,16,16},
	{'^',M_CHECKPOINT,1,2,1,0,0,0,16,32},
	{'{',M_BIRDRIGHT1,1,1,2,TP_MIRROR_BELOW|TP_NPC,0,0,16,16},
	{'E',M_EVILDUCKLEFT1,3,2,2,TP_MIRROR_BESIDE|TP_NPC,0,0,48,32},
	{'s',M_SHOOTERRIGHT1,2,2,2,TP_MIRROR_BELOW|TP_NPC,0,0,32,32},
	{'t',M_TRAMPOLINE1,4,1,1,0,0,0,64,16},	// Done as special case :-(
	{'u',M_TRAMPOLINE2,4,1,1,0,0,0,64,16},
	{'$',M_CHEST1,2,2,4,TP_ANIMATED,0,0,32,32},
	{'*',M_CHICKRIGHT1,1,1,2,TP_ANIMATED|TP_NPC|TP_MIRROR_BELOW,0,-3,16,19},
	{'Y',M_YINGYANG,1,1,1,0,0,0,16,16},
	{'S',M_SHIELD,1,1,1,0,0,0,16,16},
	{'B',M_BOMB,1,1,1,0,6,6,5,5},
	{'h',M_MINIHEALTH,1,1,1,0,0,0,16,16},
	{'H',M_FULLHEALTH,1,1,1,0,0,0,16,16},
	{'T',M_TIMEBONUS,1,1,1,0,0,0,16,16},
	{'L',M_LIFEBONUS,1,1,1,0,0,0,16,16},
	{'/',M_SLOPELEFT,2,1,1,TP_SUBSTRATE|TP_SLOPELEFT,0,0,32,16},
	{'>',M_SLOPERIGHT,2,1,1,TP_SUBSTRATE|TP_SLOPERIGHT,0,0,32,16},
	{'|',M_GENTLESLOPELEFT,3,1,1,TP_SUBSTRATE|TP_SLOPELEFT,0,0,48,16},
	{'}',M_GENTLESLOPERIGHT,3,1,1,TP_SUBSTRATE|TP_SLOPERIGHT,0,0,48,16},
	{'(',M_45SLOPELEFT,1,1,1,TP_SUBSTRATE|TP_SLOPELEFT,0,0,16,16},
	{')',M_45SLOPERIGHT,1,1,1,TP_SUBSTRATE|TP_SLOPERIGHT,0,0,16,16},
	{'a',M_SLIPPERY45LEFT,1,1,1,TP_SUBSTRATE|TP_SLOPELEFT|TP_SLIPPERY,0,0,16,16},
	{'b',M_SLIPPERY45RIGHT,1,1,1,TP_SUBSTRATE|TP_SLOPERIGHT|TP_SLIPPERY,0,0,16,16},
	{'<',M_STEEPSLOPELEFT,1,2,1,TP_SUBSTRATE|TP_SLOPELEFT,0,0,16,32},
	{'\\',M_STEEPSLOPERIGHT,1,2,1,TP_SUBSTRATE|TP_SLOPERIGHT,0,0,16,32},
	{'#',M_SOLIDGROUND,2,3,1,TP_MAP_MODULUS|TP_SUBSTRATE,0,0,16,16},
	{'.',M_SECRETAREA,2,3,1,TP_MAP_MODULUS|TP_SUBSTRATE,0,0,16,16},
	{'1',M_PLATFORMTL,1,1,1,TP_SUBSTRATE,0,0,16,16},
	{'2',M_PLATFORMTL+1,1,1,1,TP_SUBSTRATE,0,0,16,16},
	{'3',M_PLATFORMTL+2,1,1,1,TP_SUBSTRATE,0,0,16,16},
	{'4',M_PLATFORMTL+16,1,1,1,0,0,0,16,16},
	{'5',M_PLATFORMTL+17,1,1,1,0,0,0,16,16},
	{'6',M_PLATFORMTL+18,1,1,1,0,0,0,16,16},
	{'7',M_PLATFORMTL+32,1,1,1,0,0,0,16,16},
	{'8',M_PLATFORMTL+33,1,1,1,0,0,0,16,16},
	{'9',M_PLATFORMTL+34,1,1,1,0,0,0,16,16},
	{'v',M_BOMBABLEWALL,1,1,1,TP_SUBSTRATE,0,0,16,16},
	{':',M_SLIPPERYL,1,1,1,TP_SUBSTRATE|TP_SLIPPERY,0,0,16,16},
	{';',M_SLIPPERYR,1,1,1,TP_SUBSTRATE|TP_SLIPPERY,0,0,16,16},
	{'_',M_SLIPPERYT,1,1,1,TP_SUBSTRATE|TP_SLIPPERY,0,0,16,16},
	{'m',M_POUNDPLATFORM,1,1,1,TP_SUBSTRATE,0,0,16,16},
	{'w',M_ROCKETPLATFORM,1,1,1,TP_SUBSTRATE,0,0,16,16},
   	{'i',M_INVISIBLEPLATFORM,1,1,2,TP_SUBSTRATE,0,0,16,16},
	{'?',M_EXIT,3,1,1,0,0,0,48,16},
	{'O',M_RING,3,3,1,0,0,0,48,48},
	{'Q',M_EXPLODINGGROUND,1,1,3,TP_SUBSTRATE|TP_ANIMATED,0,0,16,16},
	{'j',M_DECORATION1,3,3,1,0,0,0,48,48},
	{'M',M_FANCYSOLIDGROUND,2,3,1,TP_SUBSTRATE|TP_MAP_MODULUS,0,0,16,16},
	{',',M_FANCYSKY,2,3,1,0,0,0,32},
	{'k',M_DECORATION2,3,3,1,0,0,0,48,48},
	{'l',M_TREE,2,3,1,0,0,0,32,48},
	{'f',M_FENCE,2,3,1,0,0,0,32,48},
	{'z',M_HEROSHOT,1,1,1,0,6,6,5,5},
	{'x',M_ENEMYSHOT,1,1,1,0,6,6,5,5},
	//{'c',M_HEALTHMETER,1,1,1,0,0,0,16,16},
	{'C',M_WALLCLIMBERRIGHT,1,2,2,TP_NPC,0,0,16,32},
	{'D',M_WALLCLIMBERLEFT,1,2,2,TP_NPC,0,0,16,32},
	{'g',M_YELLOWKEY,1,1,1,0,0,0,16,16},
	{'n',M_GREENKEY,1,1,1,0,0,0,16,16},
	{'p',M_BLUEKEY,1,1,1,0,0,0,16,16},
	{'r',M_REDKEY,1,1,1,0,0,0,16,16},
	{'G',M_YELLOWDOOR1,2,3,2,TP_SUBSTRATE,0,0,32,48},
	{'N',M_GREENDOOR1,2,3,2,TP_SUBSTRATE,0,0,32,48},
	{'P',M_BLUEDOOR1,2,3,2,TP_SUBSTRATE,0,0,32,48},
	{'R',M_REDDOOR1,2,3,2,TP_SUBSTRATE,0,0,32,48},
	{'A',M_TELEPORTATION,3,3,2,TP_NPC|TP_ANIMATED,0,0,48,48},
	{'x',M_YELLOWTELEPORTER,3,3,1,0,0,0,48,48},
	{'X',M_GREENTELEPORTER,3,3,1,0,0,0,48,48},
	{'z',M_BLUETELEPORTER,3,3,1,0,0,0,48,48},
	{'Z',M_REDTELEPORTER,3,3,1,0,0,0,48,48},
	
	//{0,0,0,0,0,0,0,0,0,0}
};

#define MAXCHAR 256
#define MAXTILE 512
int mapCharToTile[MAXCHAR];
struct TileProp *mapTileToProp[MAXTILE];

#define PRECISION 480
#define JUMPINITIAL 1000
#define ROCKETINITIAL 2000
#define PATROLDIST 4
#define TERMINALVELOCITY 1000
#define HIGHGRAVITY 28
#define LOWGRAVITY 13
#define HORIZ_ACCEL 5
#define HORIZ_DECEL 7
#define HORIZ_SKID 23   // Deceleration when pointing the joystick
                        // opposite to the direction of motion.
#define MAX_SPEED 900

static char *defaultlevel=
" \n"
" TTT H H BBB ++++ +   + +++  \n"
"  T  H H B   +    ++  + +  + \n"
"  T  HHH BBB +++  + + + +  + \n"
"  T  H H B   +    +  ++ +  + \n"
"  T  H H BBB ++++ +   + +++  \n"
" !!!                         12223 \n"
" !!!                         45556 \n"
" !!!                         45556 \n"
"######       #################78889 \n"
"######        #################################  \n"
"######        ####  ++++++   ################        ##\n"
"######        ####  ++++++   ######                 ###\n"
"#######       ....  ++++++   ######    +++       ######\n"
"#######       ....  ++++++   ######  $$   $$   ########\n"
"#######tttt   ....  ++++++   ######  $$   $$ ##########\n"
"#####################################################\n"
;

void set_tile(int x,int y,int tile)
{
	x+=game.l/TILEHEIGHT;
	y+=game.t/TILEWIDTH;
	if(x<0 || x>=game.w || y<0 || y>=game.h) return;
	game.map[x+y*game.w]=tile;
}

struct TileProp *tile_to_prop(int tile)
{
	if(tile<0 || tile>=MAXTILE) return NULL;
    return mapTileToProp[tile];
}

int ch_to_tile(int ch)
{
	if(ch<0 || ch>=MAXCHAR) return -1;
	
	return mapCharToTile[ch];
}

void set_tiles(int x,int y,int tile)
{
	struct TileProp *prop;
	int i,j;

	if(tile<0 || tile>=MAXTILE) {
		set_tile(x,y,-1);
		return;
	}
	
	prop=mapTileToProp[tile];
	if(prop==NULL) {
		set_tile(x,y,-1);
		return;
	}

	if(prop->flags&TP_MAP_MODULUS) {
		int tilex=x%prop->gfxw;
		int tiley=y%prop->gfxh;
		if(prop->gfxh==1) tilex=(x+y)%prop->gfxw;	// Special case
		if(prop->gfxw==1) tiley=(x+y)%prop->gfxh;	// Special case
		set_tile(x,y,prop->id+tilex+TILESACROSS*tiley);
		return;
	}

	// Otherwise draw whole object.
	for(j=0;j<prop->gfxh;j++) {
		for(i=0;i<prop->gfxw;i++) {
			set_tile(x+i,y+j,prop->id+i+TILESACROSS*j);
		}
	}
	
}

void new_game()
{
	int i;
	int mapped=0;

	game.level=1;
	game.lives=5;
	game.score=0;
	game.invalidate=0;
	game.map=0;
	// Initialize maps:

	for(i=0;i<MAXCHAR;i++) mapCharToTile[i]=-1;
	for(i=0;i<MAXTILE;i++) mapTileToProp[i]=NULL;
	// Now mark all of the ones in the list
	for(i=0;i<sizeof(tileProp)/sizeof(struct TileProp);i++) {
		int x,y;

		mapCharToTile[tileProp[i].ch]=tileProp[i].id;
		for(y=0;y<tileProp[i].gfxh*(tileProp[i].flags&TP_MIRROR_BELOW?2:1);y++) {
			for(x=0;x<tileProp[i].gfxw*tileProp[i].frames*(tileProp[i].flags&TP_MIRROR_BESIDE?2:1);x++) {
//				ASSERT((mapTileToProp[tileProp[i].id+x+y*TILESACROSS]-tileProp))==-1);
				mapTileToProp[tileProp[i].id+x+y*TILESACROSS]=tileProp+i;
				mapped++;
			}
		}
	}
#ifdef _DEBUG
	printf("char to tile table:\n");
	for(i=0;i<MAXCHAR;i++) {
		if(mapCharToTile[i]<0) continue;
		if(i>32 && i<127) printf("%c=",i);
		printf("%d\n",mapCharToTile[i]);
	}
	printf("tile to prop table:\n");
	for(i=0;i<MAXCHAR;i++) {
		printf("%02x ",(mapTileToProp[i]-tileProp)&255);
		if((i%16)==15) printf("\n");
	}
#endif
//	ASSERT(mapped==MAXTILE);
    game.player.prop = mapTileToProp[M_WALKRIGHT1];
}

int test_npc(int x,int y)
{
	int i;
	for(i=0;i<game.npcCount;i++) {
		// Overlapping
		if( x >= game.npc[i].x - game.npc[i].prop->offsetx &&
            x < game.npc[i].x - game.npc[i].prop->offsetx +
                TILEWIDTH * game.npc[i].prop->gfxw &&
            y >= game.npc[i].y - game.npc[i].prop->offsety &&
            y < game.npc[i].y - game.npc[i].prop->offsety +
                TILEHEIGHT * game.npc[i].prop->gfxh) return i;
	}
	return -1;
}

struct TileProp *tile_prop( int tilex, int tiley) {
    int m;
    if( tilex < 0 || tilex >= game.w) return NULL;
    if( tiley < 0 || tiley >= game.h) return NULL;
    m = game.map[tilex + tiley * game.w];
    if( m < 0) return NULL;
    return mapTileToProp[m];
}

// This function decides whether the given tile contains solid substrate.
int is_solid( struct TileProp *p, int tilex, int tiley, int allsolid) {
    struct TileProp *o;
    if( !(p->flags & TP_SUBSTRATE)) return 0;
    if( allsolid) return 1;
    if( tiley == 0) return 1;
    if( p->flags & (TP_SLOPELEFT|TP_SLOPERIGHT)) {
        if( p->gfxh > 1) return 1;
    } else {
        o = tile_prop(tilex,tiley-1);
        if( o && (o->flags & TP_SUBSTRATE)) return 1;
    }
    o = tile_prop(tilex,tiley+1);
    return o &&
        (o->flags&(TP_SUBSTRATE|TP_SLOPERIGHT|TP_SLOPELEFT)) == TP_SUBSTRATE;
}

// This function computes the coordinate position where a wide ray
// would intersect with an object.  The ray is specified as minimum
// coordinate on the axis perpendicular to motion, ray width, and
// direction.  The ray is assumed to proceed from infinity.  The return
// value is a coordinate on the axis parallel to motion.  The object
// is assumed to have top-left corner at 0,0.
// 
// It is the responsibility of the caller to use a simple boundbox check
// to ensure that an intersection exists.  Otherwise, the results of
// this function are undefined.  (It may even crash.)
//
// In the special case where the ray intersects but is allowed to pass
// through the object, this function returns -1.
//
int wide_ray_intersect( struct TileProp *obj, int tilex, int tiley,
        int mincoord, int breadth, int dir, int allsolid)
{
    int x;

    // Make the bottom edge of the trampoline downwards-impassable
    if( dir==D_DOWN && (obj->id==M_TRAMPOLINE1 || obj->id==M_TRAMPOLINE2)) {
        return obj->h;
    }

    if( !(obj->flags & TP_SUBSTRATE)) return -1;

    // Disallow downward passage into any type of substrate
    if( dir == D_DOWN) {
        if( obj->flags & (TP_SLOPERIGHT|TP_SLOPELEFT)) {
            x = (obj->flags & TP_SLOPERIGHT)?mincoord:obj->w-mincoord-breadth;
            if( x < 0) return 0;
            return x * obj->h / obj->w;
        }
        // A filled substrate tile - intersection point is at top
        return 0;
    }

    // Disallow upward passage into solid substrate
    if( dir == D_UP) {
        if( is_solid(obj, tilex, tiley, allsolid)) return obj->h;
        return -1;
    }

    // Disallow horizontal passage through the face of a slope.
    if( ((obj->flags & TP_SLOPERIGHT) && dir == D_LEFT) ||
        ((obj->flags & TP_SLOPELEFT) && dir == D_RIGHT)) {
        // If the slope is non-solid, then only the character's
        // toes are prohibited from entering.
        if( mincoord + breadth > obj->h &&
            !is_solid(obj,tilex,tiley, allsolid)) return -1;

        x = ((mincoord + breadth) * obj->w + obj->h - 1) / obj->h;
        if( x > obj->w) x = obj->w;
        return (obj->flags & TP_SLOPERIGHT) ? x : obj->w - x;
    }

    if( !is_solid(obj, tilex, tiley, allsolid)) return -1;

    // Disallow horizontal passage into solid substrate
    return dir == D_LEFT ? obj->w : 0;
}

// This function determines whether a rectangular object can travel a
// certain distance in a certain cardinal direction without colliding with
// terrain.  If a collision would occur, it returns the distance prior to
// the collision.  Otherwise it returns distance.
int offset_motion_limit( struct Character *c, int offset_x, int offset_y,
    int distance, int dir)
{
    struct TileProp *t, *p = c->prop;
    int mindist = -1, o, i, edge, dist, h, step;
    int major, minor, mintile, maxtile, from, limit;
    int c_x = c->x + offset_x, c_y = c->y + offset_y;

    // Just an optimization
    if( distance == 0) return 0;

    if( dir == D_UP) {
        from = (c_y - 1) / TILEHEIGHT;
        limit = (c_y - distance) / TILEHEIGHT - 1;
    } else if( dir == D_DOWN) {
        from = (c_y + p->h) / TILEHEIGHT;
        limit = (c_y + p->h + distance - 1) / TILEHEIGHT + 1;
    } else if( dir == D_LEFT) {
        from = (c_x - 1) / TILEWIDTH;
        limit = (c_x - distance) / TILEWIDTH - 1;
    } else {
        from = (c_x + p->w) / TILEWIDTH;
        limit = (c_x + p->w + distance - 1) / TILEWIDTH + 1;
    }

    step = (dir == D_UP || dir == D_LEFT) ? -1 : 1;
    h = (dir == D_LEFT || dir == D_RIGHT);

    mintile = h ? c_y / TILEHEIGHT : c_x / TILEWIDTH;
    maxtile = h ? (c_y+p->h-1)/TILEHEIGHT : (c_x+p->w-1)/TILEWIDTH;

    for( major = from; major != limit; major+=step) {       // y
        for( minor = mintile; minor <= maxtile; minor++) {  // x
            t = tile_prop( h?major:minor, h?minor:major);
            if( !t) continue;

            if( t->flags & TP_MAP_MODULUS) o = 0;
            else o = game.map[(h?major:minor)+(h?minor:major)*game.w] - t->id;

            i =  wide_ray_intersect( t, h?major:minor, h?minor:major, h
                    ? (c_y - (minor - o/TILESACROSS) * TILEHEIGHT)
                    : (c_x - (minor - o%TILESACROSS) * TILEWIDTH),
                    h ? p->h : p->w, dir, c!=&game.player);
            if( i != -1) {
                if( h) {
                    edge = i + TILEWIDTH * (major - o % TILESACROSS);
                    dist = (dir==D_LEFT) ? c_x - edge : edge - (c_x+p->w);
                } else {
                    edge = i + TILEHEIGHT * (major - o / TILESACROSS);
                    dist = (dir==D_UP) ? c_y - edge : edge - (c_y+p->h);
                }
                if( dist >= 0 && (mindist == -1 || dist < mindist))
                    mindist = dist;
            }
        }

        if( mindist != -1) {
            if( mindist < distance) distance = mindist;
            break;
        }
    }

    if( dir == D_LEFT && c->x - p->offsetx + offset_x - distance < 0)
        distance = c->x - p->offsetx + offset_x;
    if( dir == D_RIGHT &&
        c->x-p->offsetx+offset_x+p->gfxw*TILEWIDTH+distance>game.w*TILEWIDTH)
        distance = (game.w-p->gfxw)*TILEWIDTH - (c->x-p->offsetx+offset_x);

    return distance;
}

int motion_limit( struct Character *c, int distance, int dir)
{
    return offset_motion_limit( c, 0, 0, distance, dir);
}

// Check whether the character is standing on solid ground
int landed(struct Character *c) {
    return c->y_velocity >= 0 &&
        motion_limit( c, 1, D_DOWN) == 0;
}

struct TileProp *landed_on( struct Character *c) {
    struct TileProp *t, *p = c->prop;
    int i;

    if( !landed( c)) return NULL;
    for( i = 0; i <= p->w/TILEWIDTH/2; i++) {
        t = tile_prop((c->x+p->w/2)/TILEWIDTH + i,(c->y+p->h)/TILEHEIGHT);
        if( t && (t->flags & TP_SUBSTRATE)) return t;
        t = tile_prop((c->x+p->w/2)/TILEWIDTH - i - 1,(c->y+p->h)/TILEHEIGHT);
        if( t && (t->flags & TP_SUBSTRATE)) return t;
    }
    return mapTileToProp[M_SOLIDGROUND];
}

void accelerate( struct Character *c, int joystick_dir)
{
    struct TileProp *t;
    int dir = joystick_dir, accel, slipping;
    if( c->x_velocity < 0) dir = D_LEFT;
    if( c->x_velocity > 0) dir = D_RIGHT;

    // Ignore up and down directions
    if( joystick_dir != D_LEFT && joystick_dir != D_RIGHT) joystick_dir=D_NONE;

    t = landed_on(c);
    slipping = (t && (t->flags & TP_SLIPPERY));

    if( joystick_dir != D_NONE) {
        accel = dir_x[joystick_dir] * (slipping ? HORIZ_ACCEL / 2 :
            (dir==joystick_dir ? (t?HORIZ_ACCEL:HORIZ_ACCEL*3/4):HORIZ_SKID));
    } else if(t && dir != D_NONE) {
        accel = -dir_x[dir] * (slipping ? HORIZ_ACCEL / 4 : HORIZ_DECEL);
    } else {
        accel = 0;
    }

    c->x_velocity += accel;

    if( c->x_velocity < -MAX_SPEED) c->x_velocity = -MAX_SPEED;
    if( c->x_velocity > MAX_SPEED) c->x_velocity = MAX_SPEED;

    // Disallow instantaneous change of direction
    if( c->x_velocity < 0 && dir == D_RIGHT ||
        c->x_velocity > 0 && dir == D_LEFT) c->x_velocity = 0;
}

void handle_x_momentum( struct Character *c)
{
    struct TileProp *p = c->prop;
    int dir, proposed_dist, remain, backoff, dist, maxup, maxdown;

    if( c->x_velocity == 0) return;

    dir = (c->x_velocity < 0 ? D_LEFT : D_RIGHT);
    proposed_dist = (dir == D_LEFT ? -c->x_velocity : c->x_velocity);

    // Butt the character up against the edge of its current pixel
    remain = (dir == D_LEFT ? c->x_residue : PRECISION-1-c->x_residue);
    if( proposed_dist <= remain) {
        // We aren't going far enough to reach the edge of the current pixel.
        c->x_residue += (dir == D_LEFT ? -proposed_dist : proposed_dist);
        return;
    }
    c->x_residue += (dir == D_LEFT ? -remain : remain);
    proposed_dist -= remain;

    backoff = PRECISION - 1 - (proposed_dist - 1) % PRECISION;
    proposed_dist = (proposed_dist + PRECISION - 1) / PRECISION;

    dist = motion_limit( c, proposed_dist, dir);
    c->x += (dir == D_LEFT ? -dist : dist);
    proposed_dist -= dist;

    while( proposed_dist > 0) {
        dist = 0;
        // If we try to take a step up, will we bump our head?
        maxup = motion_limit( c, 2, D_UP);
        if( maxup >= 1) {
            dist = offset_motion_limit( c, 0, -1, proposed_dist, dir);
            if( dist) {
                // Walk up a step of height 1
                c->y --;
                c->x += (dir == D_LEFT ? -dist : dist);
            } else if( maxup >= 2) {
                dist = offset_motion_limit( c, 0, -2, proposed_dist, dir);
                if( dist) {
                    // Walk up a step of height 2
                    c->y -= 2;
                    c->x += (dir == D_LEFT ? -dist : dist);
                }
            }
        }
        if( dist == 0) break;
        proposed_dist -= dist;
    }

    if( proposed_dist == 0) {
        c->x_residue += (dir == D_LEFT ? backoff : -backoff);
    } else {
        // We hit a wall
        c->x_velocity = 0;
    }

    // If the player is one or two pixels above ground and his
    // y_velocity is 0, he's probably just walking downhill.  Lower him
    // so the falling logic doesn't kick in.
    if( c->y_velocity == 0) {
        maxdown = motion_limit( c, 3, D_DOWN);
        if( maxdown < 3) c->y += maxdown;
    }
}

void handle_y_momentum( struct Character *c)
{
    struct TileProp *p = c->prop;
    int dir, proposed_y, proposed_residue, proposed_dist, dist;

	proposed_y = c->y + c->y_velocity / PRECISION;
	proposed_residue = c->y_residue += c->y_velocity % PRECISION;
	if( proposed_residue < 0) {
		proposed_residue += PRECISION;
		proposed_y --;
	} else if( proposed_residue >= PRECISION) {
		proposed_residue -= PRECISION;
		proposed_y ++;
	}

    dir = (proposed_y < c->y ? D_UP : D_DOWN);
    proposed_dist = (dir == D_UP ? c->y-proposed_y : proposed_y-c->y);

    dist = motion_limit(c, proposed_dist, dir);
    if( dist < proposed_dist) {
        c->y = (dir == D_UP ? c->y - dist : c->y + dist);
        c->y_residue = 0;
        c->y_velocity = 0;
    } else {
        c->y = proposed_y;
        c->y_residue = proposed_residue;
    }
}

void apply_gravity( struct Character *c)
{
    if( !landed( c)) {
        // The player is falling - accelerate downwards
        c->y_velocity += c->gravity;
        if( c->y_velocity > TERMINALVELOCITY) {
            // Terminal velocity
            c->y_velocity = TERMINALVELOCITY;
        }
    }
}

int add_npc(int type,int x,int y,int dir,int id)
{
	int i;

	i=game.npcCount;
	if(i>=MAXNPC) return -1;
	//printf("Adding type %d at %d,%d dir=%d [prop=%d; id=%d]\n",type,x,y,dir,(mapTileToProp[id]-tileProp),id);

	game.npcCount++;

	game.npc[i].type=type;
	game.npc[i].x=x;
	game.npc[i].x_residue=PRECISION/2;
	game.npc[i].y=y;
	game.npc[i].y_residue=PRECISION/2;
	game.npc[i].homex=x;
	game.npc[i].homey=y;
	game.npc[i].facing=dir;
	game.npc[i].x_velocity=0;
	game.npc[i].y_velocity=0;
    game.npc[i].gravity=0;

	game.npc[i].frame=0;
	game.npc[i].health=100;
	game.npc[i].prop=mapTileToProp[id];
	game.npc[i].delay=0;

	return i;
}

int add_efx(int type,int x,int y,int delay)
{
	int i=game.efxCount;
	
	if(i>=MAXEFX) return -1;
	
#if 1
	// Find left edge of trampoline
	for(;;) {
		int id=-1;
		if(x>=0 && x<game.w && y>=0 && y<game.h) id=game.map[x+game.w*y];
		struct TileProp *tileProp=tile_to_prop(id);
		if(tileProp==NULL) return -1;	// Corrupt mapping
		if(tileProp->id==id) break;
		x--;
	}
	
	game.effect[i].type=type;
	game.effect[i].x=x;
	game.effect[i].y=y;
	game.effect[i].tileProp=tile_to_prop(game.map[x+game.w*y]);
	game.effect[i].delay=delay;
	game.efxCount++;
#endif
	return i;
}

int find_efx(int x,int y)
{
#if 1
	int i;
	for(i=0;i<game.efxCount;i++) {
		int dx=x-game.effect[i].x;
		int dy=y-game.effect[i].y;
		if(dx>=0 && dx<game.effect[i].tileProp->gfxw && dy>=0 && dy<game.effect[i].tileProp->gfxh) {
			return i;
		}
	}
#endif
	return -1;
}

void new_level(int level)
{
	char buf[256];
	sprintf(buf,"map/level%03d.txt",level);
	load_level(buf);
	game.level=level;
	play_sound(S_LEVELSONG);
	play_sound(S_LEVELSTART);
	game.mode=MODE_GETREADY;
}

void construct_level(int w,int h) {
	int i;
	if(game.map) free(game.map);
	if(w<SCREENWIDTH) w=SCREENWIDTH;
	if(h<SCREENHEIGHT) h=SCREENHEIGHT;
	//printf("Decided on %dx%d map\n",w,h);
	game.map=(short *)malloc(w*h*sizeof(short));
	game.w=w;
	game.h=h;
	game.t=0;
	game.l=0;
	game.level=1;
	game.npcCount=0;	// Reset for new level.
	game.efxCount=0;	// Reset for new level.
	game.checkpointX=0;
	game.checkpointY=0;
	game.collected=0;
	game.available=0;	// Number of pieces of fruit to collect.
	game.bonus[0]=0;
	game.bonus[1]=0;
	game.bonus[2]=0;
	game.activeTeleport=-1;
	for(i=0;i<4;i++) {
		game.key[i]=0;
	}
	for(i=0;i<8;i++) {
		game.teleporterX[i]=-1;
		game.teleporterY[i]=-1;
	}

	for(i=0;i<w*h;i++) game.map[i]=-1;
}

void add_teleporter(int teleporterNo,int x,int y)
{
	if(game.teleporterX[teleporterNo*2]==-1) {
		game.teleporterX[teleporterNo*2]=x;
		game.teleporterY[teleporterNo*2]=y;
	} else {
		game.teleporterX[teleporterNo*2+1]=x;
		game.teleporterY[teleporterNo*2+1]=y;
	}
}

void load_level(const char *fname)
{
	FILE *file;
	int len=0;
	unsigned char *data=0;
	int i,j,k;
	int x,y;
	int w=0,h=0;
	int bol=0;
	int attr=0;
	enum ParseMode { PM_BOL,PM_COMMENT,PM_LINE,PM_ATTRIBUTE } pm=PM_BOL;
	const char *buf=fname;

	file=fopen(buf,"rb");
	if(file) {
		fseek(file,0,SEEK_END);
		len=ftell(file);
		fseek(file,0,SEEK_SET);
		if(len>0) {
			data=(unsigned char *)malloc(len+1);
			data[len]=0;
			fread(data,len,1,file);
			fclose(file);		// Slurped it in.  
		} else {
			fprintf(stderr,"Could not load empty file %s\n",buf);
			fclose(file);
		}
	} else {
		fprintf(stderr,"Could not find needed file %s\n",buf);
	}
	if(data==0) {
		fprintf(stderr,"Loading default level\n%s\n",defaultlevel);
		// Default map to the rescue.
		data=(unsigned char *)malloc(strlen(defaultlevel)+1);
		len=strlen(defaultlevel);
		strcpy((char *)data,defaultlevel);
	}

	// Now figure out what it says:
	// Pass 1 through file:
	for(i=0;i<len;i++) {
		if(data[i]=='"') {
			pm=PM_COMMENT;
		} else if(data[i]=='\r' || data[i]=='\n') {
			if(pm==PM_LINE) {
				if(w<i-bol) w=i-bol;
				h++;
			}
			pm=PM_BOL;
			bol=i+1;
		} else if(pm==PM_COMMENT) {
			// Skip comment
		} else if(data[i]=='=') {
			attr=i+1;
			pm=PM_ATTRIBUTE;
		} else if(pm!=PM_ATTRIBUTE) {
			pm=PM_LINE;
		}
	}
	if(pm==PM_LINE) {
		if(w<i-bol) w=i-bol;
		h++;
	}
	// Now we should have the map size.
	//printf("Loading a %dx%d map...\n",w,h);
	construct_level(w,h);
	// Now create the path sections
	x=0;
	y=0;
	pm=PM_BOL;
	bol=0;
	for(i=0;i<len;i++) {
		if(data[i]=='"') {
			pm=PM_COMMENT;
		} else if(data[i]=='\r' || data[i]=='\n') {
			if(pm==PM_ATTRIBUTE) {
				// Should add it to the attributes list.
			} else if(pm==PM_LINE) {
				y++;
				x=0;
			}
			pm=PM_BOL;
			bol=i+1;
		} else if(pm==PM_COMMENT) {
			// Skip comment
		} else if(pm==PM_ATTRIBUTE) {
			// Collect attribute at the end
		} else if(data[i]=='=') {
			attr=i+1;
			pm=PM_ATTRIBUTE;
			x=0;
		} else if(pm!=PM_ATTRIBUTE) {
			int id=mapCharToTile[data[i]];
			struct TileProp *prop=NULL;
			if(id>=0) prop=mapTileToProp[id];

			if(!prop && data[i]!=' ') printf("Error loading cell %d,%d: %c\n",x,y,data[i]);

			pm=PM_LINE;
			if(prop && game.map[x+w*y]==-1) {
				if(prop->flags&TP_MAP_MODULUS) {
					int tilex=(x%prop->gfxw);
					int tiley=(y%prop->gfxh);
					int value;
					if(prop->gfxh==1) tilex=(x+y)%prop->gfxw;	// Special case
					if(prop->gfxw==1) tiley=(x+y)%prop->gfxh;	// Special case
					value=prop->id+tilex+TILESACROSS*tiley;
					//printf("(prop-tileProp)=%d: Tiling @ %d,%d, %d + picking offset +%d+%d ==> %d\n",(prop-tileProp),x,y,prop->id,tilex,tiley,value);
					game.map[x+w*y]=value;
				} else if(prop->flags&TP_NPC && editMode==0) {
					int type=T_CHICK;

					switch(prop->id) {
					case M_CHICKRIGHT1:
						type=T_CHICK;
						break;
					case M_SHOOTERRIGHT1:
						type=T_SHOOTER;
						break;
					case M_BIRDRIGHT1:
						type=T_BIRD;
						break;
					case M_WALLCLIMBERLEFT:
					case M_WALLCLIMBERRIGHT:
						type=T_BUG;
						break;
					case M_EVILDUCKLEFT1:
						type=T_EVILDUCK;
						break;
					case M_TRAMPOLINE1:
					case M_TRAMPOLINE2:
						type=T_TRAMPOLINE;
						break;
					}
					if(test_npc(x*TILEWIDTH,y*TILEHEIGHT)==-1) {
						add_npc(type,x*TILEWIDTH+mapTileToProp[id]->offsetx,
                            y*TILEHEIGHT+mapTileToProp[id]->offsety,D_LEFT,id);
					}
				} else if(prop == tileProp && editMode==0) {
					// We've found the start point for the level.
					if(game.checkpointX==0 && game.checkpointY==0) {
						game.checkpointX=x;
						game.checkpointY=y;
						//printf("Set start point to: %d,%d (\"%c\")\n",game.checkpointX,game.checkpointY,data[i]);
					}
				} else {	// Fill in the tiles for the whole object.
					for(j=0;j<prop->gfxh;j++) {
						for(k=0;k<prop->gfxw;k++) {
							game.map[x+k+(y+j)*w]=id+k+TILESACROSS*j;
						}
					}
					switch(prop->id) {
					case M_REDTELEPORTER: add_teleporter(3,x,y); break;
					case M_GREENTELEPORTER: add_teleporter(2,x,y); break;
					case M_BLUETELEPORTER: add_teleporter(1,x,y); break;
					case M_YELLOWTELEPORTER: add_teleporter(0,x,y); break;
					}
				}
				if(prop->id==M_FRUIT1) {
					game.available++;
				}
			}
			x++;
		}
	}
	game.player.health=100;
	game.player.invincible=0;
	game.player.x=game.checkpointX*TILEWIDTH+game.player.prop->offsetx;
	game.player.x_residue=PRECISION/2;
	game.player.y=game.checkpointY*TILEHEIGHT+game.player.prop->offsety;
	game.player.y_residue=PRECISION/2;
	game.player.x_velocity=0;
	game.player.y_velocity=0;
	game.player.gravity=HIGHGRAVITY;
	game.player.facing=D_RIGHT;
	game.invalidate=2;
//	if(((game.level/3)%2)==0) play_sound(S_RUSH);
//	else play_sound(S_ALONE);
#ifdef _DEBUG
	for(j=0;j<h;j++) {
		for(i=0;i<w;i++) {
			int cell=game.map[i+j*w];
			if(cell<0) printf("?");
			else printf("%c",*mapTileToProp[cell]);
		}
		printf("\n");
	}
#endif
}

void save_level(const char *fname) 
{
	int x,y;
	FILE *file;
	int blank=0;

	file=fopen(fname,"w");

	for(y=0;y<game.h;y++) {
		for(x=0;x<game.w;x++) {
			if(game.map[x+y*game.w]==-1) {
				fprintf(file," ");
				blank++;
			} else {
				int tile=game.map[x+y*game.w];
				//fprintf(file,"t=%d;p=%d;",tile,prop);
				fprintf(file,"%c",mapTileToProp[tile]->ch);
			}
		}
		fprintf(file,"\n");
	}
	fclose(file);
	printf("%d of %d tiles are sky.\n",blank,game.w*game.h);
}

void draw_map()
{
	int x,y,id,i;
    struct TileProp *prop;
	if(game.map==0) return;

	//printf("Drawing at: %d,%d @+%d+%d\n",game.w,game.h,game.l,game.t);
	for(y=0;y<=SCREENHEIGHT;y++) {
			for(x=0;x<=SCREENWIDTH;x++) {
				int pos=game.l/TILEWIDTH+x+(game.t/TILEHEIGHT+y)*game.w;
				if(pos<0 || pos>=game.w*game.h) pos=0;
				draw_tile(game.map[pos],
                    x*TILEWIDTH-(game.l%TILEWIDTH),
                    y*TILEHEIGHT-(game.t%TILEHEIGHT));
			}
	}
	if(game.invalidate>0) game.invalidate--;
	prop=game.player.prop;
	//printf("Hero prop=%d; width=%d; frame=%d; ",prop-tileProp,prop->.w,game.player.frame);
	id=prop->id+prop->gfxw*game.player.frame;
	if(game.player.facing==D_LEFT && (prop->flags&TP_MIRROR_BELOW)) {
		id+=prop->gfxh*TILESACROSS;
	}
	if(game.player.facing==D_LEFT && (prop->flags&TP_MIRROR_BESIDE)) {
		id+=prop->gfxw*prop->frames;
	}
	//printf("Hero id=%d\n",id);
	if( (game.player.invincible % 20) < 10) {
		draw_tile_rect(id,game.player.x-game.player.prop->offsetx-game.l,game.player.y-game.player.prop->offsety-game.t,prop->gfxw,prop->gfxh);
	}
	if( game.player.invincible > 0) game.player.invincible --;

	// Now draw all the npcs:
	for(i=0;i<game.npcCount;i++) {
		prop=game.npc[i].prop;
		//printf("Hero prop=%d; width=%d; frame=%d; ",prop-tileProp,prop->w,game.player.frame);
        id=prop->id;
        if(prop->flags & TP_MAP_MODULUS) {
            id+=(game.npc[i].frame%prop->gfxw) +
                (game.npc[i].frame/prop->gfxw)*TILESACROSS;
        } else {
            id+=prop->gfxw*game.npc[i].frame;
        }
		if(game.npc[i].facing==D_LEFT && (prop->flags&TP_MIRROR_BELOW)) {
			id+=prop->gfxh*TILESACROSS;
		}
		if(game.npc[i].facing==D_LEFT && (prop->flags&TP_MIRROR_BESIDE)) {
			id+=prop->gfxw*prop->frames;
		}
        if(prop->flags & TP_MAP_MODULUS) {
            draw_tile_rect(id,game.npc[i].x-game.npc[i].prop->offsetx-game.l,game.npc[i].y-game.npc[i].prop->offsety-game.t,1,1);
        } else {
            draw_tile_rect(id,game.npc[i].x-game.npc[i].prop->offsetx-game.l,game.npc[i].y-game.npc[i].prop->offsety-game.t,prop->gfxw,prop->gfxh);
        }
	}

#if 0
	// Now decode the number into decimal.
	int num;
	num=game.level;
	draw_number((num/10)%10,TILEWIDTH*3,0);
	draw_number(num%10,TILEWIDTH*4,0);

	num=game.score;
	draw_number((num/1000)%10,TILEWIDTH*8,0);
	draw_number((num/100)%10,TILEWIDTH*9,0);
	draw_number((num/10)%10,TILEWIDTH*10,0);
	draw_number(num%10,TILEWIDTH*11,0);	

	num=game.lives;
	if(num<0) num=0;
	draw_number(num%10,TILEWIDTH*18,0);
	
	if(game.lives<1) {
		for(i=0;i<5;i++) {
			// Game Over
			draw_number(10+i,TILEWIDTH*(SCREENWIDTH/2-2+i),TILEHEIGHT*5);
		}
	}
#endif
}

void set_player_direction(int direction)
{
	if(game.activeTeleport==-1) 
		game.joystick=direction;
	else {
		game.joystick=-1;
		game.player.x_velocity=0;
		game.player.y_velocity=0;
	}
}

void do_jump()		// Handle CROSS
{
	if(game.lives==0) {
		new_game();
		new_level(game.level);
		return;
	}
	if(game.activeTeleport>=0) return;
	if(landed(&game.player)) {
        game.player.y_velocity=-JUMPINITIAL;
        play_sound(S_JUMP);
    }
    game.player.gravity = LOWGRAVITY;
}

void release_jump()
{
    game.player.gravity = HIGHGRAVITY;
}

void do_attack()		// Handle SQUARE
{
	// Check map for teleporter.
	int i;
	for(i=0;i<8;i++) {
		if(game.teleporterX[i]==-1 || game.teleporterY[i]==-1) continue;
		if(game.player.x>game.teleporterX[i]*TILEWIDTH && 
		game.player.x<(game.teleporterX[i]+3)*TILEWIDTH &&
		game.player.y>game.teleporterY[i]*TILEHEIGHT &&
		game.player.y<(game.teleporterY[i]+3)*TILEHEIGHT &&
		game.teleporterX[i^1]!=-1 && game.teleporterY[i^1]!=-1) {
			// Begin teleportation.
			int n=add_npc(T_SPECIALEFX,game.teleporterX[i]*TILEWIDTH,game.teleporterY[i]*TILEHEIGHT,D_LEFT,M_TELEPORTATION);
			if(n>=0) game.npc[n].delay=TILEWIDTH*PATROLDIST*3;
			game.activeTeleport=i;
			play_sound(S_TELEPORT);
			return;
		}
	}
	play_sound(S_BEEP);
}

int add_shot(struct Character *c,int dir,int type) {
    int shot;
    int x = c->x+(c->prop->w-mapTileToProp[type]->w)/2;
    if( dir == D_UP || dir == D_DOWN) x += TILEWIDTH * dir_x[c->facing];

    shot = add_npc(T_SHOT,x,c->y+(c->prop->h-mapTileToProp[type]->h)/2,D_NONE,type);
    if( shot >= 0) {
        game.npc[shot].x_velocity = 2 * PRECISION * dir_x[dir];
        game.npc[shot].y_velocity = 2 * PRECISION * dir_y[dir];
    }
    return shot;
}

void do_special()		// Handle CIRCLE (shoot)
{
	int dir=game.player.facing,shot;
	if(dir!=D_LEFT) dir=D_RIGHT;
	game.player.prop=mapTileToProp[M_FIRELEFT];
	game.player.frame=0;
	if(game.joystick==D_DOWN || game.joystick==D_UP) {
        int tile = game.joystick==D_DOWN?M_SHOOTDOWNLEFT:M_SHOOTUPLEFT;
		game.player.prop=mapTileToProp[tile];
        add_shot(&game.player,game.joystick,M_HEROSHOT);
		play_sound(S_SHOOT);
		return;
	}
    add_shot(&game.player,dir,M_HEROSHOT);
	play_sound(S_SHOOT);
}

void do_bomb()		// Handle TRIANGLE (bomb)
{
	int dir=game.player.facing,shot;
	if(dir!=D_LEFT) dir=D_RIGHT;
	game.player.prop=mapTileToProp[M_FIRELEFT];
	game.player.frame=0;
	if(game.bomb>0) {
		if(game.joystick==D_DOWN || game.joystick==D_UP) {
            int tile = game.joystick==D_DOWN?M_SHOOTDOWNLEFT:M_SHOOTUPLEFT;
			game.player.prop=mapTileToProp[tile];
            add_shot(&game.player,game.joystick,M_BOMB);
			play_sound(S_BOMB);
			game.bomb--;
			return;
		}
        add_shot(&game.player,dir,M_BOMB);
		play_sound(S_BOMB);
		game.bomb--;
	} else {
		play_sound(S_BEEP);
	}
}

int npc_damage(struct Character *npc,int percent)
{
	if( game.player.invincible > 0 && percent>0) return 0;

    if( game.player.x < npc->x + npc->prop->w &&
        game.player.x + game.player.prop->w > npc->x &&
        game.player.y < npc->y + npc->prop->h &&
        game.player.y + game.player.prop->h > npc->y)
    {
		if(percent>0) play_sound(S_DIE);
		game.player.health-=percent;
		if(percent>0) game.player.invincible = 200;
		return 1;
	}
	return 0;
}

void clear_across_type(int x,int y,int type)
{
	int i;
	int dir;
	int changed;
	
	// Blast a hole in the secret area
	dir=-1;
	while(dir<=1) {
		i=dir<0?-dir:0;
		changed=1;
		while(changed) {
			i+=dir;
			changed=0;
			if(x+i>=0 && x+i<game.w && y>=0 && y<game.h) {
				int pos=(x+i)+y*game.w;
				int id=game.map[pos];
				struct TileProp *prop=NULL;
				if(id>-1) prop=mapTileToProp[id];
				if(prop && prop->id==type) {
					game.map[pos]=-1;
					changed=1;
				}
			}
		}	
		dir+=2;
	}
}

void clear_down_type(int x,int y,int type)
{
	int i;
	int dir;
	int changed;
	
	// Blast a hole in the secret area
	dir=-1;
	while(dir<=1) {
		i=dir<0?-dir:0;
		changed=1;
		while(changed) {
			i+=dir;
			changed=0;
			if(y+i>=0 && y+i<game.h && x>=0 && x<game.w) {
				int pos=x+(i+y)*game.w;
				int id=game.map[pos];
				struct TileProp *prop=NULL;
				if(id>-1) prop=mapTileToProp[id];
				if(prop && prop->id==type) {
					game.map[pos]=-1;
					changed=1;
				}
			}
		}
		dir+=2;
	}
}

struct TileProp *find_corner(int *x,int *y)
{
	struct TileProp *prop;
	int old;
	if((*x)<0 || (*x)>=game.w || (*y)<0 || (*y)>=game.h) return NULL;
	old=game.map[(*x)+(*y)*game.w];
    if( old == -1) return NULL;
	prop=mapTileToProp[old];
    *y -= (old - prop->id) / TILESACROSS;
    *x -= ((old - prop->id) % TILESACROSS) % prop->gfxw;
    return prop;
}

void clear_ring(int x,int y)
{
	int i,j;
	int w=1,h=1;
	int pos;
	int old;

	struct TileProp *prop=find_corner(&x,&y);
	if(prop) {
		w=prop->gfxw;
		h=prop->gfxh;
	}
		
	pos=0;
	if(x<0 || x>=game.w || y<0 || y>=game.h) return;
	old=game.map[x+game.w*y];
	for(j=0;j<h;j++) {
		for(i=0;i<w;i++) {
			if(x>=game.w) continue;
			if(y>=game.h) break;
			//printf("Clearing tile %d,%d (%d)\n",x+i,y+j,game.map[x+i+game.w*(y+j)]);
			game.map[x+i+(y+j)*game.w]=-1;
		}
	}
}

void update_npc(struct Character *npc)
{
    struct TileProp *lookahead;
	int x,y,d,i;

	switch(npc->type) {
	case T_CHICK:
	case T_BIRD:
	case T_EVILDUCK:
        if(npc->type==T_BIRD || npc->type==T_CHICK || npc->type==T_EVILDUCK) {
            npc->frame=(npc->x/24)%npc->prop->frames;
        }

		// Patrol vacinity of checkpoint. with Gravity/Flight
        if( npc->x - npc->homex > TILEWIDTH*PATROLDIST ||
            npc->homex - npc->x > TILEWIDTH*PATROLDIST) npc->x_velocity = 0;
        if( npc->x_velocity == 0) {
            if(npc->facing!=D_LEFT && npc->facing!=D_RIGHT) npc->facing=D_LEFT;
            npc->facing = D_RIGHT + D_LEFT - npc->facing;
            npc->x_velocity = (npc->facing == D_LEFT ? -PRECISION : PRECISION);
        }

        if(npc->type==T_EVILDUCK) {
            // Make the duck jump
            if( landed(npc)) npc->y_velocity=-JUMPINITIAL;
            npc->gravity = LOWGRAVITY;
			npc_damage(npc,45);
		} else if(npc_damage(npc,20)) {
			npc->type=-1;
		}

		break;
	case T_BUG:
        if(npc->facing!=D_UP && npc->facing!=D_DOWN) npc->facing=D_UP;

		// Patrol vacinity of checkpoint. Climber
        if( npc->y - npc->homey > TILEHEIGHT*PATROLDIST ||
            npc->homey - npc->y > TILEHEIGHT*PATROLDIST) npc->y_velocity = 0;
        if( npc->y_velocity == 0) {
            npc->facing = D_UP + D_DOWN - npc->facing;
            npc->y_velocity = (npc->facing == D_UP ? -PRECISION : PRECISION)/2;
        }
        npc->frame = (npc->facing==D_DOWN?1:0);
        
		npc_damage(npc,35);

		break;
	case T_SHOOTER:
		// Count down and then shoot towards player.
		if(npc->delay>0) {
			npc->delay--;
			break;
		}
		npc->frame++;
		if(npc->frame==2) {
			npc->frame=0;
            npc->facing = (game.player.x<npc->x ? D_LEFT : D_RIGHT);
            add_shot(npc,npc->facing,M_ENEMYSHOT);
		}
		npc->delay=TILEWIDTH*PATROLDIST*3;
		break;
	case T_SHOT:
        if( npc->x_velocity == 0 && npc->y_velocity == 0) {
            npc->type = -1;
            break;
        }

        x = npc->x + npc->prop->w / 2;
        y = npc->y + npc->prop->h / 2;
        if( npc->x_velocity) {
            x = npc->x + npc->x_velocity / PRECISION +
                (npc->x_velocity < 0 ? 0 : npc->prop->w);
        }
        if( npc->y_velocity) {
            y = npc->y + npc->y_velocity / PRECISION +
                (npc->y_velocity < 0 ? 0 : npc->prop->h);
        }
        x /= TILEWIDTH;
        y /= TILEHEIGHT;
        lookahead = tile_prop(x,y);

        // Move until we hit something
        if(lookahead && ((lookahead->flags&TP_SUBSTRATE) ||
            (lookahead->id==M_CHEST1 && find_efx(x,y)==-1 &&
             game.map[x+game.w*y]%TILESACROSS/lookahead->gfxw==0)) ) {
			// Die, since we hit something.
			npc->type=-1;
			if(npc->prop!=mapTileToProp[M_ENEMYSHOT]) {
				int i;
				switch(lookahead->id) {
				case M_SECRETAREA:
					// Blast a hole in the secret area
					if(npc->y_velocity) {
						clear_across_type(x,y,lookahead->id);
					} else {
						clear_down_type(x,y,lookahead->id);
					}
					break;
				}
			}
			if(npc->prop==mapTileToProp[M_BOMB]) {
				switch(lookahead->id) {
				case M_BOMBABLEWALL:
                    if(npc->y_velocity) {
						clear_across_type(x,y,lookahead->id);
					} else {
						clear_down_type(x,y,lookahead->id);
					}
					break;
				case M_ROCKETPLATFORM:
					if(npc->y_velocity < 0) {
						clear_across_type(x,y,lookahead->id);
					}
					break;
				case M_POUNDPLATFORM:
					if(npc->y_velocity > 0) {
						clear_across_type(x,y,lookahead->id);
					}
					break;
				case M_CHEST1:
                    {
                    int xx=x,yy=y;
                    find_corner(&xx,&yy);
                    add_efx(T_CHEST,xx,yy,1);
                    }
					break;
				}				
			}
		}
		if( npc->x-npc->prop->offsetx >= game.w*TILEWIDTH ||
            npc->x-npc->prop->offsetx < 0) npc->type=-1;
		// Now test for colision with list of npcs, if it is HEROSHOT
		if(npc->prop!=mapTileToProp[M_ENEMYSHOT]) for(i=0;i<game.npcCount;i++) {
			if(game.npc+i==npc) continue;
			if(game.npc[i].type==-1) continue;
			if( game.npc[i].x > npc->x + npc->prop->w ||
                game.npc[i].y > npc->y + npc->prop->h ||
                game.npc[i].x + game.npc[i].prop->w < npc->x ||
                game.npc[i].y + game.npc[i].prop->h < npc->y)
				continue;
			// HEROSHOT can only kill small critters in one shot.
			if(npc->prop==mapTileToProp[M_HEROSHOT]) {
				struct TileProp *targetprop=game.npc[i].prop;
				int area=targetprop->gfxw*targetprop->gfxh;
				if(area<=0) area=1;
				game.npc[i].health-=100/area;
				if(game.npc[i].health<=0) {
					game.npc[i].type=-1;
				}
			} else {
				// We have a colision -- bomb explodes, and they do too.
				game.npc[i].type=-1;
			}
			npc->type=-1;
			break;
		}

		// Now test for colision with hero if it is ENEMYSHOT
		if(npc->prop==mapTileToProp[M_ENEMYSHOT]) {
			if(npc_damage(npc,25)) {
				npc->type=-1;	// Stop in it's tracks.
			}
		}
		break;
	case T_FRUIT:
		if(landed(npc)) {
	        npc->y_velocity=0;
	        npc->x_velocity=0;
            if(npc_damage(npc,-1)) {
                npc->type=-1;	// Stop in it's tracks.
                game.score+=10;
            }
		}
        npc->gravity = HIGHGRAVITY;
        break;
    case T_SPECIALEFX:	// Teleporter
    	game.player.x_velocity=0;
    	game.player.y_velocity=0;
		if(npc->delay>0) {
			npc->delay--;
			npc->frame=(npc->delay/15)%npc->prop->frames;
		} else if(npc->facing==D_LEFT) {	// Teleport transfer
			game.activeTeleport^=1;
			npc->facing=D_RIGHT;
			npc->x=game.teleporterX[game.activeTeleport]*TILEWIDTH;
			npc->y=game.teleporterY[game.activeTeleport]*TILEHEIGHT;
			npc->delay=TILEWIDTH*PATROLDIST*3;
			game.player.x=npc->x+game.player.prop->offsetx;
			game.player.y=npc->y+game.player.prop->offsety;
			play_sound(S_TELEPORT);
		} else {
			npc->type=-1;	// Effect done
			game.activeTeleport=-1;
		}
	}

    handle_x_momentum( npc);
    handle_y_momentum( npc);
    apply_gravity( npc);
}

void update_efx(struct BoardEffect *efx)
{
	int id;
	int x=efx->x;
	int y=efx->y;
	int i;
	int frame;
	
	if(efx->delay>0) {
		efx->delay--;
		return;
	}
	switch(efx->type) {
	case T_TRAMPOLINE:
		//printf("Updating Trampoline: %d,%d[]=%d (%d frames)\n",x,y,game.map[x+game.w*y],efx->tileProp->frames);
		if(game.map[x+game.w*y]==M_TRAMPOLINE2) {
			for(i=0;i<efx->tileProp->gfxw;i++) {
				game.map[x+i+game.w*y]=M_TRAMPOLINE1+i;
			}
			efx->type=-1;
		} else if(game.map[x+game.w*y]==M_TRAMPOLINE1) {
			for(i=0;i<efx->tileProp->gfxw;i++) {
				game.map[x+i+game.w*y]=M_TRAMPOLINE2+i;
			}
			efx->delay=PATROLDIST*TILEWIDTH;
		} else {
			efx->type=-1;
		}
		break;
	case T_CHEST:
		id=efx->tileProp->id;
		// Exploding chest
		if(game.map[x+game.w*y]>=id && game.map[x+game.w*y]-id<efx->tileProp->gfxw*(efx->tileProp->frames-1)) {
			int i,j;
			// Chest Exploding soon...
			for(j=0;j<efx->tileProp->gfxh;j++) {
				for(i=0;i<efx->tileProp->gfxw;i++) {
					printf("%d,%d=%d+%d\n",x+i,y+i,game.map[x+i+(y+j)*game.w],efx->tileProp->gfxw);
					game.map[x+i+(y+j)*game.w]+=efx->tileProp->gfxw;
				}
			}
			//game.map[x+game.w*y]++;
			efx->delay=TILEWIDTH;
		} else {
			int i;
			// Exploded now...
			// We add 16 fruit...
			efx->type=-1;
			for(i=0;i<8;i++) {
				int npc=add_npc(T_FRUIT,x*TILEWIDTH+mapTileToProp[M_FRUIT1]->offsetx,y*TILEHEIGHT+mapTileToProp[M_FRUIT1]->offsety,D_LEFT,M_FRUIT1);
				if(npc>=0) {
					game.npc[npc].y_velocity=-ROCKETINITIAL*(i+1)/8;
					game.npc[npc].x_velocity=-PRECISION;
                    game.npc[npc].frame = i % 4;
				}
				npc=add_npc(T_FRUIT,x*TILEWIDTH+mapTileToProp[M_FRUIT1]->offsetx,y*TILEHEIGHT+mapTileToProp[M_FRUIT1]->offsety,D_RIGHT,M_FRUIT1);
				if(npc>=0) {
					game.npc[npc].y_velocity=-ROCKETINITIAL*(i+1)/8;
					game.npc[npc].x_velocity=PRECISION;
                    game.npc[npc].frame = (i+2) % 4;
				}
			}
		}
		break;
	case T_GROUND:
		id=efx->tileProp->id;
		if(game.map[x+game.w*y]>=id && game.map[x+game.w*y]-id<efx->tileProp->frames-1) {
			// Exploding soon...
			game.map[x+game.w*y]++;
		} else {
			// Exploded now...
			if(id==M_EXPLODINGGROUND) {
				game.map[x+game.w*y]=-1;
				//efx->type=-1;
			}
		}
		efx->delay=PATROLDIST*TILEWIDTH*5;
		break;
	}
}

void reset_efx()
{
	int i;
	for(i=0;i<game.efxCount;i++) {
		if(game.effect[i].type==T_GROUND) {
			game.map[game.effect[i].x+game.w*game.effect[i].y]=game.effect[i].tileProp->id;
			game.effect[i].type=-1;	// Restore until next interaction.
		}
	}
}

void enter_door(int x,int y)
{
	int i,j;
	struct TileProp *p=find_corner(&x,&y);
	int frame=(game.map[x+y*game.w]-p->id)/p->gfxw;
	if(frame>0) return;
	frame++;
	if(frame>p->frames) return;
	for(j=0;j<p->gfxh;j++) {
		for(i=0;i<p->gfxw;i++) {
			game.map[x+i+(y+j)*game.w]=p->id+i+p->gfxw*frame+j*TILESACROSS;
		}
	}
	play_sound(S_DOOR);
}

void next_frame_tick();

void next_frame(int elapsed)
{
	while(elapsed>5) {
		next_frame_tick();
		elapsed-=5;
	}
//	next_frame_tick();
//	SDL_Delay(20);
}

void next_frame_tick()
{
	int i,j;
	int k,l;
	int x,y;
	
	if( game.player.health <= 0) {
		reset_efx();
		game.player.health = 100;
		game.player.x=game.checkpointX*TILEWIDTH-game.player.prop->offsetx;
		game.player.x_residue=PRECISION/2;
		game.player.y=game.checkpointY*TILEHEIGHT-game.player.prop->offsety;
		game.player.y_residue=PRECISION/2;
		game.player.x_velocity=0;
		game.player.y_velocity=0;
        game.player.gravity=HIGHGRAVITY;
		game.player.facing=D_RIGHT;
		game.lives --;
		if(game.lives>0) {
			play_sound(S_LEVELSTART);
			game.mode=MODE_GETREADY;
		} else {
			game.mode=MODE_GAMEOVER;
			play_sound(S_LEVELEND);
		}
	}
	if(game.lives<1) return;
	if(game.player.y - game.player.prop->offsety > game.h*TILEHEIGHT)
        game.player.health=0;

	// See if moving in direction is currently valid.
	if(game.joystick==D_LEFT || game.joystick==D_RIGHT) {
        game.player.prop=mapTileToProp[M_WALKRIGHT1];
		game.player.facing=game.joystick;
	}

    accelerate( &game.player, game.joystick);

    handle_x_momentum( &game.player);
    handle_y_momentum( &game.player);
    apply_gravity( &game.player);

	// Attempt to centre the player on screen.
    x = game.player.x + game.player.prop->w / 2;
    y = game.player.y + game.player.prop->h / 2;
    if( x - game.l < SCREENWIDTH * TILEWIDTH / 3)
        game.l = x - SCREENWIDTH * TILEWIDTH / 3;
    else if( x - game.l > SCREENWIDTH * TILEWIDTH * 2 / 3)
        game.l = x - SCREENWIDTH * TILEWIDTH * 2 / 3;
    if( y - game.t < SCREENHEIGHT * TILEHEIGHT / 3)
        game.t = y - SCREENHEIGHT * TILEHEIGHT / 3;
    else if( y - game.t > SCREENHEIGHT * TILEHEIGHT * 2 / 3)
        game.t = y - SCREENHEIGHT * TILEHEIGHT * 2 / 3;
	if(game.t<0) game.t=0;
	if(game.t+TILEHEIGHT*SCREENHEIGHT>game.h*TILEHEIGHT) game.t=(game.h-SCREENHEIGHT)*TILEHEIGHT;
	if(game.l<0) game.l=0;
	if(game.l+TILEWIDTH*SCREENWIDTH>game.w*TILEWIDTH) game.l=(game.w-SCREENWIDTH)*TILEWIDTH;

/*	if(game.player.y->TILEHEIGHT*(SCREENHEIGHT-1)) {
		// finish level.
		game.level++;
		play_sound(S_LEVELEND);
		new_level(game.level);
		return;
	}
*/

    game.player.frame=(game.player.x/10)%game.player.prop->frames;

    x=game.player.x-game.player.prop->offsetx;
    y=game.player.y-game.player.prop->offsety;
    x/=TILEWIDTH;
    y/=TILEHEIGHT;
    // colision detection with map elements
    for(j=0;j<3;j++) {
        for(i=0;i<3;i++) {
            int id;
            if(y+j>=game.h || y+j<0 || x+i>=game.w || x+i<0) continue;
            id = game.map[(y+j)*game.w+x+i];
            if( id == -1) continue;
            switch(mapTileToProp[id]->id) {
            case M_FRUIT1:
                game.score+=10;
                game.collected++;
                game.map[(y+j)*game.w+x+i]=-1;
#ifdef _DEBUG
                //printf("Score=%d.  Got fruit!\n",game.score);
#endif
                play_sound(S_FRUIT);
                break;
            // Bonuses, need some symantics added.
            case M_RING:
				//printf("Clearing ring %d,%d (%d)\n",x+i,y+j,game.map[(x+i)+game.w*(y+j)]);
				clear_ring(x+i,y+j);
                if(game.bonus[0]==0) 
                    game.bonus[0]=1;
                else if(game.bonus[1]==0) 
                    game.bonus[1]=1;
                else if(game.bonus[2]==0)
                    game.bonus[2]=1;

                play_sound(S_LEVELEND);
                break;
            case M_MINIHEALTH:
                if(game.player.health+20<=100) {
					game.player.health+=20;
		            game.map[(y+j)*game.w+x+i]=-1;
		            play_sound(S_POWERUP);
                } else {
					// Don't collect it, if you are a full health.
					game.player.health=100;
				}
                break;
            case M_FULLHEALTH:
				if(game.player.health<100) {
	                game.player.health=100;
	                game.map[(y+j)*game.w+x+i]=-1;
	                play_sound(S_POWERUP);
				}
                break;
            case M_TIMEBONUS:
                // Need countdown clock on the level.
                game.map[(y+j)*game.w+x+i]=-1;
                play_sound(S_POWERUP);
                break;
            case M_LIFEBONUS:
                game.lives++;
                if(game.lives>99) game.lives=99;
                game.map[(y+j)*game.w+x+i]=-1;
                play_sound(S_POWERUP);
                break;
            case M_YINGYANG:
                // Need to implement yingyang.
                game.map[(y+j)*game.w+x+i]=-1;
                play_sound(S_POWERUP);
                break;
            case M_SHIELD:
                // Need to implement shield.
                game.map[(y+j)*game.w+x+i]=-1;
                game.player.invincible=2000;
                play_sound(S_POWERUP);
                break;
            case M_REDKEY:
				game.key[3]=1;
                game.map[(y+j)*game.w+x+i]=-1;
                play_sound(S_KEY);
                break;
            case M_GREENKEY:
				game.key[2]=1;
                game.map[(y+j)*game.w+x+i]=-1;
                play_sound(S_KEY);
                break;
            case M_BLUEKEY:
				game.key[1]=1;
                game.map[(y+j)*game.w+x+i]=-1;
                play_sound(S_KEY);
                break;
            case M_YELLOWKEY:
				game.key[0]=1;
                game.map[(y+j)*game.w+x+i]=-1;
                play_sound(S_KEY);
                break;
            case M_REDDOOR1:
				if(game.key[3]) enter_door(x+i,y+j);
				else play_sound(S_LOCKED);
				break;
            case M_GREENDOOR1:
				if(game.key[2]) enter_door(x+i,y+j);
				else play_sound(S_LOCKED);
				break;
            case M_BLUEDOOR1:
				if(game.key[1]) enter_door(x+i,y+j);
				else play_sound(S_LOCKED);
				break;
            case M_YELLOWDOOR1:
				if(game.key[0]) enter_door(x+i,y+j);
				else play_sound(S_LOCKED);
				break;
            case M_BOMB:
                game.map[(y+j)*game.w+x+i]=-1;
                game.bomb++;
                play_sound(S_POWERUP);
                break;
            case M_CHECKPOINT:
                // Set as current checkpoint.
                if(game.checkpointX!=x+i || game.checkpointY<y+j-2 || game.checkpointY>y+j+2) {
                    // New checkpoint sound.
                    play_sound(S_CHECKPOINT);
                }
                if(game.map[(y+j-1)*game.w+x+i]==M_CHECKPOINT) {
                    game.checkpointX=x+i-1;
                    game.checkpointY=y+j-2;
                } else {
                    game.checkpointX=x+i-1;
                    game.checkpointY=y+j-1;
                }
                break;
            case M_EXIT:
                if(game.collected>5*game.available/10) {
					new_level(game.level+1);
					game.mode=MODE_LEVELOVER;
				}
                break;
        	case M_EXPLODINGGROUND:
				if(find_efx(x+i,y+j)<0) 
					add_efx(T_GROUND,x+i,y+j,PATROLDIST*TILEWIDTH);
				break;
        	case M_INVISIBLEPLATFORM:
				if(find_efx(x+i,y+j)<0)
					add_efx(T_GROUND,x+i,y+j,1);
				break;
            case M_TRAMPOLINE1:
            case M_TRAMPOLINE2:
                //printf("[game.player.y=%d]; %d<%d; [y=%d,j=%d]\n",game.player.y,game.player.y+2*TILEHEIGHT,(y+j)*TILEHEIGHT,y,j);
                if(game.player.y-game.player.prop->offsety+2*TILEHEIGHT<=(y+j)*TILEHEIGHT) {	// Jumping or Falling
                    game.player.y_velocity=-ROCKETINITIAL;
                    // Now should animate the board element.
                    play_sound(S_TRAMPOLINE);
                    {
                        int i;
                        i=find_efx(x,y+j);
                        if(i<0) {
                            add_efx(T_TRAMPOLINE,x,y+j,1);
                        } else {
                            game.effect[i].delay=1;
                        }
                    }
                }
                break;
            }
        }
    }
	
	// Also needs to check npcs.
	for(i=0;i<game.npcCount;i++) {
		update_npc(&game.npc[i]);
	}
	// Now compact the npc list
	j=0;
	for(i=0;i<game.npcCount;i++) {
		if(j!=i) {
			game.npc[j]=game.npc[i];
		}
		if(game.npc[i].type!=-1) {
			j++;	
		}
	}
	game.npcCount=j;

	// Also needs to check board effects.
	for(i=0;i<game.efxCount;i++) {
		update_efx(&game.effect[i]);
	}
	// Now compact the board effect list
	j=0;
	for(i=0;i<game.efxCount;i++) {
		if(j!=i) {
			game.effect[j]=game.effect[i];
		}
		if(game.effect[i].type!=-1) {
			j++;	
		}
	}
	game.efxCount=j;
}
