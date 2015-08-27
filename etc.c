#include "externs.h"

angle(dr, dc)
register int dr, dc;
{
	int add = 0, sub = 0;

	if (dr <= 0 && dc < 0)
		add = 180;
	if (dr > 0 && dc < 0)
		sub = 360;
	if (dr < 0 && dc >= 0)
		sub = 180;
	if (abs(dc) > abs(dr) * 2.4)
		return(sub ? sub - 90 : 90 + add);
	else if (abs(dr) > abs(dc) * 2.4)
		return(sub == 360 ? 0 : sub + add);
	else
		return(sub ? sub - 45 : 45 + add);
}

planesize(planes)
register struct squadron *planes;
{
	register int n = 0;

	for (; planes; planes = planes -> s_next)
		n++;
	return(n);
}

wreadstr(win, str)
WINDOW *win;
register char *str;
{
	register int n = 0;
	register int ch;

	while((ch = getchar()) != '\n') {
		if (ch == '' && n > 0)
			n--;
		else if (ch == '')
			continue;
		else
			str[n++] = ch;
		waddch(win, ch);
		wrefresh(win);
	}
	str[n] = '\0';
}

inform(fmt, jerry)
char *fmt;
int jerry;
{
	scrollrow++;
	if (scrollrow > 11) scrollrow = 0;
	wmove(notes, scrollrow, 0);
	wclrtoeol(notes);
	if (jerry) {
		wprintw(notes, fmt, shiplist[jerry -1].row, shiplist[jerry -1].col);
	} else
		wprintw(notes, fmt);
	wrefresh(notes);
	if (automatic == -2)
		automatic = 0;
}

char *daytime(time, buf)
int time;
register char buf[];
{
	float minutes;
	int hours, mins, over = ZERODAY;
	
	minutes = time * 2.5 + ZEROMIN;
	hours = minutes/60 + ZEROHOUR;
	mins = (int) minutes % 60;
	while (hours >= 24) {
		hours -= 24;
		over++;
	}
	sprintf(buf, "June %d %02d:%02d:00", over, hours, mins);
	return(buf);
}

char shiphit(type, dir, row, col)	/* to see if a ship hit by a shell */
register int type, dir, row, col;	/* shiphit(type, dir, hit.r - ship.r,*/
{					/* hit.c - ship.c) != ' ' */

	if (col >= -2 && col <= 2 && row >= -2 && row <= 2)
		return(shapes[type - CV][dir][row+2][col+2]);
	else
		return(' ');
}

hit (ran, ar, ac, br, bc, dir, offset)	/* sees if a plane is hit */
register int ran, ar, ac, br, bc, dir, offset;
{
	int dr, dc;

	if (range(ar,ac,br,bc) < 5) {
		vshape(dir, offset, &dr, &dc);
		return(ar - br - dr <= ran && ar - br - dr >= -ran && ac - bc - dc <= ran && ac - bc - dc >= -ran);
	} else
		return(0);
}

vshape(dir, offset, dr, dc)
int dir, offset, *dr, *dc;
{
	switch(dir/45) {

		case 0:
			*dr = offset < 0 ? -offset : offset;
			*dc = offset;
			break;

		case 1:
			*dr = offset < 0 ? 0 : offset;
			*dc = offset > 0 ? 0 : offset;
			break;
		
		case 2:
			*dr = offset;
			*dc = offset > 0 ? -offset : offset;
			break;

		case 3:
			*dr = offset < 0 ? offset : 0;
			*dc = offset < 0 ? 0 : -offset;
			break;

		case 4:
			*dr = offset < 0 ? offset : -offset;
			*dc = -offset;
			break;

		case 5:
			*dr = offset < 0 ? 0 : -offset;
			*dc = offset < 0 ? -offset : 0;
			break;

		case 6:
			*dr = -offset;
			*dc = offset < 0 ? -offset : offset;
			break;

		case 7:
			*dr = offset < 0 ? -offset : 0;
			*dc = offset < 0 ? 0 : offset;
			break;
	}
}

ditch(planes, head)
register struct squadron *planes, **head;
{
	if (planes -> previous)
		planes -> previous -> s_next = planes -> s_next;
	else {
		*head = planes -> s_next;
	}
	if (planes -> s_next)
		planes -> s_next -> previous = planes -> previous;
	// XXX results in double free crashes
	// XXX ought to mark the freed object to check later
	//free(planes);
}

range(ar, ac, br, bc)
register int ar, ac, br, bc;
{
	ar -= br;
	ac -= bc;
	ar = abs(ar);
	ac = abs(ac);
	return(max(ar,ac));
}
