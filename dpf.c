#include "dpf.h"

void open_dpf(char *file, DPF *dpf) {
	FILE *fp;
	word count = 0;	

	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL) {
		printf("Error opening file %s.\n",file);
		exit(1);
	}

	/* try to allocate memory */
	if ((dpf->data = (byte *) malloc((word)(SCREEN_SIZE))) == NULL) {
		fclose(fp);
		printf("Error allocating memory for file %s.\n",file);
		exit(1);
	}		
		
	for(count=0;count<SCREEN_SIZE;count++) {
		dpf->data[count] = (byte) fgetc(fp);		
	}
	fclose(fp);
}

void save_dpf(char *file, byte* bitmap) {
	FILE *fp;	
	
	if((fp = fopen(file, "wb")) == NULL) {
		printf("Error opening file %s.\n",file);
		exit(1);
	}
		
	fwrite(bitmap, 1, SCREEN_SIZE, fp);	
	fclose(fp);
}

void draw_dpf(DPF dpf) {	
	memcpy(canvas, dpf.data, SCREEN_SIZE);
}