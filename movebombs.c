#include "externs.h"

movebombs()
{
	cleanfish(amfish, 1);	/* get rid of old fish and move live ones */
	cleanfish(japfish, 0);
	movefish(amfish, 1);
	movefish(japfish, 0);
}

movefish(torp, yank)
register struct torpedo *torp;
int yank;
{
	register int n;
	int start, stop;
	char buf[32];

	if (yank) {
		start = JAPANESE;
		stop = MAXSHIPS;
	} else {
		start = 0;
		stop = JAPANESE;
	}
	for (; torp; torp = torp -> next_torp) {
		for (n = start; n < stop; n++) {
			if (shiplist[n].hits && shiplist[n].torps && shiphit(shiplist[n].type, shiplist[n].course/45, torp -> row - shiplist[n].row, torp -> col - shiplist[n].col) != ' ') {
				sprintf(buf, "%s torpedoed.", shiplist[n].name);
				inform(buf, 0);
				plotsplash(torp -> row, torp -> col, '#');
				torp -> timeleft = 1;
				if (!--shiplist[n].torps) {
					sprintf(buf, "%s sinking!", shiplist[n].name);
					inform(buf, 0);
					shiplist[torp -> fromship].points += shiplist[n].value;
					if (shiplist[n].flagship == n)
						transferflag(n);
					else if (n == player)
						die();
					break;
				}
			}
		}
	}
}

cleanfish(fish, yank)
register struct torpedo *fish;
int yank;
{
	struct torpedo *thud;

	thud = fish;
	for (; fish; fish = fish -> next_torp) {
		if (!--fish -> timeleft) {
			if (yank && thud == amfish) {
				thud = amfish = fish -> next_torp;
			} else if (!yank && thud == japfish) {
				thud = japfish = fish -> next_torp;
			} else {
				thud -> next_torp = fish -> next_torp;
			}
			cfree(fish);
		} else {
			thud = fish;
			drdc(fish -> course, &fish -> row, &fish -> col);
			drdc(fish -> course, &fish -> row, &fish -> col);
			plotsplash(fish -> row, fish -> col, '.');
		}
	}
}

plotsplash(row, col, symbol)
register int row, col;
register char symbol;
{
	if (scale == 0) {
		row -= viewrow;
		col -= viewcol;
		if (row >= 0 && row < MAXROWS && col >= 0 && col < MAXCOLS)
			mvwaddch(view, row, col, symbol);
	}
}

transferflag(from)
int from;
{
	register int n;
	int new;
	char buf[5];

	if (from == player) {
		inform("Your ship was sunk.",0);
		inform("Attempt to transfer flag? ", 0);
		wreadstr(notes, buf);
		if (*buf == 'n')
			die();
	}
	new = from + 1;
	while (new < MAXSHIPS && (!shiplist[new].torps || !shiplist[new].hits) && shiplist[new].flagship == from)
		new++;
	if (new == MAXSHIPS || shiplist[new].flagship != from) {
		if (from == player) die();
	} else {
		for (n = from; shiplist[n].flagship == from; n++)
			shiplist[n].flagship = new;
		if (Japanese(from)) {
			for (n=0; japtable[n] != from; n++);
			japtable[n] = new;
		} else {
			for (n=0; amtable[n] != from; n++);
			amtable[n] = new;
		}
		if (from == player) {
			player = virtual = new;
			redraw();
		}
	}
}

redraw()
{
	werase(notes);
	werase(hole);
	werase(panel1);
	werase(panel2);
	werase(date);
	werase(stats);
	drawboard();
}

flack(dir, from)
int dir, from;
{
	register struct squadron *planes, *target;
	register int n, r, c, k, l;
	int row = 0, col = 0, head; 
	int gothim;
	char buf[32];

	if (!firedflack[from] && shiplist[from].hits && shiplist[from].torps) {
		firedflack[from] = 1;
		if (Japanese(from)) {
			planes = american;
			head = 0;
		} else {
			planes = japanese;
			head = JAPANESE;
		}
		drdc(dir, &row, &col);
		row *= 7;
		col *= 7;			/* ack_ack range */
		row += shiplist[from].row;
		col += shiplist[from].col;
		for (n=0; n < shiplist[from].ack_ack; n++) {
			gothim = 0;
			r = row + rnd(7) - 3;
			c = col + rnd(7) - 3;
			for (target = planes; target; target = target -> s_next) {
				if (range(target -> row, target -> col, r, c) < 10) {
					for (k = -(l=target -> planes)/2; k < l - l/2; k++) {
						if (hit(1, r, c, target -> row, target -> col, setcourse(target, target -> course), k)) {
							target -> planes -= rnd(3);
							gothim = 1;
							break;
						}
					}
					if (target -> planes <= 0)
						if (head)
							ditch(target, &japanese);
						else
							ditch(target, &american);
				}
			}
			if (gothim)
				plotsplash(r, c, '@');
			else
				plotsplash(r, c, '*');
		}
	
	}
}

fireguns(from, to)
int from, to;
{
	register int k, ran, r, c;
	int row, col;
	char buf[32];

	if (!firedguns[from] && shiplist[from].hits && shiplist[from].torps && shiplist[to].hits) {
		firedguns[from] = 1;
		r = shiplist[to].row;
		c = shiplist[to].col;
		ran = range(shiplist[from].row, shiplist[from].col, r, c);
		for (k=0; k < shiplist[from].guns; k++) {
			row = r + rnd(ran/2) - ran/4;
			col = c + rnd(ran/2) - ran/4;
			if (shiphit(shiplist[to].type, shiplist[to].course/45, row - r, col - c) != ' ') { 
				sprintf(buf, "%s shelled by %s.", shiplist[to].name, shiplist[from].name);
				inform(buf, 0);
				plotsplash(row, col, '#');
				if (--shiplist[to].hits <= 0) {
					shiplist[to].hits = 0;
					sprintf(buf, "%s sinking!", shiplist[to].name);
					inform(buf, 0);
					shiplist[from].points += shiplist[to].value;
					if (shiplist[to].flagship == to)
						transferflag(to);
					else if (to == player)
						die();
					break;
				}
			} else
				plotsplash(row, col, '^');
		}
	}
}

launch(type)
int type;
{
	int num, course;
	char buf[10];
	struct squadron *planes;

	switch (type) {

		case CAP:
			if (shiplist[player].f4f || capplanes[player]) {
				inform("Combat Air Patrol: ", 0);
				wreadstr(notes, buf);
				sscanf(buf, "%d", &num);
				if (num < 0) num = -num;
				if (num <= capplanes[player] + shiplist[player].f4f) {
					shiplist[player].f4f -= num - capplanes[player];
					capplanes[player] = num;
				} else {
					capplanes[player] += shiplist[player].f4f;
					shiplist[player].f4f = 0;
				}
			}
			break;

		case SCOUT:
			if (shiplist[player].tbf) {
				inform("Launching Scout Plane",0);
				inform("   Course? ", 0);
				wreadstr(notes, buf);
				sscanf(buf, "%d", &course);
				planes = catapult(player, 1, SCOUT);
				planes -> course = course/45;
			}
			break;
			
		case RECOVER:
			inform("Break Radio Silence? ", 0);
			wreadstr(notes, buf);
			if (*buf == 'y') {
				inform("Signalling all planes.", 0);
				for (planes = (Japanese(player) ? japanese : american); planes; planes = planes -> s_next) {
					if (planes -> from == player) {
						planes -> attack = -1;
						planes -> course = player;
						planes -> fuel = range(planes -> row, planes -> col, shiplist[player].row, shiplist[player].col) / 10;
					}
				}
				for (planes = (Japanese(player) ? japscouts : amscouts); planes; planes = planes -> s_next) {
					if (planes -> from == player) {
						planes -> attack = -1;
						planes -> course = setcourse(planes, player);
						planes -> fuel = range(planes -> row, planes -> col, shiplist[player].row, shiplist[player].col) / 10;
					}
				}
				sighted[player] = 1;
			}
			break;

		case SUPER:
			if (shiplist[player].tbf) {
				inform("Launching Random Scouts", 0);
				inform("   How many planes? ", 0);
				wreadstr(notes, buf);
				sscanf(buf, "%d", &num);
				if (num < 0) num = -num;
				if (num) {
					if (num > shiplist[player].tbf)
						num = shiplist[player].tbf;
					for (course = 0; course < num; course++) {
						planes = catapult(player, 1, SCOUT);
						planes -> course = 8;
					}
				}
			}
			break;

		case TBF:
			if (shiplist[player].tbf) {
				inform("Launching Torpedo Bombers", 0);
				inform("   How many planes? ", 0);
				wreadstr(notes, buf);
				sscanf(buf, "%d", &num);
				if (num < 0) num = -num;
				if (num) {
					if (num > shiplist[player].tbf)
						num = shiplist[player].tbf;
					inform("   Target? ", 0);
					wreadstr(notes, buf);
					if ((course = scanwho(buf)) != -1) {
						planes = catapult(player, num, TBF);
						planes -> course = course;
						if (Japanese(player)){
							putchar('\7');
							inform("Banzai!", 0);
						}
					}
				}
			} 
			break;

		case SBD:
			if (shiplist[player].sbd) {
				inform("Launching Dive Bombers", 0);
				inform("   How many planes? ", 0);
				wreadstr(notes, buf);
				sscanf(buf, "%d", &num);
				if (num < 0) num = -num;
				if (num) {
					if (num > shiplist[player].sbd)
						num = shiplist[player].sbd;
					inform("   Target? ", 0);
					wreadstr(notes, buf);
					if ((course = scanwho(buf)) != -1) {
						planes = catapult(player, num, SBD);
						planes -> course = course;
						if (Japanese(player)){
							putchar('\7');
							inform("Banzai!", 0);
						}
					}
				}
			} 
			break;
		case F4F:
			if (shiplist[player].f4f) {
				inform("Launching Fighters", 0);
				inform("   How many planes? ", 0);
				wreadstr(notes, buf);
				sscanf(buf, "%d", &num);
				if (num < 0) num = -num;
				if (num) {
					if (num > shiplist[player].f4f)
						num = shiplist[player].f4f;
					inform("   Target? ", 0);
					wreadstr(notes, buf);
					if ((course = scanwho(buf)) != -1) {
						planes = catapult(player, num, F4F);
						planes -> course = course;
						if (Japanese(player)){
							putchar('\7');
							inform("Banzai!", 0);
						}
					}
				}
			} 
			break;
	} /* end switch */
} /* end launch */
