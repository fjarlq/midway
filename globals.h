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

struct torpedo {
	int row, col;
	int course;
	int fromship;
	int timeleft;
	struct torpedo *next_torp;
} *amfish, *japfish;

struct ship {
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

struct squadron {
	int row, col;
	int course;
	int type;
	int from;
	struct squadron *s_next, *previous;
	int planes;
	int fuel;
	int attack;
};

char *describe[8] = {"Torpedo Bombers", "Dive Bombers", "Fighters", "Carrier", "Battleship", "Heavy Cruiser", "Light Cruiser", "Island"};
char *overviews[8] = {"TBF", "SBD", "F4F", "CV", "BB", "HC", "LC", "xx"};
int Fuel[3] = {175, 175, 140};
int capplanes[MAXSHIPS];
int scaler[10] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
int amtable[AMFLEET + 1] = {0, 8, 11, MAXSHIPS};
int japtable[JAPFLEET + 1] = {12, 19, 28, 37, 43, 47, 48, MAXSHIPS};
int clock, scale, player, virtual, wizard;
int scroll = -1, automatic, viewrow, viewcol;
int sighted[MAXSHIPS];
int firedflack[MAXSHIPS];
int firedguns[MAXSHIPS];
struct squadron *american, *japanese, *amscouts, *japscouts;
WINDOW *view, *bridge, *date, *panel1, *panel2, *stats, *notes, *hole;
char *jmess[9] = {
	"\nThe Japanese fleet is divided into seven task forces:\n",
	"0) Main Force \t\t\t\t(Admiral Yamamoto)",
	"1) First Carrier Striking Force \t(Admiral Nagumo)",
	"2) Midway Invasion Force \t\t(Admiral Kondo)",
	"3) Alutian Screen \t\t\t(Admiral Taskasu)",
	"4) Second Carrier Striking Force\t(Admiral Kakuta)",
	"5) Midway Occupation Transports \t(Admiral Tanaka)",
	"6) Occupation Support Group \t\t(Admiral Kurita)",
	0
};
char *amess[5] = {
	"\nThe American fleet is divided into two task forces and Midway:\n",
	"0) Task Force 16\t\t\t(Admirals Halsey and Spruance)",
	"1) Task Force 17\t\t\t(Admiral Fletcher)",
	"2) Midway Island",
	0
};
char *Reef[6] = {
	"         xxxxxx          ",
	"     xxxxxxxxxxxxx       ",
	"  xxxxx          xxx     ",
	"xxx                xxx   ",
	"                    xxxxx",
	"    xx                xxx"
};
char *Sand_Island[18] = {
	"x             xx              ",
	"x                             ",
	"x                             ",
	"x                             ",
	"x                             ",
	"x                  xxxxxx xxx ",
	"x              xxxxxxxxxxxx   ",
	"x  xx         xxxxxxxxxxxx    ",
	"x              xMidwayxxx     ",
	"x             xxxxxxxxxx      ",
	"x             xxIslandxxxx    ",
	"x             xxxxxxxxxxxx    ",
	" x           xxxxxxxxxx       ",
	" x          xxxxxxxxx   xxxxx ",
	"  x       xxxxxxxxxx  xx      ",
	"  xx       xxxxxxx  xx        ",
	"   xx             xx          ",
	"     xxxxxxxxxxxxx            "
};
char *Eastern_Island[8] = {
	"               xx      ",
	"  x           xxx     x",
	"x xx         xxxx     x",
	"            xxxxxx    x",
	"  x      xxxxxxxxxx  x ",
	"  x      xxxxxxxxxx  x ",
	"   x                x  ",
	"    xxxxxxxxxxxxxxxx   "
};

char shiphit();
char *daytime();
struct squadron *catapult();

char shapes[5][8][5][6] = {
	"  ^  ",
	" /+\\ ",
	" |+I ",
	" |+I ",
	" \\_/ ",

	"   -/",
	" /++|",
	"/++//",
	"|+// ",
	" -   ",
	
	"     ",
	"/--\\ ",
	"|+++>",
	"\\--/ ",
	"     ",

	" -   ",
	"|++\\ ",
	"\\\\++\\",
	" \\\\+|",
	"   -\\",

	" /-\\ ",
	" I+| ",
	" I+| ",
	" \\+/ ",
	"  v  ",

	"   - ",
	" //+|",
	"//++/",
	"|++/ ",
	"/-   ",
	
	"     ",
	" /--\\",
	"<+++|",
	" \\--/",
	"     ",

	"\\-   ",
	"|+\\\\ ",
	"\\++\\\\",
	" \\++|",
	"   - ",

	"  ^  ",
	"  |  ",
	" ||| ",
	"|||||",
	"\\|||/",

	"    /",
	"  // ",
	"//// ",
	"///  ",
	"\\//  ",

	"     ",
	"/--  ",
	"---->",
	"\\--  ",
	"     ",

	"/\\\\  ",
	"\\\\\\  ",
	"\\\\\\\\ ",
	"  \\\\ ",
	"    \\",

	"/|||\\",
	"|||||",
	" ||| ",
	"  |  ",
	"  v  ",

	"  //\\",
	"  ///",
	" ////",
	" //  ",
	"/    ",

	"     ",
	"  --\\",
	"<----",
	"  --/",
	"     ",

	"\\    ",
	" \\\\  ",
	" \\\\\\\\",
	"  \\\\\\",
	"  \\\\/",

	"  ^  ",
	"  |  ",
	"  |  ",
	" ||| ",
	"  |  ",

	"     ",
	"   / ",
	"  /  ",
	"//   ",
	"//   ",

	"     ",
	" -   ",
	"---->",
	" -   ",
	"     ",

	"\\\\   ",
	"\\\\   ",
	"  \\  ",
	"   \\ ",
	"     ",

	"  |  ",
	" ||| ",
	"  |  ",
	"  |  ",
	"  v  ",

	"   //",
	"   //",
	"  /  ",
	" /   ",
	"     ",

	"     ",
	"   - ",
	"<----",
	"   - ",
	"     ",

	"     ",
	" \\   ",
	"  \\  ",
	"   \\\\",
	"   \\\\",

	"     ",
	"  ^  ",
	"  |  ",
	"  |  ",
	"     ",

	"     ",
	"   ' ",
	"  /  ",
	" /   ",
	"     ",

	"     ",
	"     ",
	" --> ",
	"     ",
	"     ",

	"     ",
	" \\   ",
	"  \\  ",
	"   ' ",
	"     ",

	"     ",
	"  |  ",
	"  |  ",
	"  v  ",
	"     ",

	"     ",
	"   / ",
	"  /  ",
	" '   ",
	"     ",

	"     ",
	"     ",
	" <-- ",
	"     ",
	"     ",

	"     ",
	" '   ",
	"  \\  ",
	"   \\ ",
	"     ",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx",

};

struct ship shiplist[MAXSHIPS] = {
	760, 1240, 225, CV, 26, 37, 15, 5, 4, 0, 4, 0, 0, 0, 20,
		"Enterprise", 0, 0, 16, 2, 1,
	760, 1250, 225, CV, 26, 37, 15, 5, 4, 0, 4, 0, 0, 0, 20,
		"Hornet", 0, 0, 16, 2, 1,
	760, 1230, 225, HC, 0, 0, 0, 5, 4, 3, 2, 0, 0, 0, 10,
		"New Orleans", 9, 8, 8, 5, 5,
	750, 1240, 225, HC, 0, 0, 0, 5, 4, 3, 2, 0, 0, 0, 10,
		"Minneapolis", 9, 8, 8, 5, 5,
	770, 1240, 225, HC, 0, 0, 0, 5, 4, 3, 2, 0, 0, 0, 10,
		"Vincennes", 9, 8, 8, 5, 5,
	750, 1250, 225, HC, 0, 0, 0, 3, 2, 3, 1, 0, 0, 0, 9,
		"Northampton", 9, 8, 4, 3, 2,
	770, 1250, 225, HC, 0, 0, 0, 3, 2, 3, 1, 0, 0, 0, 9,
		"Pensacola", 10, 8, 4, 3, 2,
	760, 1260, 225, LC, 0, 0, 0, 3, 2, 2, 1, 0, 0, 0, 6,
		"Atlanta", 9, 6, 4, 3, 2,
	
	790, 1250, 225, CV, 25, 37, 13, 5, 4, 0, 4, 8, 0, 0, 20,
		"Yorktown", 0, 0, 16, 2, 1,
	790, 1240, 225, HC, 0, 0, 0, 5, 4, 3, 2, 8, 0, 0, 10,
		"Astoria", 9, 8, 8, 5, 5,
	790, 1260, 225, HC, 0, 0, 0, 3, 2, 3, 2, 8, 0, 0, 10,
		"Portland", 9, 8, 8, 3, 2,
	
	1013, 1015, 315, FT, 30, 30, 30, 50, 50, 5, 10, 11, 0, 0,
		250, "Midway", 10, 14, 40, 24, 24,

	1000, 200, 90, BB, 0, 0, 1, 26, 18, 10, 3, 12, 0, 0, 73,
		"Yamato", 9, 18, 12, 14, 6,
	990, 190, 90, BB, 0, 0, 0, 12, 5, 8, 2, 12, 0, 0, 33,
		"Nagato", 8, 16, 8, 13, 3,
	1010, 190, 90, BB, 0, 0, 0, 12, 5, 8, 2, 12, 0, 0, 33,
		"Mutu", 8, 16, 8, 13, 3,
	990, 200, 90, BB, 0, 0, 0, 11, 5, 9, 2, 12, 0, 0, 30,
		"Ise", 12, 14, 8, 12, 3,
	1010, 200, 90, BB, 0, 0, 3, 11, 5, 9, 2, 12, 0, 0, 30,
		"Hyuga", 12, 14, 8, 12, 3,
	1000, 190, 90, CV, 10, 7, 9, 2, 2, 1, 1, 12, 0, 0, 7,
		"Hosho", 4, 5, 1, 0, 0,
	1000, 180, 90, LC, 0, 0, 0, 1, 1, 1, 1, 12, 0, 0, 5,
		"Naka", 7, 5, 1, 2, 0,
	
	700, 700, 135, CV, 26, 22, 25, 6, 4, 3, 3, 19, 0, 0, 27,
		"Akagi", 10, 8, 12, 2, 1,
	690, 710, 135, CV, 26, 22, 25, 6, 4, 3, 3, 19, 0, 0, 27,
		"Kaga", 10, 8, 12, 2, 1,
	690, 690, 135, CV, 21, 21, 21, 3, 2, 0, 3, 19, 0, 0, 10,
		"Soryu", 0, 0, 12, 0, 0,
	680, 700, 135, CV, 21, 21, 21, 3, 2, 0, 3, 19, 0, 0, 10,
		"Hiryu", 0, 0, 12, 0, 0,
	690, 700, 135, BB, 0, 0, 0, 7, 5, 7, 2, 19, 0, 0, 29,
		"Haruna", 8, 14, 8, 8, 3,
	700, 710, 135, BB, 0, 0, 0, 7, 5, 7, 2, 19, 0, 0, 29,
		"Kirisima", 8, 14, 8, 8, 3,
	700, 690, 135, HC, 0, 0, 0, 2, 2, 3, 2, 19, 0, 0, 9,
		"Chickuma", 12, 8, 8, 2, 2,
	680, 710, 135, HC, 0, 0, 0, 2, 2, 3, 2, 19, 0, 0, 9,
		"Tone", 12, 8, 8, 2, 2,
	680, 690, 135, LC, 0, 0, 0, 1, 1, 1, 1, 19, 0, 0, 5,
		"Nagara", 7, 5, 1, 2, 0,

	1030, 300, 90, HC, 0, 0, 1, 4, 3, 3, 2, 28, 0, 0, 10,
		"Atago", 10, 8, 8, 4, 3,
	1020, 300, 90, HC, 0, 0, 0, 4, 3, 3, 2, 28, 0, 0, 10,
		"Chokai", 10, 8, 8, 4, 3,
	1040, 300, 90, HC, 0, 0, 0, 4, 3, 3, 2, 28, 0, 0, 10,
		"Takao", 10, 8, 8, 4, 3,
	1050, 300, 90, HC, 0, 0, 0, 4, 3, 3, 2, 28, 0, 0, 10,
		"Maya", 10, 8, 8, 4, 3,
	1036, 305, 90, CV, 9, 7, 8, 2, 2, 0, 3, 28, 0, 0, 7,
		"Zuiho", 0, 0, 12, 2, 1,
	1030, 280, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Isuzu", 7, 5, 1, 2, 0,
	1040, 280, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Natori", 7, 5, 1, 2, 0,
	1030, 290, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Kinu", 7, 5, 1, 2, 0,
	1040, 290, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Yura", 7, 5, 1, 2, 0,

	250, 400, 135, BB, 0, 0, 1, 11, 4, 9, 2, 37, 0, 0, 29,
		"Huso", 12, 14, 8, 12, 2,
	240, 390, 135, BB, 0, 0, 1, 11, 4, 9, 2, 37, 0, 0, 29,
		"Yamasiro", 12, 14, 8, 12, 2,
	250, 390, 135, BB, 0, 0, 0, 7, 5, 7, 2, 37, 0, 0, 29,
		"Kongo", 8, 14, 8, 8, 3,
	240, 400, 135, BB, 0, 0, 0, 7, 5, 7, 2, 37, 0, 0, 29,
		"Hiei", 8, 14, 8, 8, 3,
	230, 400, 135, LC, 0, 0, 0, 1, 1, 1, 1, 37, 0, 0, 5,
		"Kitakami", 7, 5, 1, 2, 0,
	250, 380, 135, LC, 0, 0, 0, 1, 1, 1, 1, 37, 0, 0, 5,
		"Kiso", 7, 5, 1, 2, 0,

	200, 1200, 180, CV, 27, 13, 13, 3, 2, 0, 3, 43, 0, 0, 10,
		"Ryujo", 0, 0, 12, 0, 0,
	190, 1200, 180, CV, 13, 8, 8, 2, 2, 0, 2, 43, 0, 0, 7,
		"Junyo", 0, 0, 8, 0, 0,
	195, 1210, 180, HC, 0, 0, 0, 3, 2, 3, 2, 43, 0, 0, 10,
		"Asigara", 10, 8, 8, 3, 2,
	195, 1190, 180, HC, 0, 0, 0, 3, 2, 3, 2, 43, 0, 0, 10,
		"Haguro", 10, 8, 8, 3, 2,
	
	1100, 600, 45, LC, 0, 0, 1, 1, 1, 1, 1, 47, 0, 0, 5,
		"Jintsu", 7, 5, 1, 2, 0,

	1200, 700, 45, HC, 0, 0, 1, 2, 2, 4, 2, 48, 0, 0, 9,
		"Kumano", 15, 8, 8, 2, 2,
	1200, 690, 45, HC, 0, 0, 0, 2, 2, 4, 2, 48, 0, 0, 9,
		"Mikuma", 15, 8, 8, 2, 2,
	1210, 690, 45, HC, 0, 0, 0, 2, 2, 4, 2, 48, 0, 0, 9,
		"Mogami", 15, 8, 8, 2, 2,
	1210, 700, 45, HC, 0, 0, 0, 2, 2, 4, 2, 48, 0, 0, 9,
		"Suzuya", 15, 8, 8, 2, 2
};
