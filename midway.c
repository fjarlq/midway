#include "globals.h"

#define VAX

main(argc)
int argc;
{
	int pr, pc;
	int interrupt();
	register int n;
	int vec[3];
	int uid;

	/* check the load */

#ifndef VAX
	uid = getuid();
	wizard = uid == 25898 || uid == 25978;
	gldav(vec);
	if ((vec[2] >> 8) > 7 && !wizard)
	{
		fprintf(stderr, "Load average is too high.\n");
		execl("/usr/public/wotd","/usr/public/wotd","-a",  0);
		exit(1);
	}
#endif
	
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
	initialize();
	signal(SIGINT, interrupt);
	crmode();
	noecho();

	for (n=0; n < MAXSHIPS; n++) {		/* slightly random positions */
		if (shiplist[n].flagship == n) {
			if (!rnd(4)) {	/* 25% of the time */
				pr = shiplist[MIDWAY].row - shiplist[n].row + rnd(3000) - 1500;	/* centered on MIDWAY */
				pr = shiplist[MIDWAY].col - shiplist[n].col + rnd(3000) - 1500;	
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
}

playit()
{

	register int n;
	int pr, pc;
	int ran, tar;
	int c;
	char buf[32];

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

drawboard()
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
	mvwputs(stats, 0, 0, "CAP");
	if (Japanese(player)) {
		mvwputs(stats, 1, 0, "00F");
		mvwputs(stats, 2, 0, "97T");
		mvwputs(stats, 3, 0, "97B");
	} else {
		mvwputs(stats, 1, 0, "F4F");
		mvwputs(stats, 2, 0, "TBF");
		mvwputs(stats, 3, 0, "SBD");
	}
	mvwputs(stats, 0, 8, "Hits");
	mvwputs(stats, 1, 8, "Torps");
	mvwputs(stats, 3, 8, "Score");
	mvwputs(stats, 0, 18, "Row");
	mvwputs(stats, 2, 18, "Col");
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

initialize()
{
	int c, s;
	char r[20];
	register int n;
	char **mess;
	int *table;

	printf("Choose a side (Japanese or American) ? ");
	gets(r);
	if (*r == 'J' || *r == 'j') {
		mess = jmess;
		table = japtable;
	} else {
		mess = amess;
		table = amtable;
	}
	for (n=0; mess[n]; n++)
		puts(mess[n]);
	printf("\nWhich force do you like? ");
	scanf("%d", &c);
	printf("\n");
	for (n = table[c]; shiplist[n].flagship == table[c]; n++)
		printf("%d) %s %s (%d Kilotons)\n", n - table[c], describe[shiplist[n].type], shiplist[n].name, shiplist[n].value);
	printf("\nWhich ship do you like? ");
	scanf("%d", &s);
	if ((virtual = player = s + table[c]) >= MAXSHIPS || player < 0) {
		fprintf(stderr, "Ship not found.\n");
		exit(1);
	}
}
scanwho(buf)
char buf[];
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
