char version[] = "%W%";

#include "externs.h"

airstrike()		/* launch computer searches, strikes, recover. */
{
	int enemy;
	int send;
	int type;
	register int n;
	struct squadron *planes;
	char buf[36];

	enemy = goodbogey(0);
	for (n=0; n < MAXSHIPS; n++) {
		if (n == JAPANESE) enemy = goodbogey(JAPANESE);
		if (n != player && shiplist[n].hits && shiplist[n].torps) {
			if (shiplist[n].type != CV || planesize(Japanese(n) ? japscouts : amscouts) < 7) {
				if (shiplist[n].tbf) {
					planes = catapult(n, 1, SCOUT);
					planes -> course = rnd(8);
				}
			}

			/* strike! */

			if (shiplist[n].type == CV || shiplist[n].type == FT) {
				if (enemy) {
					if (!shiplist[n].launching) {
						if (range(shiplist[enemy - 1].row, shiplist[enemy - 1].col, shiplist[n].row, shiplist[n].col) < Fuel[TBF]*5) {
							shiplist[n].launching = 1;
						}
					}
					switch (shiplist[n].launching++) {
						case 0:
						case 4:
							shiplist[n].launching = 0;
							send = 0;
							break;
						case 1:
							if (shiplist[n].f4f > 10)
								send = shiplist[n].f4f/2 + 1;
							else
								send = shiplist[n].f4f;
							type = F4F;
							break;
						case 2:
							if (shiplist[n].sbd > 20)
								send = shiplist[n].sbd/2;
							else
								send = shiplist[n].sbd;
							type = SBD;
							break;
						case 3:
							if (shiplist[n].tbf > 15)
								send = shiplist[n].tbf/2;
							else
								send = shiplist[n].tbf;
							type = TBF;
							break;
					}
					if (send) {
						planes = catapult(n, send, type);
						planes -> course = enemy - 1;
						if (Japanese(n) == Japanese(player)) {
							sprintf(buf, "%s launching planes", shiplist[n].name);
							inform(buf, 0);
						}
					}
				}
			}
		}
	}
}

newbogey(boat)
int boat;
{
	register int n;
	char buf[32];

	if (!sighted[boat]) {
		for (n = boat; n < MAXSHIPS && shiplist[n].flagship == boat; n++)
			sighted[n] = 1;
		if (Japanese(boat) == Japanese(player)) {
			if (boat == shiplist[player].flagship)
				inform("Enemy scout plane overhead!", 0);
		} else {
			sprintf(buf, "%s %s sighted", describe[shiplist[boat].type], shiplist[boat].name);
			inform(buf, 0);
			inform("@ %d, %d", boat + 1);
		}
	}
}

struct squadron *catapult(from, size, type)
int from, size, type;
{
	register struct squadron *temp;

	temp = (struct squadron *) calloc(1, sizeof(*temp));
	if(!temp) {
		perror("the first calloc in catapult");
		kill(getpid(), 3);
	}
	if (Japanese(from)) {
		if (type == SCOUT) {
			if (japscouts) japscouts -> previous = temp;
			temp -> s_next = japscouts;
			japscouts = temp;
			type = TBF;
		} else {
			if (japanese) japanese -> previous = temp;
			temp -> s_next = japanese;
			japanese = temp;
		}
	} else {
		if (type == SCOUT) {
			if (amscouts) amscouts -> previous = temp;
			temp -> s_next = amscouts;
			amscouts = temp;
			type = TBF;
		} else {
			if (american) american -> previous = temp;
			temp -> s_next = american;
			american = temp;
		}
	}
	temp -> row = shiplist[from].row;
	temp -> col = shiplist[from].col;
	temp -> from = from;
	temp -> type = type;
	temp -> planes = size;
	temp -> fuel = Fuel[type];
	
	/* the rest zeros */

	switch (type) {

		case TBF:
			shiplist[from].tbf -= size;
			break;

		case SBD:
			shiplist[from].sbd -= size;
			break;

		case F4F:
			shiplist[from].f4f -= size;
			break;
	}
	return(temp);
}

goodbogey(ship)			/* finds best bogey ~ 20% type, 20% newness */
int ship;			/* rest range */
{
	register int *table;
	register int score, bestscore = -1;
	register int jerry, best = 0;

	table = Japanese(ship) ? amtable : japtable;
	for (jerry = 0; table[jerry] < MAXSHIPS; jerry++) {
		if (sighted[table[jerry]] && shiplist[table[jerry]].hits && shiplist[table[jerry]].torps) {
			score = FT - shiplist[jerry].type;
			if (score > bestscore) {
				bestscore = score;
				best = table[jerry] + 1;
			}
		}
	}
	return(best);
}
