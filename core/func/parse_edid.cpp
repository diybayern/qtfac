/* Complete parse-edid rewrite. Using some unix-specific stuff - I don't care about dos
 *
 * Hope it works :P 
 *
 * 
 * (C)opyright 2008-2014 Matthew Kern
 * Full license terms in file LICENSE
 * */

#include <stdio.h> //for the *printf's et al.
#include <unistd.h>    //read, and because it's nice to have
#include <stdlib.h>    //exit, and because it's nice to have
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "../../inc/modes.h"
#include "../../inc/fac_utils.h"
#include "../../inc/fac_log.h"


typedef unsigned char byte;

static byte edid[128]; //edid itself
static byte extb[128]; //extension block. Hope there's only 1, for now.

static char modearray[128][128];
static int native;
static int currentmode;

static const char *invalid_model_name[] = {
	"DP2VGA"
};

#if 0
static void die(char *msg) {
	fprintf(stderr, "%s", msg);
	exit(1);
}

static void warn(char *msg) {
	fprintf(stderr, "WARNING: ");
	fprintf(stderr, "%s", msg);
	fprintf(stderr, "Trying to continue...\n");
}

static void diewemail(char *msg) {
	fprintf(stderr, "%s", msg);
	fprintf(stderr, "Something strange happened. Please contact the author,\nMatthew Kern at <pyrophobicman@gmail.com>\n");
	exit(1);
}
#endif

static int detect_invalid_device(const char *name)
{
	int i;
	int idx = -1;
	int size = sizeof(invalid_model_name) / sizeof(invalid_model_name[0]);

	for (i = 0; i < size; i++) {
		if (strcmp(invalid_model_name[i], name) == 0) {
			idx = i;
			break;
		}
	}

	return idx;
}

int parseedid() {
	int i;
	int j;
	int idx;
	byte sum = 0;
	char modelname[13];
	//int ptm;
//	int compsync = 0;
	int hres;
	int vres;
    int ret;

	//check the checksum
	for (i = 0; i<128; i++) {
		sum += edid[i];
	}
	if (sum) {
		LOG_ERROR("ERROR: EDID Checksum failed!\n");
        ret = FAIL;
	} else {
		LOG_INFO("\tEDID Checksum Correct.\n\n");
        ret = SUCCESS;
	}
	
	//check header
	for (i = 0; i < 8; i++) {
		if (!(((i == 0 || i == 7) && edid[i] == 0x00) || (edid[i] == 0xff))) {
            //0x00 0xff 0xff 0xff 0xff 0xff 0x00
			LOG_ERROR("ERROR: Header incorrect. Probably not an edid\n");
            ret = FAIL;
            break;
		}
	}
	
	//Make a monitor section...
	LOG_INFO("Section \"Monitor\"\n");

	//Product Identification
	/* Model Name: Only thing I do out of order of edid, to comply with X standards... */
	for (i = 0x36; i < 0x7E; i += 0x12) { //read through descriptor blocks...
		if (edid[i] == 0x00) { //not a timing descriptor
			if (edid[i+3] == 0xfc) { //Model Name tag
				for (j = 0; j < 13; j++) {
					if (edid[i+5+j] == 0x0a)
						modelname[j] = 0x00;
					else
						modelname[j] = edid[i+5+j];
				}
			}
		}
	}
	LOG_INFO("\tIdentifier \"%s\"\n\tModelName \"%s\"", modelname, modelname);
	idx = detect_invalid_device(modelname);
	if (idx >= 0) {
		ret = FAIL;
		LOG_ERROR("Error identifier/modelname: \"%s\"\n", invalid_model_name[idx]);
	} else {
		LOG_INFO("\n");
	}


	/* Vendor Name: 3 characters, standardized by microsoft, somewhere.
	 * bytes 8 and 9: f e d c b a 9 8  7 6 5 4 3 2 1 0
	 * Character 1 is e d c b a
	 * Character 2 is 9 8 7 6 5
	 * Character 3 is 4 3 2 1 0
	 * Those values start at 0 (0x00 is 'A', 0x01 is 'B', 0x19 is 'Z', etc.)
	 */
	LOG_INFO("\tVendorName \"%c%c%c\"\n", (edid[8] >> 2 & 0x1f) + 'A' - 1, (((edid[8] & 0x3) << 3) | ((edid[9] & 0xe0) >> 5)) + 'A' - 1, (edid[9] & 0x1f) + 'A' - 1 );

	/* Skip Product ID and Serial Number. */
	/* Week and Year: not required, but do it for fun. */
	if (edid[0x10] <= 54)
		LOG_INFO("\t# Monitor Manufactured week %i of %i\n", (int)(edid[0x10]), ((int)(edid[0x11])) + 1990);
	else if (edid[0x10] != 0xff)
		LOG_INFO("\t# Monitor Manufactured in %i\n", ((int)(edid[0x11])) + 1990);
	else if (edid[0x10] == 0xff)
		LOG_INFO("\t# Monitor Model Year: %i\n", ((int)(edid[0x11])) + 1990);
	
	//Edid Version
	LOG_INFO("\t# EDID version %i.%i\n", (int)(edid[0x12]), (int)(edid[0x13]));

	//Basic Display Parameter
	/* Digital or not? */
	if (edid[0x14] & 0x80)
		LOG_INFO("\t# Digital Display\n");
	else {
		LOG_INFO("\t# Analog Display\n");
		if (edid[0x14] & 0x02) //sync on green.
			LOG_INFO("\tOption \"SyncOnGreen\" \"true\"\n");
//		if (edid[0x14] & 0x04)
//			compsync = 1; //Composite sync. Save for modelines.
	}
	/* Ignore video input definitions, because X doesn't care. */

	/* Size parameters: H and V, in centimeters. Projectors put 0 here. 
	 * DiplaySize is in millimeters, so multiply by 10 
	 * If 0x16 is 0, but not 0x15, you really should do aspect ratio... */
	if (edid[0x15] && edid[0x16])
		LOG_INFO("\tDisplaySize %i %i\n", ((int)(edid[0x15])) * 10, ((int)(edid[0x16])) * 10);
	else
		LOG_INFO("\t# Display Physical Size not given. Normal for projectors.\n");
	
	/* Gamma. Divide by 100, add 1. Defaults to 1, so if 0, it'll be 1 anyway. */
	if (edid[0x17] != 0xff)
		LOG_INFO("\tGamma %.2f\n", (float)((((float)edid[0x17]) / 100) + 1));

	/* DPMS. Simple yes or no. */
	LOG_INFO("\tOption \"DPMS\" \"%s\"\n", (edid[0x18] & 0xE0) ? "true" : "false");
	
	/* Preferred timing mode. This has no meaning, really. The first mode given is preferred. */
	//ptm = ((edid[0x18] & 0x02) ? 1 : 0);
	//printf("\t# preferred timing mode %sfound\n", ptm ? "" : "not ");

	/* GTF. Not sure what it means to X, so ignore it (for now) */
	
	/* ignore chroma stuff as well. */
	
	//Monitor Limits... okay, another one out of order with the EDID...
	
	for (i = 0x36; i < 0x7E; i += 0x12) { //read through descriptor blocks...
		if (edid[i] == 0x00) { //not a timing descriptor
			if (edid[i+3] == 0xfd) { //monitor limits tag
				LOG_INFO("\tHorizsync %i-%i\n", (int)edid[i+7] + (((edid[i+4] & 0x0c) & 0x04) ? 255 : 0), (int)edid[i+8] + ((edid[i+4] & 0x08) ? 255 : 0));
				LOG_INFO("\tVertRefresh %i-%i\n", (int)edid[i+5] + (((edid[i+4] & 0x03) & 0x01) ? 255 : 0), (int)edid[i+6] + ((edid[i+4] & 0x02) ? 255 : 0));
				LOG_INFO("\t# Maximum pixel clock is %iMHz\n", (int)edid[i+9] * 10);
			}
		}
	}

	//I can ignore predefined modes - X knows these already.
	
	int hactive, vactive, pixclk, hsyncoff, hsyncwidth, hblank, vsyncoff, vsyncwidth, vblank;
	//Parse for Detailed Timing Descriptors...
	for (i = 0x36; i < 0x7E; i += 0x12) { //read through descriptor blocks...
		if ((edid[i] != 0x00) && (edid[i+1] != 0x00)) { //a dtd
			hactive = edid[i+2] + ((edid[i+4] & 0xf0) << 4);
			hblank = edid[i+3] + ((edid[i+4] & 0x0f) << 8);
			vactive = edid[i+5] + ((edid[i+7] & 0xf0) << 4);
			vblank = edid[i+6] + ((edid[i+7] & 0x0f) << 8);

			//printf("\tModeline \t\"%dx%d\" ", hactive, vactive);
			
			pixclk = (edid[i+1] << 8) | (edid[i]);

			sprintf(modearray[currentmode], "%.2f ", (double)pixclk / 100.0);
			
			//I'm using Fremlin's nomenclature...
			//sync offset = front porch
			//sync width = sync pulse width

			hsyncoff = edid[i+8] | ((edid[i+11] & 0xC0) << 2);
			hsyncwidth = edid[i+9] | ((edid[i+11] & 0x30) << 4);
			vsyncoff = ((edid[i+10] & 0xf0) >> 4) | ((edid[i+11] & 0x0C) << 2);
			vsyncwidth = (edid[i+10] & 0x0f) | ((edid[i+11] & 0x03) << 4);


			sprintf(modearray[currentmode], "%s%u %u %u %u ", modearray[currentmode], hactive, hactive+hsyncoff, hactive+hsyncoff+hsyncwidth, hactive+hblank);
			sprintf(modearray[currentmode], "%s%u %u %u %u ", modearray[currentmode], vactive, vactive+vsyncoff, vactive+vsyncoff+vsyncwidth, vactive+vblank);

			if ( (edid[i+17]&0x80) || ((edid[i+17]&0x18) == 0x18) ) {
				sprintf(modearray[currentmode], "%s%shsync %svsync %s", modearray[currentmode], ((edid[i+17]&0x10) && (edid[i+17]&0x02)) ? "+": "-", ((edid[i+17]&0x10) && (edid[i+17]&0x04)) ? "+": "-", (edid[i+17]&0x80) ? "interlace": "");
			//hehe... there's been at least 2 bugs in the old parse-edid the whole time - somebody caught the htimings one, and I just caught two problems right here - lack of checking for analog sync and getting hsync and vsync backwards... yes, vsync and hsync have been flipped this whole time. Glad I'm rewriting
			//printf("%s\n", modearray[currentmode]);
			currentmode++;
			
			}
			//printf("\tEndmode\n");
		}
	}

	//Standard Timings - put these after dtd's, because this way preferred is up top.
	/* I started doing this, but I think it's unnecessary. Think not? you do it. I'll comment what you're missing.*/
	int refresh;
	for (i = 0x26; i < 0x36; i += 0x2) { //read through list of resolutions...
		if (!(edid[i] == 1 && edid[i+1] == 1)) { //skip if it's "blank"
			hres = (((int)edid[i]) + 31) * 8;
			switch ((edid[i+1] & 0xC0) >> 6) {
			case 0: vres = (hres * 10) / 16; //16:10 aspect ratio
				break;
			case 1: vres = (hres * 3) / 4; //4:3
				break;
			case 2: vres = (hres * 4) / 5; //5:4
				break;
			case 3: vres = (hres * 9) / 16; //16:9
				break;
			default:
                LOG_ERROR("The impossible has happened.\n");
                break;
			}
			refresh = (edid[i+1] & 0x3F) + 60;
			LOG_INFO("\t#Not giving standard mode: ");
			LOG_INFO("%ix%i, %iHz\n", hres, vres, refresh);
			/*
			printf("\tMode\t\"%ix%i\"\n", hres, vres);
			printf("\t\tDotClock\t%.6f\n", (float)((hres * vres) * (((edid[i+1] & 0x3f) + 60)) / 1000000)); //(pixels/screen) * (screen/sec) / a million = megapixels/sec, aka dot clock in mHz
			printf("\t\tHTimings\t\n");*/
		}
	}

    return ret;
}

int parseextb() {
	int i, curloc;
	//char nativename[64];
	byte sum =0;
	LOG_INFO("\n\t#Extension block found. Parsing...\n");
	/*
	for (i = 0; i < 128; i++)
		printf("byte %xh is 0x%x\n", i, extb[i]);
	*/
	//printf("Tag: %x\n", extb[0]);
	
	for (i=0;i<128;i++) {
		sum +=extb[i];
	}
	if (sum != 0x00) {
		LOG_ERROR("Extension block checksum failed\n");
	}
	if (extb[0] != 0x02) 
		LOG_INFO("I only know about extension blocks of type 02h. PLEASE email me!\n");
	curloc = extb[2];
	if (curloc == 0) {
		LOG_INFO("#No data in the extension block\n");
		return 0;
	}

	//printf("There are %i bytes of data block\n", curloc - 4);

	//see CEA tables 28-30
	if (curloc > 4) {
		if ((extb[4] & 0xE0) != 0x40) { //if the first one is not a video one
		    LOG_INFO("extb[4]: 0x%x (0x%x)\n", extb[4], extb[4]&0xE0);
			LOG_ERROR("Hmm, you have data blocks, but not video ones... weird\n");
			
		}

		for (i=0;i<(extb[4]&0x1F);i++) {
				if ((extb[5+i]&0x7f) > 59) {
					LOG_WARN("#WARNING: I may have missed a mode (CEA mode %i)\n", extb[5+i]&0x7f);
					if (native==-1)
						LOG_WARN("#DOUBLE WARNING: It's your first mode, too, so this may actually be important.\n");
				}
				else {
					if (native==-1)
						native = currentmode;
					sprintf(modearray[currentmode], "%s", ceamodes[extb[5+i]&0x7F]);
					currentmode++;
				}
		}

	}

	//starting 18-byte DTD's.


	//Copypaste the DTD stuff from above.
	int hactive, vactive, pixclk, hsyncoff, hsyncwidth, hblank, vsyncoff, vsyncwidth, vblank;
	//Parse for Detailed Timing Descriptors...
	for (i = curloc; i < curloc+(18*4); i += 0x12) { //read through descriptor blocks...
		if ((extb[i] != 0x00) && (extb[i+1] != 0x00)) { //a dtd
			hactive = extb[i+2] + ((extb[i+4] & 0xf0) << 4);
			hblank = extb[i+3] + ((extb[i+4] & 0x0f) << 8);
			vactive = extb[i+5] + ((extb[i+7] & 0xf0) << 4);
			vblank = extb[i+6] + ((extb[i+7] & 0x0f) << 8);

			//printf("\tModeline \t\"%dx%d\" ", hactive, vactive);
			
			if (i == curloc && (extb[3]&0x0F) > 0) {
				native = currentmode;
				//sprintf(nativename, "%dx%d", hactive, vactive);
			}

			pixclk = (extb[i+1] << 8) | (extb[i]);

			sprintf(modearray[currentmode], "%.2f ", (double)pixclk / 100.0);
			
			//I'm using Fremlin's nomenclature...
			//sync offset = front porch
			//sync width = sync pulse width

			hsyncoff = extb[i+8] | ((extb[i+11] & 0xC0) << 2);
			hsyncwidth = extb[i+9] | ((extb[i+11] & 0x30) << 4);
			vsyncoff = ((extb[i+10] & 0xf0) >> 4) | ((extb[i+11] & 0x0C) << 2);
			vsyncwidth = (extb[i+10] & 0x0f) | ((extb[i+11] & 0x03) << 4);


			sprintf(modearray[currentmode], "%s%u %u %u %u ", modearray[currentmode], hactive, hactive+hsyncoff, hactive+hsyncoff+hsyncwidth, hactive+hblank);
			sprintf(modearray[currentmode], "%s%u %u %u %u ", modearray[currentmode], vactive, vactive+vsyncoff, vactive+vsyncoff+vsyncwidth, vactive+vblank);


			if ( (extb[i+17]&0x80) || ((extb[i+17]&0x18) == 0x18) ) {
				sprintf(modearray[currentmode], "%s%shsync %svsync %s", modearray[currentmode], ((extb[i+17]&0x10) && (extb[i+17]&0x02)) ? "+": "-", ((extb[i+17]&0x10) && (extb[i+17]&0x04)) ? "+": "-", (extb[i+17]&0x80) ? "interlace": "");
			//hehe... there's been at least 2 bugs in the old parse-edid the whole time - somebody caught the htimings one, and I just caught two problems right here - lack of checking for analog sync and getting hsync and vsync backwards... yes, vsync and hsync have been flipped this whole time. Glad I'm rewriting

			}
			//printf("\n");
			currentmode++;
		}
	}
	return 0;
}

int dofooter() {
	int i;
	if (native != -1)
		LOG_INFO("\tModeline \t\"Mode %i\" %s\n", native, modearray[native]);
	for (i=0;i<currentmode;i++) {
		if (i != native)
			LOG_INFO("\tModeline \t\"Mode %i\" %s\n", i, modearray[i]);
	}

	if (native != -1)
		LOG_INFO("\tOption \"PreferredMode\" \"Mode %i\"\n", native); //half a chance of giving us the native default... The preferred mode is already in front.
	LOG_INFO("EndSection\n");
	return 0;
}


int parse_edid_from_file(const char *edid_info) {
	int infd;
	int i;
	int extblock = 0;
    int ret;

	native = -1;
	currentmode = 0;

    infd = open(edid_info, O_RDONLY);
    if (infd < 0) {
        LOG_ERROR("Failed to open %s: %s\n", edid_info, strerror(errno));
        return FAIL;
    }

	if (read(infd, &edid, 128) != 128) {
		LOG_ERROR("EDID Partial Read error... (%s)\n", strerror(errno));
        ret = FAIL;
        goto _exit;
	}
	for (i = 0; i < 128; i++) {
		LOG_ERROR("byte %i: %x\n", i, edid[i]);
	}
	if (edid[126] == 0x01) {
		if (read(infd, &extb, 128) != 128) {
			LOG_ERROR("EDID extension block Partial Read error... (%s)\n", strerror(errno));
            ret = FAIL;
            goto _exit;
		}
        for (i = 0; i < 128; i++) {
            LOG_ERROR("byte %i: %02x\n", i+128, extb[i]);
        }
		extblock = 1;
	} else if (edid[126] > 0x01) {
		LOG_ERROR("You seem to have too many extension blocks. Will not continue to parse\n");
        ret = FAIL;
        goto _exit;
	}
	
	ret = parseedid();
	if (extblock)
		parseextb();
	dofooter();

_exit:
    close(infd);
	return ret;
}
