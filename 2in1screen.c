/* this is a cool auto rotate script that rotates screen and touchscreen together
   from https://github.com/aleozlx/2in1screen/blob/master/2in1screen.c
   build with:
   gcc -O2 -o 2in1screen 2in1screen.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DATA_SIZE 256
//#define N_STATE 2 //for only normal and invert
#define N_STATE 4 //for all 4 positions
char basedir[DATA_SIZE];
char *basedir_end = NULL;
char content[DATA_SIZE];
char command[DATA_SIZE*4];

char *ROT[]   = {"normal", 				"inverted", 			"left", 				"right"};
char *COOR[]  = {"1 0 0 0 1 0 0 0 1",	"-1 0 1 0 -1 1 0 0 1", 	"0 -1 1 1 0 0 0 0 1", 	"0 1 0 -1 0 1 0 0 1"};
// char *TOUCH[] = {"enable", 				"disable", 				"disable", 				"disable"};
char WIDE_WALLPAPER[84];
char TALL_WALLPAPER[] = "~/Pictures/TALL-1080x1920.jpg";

double accel_y = 0.0,
#if N_STATE == 4
	   accel_x = 0.0,
#endif
	   accel_g = 7.0;

int current_state = 0;

int rotation_changed(){
	int state = 0;

	if(accel_y < -accel_g) state = 0;
	else if(accel_y > accel_g) state = 1;
#if N_STATE == 4
	else if(accel_x > accel_g) state = 2;
	else if(accel_x < -accel_g) state = 3;
#endif

	if(current_state!=state){
		current_state = state;
		return 1;
	}
	else return 0;
}

FILE* bdopen(char const *fname, char leave_open){
	*basedir_end = '/';
	strcpy(basedir_end+1, fname);
	FILE *fin = fopen(basedir, "r");
	setvbuf(fin, NULL, _IONBF, 0);
	fgets(content, DATA_SIZE, fin);
	*basedir_end = '\0';
	if(leave_open==0){
		fclose(fin);
		return NULL;
	}
	else return fin;
}

void rotate_screen(){
	sprintf(command, "xrandr -o %s", ROT[current_state]);
	system(command);
//	sprintf(command, "xinput set-prop \"%s\" \"Coordinate Transformation Matrix\" %s", "Wacom HID 4846 Finger", COOR[current_state]);
	sprintf(command, "xinput set-prop \"%s\" \"Coordinate Transformation Matrix\" %s", "Wacom HID 4848 Finger touch", COOR[current_state]);
	system(command);
	system("killall docky");
	system("killall spacefm");
	if (current_state < 2) {
		sprintf(command, "/usr/bin/spacefm --set-wallpaper %s", WIDE_WALLPAPER);
	}
	else {
		sprintf(command, "/usr/bin/spacefm --set-wallpaper %s", TALL_WALLPAPER);
	}
	system("/usr/bin/spacefm --desktop &");
	usleep(300000);	
	system(command);
	system("/usr/bin/docky &");
}

int main(int argc, char const *argv[]) {
	FILE* stream = popen("grep ^wallpaper= ~/.config/spacefm/session|cut -d '=' -f2", "r");
	while (fgets(WIDE_WALLPAPER, 84, stream) != NULL)
	pclose(stream);
	WIDE_WALLPAPER[strcspn(WIDE_WALLPAPER, "\r\n")] = 0;
	FILE *pf = popen("ls /sys/bus/iio/devices/iio:device*/in_accel*", "r");
	if(!pf){
		fprintf(stderr, "IO Error.\n");
		return 2;
	}

	if(fgets(basedir, DATA_SIZE , pf)!=NULL){
		basedir_end = strrchr(basedir, '/');
		if(basedir_end) *basedir_end = '\0';
		fprintf(stderr, "Accelerometer: %s\n", basedir);
	}
	else{
		fprintf(stderr, "Unable to find any accelerometer.\n");
		return 1;
	}
	pclose(pf);

	bdopen("in_accel_scale", 0);
	double scale = atof(content);

	FILE *dev_accel_y = bdopen("in_accel_y_raw", 1);
#if N_STATE == 4
	FILE *dev_accel_x = bdopen("in_accel_x_raw", 1);
#endif

	while(1){
		fseek(dev_accel_y, 0, SEEK_SET);
		fgets(content, DATA_SIZE, dev_accel_y);
		accel_y = atof(content) * scale;
#if N_STATE == 4
		fseek(dev_accel_x, 0, SEEK_SET);
		fgets(content, DATA_SIZE, dev_accel_x);
		accel_x = atof(content) * scale;
#endif
		if(rotation_changed())
			rotate_screen();
		sleep(2);
	}
	
	return 0;
}
