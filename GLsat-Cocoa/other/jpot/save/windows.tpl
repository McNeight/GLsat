#include <stdio.h>
#include <signal.h>
#include <windows.h>
#include <winbase.h>

FILE *fp = NULL;
int firsttime = 1;
HANDLE event;

<Global Variable>

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
			<Read Data>
		}
		fclose(fp);
	}
}

void CheckRefresh() {
	DWORD ret;
	
	if (firsttime == 1) {
		firsttime = 0;
		glutPostRedisplay();
	}
	else
	{
		ret = WaitForSingleObject(event, 200);
		if ((ret == WAIT_OBJECT_0)) {
			glutPostRedisplay();
		}
	}
}
