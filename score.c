#include <stdio.h>
#include <pwd.h>
#include "globals.h"

#define LOGFILE "/usr/public/.midwaylog"

int maxpoints[2] = {637, 384};
struct logs {
	int uid;
	int fshipnum;
	int netpoints;
} log[20], zero;

main()
{
	FILE *fp;
	register int k, n;
	struct passwd *getpwuid();
	int score;
	long st;
	char buf[32];

	if (fp = fopen(LOGFILE, "r+")){
		n = fread(log, sizeof(struct logs), 20, fp);
		rewind(fp);
		printf("\n\nNAME\t\tSHIP\t\t\tTONS SUNK\tPERCENT\n\n");
		for (k=0; k < 20; k++) {
			st = (long) log[k].netpoints * 100L;
			score = (int) (st / (long) maxpoints[(Japanese(log[k].fshipnum))]);
			printf("%-10s%16s%20ld%10d%%\n", getpwuid(log[k].uid) -> pw_name, shiplist[log[k].fshipnum].name, log[k].netpoints * 1000L, score);
			printf("\nSave? ");
			scanf("%s", buf);
			if (*buf == 'y') 
				fwrite(log + k, sizeof(struct logs), 1, fp);
			else {
				printf("Removing...\n");
				n--;
			}
		}
		fwrite(&zero, sizeof(struct logs), 20 - n, fp);
	}
	fclose(fp);
}
