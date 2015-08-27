#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "midway.h"

struct torpedo *amfish, *japfish;

char *describe[8] = {"Torpedo Bombers", "Dive Bombers", "Fighters", "Carrier", "Battleship", "Heavy Cruiser", "Light Cruiser", "Island"};
char *overviews[8] = {"TBF", "SBD", "F4F", "CV", "BB", "HC", "LC", "xx"};
int Fuel[3] = {175, 175, 140};
int capplanes[MAXSHIPS];
int scaler[10] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
int amtable[AMFLEET + 1] = {0, 8, 11, MAXSHIPS};
int japtable[JAPFLEET + 1] = {12, 19, 28, 37, 43, 47, 48, MAXSHIPS};
int clock, scale, player, virtual, wizard;
int scrollrow = -1, automatic, viewrow, viewcol;
int sighted[MAXSHIPS];
int firedflack[MAXSHIPS];
int firedguns[MAXSHIPS];
struct squadron *american, *japanese, *amscouts, *japscouts;
WINDOW *view, *bridge, *date, *panel1, *panel2, *stats, *notes, *hole;
char *jmess[9] = {
	"\nThe Japanese fleet is divided into seven task forces:\n",
	"0) Main Force                      (Admiral Yamamoto)",
	"1) First Carrier Striking Force    (Admiral Nagumo)",
	"2) Midway Invasion Force           (Admiral Kondo)",
	"3) Alutian Screen                  (Admiral Taskasu)",
	"4) Second Carrier Striking Force   (Admiral Kakuta)",
	"5) Midway Occupation Transports    (Admiral Tanaka)",
	"6) Occupation Support Group        (Admiral Kurita)",
	NULL
};
char *amess[5] = {
	"\nThe American fleet is divided into two task forces and Midway:\n",
	"0) Task Force 16                   (Admirals Halsey and Spruance)",
	"1) Task Force 17                   (Admiral Fletcher)",
	"2) Midway Island",
	NULL
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

char shapes[5][8][5][6] = {
   {
      {	"  ^  ",
	" /+\\ ",
	" |+I ",
	" |+I ",
	" \\_/ ", },

      {	"   -/",
	" /++|",
	"/++//",
	"|+// ",
	" -   ", },
	
      {	"     ",
	"/--\\ ",
	"|+++>",
	"\\--/ ",
	"     ", },

      {	" -   ",
	"|++\\ ",
	"\\\\++\\",
	" \\\\+|",
	"   -\\", },

      {	" /-\\ ",
	" I+| ",
	" I+| ",
	" \\+/ ",
	"  v  ", },

      {	"   - ",
	" //+|",
	"//++/",
	"|++/ ",
	"/-   ", },
	
      {	"     ",
	" /--\\",
	"<+++|",
	" \\--/",
	"     ", },

      {	"\\-   ",
	"|+\\\\ ",
	"\\++\\\\",
	" \\++|",
	"   - ", },
   },
   {
      {	"  ^  ",
	"  |  ",
	" ||| ",
	"|||||",
	"\\|||/", },

      {	"    /",
	"  // ",
	"//// ",
	"///  ",
	"\\//  ", },

      {	"     ",
	"/--  ",
	"---->",
	"\\--  ",
	"     ", },

      {	"/\\\\  ",
	"\\\\\\  ",
	"\\\\\\\\ ",
	"  \\\\ ",
	"    \\", },

      {	"/|||\\",
	"|||||",
	" ||| ",
	"  |  ",
	"  v  ", },

      {	"  //\\",
	"  ///",
	" ////",
	" //  ",
	"/    ", },

      {	"     ",
	"  --\\",
	"<----",
	"  --/",
	"     ", },

      {	"\\    ",
	" \\\\  ",
	" \\\\\\\\",
	"  \\\\\\",
	"  \\\\/", },
   },
   {
      {	"  ^  ",
	"  |  ",
	"  |  ",
	" ||| ",
	"  |  ", },

      {	"     ",
	"   / ",
	"  /  ",
	"//   ",
	"//   ", },

      {	"     ",
	" -   ",
	"---->",
	" -   ",
	"     ", },

      {	"\\\\   ",
	"\\\\   ",
	"  \\  ",
	"   \\ ",
	"     ", },

      {	"  |  ",
	" ||| ",
	"  |  ",
	"  |  ",
	"  v  ", },

      {	"   //",
	"   //",
	"  /  ",
	" /   ",
	"     ", },

      {	"     ",
	"   - ",
	"<----",
	"   - ",
	"     ", },

      {	"     ",
	" \\   ",
	"  \\  ",
	"   \\\\",
	"   \\\\", },
   },
   {
      {	"     ",
	"  ^  ",
	"  |  ",
	"  |  ",
	"     ", },

      {	"     ",
	"   ' ",
	"  /  ",
	" /   ",
	"     ", },

      {	"     ",
	"     ",
	" --> ",
	"     ",
	"     ", },

      {	"     ",
	" \\   ",
	"  \\  ",
	"   ' ",
	"     ", },

      {	"     ",
	"  |  ",
	"  |  ",
	"  v  ",
	"     ", },

      {	"     ",
	"   / ",
	"  /  ",
	" '   ",
	"     ", },

      {	"     ",
	"     ",
	" <-- ",
	"     ",
	"     ", },

      {	"     ",
	" '   ",
	"  \\  ",
	"   \\ ",
	"     ", },
   },
   {
      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },

      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },

      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },

      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },

      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },

      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },

      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },

      {	" xxxx",
	"x\\-/x",
	"x|o|x",
	"x/-\\x",
	"xxxxx", },
   },
};

struct ship shiplist[MAXSHIPS] = {
    {	760, 1240, 225, CV, 26, 37, 15, 5, 4, 0, 4, 0, 0, 0, 20,
		"Enterprise", 0, 0, 16, 2, 1, },
    {	760, 1250, 225, CV, 26, 37, 15, 5, 4, 0, 4, 0, 0, 0, 20,
		"Hornet", 0, 0, 16, 2, 1, },
    {	760, 1230, 225, HC, 0, 0, 0, 5, 4, 3, 2, 0, 0, 0, 10,
		"New Orleans", 9, 8, 8, 5, 5, },
    {	750, 1240, 225, HC, 0, 0, 0, 5, 4, 3, 2, 0, 0, 0, 10,
		"Minneapolis", 9, 8, 8, 5, 5, },
    {	770, 1240, 225, HC, 0, 0, 0, 5, 4, 3, 2, 0, 0, 0, 10,
		"Vincennes", 9, 8, 8, 5, 5, },
    {	750, 1250, 225, HC, 0, 0, 0, 3, 2, 3, 1, 0, 0, 0, 9,
		"Northampton", 9, 8, 4, 3, 2, },
    {	770, 1250, 225, HC, 0, 0, 0, 3, 2, 3, 1, 0, 0, 0, 9,
		"Pensacola", 10, 8, 4, 3, 2, },
    {	760, 1260, 225, LC, 0, 0, 0, 3, 2, 2, 1, 0, 0, 0, 6,
		"Atlanta", 9, 6, 4, 3, 2, },
	
    {	790, 1250, 225, CV, 25, 37, 13, 5, 4, 0, 4, 8, 0, 0, 20,
		"Yorktown", 0, 0, 16, 2, 1, },
    {	790, 1240, 225, HC, 0, 0, 0, 5, 4, 3, 2, 8, 0, 0, 10,
		"Astoria", 9, 8, 8, 5, 5, },
    {	790, 1260, 225, HC, 0, 0, 0, 3, 2, 3, 2, 8, 0, 0, 10,
		"Portland", 9, 8, 8, 3, 2, },
	
    {	1013, 1015, 315, FT, 30, 30, 30, 50, 50, 5, 10, 11, 0, 0,
		250, "Midway", 10, 14, 40, 24, 24, },

    {	1000, 200, 90, BB, 0, 0, 1, 26, 18, 10, 3, 12, 0, 0, 73,
		"Yamato", 9, 18, 12, 14, 6, },
    {	990, 190, 90, BB, 0, 0, 0, 12, 5, 8, 2, 12, 0, 0, 33,
		"Nagato", 8, 16, 8, 13, 3, },
    {	1010, 190, 90, BB, 0, 0, 0, 12, 5, 8, 2, 12, 0, 0, 33,
		"Mutu", 8, 16, 8, 13, 3, },
    {	990, 200, 90, BB, 0, 0, 0, 11, 5, 9, 2, 12, 0, 0, 30,
		"Ise", 12, 14, 8, 12, 3, },
    {	1010, 200, 90, BB, 0, 0, 3, 11, 5, 9, 2, 12, 0, 0, 30,
		"Hyuga", 12, 14, 8, 12, 3, },
    {	1000, 190, 90, CV, 10, 7, 9, 2, 2, 1, 1, 12, 0, 0, 7,
		"Hosho", 4, 5, 1, 0, 0, },
    {	1000, 180, 90, LC, 0, 0, 0, 1, 1, 1, 1, 12, 0, 0, 5,
		"Naka", 7, 5, 1, 2, 0, },
	
    {	700, 700, 135, CV, 26, 22, 25, 6, 4, 3, 3, 19, 0, 0, 27,
		"Akagi", 10, 8, 12, 2, 1, },
    {	690, 710, 135, CV, 26, 22, 25, 6, 4, 3, 3, 19, 0, 0, 27,
		"Kaga", 10, 8, 12, 2, 1, },
    {	690, 690, 135, CV, 21, 21, 21, 3, 2, 0, 3, 19, 0, 0, 10,
		"Soryu", 0, 0, 12, 0, 0, },
    {	680, 700, 135, CV, 21, 21, 21, 3, 2, 0, 3, 19, 0, 0, 10,
		"Hiryu", 0, 0, 12, 0, 0, },
    {	690, 700, 135, BB, 0, 0, 0, 7, 5, 7, 2, 19, 0, 0, 29,
		"Haruna", 8, 14, 8, 8, 3, },
    {	700, 710, 135, BB, 0, 0, 0, 7, 5, 7, 2, 19, 0, 0, 29,
		"Kirisima", 8, 14, 8, 8, 3, },
    {	700, 690, 135, HC, 0, 0, 0, 2, 2, 3, 2, 19, 0, 0, 9,
		"Chickuma", 12, 8, 8, 2, 2, },
    {	680, 710, 135, HC, 0, 0, 0, 2, 2, 3, 2, 19, 0, 0, 9,
		"Tone", 12, 8, 8, 2, 2, },
    {	680, 690, 135, LC, 0, 0, 0, 1, 1, 1, 1, 19, 0, 0, 5,
		"Nagara", 7, 5, 1, 2, 0, },

    {	1030, 300, 90, HC, 0, 0, 1, 4, 3, 3, 2, 28, 0, 0, 10,
		"Atago", 10, 8, 8, 4, 3, },
    {	1020, 300, 90, HC, 0, 0, 0, 4, 3, 3, 2, 28, 0, 0, 10,
		"Chokai", 10, 8, 8, 4, 3, },
    {	1040, 300, 90, HC, 0, 0, 0, 4, 3, 3, 2, 28, 0, 0, 10,
		"Takao", 10, 8, 8, 4, 3, },
    {	1050, 300, 90, HC, 0, 0, 0, 4, 3, 3, 2, 28, 0, 0, 10,
		"Maya", 10, 8, 8, 4, 3, },
    {	1036, 305, 90, CV, 9, 7, 8, 2, 2, 0, 3, 28, 0, 0, 7,
		"Zuiho", 0, 0, 12, 2, 1, },
    {	1030, 280, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Isuzu", 7, 5, 1, 2, 0, },
    {	1040, 280, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Natori", 7, 5, 1, 2, 0, },
    {	1030, 290, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Kinu", 7, 5, 1, 2, 0, },
    {	1040, 290, 90, LC, 0, 0, 0, 1, 1, 1, 1, 28, 0, 0, 5,
		"Yura", 7, 5, 1, 2, 0, },

    {	250, 400, 135, BB, 0, 0, 1, 11, 4, 9, 2, 37, 0, 0, 29,
		"Huso", 12, 14, 8, 12, 2, },
    {	240, 390, 135, BB, 0, 0, 1, 11, 4, 9, 2, 37, 0, 0, 29,
		"Yamasiro", 12, 14, 8, 12, 2, },
    {	250, 390, 135, BB, 0, 0, 0, 7, 5, 7, 2, 37, 0, 0, 29,
		"Kongo", 8, 14, 8, 8, 3, },
    {	240, 400, 135, BB, 0, 0, 0, 7, 5, 7, 2, 37, 0, 0, 29,
		"Hiei", 8, 14, 8, 8, 3, },
    {	230, 400, 135, LC, 0, 0, 0, 1, 1, 1, 1, 37, 0, 0, 5,
		"Kitakami", 7, 5, 1, 2, 0, },
    {	250, 380, 135, LC, 0, 0, 0, 1, 1, 1, 1, 37, 0, 0, 5,
		"Kiso", 7, 5, 1, 2, 0, },

    {	200, 1200, 180, CV, 27, 13, 13, 3, 2, 0, 3, 43, 0, 0, 10,
		"Ryujo", 0, 0, 12, 0, 0, },
    {	190, 1200, 180, CV, 13, 8, 8, 2, 2, 0, 2, 43, 0, 0, 7,
		"Junyo", 0, 0, 8, 0, 0, },
    {	195, 1210, 180, HC, 0, 0, 0, 3, 2, 3, 2, 43, 0, 0, 10,
		"Asigara", 10, 8, 8, 3, 2, },
    {	195, 1190, 180, HC, 0, 0, 0, 3, 2, 3, 2, 43, 0, 0, 10,
		"Haguro", 10, 8, 8, 3, 2, },
	
    {	1100, 600, 45, LC, 0, 0, 1, 1, 1, 1, 1, 47, 0, 0, 5,
		"Jintsu", 7, 5, 1, 2, 0, },

    {	1200, 700, 45, HC, 0, 0, 1, 2, 2, 4, 2, 48, 0, 0, 9,
		"Kumano", 15, 8, 8, 2, 2, },
    {	1200, 690, 45, HC, 0, 0, 0, 2, 2, 4, 2, 48, 0, 0, 9,
		"Mikuma", 15, 8, 8, 2, 2, },
    {	1210, 690, 45, HC, 0, 0, 0, 2, 2, 4, 2, 48, 0, 0, 9,
		"Mogami", 15, 8, 8, 2, 2, },
    {	1210, 700, 45, HC, 0, 0, 0, 2, 2, 4, 2, 48, 0, 0, 9,
		"Suzuya", 15, 8, 8, 2, 2 },
};

int main(int argc, char *argv[])
{
	int pr, pc;
	register int n;

	(void)argv;
	initialize();
	
	if (argc > 1) {
		for (n=0; n < MAXSHIPS; n++)
			if (shiplist[n].type == CV || shiplist[n].type == FT)
				shiplist[n].f4f = shiplist[n].tbf = shiplist[n].sbd = 0;
	}

	/* lots of stuff */

	/* Ken arnold stuff */

	srand(getpid());
	initscr();
	view = newwin(24, 48, 0, 0);
	leaveok(view, TRUE);
	notes = newwin(12, 31, 0, 49);
	hole = newwin(1, 1, 13, 48);
	leaveok(hole, TRUE);
	date = newwin(1, 15, 13, 60);
	leaveok(date, TRUE);
	panel1 = newwin(9, 1, 15, 48);
	leaveok(panel1, TRUE);
	panel2 = newwin(9, 1, 15, 54);
	bridge = newwin(9, 5, 15, 49);
	leaveok(bridge, TRUE);
	stats = newwin(4, 24, 20, 55);
	leaveok(stats, TRUE);
	signal(SIGINT, interrupt);
	crmode();
	noecho();

	pr = pc = 0;
	for (n=0; n < MAXSHIPS; n++) {		/* slightly random positions */
		if (shiplist[n].flagship == n) {
			if (!rnd(4)) {	/* 25% of the time */
				pr = shiplist[MIDWAY].row - shiplist[n].row + rnd(3000) - 1500;	/* centered on MIDWAY */
				pc = shiplist[MIDWAY].col - shiplist[n].col + rnd(3000) - 1500;	
			} else {
				pr = rnd(3000) - rnd(1500);	/* off centered on the ship's starting location */
				pc = rnd(3000) - rnd(1500);
			}
		}
		if (n != MIDWAY) {
			shiplist[n].row += pr;
			shiplist[n].col += pc;
		}
	}

	sighted[MIDWAY] = 1;

  	/* launch CAP */

	for (n=0; n < MAXSHIPS; n++)
		if (n != player && shiplist[n].f4f) {
			capplanes[n] = shiplist[n].f4f/2;
			shiplist[n].f4f -= capplanes[n];
		}
	
	drawboard();
	screen();
	if (Japanese(player)) {
		inform("Begin launching first wave", 0);
		inform("of Midway attack. -- Nagumo", 0);
	} else {
		inform("Japanese fleet expected to", 0);
		inform("be approx. 300 miles NW of", 0);
		inform("Midway. -- Nimitz", 0);
	}
	playit();
	return 0;
}

void playit(void)
{
	register int n;
	int pr, pc;
	int ran, tar;
	int c;
	char buf[128];

	for (;;) {
		switch (c = getchar()) {

			case 'h':
				flack(270, player);
				wrefresh(view);
				break;

			case 'l':
				flack(90, player);
				wrefresh(view);
				break;

			case 'j':
				flack(180, player);
				wrefresh(view);
				break;

			case 'k':
				flack(0, player);
				wrefresh(view);
				break;

			case 'i':
			case 'o':
				flack(45, player);
				wrefresh(view);
				break;

			case '.':
			case ',':
				flack(135, player);
				wrefresh(view);
				break;

			case 'n':
			case 'm':
				flack(225, player);
				wrefresh(view);
				break;

			case 'y':
			case 'u':
				flack(315, player);
				wrefresh(view);
				break;

			case 'H':
				shiplist[player].course = 270;
				break;

			case 'L':
				shiplist[player].course = 90;
				break;

			case 'J':
				shiplist[player].course = 180;
				break;

			case 'K':
				shiplist[player].course = 0;
				break;

			case 'I':
			case 'O':
				shiplist[player].course = 45;
				break;

			case '>':
			case '<':
				shiplist[player].course = 135;
				break;

			case 'N':
			case 'M':
				shiplist[player].course = 225;
				break;

			case 'Y':
			case 'U':
				shiplist[player].course = 315;
				break;

			case '\n':
				pc = Japanese(player) ? JAPANESE : MAXSHIPS;
				ran = 35;	/* give player an advantage */
				tar = -1;
				for (n = (Japanese(player) ? 0 : JAPANESE); n < pc; n++) {
					if (shiplist[n].hits && shiplist[n].torps && (pr = range(shiplist[player].row, shiplist[player].col, shiplist[n].row, shiplist[n].col)) < ran) {
						ran = pr;
						tar = n;
					}
				}
				if (tar != -1) {
					fireguns(player, tar);
					wrefresh(view);
				}
				break;

			case 'c':
				launch(CAP);
				break;

			case '@':
				launch(SUPER);
				break;

			case 'r':
				launch(RECOVER);
				break;

			case 's':
				launch(SCOUT);
				break;

			case 't':
				launch(TBF);
				break;

			case 'f':
				launch(F4F);
				break;

			case 'b':
				launch(SBD);
				break;

			case 'w':
				inform("waiting...", 0);
				automatic = -2;
				break;

			case 'A':
				inform("Rig for silent running? ", 0);
				wreadstr(notes, buf);
				if (*buf == 'y')
					automatic = -1;
				break;

			case 'a':
				automatic = 1;
				break;
			
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				scale = c - '0';
				virtual = player;
				break;

			case 'S':
				inform("Scan whom? ",0);
				wreadstr(notes, buf);
				if ((n = scanwho(buf)) != -1) {
					scale = 0;
					virtual = n;
				}
				break;

			case '\f':
				redraw();
				break;

			case 'q':
				inform("Really quit? ", 0);
				wreadstr(notes, buf);
				if (*buf == 'y')
					die();
				break;
		}
		do {
			clock++;
			airstrike();
			moveships();
			movebombs();
			if (automatic >= 0)
				screen();
		} while (automatic);
	}
}

void drawboard(void)
{
	register int n, k;

	clear();
	for (n=0; n < 15; n++)
		mvaddch(n, 48, '|');
	for (k=12; k < 15; k += 2)
		for (n=49; n < COLS; n++)
			mvaddch(k, n, '-');
	mvaddstr(13, 50, shiplist[player].name);
	mvaddstr(13, 76, "1942");
	mvprintw(15, 55, "%s %ld tons", describe[shiplist[player].type], (long) shiplist[player].value * 1000L);
	mvaddstr(17, 55, "Guns:");
	mvaddstr(18, 55, "Armor:");
	mvwaddstr(stats, 0, 0, "CAP");
	if (Japanese(player)) {
		mvwaddstr(stats, 1, 0, "00F");
		mvwaddstr(stats, 2, 0, "97T");
		mvwaddstr(stats, 3, 0, "97B");
	} else {
		mvwaddstr(stats, 1, 0, "F4F");
		mvwaddstr(stats, 2, 0, "TBF");
		mvwaddstr(stats, 3, 0, "SBD");
	}
	mvwaddstr(stats, 0, 8, "Hits");
	mvwaddstr(stats, 1, 8, "Torps");
	mvwaddstr(stats, 3, 8, "Score");
	mvwaddstr(stats, 0, 18, "Row");
	mvwaddstr(stats, 2, 18, "Col");
	move(17, 61);
	if (shiplist[player].turrets)
		printw("%d %d\", ", shiplist[player].turrets, shiplist[player].calibre);
	printw("%d 5\" AA", shiplist[player].aa);
	move(18, 62);
	if (!shiplist[player].belt && !shiplist[player].deck)
		addstr("none");
	else
		printw("%d\" belt, %d\" deck", shiplist[player].belt, shiplist[player].deck);
	refresh();
	wrefresh(stats);
}

void initialize(void)
{
	int c, s, items, numforces, numships;
	char r[20];
	register int n;
	char **mess;
	int *table;

	setbuf(stdout, NULL);

	printf("Choose a side (Japanese or American) ? ");
	if (fgets(r, sizeof r, stdin) == NULL)
		exit(0);
	if (*r == 'J' || *r == 'j') {
		mess = jmess;
		table = japtable;
		numforces = JAPFLEET;
	} else {
		mess = amess;
		table = amtable;
		numforces = AMFLEET;
	}

forceagain:
	for (n=0; mess[n]; n++)
		puts(mess[n]);
	printf("\nWhich force do you like? ");
	items = scanf("%d", &c);
	if (items == EOF)
		exit(0);
	if (items != 1 || c < 0 || c >= numforces)
		goto forceagain;
	printf("\n");

shipagain:
	for (n = table[c], numships = 0; shiplist[n].flagship == table[c]; n++) {
		printf("%d) %s %s (%d Kilotons)\n", n - table[c], describe[shiplist[n].type], shiplist[n].name, shiplist[n].value);
		numships++;
	}
	printf("\nWhich ship do you like? ");
	items = scanf("%d", &s);
	if (items == EOF)
		exit(0);
	if (items != 1 || s < 0 || s >= numships)
		goto shipagain;
	if ((virtual = player = s + table[c]) >= MAXSHIPS || player < 0)
		goto shipagain;
}

int scanwho(char buf[])
{
	register int n;

	for (n=0; n < MAXSHIPS; n++)
		if ((sighted[n] || (Japanese(player) == Japanese(n))) && !strcmp(buf, shiplist[n].name) && shiplist[n].hits && shiplist[n].torps)
			break;
	if (n < MAXSHIPS) {
		return(n);
	} else {
		inform("Ship not found.", 0);
		return(-1);
	}
}
