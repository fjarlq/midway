#include "externs.h"

moveships()
{
   register int n,i,p;
   int target;

   if (target = goodbogey(0))
      for (n=0; amtable[n] < MAXSHIPS; n++)
         if (amtable[n] != player)
            shiplist[amtable[n]].course = intercept(amtable[n], target -1);
   if (target = goodbogey(JAPANESE))
      for (n=0; japtable[n] < MAXSHIPS; n++)
         if (japtable[n] != player)
            shiplist[japtable[n]].course = intercept(japtable[n], target -1);
   for (n=0; n < MAXSHIPS; n++) {
      firedflack[n] = 0;
      firedguns[n] = 0;
      if (shiplist[n].torps && shiplist[n].hits && n != player && n != MIDWAY)
         drdc((shiplist[n].course = shiplist[shiplist[n].flagship].course), &shiplist[n].row, &shiplist[n].col);
   }
   if (player != MIDWAY)
      drdc(shiplist[player].course, &shiplist[player].row, &shiplist[player].col);
   if (automatic >= 0)
	   plotships();

   for (n=0; amtable[n] < MAXSHIPS; n++) {
      if (shiplist[amtable[n]].hits && shiplist[amtable[n]].torps) {
         for (i = 0; japtable[i] < MAXSHIPS; i++) {
            if (shiplist[japtable[i]].hits && shiplist[japtable[i]].torps) {
               if (range(shiplist[amtable[n]].row, shiplist[amtable[n]].col, shiplist[japtable[i]].row, shiplist[japtable[i]].col) < 25) {   /* within sight */
                  for (p=japtable[i]; p < MAXSHIPS && shiplist[p].flagship == japtable[i]; p++)
                     if (p != player)
                        fireguns(p, amtable[n]);
                  for (p=amtable[n]; p < MAXSHIPS && shiplist[p].flagship == amtable[n]; p++)
                     if (p != player)
                        fireguns(p, japtable[i]);
                  newbogey(japtable[i]);
                  newbogey(amtable[n]);
               }
            }
         }
      }
   }
   fly(american,0,1);
   fly(japanese,0,0);
   fly(amscouts,1,1);
   fly(japscouts,1,0);
   if (automatic >= 0)
	   plotplanes();
   steer(american,0,1);
   steer(japanese,0,0);
   steer(amscouts,1,1);
   steer(japscouts,1,0);
}

fly(planes, scout, yank)
register struct squadron *planes;
int scout, yank;
{
   register int n;
   int speed;
   char buf[32];
   int dr, dc;

   for (; planes; planes = planes -> s_next) {
      speed = scout ? 15 : 10;
      if (planes -> attack > 0) speed = 3;
      dr = dc = 0;
      if (scout)
         drdc(planes -> course, &dr, &dc);
      else
         drdc(setcourse(planes, planes -> course), &dr, &dc);
      planes -> row += dr * speed;
      planes -> col += dc * speed;
      if (planes -> fuel-- <= 0) {
         switch (planes -> type) {
            case F4F:
               shiplist[planes -> from].f4f += planes -> planes;
               break;
            case SBD:
               shiplist[planes -> from].sbd += planes -> planes;
               break;
            case TBF:
               shiplist[planes -> from].tbf += planes -> planes;
               break;
         }
         if (planes -> from == player) {
            sprintf(buf, "Recovering %d %s", planes -> planes, describe[planes -> type]);
            inform(buf, 0);
         }
         if (yank)
            if (scout)
               ditch(planes, &amscouts);
            else
               ditch(planes, &american);
         else
            if (scout)
               ditch(planes, &japscouts);
            else
               ditch(planes, &japanese);
      }
   }
}

steer(planes, scout, yank)
register struct squadron *planes;
int scout, yank;
{
   register int p, k, n, s;
   int *table;
   char buf[32];
   int ran;
   struct torpedo *torp;

   for (; planes; planes = planes -> s_next) {
      table = yank ? japtable : amtable;
      for (k=0; table[k] < MAXSHIPS; k++) {
         if (shiplist[table[k]].hits && shiplist[table[k]].torps) {
            if ((ran = range(planes -> row, planes -> col, shiplist[table[k]].row, shiplist[table[k]].col)) < (scout ? 50 : 30)) {
               newbogey(table[k]);
               if (!scout && planes -> attack >= 0 && table[k] == shiplist[planes -> course].flagship) {
                  if (!planes -> attack) {
                     planes -> attack = 1;
                     sprintf(buf, "%s under attack", shiplist[table[k]].name);
                     inform(buf, 0);
                     for (p=table[k]; p < MAXSHIPS && shiplist[p].flagship == table[k]; p++)
                        sendcap(planes, p);
                  }
                  s = planes -> course = table[k];
                  if (s != player)
                     flack(aimflack(s, planes), s);
                  for (p=s+1; p < MAXSHIPS && shiplist[p].flagship == s; p++)
                     if (p != player)
                        if (rnd(2))
                           flack(aimflack(p, planes), p);
                        else
                           flack(intercept(p,s),p);
                  if (planes -> type == TBF) {
                     if (ran < 7) {
                        planes -> attack = -1;
                        for (n=0; n < planes -> planes; n++) {
                           if (!(torp = (struct torpedo *) calloc(1, sizeof(struct torpedo)))){
                              perror("calloc");
                              exit(1);
                           }
                           torp -> next_torp = yank ? amfish : japfish;
                           if (yank) amfish = torp;
                           else japfish = torp;
                           vshape(setcourse(planes, s), n - planes -> planes / 2, &torp -> row, &torp ->col);
                           torp -> row += planes -> row + rnd(3) - 1;
                           torp -> col += planes -> col + rnd(3) - 1;
                           torp -> fromship = planes -> from;
                           torp -> timeleft = 4 + rnd(12);
		           torp -> course = setcourse(planes, s);
                           if (!rnd(10))
                              torp -> course += rnd(5) -2;  /* a few duds */
                           planes -> course = planes -> from;
                           planes -> fuel = range(planes -> row, planes -> col, shiplist[planes -> from].row, shiplist[planes -> from])/10;
                        }
                     }
                  } else if (ran < 4) {
                     planes -> attack = -1;
                     planes -> course = planes -> from;
                     planes -> fuel = range(planes -> row, planes -> col, shiplist[planes -> from].row, shiplist[planes -> from])/10;
                     if (planes -> type == SBD) {
                        for (n=0; n < planes -> planes; n++) {
                           if (rnd(2)) {
                              sprintf(buf, "%s bombed.", shiplist[s].name);
                              inform(buf, 0);
                              shiplist[s].hits--;
                              shiplist[s].f4f -= shiplist[s].f4f/3;
                              shiplist[s].tbf -= shiplist[s].tbf/3;
                              shiplist[s].sbd -= shiplist[s].sbd/3;
                           }
                        }
                        plotsplash(shiplist[s].row, shiplist[s].col, '#');
                        if (shiplist[s].hits <= 0) {
                           shiplist[s].hits = 0;
                           sprintf(buf, "%s sinking!", shiplist[s].name);
                           inform(buf, 0);
                           shiplist[planes -> from].points += shiplist[s].value;
                           if (shiplist[s].flagship == s)
                              transferflag(s);
                           else if (s == player)
                              die();
                           break;
                        }
                     }
                  }
               }
            }
         }
      }

      if (!planes -> attack && planes -> fuel <= Fuel[planes -> type]/2) {
         planes -> attack = -1;   /* return home */
         if (scout) planes -> course = setcourse(planes, planes -> from);
         else
            planes -> course = planes -> from;
      }
      if (!scout && (!shiplist[planes -> course].hits || !shiplist[planes -> course].torps)) {
         if (planes -> course == planes -> from) {
            if (!Japanese(planes -> from))
               planes -> course = planes -> from = MIDWAY;
            else
               planes -> course = planes -> from = 22;   /* the Hiryu */
         } else {
            if (shiplist[shiplist[planes -> course].flagship].hits && shiplist[shiplist[planes -> course].flagship].torps) {
               planes -> course = shiplist[planes -> course].flagship;
            } else {
               planes -> course = planes -> from;
               planes -> attack = -1;
               planes -> fuel = range(planes -> row, planes -> col, shiplist[planes -> from].row, shiplist[planes -> from])/10;
            }
         }
      }
   }
}

drdc(dir, dr, dc)
register int dir, *dr, *dc;
{
   switch (dir) {

      case 0:
         *dr -= 1;
         break;

      case 1:
      case 45:
         *dr -= 1;
         *dc += 1;
         break;

      case 2:
      case 90:
         *dc += 1;
         break;

      case 3:
      case 135:
         *dr += 1;
         *dc += 1;
         break;

      case 4:
      case 180:
         *dr += 1;
         break;

      case 5:
      case 225:
         *dr += 1;
         *dc -= 1;
         break;

      case 6:
      case 270:
         *dc -= 1;
         break;

      case 7:
      case 315:
         *dr -= 1;
         *dc -= 1;
         break;

      default:
         *dr += rnd(3) - 1;
         *dc += rnd(3) - 1;
         break;
   }
}

sendcap(enemy, from)
struct squadron *enemy;
int from;
{
   int killed;
   char buf[32];

   if (shiplist[from].hits && shiplist[from].torps && capplanes[from]) {
      sprintf(buf, "%s CAP intercepting", shiplist[from].name);
      inform(buf, 0);
      switch (enemy -> type) {
         
         case F4F:
            if (capplanes[from] > 2 * enemy -> planes) {
               capplanes[from] -= (killed = enemy -> planes);
               enemy -> planes = 0;
            } else if (enemy -> planes > 2 * capplanes[from]) {
               enemy -> planes -= (killed = capplanes[from]);
               capplanes[from] = 0;
            } else {
               killed = min(capplanes[from], enemy -> planes);
               capplanes[from] -= killed;
               enemy -> planes -= killed;
            }
            break;

         case SBD:
            enemy -> planes -= (killed = capplanes[from] * 3 / 10);
            break;

         case TBF:
            killed = capplanes[from] * 35 / 100;
            capplanes[from] -= enemy -> planes * 15 / 100;
            enemy -> planes -= killed;
      }
      if (killed > 0) {
         sprintf(buf, "*** %d %s shot down", killed, describe[enemy -> type]);
         inform(buf, 0);
      }
      if (enemy -> planes <= 0) {
         if (Japanese(from))
            ditch(enemy, &american);
         else
            ditch(enemy, &japanese);
      }
   }
}
