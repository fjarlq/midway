
#include <stdio.h>
#include <curses.h>
#include <signal.h>

#define MIDWAY 11
#define JAPANESE 12
#define MAXSHIPS 52
#define HALFROW 11
#define HALFCOL 23
#define MAXCOLS 48
#define MAXROWS 24
#define AMFLEET 3
#define JAPFLEET 7


#define Japanese(a) (a >= JAPANESE)
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define abs(a) ((a) < 0 ? -(a) : (a))
#define rnd(x) ((x) ? (rand() >> 6) % (x) : 0)
#define intercept(here, there) angle(shiplist[here].row - shiplist[there].row, shiplist[there].col - shiplist[here].col)
#define setcourse(planes, ship) angle(planes -> row - shiplist[ship].row, shiplist[ship].col - planes -> col)
#define aimflack(ship, planes) angle(shiplist[ship].row - planes -> row, planes -> col - shiplist[ship].col)

#define SUPER -10
#define SCOUT -1
#define TBF 0
#define SBD 1
#define F4F 2
#define CV  3
#define BB  4
#define HC  5
#define LC  6
#define FT  7

#define CAP 	 8
#define RECOVER  9
#define ZEROHOUR 5
#define ZEROMIN 34
#define ZERODAY  3

extern struct torpedo {
	int row, col;
	int course;
	int fromship;
	int timeleft;
	struct torpedo *next_torp;
};
extern struct torpedo *amfish, *japfish;

extern struct ship {
	int row, col;
	int course;
	int type;
	int f4f, sbd, tbf;
	int hits, torps;
	int guns, ack_ack;
	int flagship;
	int launching;
	int points, value;
	char *name;
	int turrets, calibre, aa, belt, deck;
};

extern struct squadron {
	int row, col;
	int course;
	int type;
	int from;
	struct squadron *s_next, *previous;
	int planes;
	int fuel;
	int attack;
};

extern char *describe[8];
extern char *overviews[8];
extern int Fuel[3];
extern int capplanes[MAXSHIPS];
extern int scaler[10];
extern int amtable[AMFLEET + 1];
extern int japtable[JAPFLEET + 1];
extern int clock, scale, player, virtual;
extern int scroll, automatic, viewrow, viewcol;
extern int sighted[MAXSHIPS];
extern int firedflack[MAXSHIPS];
extern int firedguns[MAXSHIPS];
extern struct squadron *american, *japanese, *amscouts, *japscouts;
extern WINDOW *view, *bridge, *date, *panel1, *panel2, *stats, *notes, *hole;
extern char *jmess[9];
extern char *amess[5];
extern char *Reef[6];
extern char *Sand_Island[18];
extern char *Eastern_Island[8];

char shiphit();
char *daytime();
struct squadron *catapult();

extern char shapes[5][8][5][6];
extern struct ship shiplist[MAXSHIPS];
