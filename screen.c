#include "externs.h"

plotships()
{
	register int n, k, r, c;
	int dr = 0, dc = 0;
	int *table1, *table2, start1, start2, end1, end2;

	viewrow = shiplist[virtual].row - HALFROW;
	viewcol = shiplist[virtual].col - HALFCOL;
	werase(view);
	werase(bridge);
	if (scale == 0) {
		if (viewrow < 1018 && viewrow > 957 && viewcol > 952 && viewcol < 1057) {	/* land ho! */
			for (n=0; n < 6; n++)
				mvwputs(view, 981 + n - viewrow, 1012 - viewcol, Reef[n]);
			for (n=0; n < 18; n++)
				mvwputs(view, 1000 + n - viewrow, 1000 - viewcol, Sand_Island[n]);
			for (n=0; n < 8; n++)
				mvwputs(view, 1007 + n - viewrow, 1034 - viewcol, Eastern_Island[n]);
		}
		for (n = 0; n < MAXSHIPS; n++) {
			if (shiplist[n].hits && shiplist[n].torps && (r = shiplist[n].row - viewrow) >= 0 && r < MAXROWS && (c = shiplist[n].col - viewcol) >= 0 && c < MAXCOLS) {
				wmove(view, r, c);
				if (n != MIDWAY && winch(view) == 'x') {
					dr = dc = 0;
					drdc(shiplist[n].course, &dr, &dc);
					shiplist[n].row -= dr;
					shiplist[n].col -= dc;
				}
				for (k=0; k < 5; k++)
					mvwputs(view, r+k-2, c-2, shapes[shiplist[n].type - CV][shiplist[n].course/45][k]);
			}
		}
	} else {
		if (Japanese(player)) {
			if (scale < 4) {
				start1 = JAPANESE;
				end1 = MAXSHIPS;
				start2 = 0;
				end2 = JAPANESE;
			} else {
				start1 = japtable[0];
				end1 = MAXSHIPS;
				start2 = amtable[0];
				end2 = MAXSHIPS;
			}
			table1 = japtable;
			table2 = amtable;
		} else {
			if (scale < 4) {
				start1 = 0;
				end1 = JAPANESE;
				start2 = JAPANESE;
				end2 = MAXSHIPS;
			} else {
				start1 = amtable[0];
				end1 = MAXSHIPS;
				start2 = japtable[0];
				end2 = MAXSHIPS;
			}
			table1 = amtable;
			table2 = japtable;
		}
		for (n = start1; n < end1; scale < 4 ? n++ : (n = *++table1)) {
			if (shiplist[n].hits && shiplist[n].torps) {
				r = (shiplist[n].row - viewrow - HALFROW) / scaler[scale] + HALFROW;
				c = (shiplist[n].col - viewcol - HALFCOL) / scaler[scale] + HALFCOL;
				if (r >= 0 && r < MAXROWS && c >= 0 && c < MAXCOLS) {
					mvwputs(view, r, c, overviews[shiplist[n].type]);
				}
			}
		}
		for (n = start2; n < end2; scale < 4 ? n++ : (n = *++table2)) {
			if (sighted[n] && shiplist[n].hits && shiplist[n].torps) {
				r = (shiplist[n].row - viewrow - HALFROW) / scaler[scale] + HALFROW;
				c = (shiplist[n].col - viewcol - HALFCOL) / scaler[scale] + HALFCOL;
				if (r >= 0 && r < MAXROWS && c >= 0 && c < MAXCOLS) {
					mvwputs(view, r, c, overviews[shiplist[n].type]);
					mvwputs(view, r, c + 3, shiplist[n].name);
				}
			}
		}
	}
	if (scale == 0 && virtual == player) {
		for (n = 7; n < 16; n++) {
			for (k = 21; k < 26; k++) {
				wmove(view, n, k);
				wmove(bridge, n - 7, k - 21);
				waddch(bridge, winch(view));
			}
		}
	} else {
		for (k=0; k < 5; k++)
			mvwputs(bridge, k+2, 0, shapes[shiplist[player].type - CV][shiplist[player].course/45][k]);
	}
}

plotplanes()
{
	register int n, k, r, c;
	int danger = 0;
	int dr = 0, dc = 0;
	struct squadron *planes;

	if (scale == 0) {
		for (planes = (Japanese(player) ? japscouts : amscouts); planes; planes = planes -> s_next)
			if ((r = planes -> row - viewrow) >= 0 && r < MAXROWS && (c = planes -> col - viewcol) >= 0 && c < MAXCOLS)
				mvwaddch(view, r, c, '?');
		for (planes = japanese; planes; planes = planes -> s_next) {
			if ((r = planes -> row - viewrow) >= 0 && r < MAXROWS && (c = planes -> col - viewcol) >= 0 && c < MAXCOLS) {
				for (n = - (k = planes -> planes)/2; n < k - k/2; n++) {
					dr = dc = 0;
					vshape(setcourse(planes, planes -> course), n, &dr, &dc);
					mvwaddch(view, r + dr, c + dc, '-');
				}
			}
		}
		for (planes = american; planes; planes = planes -> s_next) {
			if ((r = planes -> row - viewrow) >= 0 && r < MAXROWS && (c = planes -> col - viewcol) >= 0 && c < MAXCOLS) {
				for (n = - (k = planes -> planes)/2; n < k - k/2; n++) {
					dr = dc = 0;
					vshape(setcourse(planes, planes -> course), n, &dr, &dc);
					mvwaddch(view, r + dr, c + dc, '+');
				}
			}
		}
	} else {
		for (planes = (Japanese(player) ? japscouts : amscouts); planes; planes = planes -> s_next) {
			r = (planes -> row - viewrow - HALFROW) / scaler[scale] + HALFROW;
			c = (planes -> col - viewcol - HALFCOL) / scaler[scale] + HALFCOL;
			if (r >= 0 && r < MAXROWS && c >= 0 && c < MAXCOLS) {
				mvwaddch(view, r, c, '?');
			}
		}
		for (planes = (Japanese(player) ? japanese : american); planes; planes = planes -> s_next) {
			r = (planes -> row - viewrow - HALFROW) / scaler[scale] + HALFROW;
			c = (planes -> col - viewcol - HALFCOL) / scaler[scale] + HALFCOL;
			if (r >= 0 && r < MAXROWS && c >= 0 && c < MAXCOLS) {
				mvwaddch(view, r, c, (Japanese(player) ? '-' : '+'));
			}
		}
		for (planes = (Japanese(player) ? american : japanese); planes; planes = planes -> s_next) {
			r = (planes -> row - viewrow - HALFROW) / scaler[scale] + HALFROW;
			c = (planes -> col - viewcol - HALFCOL) / scaler[scale] + HALFCOL;
			if ((scale < 4 || sighted[planes -> from]) && r >= 0 && r < MAXROWS && c >= 0 && c < MAXCOLS) {
				mvwaddch(view, r, c, (Japanese(player) ? '+' : '-'));
				if (planes -> course == player) danger = 1;
			}
		}
	}
	if (danger)
		c = '?';
	else
		c = '|';
	for (n=0; n < 9; n++) {
		mvwaddch(panel1, n, 0, c);
		mvwaddch(panel2, n, 0, c);
	}
}

screen()
{
	char buf[32];

	mvwputs(date, 0, 0, daytime(clock, buf));
	mvwprintw(stats, 0, 4, "%2d", capplanes[virtual]);
	mvwprintw(stats, 1, 4, "%2d", shiplist[virtual].f4f);
	mvwprintw(stats, 2, 4, "%2d", shiplist[virtual].tbf);
	mvwprintw(stats, 3, 4, "%2d", shiplist[virtual].sbd);
	mvwprintw(stats, 0, 14, "%2d", shiplist[virtual].hits);
	mvwprintw(stats, 1, 14, "%2d", shiplist[virtual].torps);
	mvwprintw(stats, 3, 14, "%-3d", shiplist[virtual].points);
	mvwprintw(stats, 1, 18, "%-6d", shiplist[virtual].row);
	mvwprintw(stats, 3, 18, "%-6d", shiplist[virtual].col);
	mvwaddch(hole, 0, 0, scale + '0');
	wrefresh(view);
	wrefresh(bridge);
	wrefresh(stats);
	wrefresh(date);
	wrefresh(hole);
	wrefresh(panel1);
	wrefresh(panel2);
} /*end screen */

#include <pwd.h>
#define LOGFILE "/usr/public/.midwaylog"
int maxpoints[2] = {637, 384};
struct logs {
	int uid;
	int fshipnum;
	int netpoints;
};

die()
{
	FILE *fp;
	register int n, k;
	struct passwd *getpwuid();
	int score;
	long st;
	struct logs log[20], temp;

	if (!shiplist[player].hits || !shiplist[player].torps)
		inform("You went down with the ship!", 0);
	mvcur(0, COLS-1, LINES-1, 0);
	echo();
	nocrmode();

	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	if (fp = fopen(LOGFILE, "r+")){
		n = fread(log, sizeof(struct logs), 20, fp);
		for (; n < 20; n++)
			log[n].uid = log[n].fshipnum = log[n].netpoints = 0;
		rewind(fp);
		for (n=0; n < 20; n++)
			if (shiplist[player].points > log[n].netpoints){
				fwrite(log, sizeof(struct logs), n, fp);
				temp.uid = getuid();
				temp.fshipnum = player;
				temp.netpoints = shiplist[player].points;
				fwrite(&temp, sizeof(struct logs), 1, fp);
				fwrite(log + n, sizeof(struct logs), 19 - n, fp);
				break;
			}
		printf("\n\nNAME\t\tSHIP\t\t\tTONS SUNK\tPERCENT\n\n");
		if (n == 21) n = 20;
		for (k=0; k < n; k++) {
			if (log[k].netpoints) {
				st = (long) log[k].netpoints * 100L;
				score = (int) (st / (long) maxpoints[(Japanese(log[k].fshipnum))]);
				printf("%-10s%16s%20ld%10d%%\n", getpwuid(log[k].uid) -> pw_name, shiplist[log[k].fshipnum].name, log[k].netpoints * 1000L, score);
			}
		}
		if (shiplist[player].points) {
			st = (long) temp.netpoints * 100L;
			score = (int) (st / (long) maxpoints[(Japanese(temp.fshipnum))]);
			printf("%-10s%16s%20ld%10d%%\n", getpwuid(temp.uid) -> pw_name, shiplist[temp.fshipnum].name, temp.netpoints * 1000L, score);
		}
		for (; k < 20; k++) {
			if (log[k].netpoints) {
				st = (long) log[k].netpoints * 100L;
				score = (int) (st / (long) maxpoints[(Japanese(log[k].fshipnum))]);
				printf("%-10s%16s%20ld%10d%%\n", getpwuid(log[k].uid) -> pw_name, shiplist[log[k].fshipnum].name, log[k].netpoints * 1000L, score);
			}
		}

	}
	exit(0);
}

interrupt()
{
	automatic = 0;
	signal(SIGINT, interrupt);
}

mvwputs(win, row, col, string)
WINDOW *win;
register int row, col;
register char *string;
{
	register int n;

	for (n=0; string[n]; n++)
		if (row >= 0 && row < win -> _maxy && col+n >= 0 && col+n < win -> _maxx)
			mvwaddch(win, row, col + n, string[n]);
}
