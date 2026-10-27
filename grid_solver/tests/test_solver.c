#include <stdio.h>
#include <string.h>
#include "solver.h"

static char *grid[] = {
	"HORIZONTAL",
	"DXRAHCLBGA",
	"DIKCILEOKC",
	"IGAJHYLYHI",
	"HGFGODTIOT",
	"GDLROWKBFR",
	"PLNRDNERGE",
	"JHAIDUAJGV",
	"UKGFFOLLEH"};

static int test_word(const char *word, const char *expected){
	struct Position start;
	struct Position end;
	char r[64];
	if (solve(grid,9,10, word,&start,&end))
		sprintf(r,"(%d,%d)(%d,%d)",start.x,start.y,end.x,end.y);
	else{
		strcpy(r,"Not found");}
	printf("%s\n",word);
	printf("expected : %s\n", expected);
	printf("got : %s\n",r);
	if (strcmp(r,expected)==0){
		printf("GOOD !\n\n");
		return 1;}
	printf("ERROR!!!\n\n");
	return 0;
}

int main(void){
	int test=0;
	test +=test_word("HORIZONTAL","(0,0)(9,0)");
	test += test_word("VERTICAL", "(9,7)(9,0)");
	test += test_word("DIAGONAL", "(0,1)(7,8)");
	test += test_word("FIND", "(4,8)(1,5)");
	test += test_word("HELLO", "(9,8)(5,8)");
	test += test_word("GOLDORAK", "(8,1)(1,8)");
	test += test_word("WORLD", "(5,5)(1,5)");
	test += test_word("EPITA", "Not found");
	printf("%d/8 tests passed\n\n", test);
	if (test!=8)
		return 1;
	return 0;
}
