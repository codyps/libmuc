#define avrCamVer	1
#define avrCamRev	0

/****************************************************************************
*
*   Copyright (c) 2009 Matt Bateman
*   Robotics4Fun http://www.laughingsky.com/hobbies/robotics/
*   http://www.laughingsky.com/hobbies/robotics/pages/axon_modules/avrcam/index.html
*   (please link back if you use this code!)
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   Alternatively, this software may be distributed under the terms of BSD
*   license.
*
****************************************************************************/

/****************************************************************************
*   Version Notes
*	
*   1.0   1/2/2009    Initial release
****************************************************************************/

//#define avrCamDebug 			1				// Turn on debug output
#define avrCamShowInitStatus	1				// Display init status and version info

#define avrCamUART				2				// Which  UART is the camera attached to
#define avrCamGetByte()			uart2GetByte()	// Camera UART GetByte  function
#define avrCamSendByte			uart2SendByte	// Camera UART SendByte function
#define avrCamSTDOUTSendByte	uart1SendByte	// Debug  UART SendByte function


#define	avrCamTimeout			300				// Max time to wait for line to complete
#define	avrCamMaxLineLen		255				// Max length of a response line
char	avrCamResponse[avrCamMaxLineLen];		// Array to hold response string
uint8_t	avrCamTracking			=0;				// Is the camera in tracking mode
uint8_t	avrCamOn				=0;				// Is the camera on

#define avrCamErrTimeout		-1				// Return code for timed out response
#define avrCamErrToolong		-2				// Return code for a response line that is too long

#define avrCamCenterX			88				// Center pixel on X axis
#define avrCamCenterY			72				// Center pixel on Y axis
//#define avrCamCenterSquare		24				// Size of center sample square in pixels
#define avrCamPixPerDegX		12				// Tune this to smooth out Pan
#define avrCamPixPerDegY		20				// Tune this to smooth out Tilt


#define avrCamMaxBlobs			8				// Max number of blobs the camera supports
struct {										// Structure to hold blob data
	uint8_t		color;								// Which color map entry did it match
	uint8_t		x1;									// Left
	uint8_t		y1;									// Top
	uint8_t		x2;									// Right
	uint8_t		y2;									// Bottom
	int16_t		xDist;								// Horizontal dist from center
	int16_t		yDist;								// Vertical dist from center
	uint16_t	size;								// Area (w*h)
} avrCamBlob[avrCamMaxBlobs];					// Array to hold all blobs

struct {										// Structure to hold meta data about blobs
	uint8_t		cnt;								// How many blobs are valid
	uint8_t		largest;							// Which blob is the largest
} avrCamBlobInfo;	

#define avrCamMaxColorMaps		8				// Max number of color map entries the camera supports
struct _avrCamColorMap {						// Structure to hold color map entry
	uint8_t		rL;									// Lowest  red   value
	uint8_t		rH;									// Highest red   value
	uint8_t		gL;									// Lowest  green value
	uint8_t		gH;									// Highest green value
	uint8_t		bL;									// Lowest  blue  value
	uint8_t		bH;									// Highest blue  value
};
struct _avrCamColorMap avrCamColorMap[avrCamMaxColorMaps];	// Array of color map entries to be written to the camera


#ifndef getNow()
	// *******************************************************
	// Timer functions using timer2. By default on the Axon,
	// the overflow of timer2 is ~1ms. Be sure to init timer2
	// somewhere prior to calling any of the avrCam functions.
	//
	// timer2Init();
	//
	// These defines are in a misc module, but are included
	// here in case that module is not loaded. These are used
	// to handle timeouts when communicating with the camera.
	// *******************************************************
	#define getNow()				timer2GetOverflowCount()
	#define getTimeDiff(now,last)	((last>now)?(65535-last+now):(now-last))
#endif

int16_t avrCamGetPanDelta(uint16_t minSize) {
	// ********************************
	// Return the number of degrees to
	// pan to center the largest blob
	// ********************************
	uint8_t largest=avrCamBlobInfo.largest;					// Use the largest blob
	if (avrCamBlob[largest].size>minSize) {					// Is it big enough?
		return avrCamBlob[largest].xDist/avrCamPixPerDegX;		// Return the amount to pan
	} else {
		return 0;												// If not, don't pan
	}
}

int16_t avrCamGetTiltDelta(uint16_t minSize) {
	// ********************************
	// Return the number of degrees to
	// tilt to center the largest blob
	// ********************************
	uint8_t largest=avrCamBlobInfo.largest;					// Use the largest blob
	if (avrCamBlob[largest].size>minSize) {					// Is it big enough?
		return avrCamBlob[largest].yDist/avrCamPixPerDegY;		// Return the amount to tilt
	} else {
		return 0;												// If not, don't tilt
	}
}

int16_t avrCamGetLine(char chStart, char chEnd, uint8_t minLen, uint8_t maxLen) {
	// ************************************************************************
	// Get a line of response until we get the specified end character, reach
	// the max line length, or take too long. Return either the length of the
	// line, or a negative number error code.
	//
	// chStart		Character that defines the start of a line. If	0, then
	//				start with the first char.
	//
	// chEnd		Character that defines the end of a line.
	//
	// minLen		Minimum # of characters to go before starting to look for
	//				the end character.
	//
	// maxLen		Maximum # of characters in a line. If 0, then use the
	//				defined max length.
	// *************************************************************************
	int16_t		temp;			// Holds the next byte
	uint16_t	counter=0;		// Counts the length of the string
	uint16_t	startTime,now;	// Holds time stamps
	uint8_t		inLine;			// Are we in the line yet?

#ifdef avrCamDebug
	rprintfInit(avrCamSTDOUTSendByte);
#endif
	
	if (chStart) {												// Do we need to look for the start of the line?
		inLine=0;
	} else {
		inLine=1;
	}
	if (! maxLen) {												// Is max length set?
		maxLen=avrCamMaxLineLen;
	}

	startTime=getNow();											// Get starting time
	while(1) {
		now=getNow();											// Get current time
		if (getTimeDiff(now,startTime)>avrCamTimeout) {			// Took too long
#ifdef avrCamDebug
			rprintf("Timeout: %d\n",getTimeDiff(now,startTime));
#endif
			return avrCamErrTimeout;								// Return error
		}
		temp=avrCamGetByte();									// Get next byte. Returns -1 if no data present
		if (temp == chStart && ! inLine) {						// Found the start of the line
			inLine=1;
		} else if (temp==chEnd && inLine && counter>=minLen) {	// Found the end of the line
			avrCamResponse[counter]='\0';							// Null terminate the string
#ifdef avrCamDebug
		rprintf("Time: %d\n",getTimeDiff(now,startTime));
#endif
			return counter;											// Return the length of the string
		} else if (temp != -1 && inLine) {						// We saw a valid character
			avrCamResponse[counter++]=temp;							// Store character into an string array
		}
		if (counter>=maxLen) {									// Line too long
			return avrCamErrToolong;								// Return error
		}
	}
}

uint8_t avrCamSendCmd( char cmd[]) {
	// **************************************************************
	// Send the requested command string, then wait for the response.
	// Return 1 for ACK, 0 for anything else.
	// **************************************************************
	int16_t len;
	char *response;

#ifdef avrCamDebug
	rprintfInit(avrCamSTDOUTSendByte);
	rprintf("Sending command: ");
	rprintfStr(cmd);
	rprintf("\n");
#endif

	rprintfInit(avrCamSendByte);			// Set rprintf to use the camera's UART
	uartFlushReceiveBuffer(avrCamUART);		// Flush receive buffer
	rprintfStr(cmd);						// Send command
	rprintf("\r");							// Send command terminating character

	len=avrCamGetLine(0,'\r',0,0);			// Get response. No start char, no min, no max
	rprintfInit(avrCamSTDOUTSendByte);		// Set rprintf to use debug UART


	if (len>0) {							// Got a response
		response=avrCamResponse+len-3;			// Look at last 3 characters

#ifdef avrCamDebug
		rprintf("Response: ");
		rprintfStr(response);
		rprintf("\n");
#endif

		if (strcmp(response,"ACK")==0) {		// ACK
			return 1;
		} else if (strcmp(response,"NCK")==0) {	// NCK

#ifdef avrCamDebug
			rprintf("Error: NCK\n");
#endif

			return 0;
		} else {								// Some unexpected response

#ifdef avrCamDebug
			rprintf("Error: Unknown response\n");
#endif

			return 0;					
		}
	} else {								// GetLine had an error

#ifdef avrCamDebug
		rprintf("Error: %d\n",len);
#endif

		return 0;
	}
}

void avrCamInitColorMap(void) {
	// **************************************************
	// Initialize local copies of color map to all zeros
	// **************************************************
	for (uint8_t index=0;index<avrCamMaxColorMaps;index++) {
		avrCamColorMap[index].rL=0;
		avrCamColorMap[index].rH=0;
		avrCamColorMap[index].gL=0;
		avrCamColorMap[index].gH=0;
		avrCamColorMap[index].bL=0;
		avrCamColorMap[index].bH=0;
	}
}

void avrCamSetColorMap(uint8_t index,uint8_t rL,uint8_t rH,uint8_t gL,uint8_t gH,uint8_t bL,uint8_t bH) {
	// ************************************
	// Set values in local color map entry
	// ************************************
	avrCamColorMap[index].rL=rL;
	avrCamColorMap[index].rH=rH;
	avrCamColorMap[index].gL=gL;
	avrCamColorMap[index].gH=gH;
	avrCamColorMap[index].bL=bL;
	avrCamColorMap[index].bH=bH;
#ifdef avrCamDebug
	rprintfInit(avrCamSTDOUTSendByte);
	rprintf("\nSet Color Map %d: %d,%d %d,%d %d,%d\n",index,rL,rH,gL,gH,bL,bH);
#endif
}

uint8_t avrCamSendColorMap(void) {
	// *******************************************************
	// Send the local color map to the camera. Camera color
	// maps use a grid of bit values that are calculated from
	// the desired high and low values for red, green, and
	// blue of the 8 color map entries.
	// *******************************************************
#ifdef avrCamDebug
	rprintfInit(avrCamSTDOUTSendByte);
	rprintf("Sending command: SM");
#endif

	rprintfInit(avrCamSendByte);			// Prepare to send to camera UART
	rprintf("SM");							// Send command

	for (uint8_t color=0;color<3;color++) {	// Do for red, green, then blue
		uint8_t high,low;

		rprintf(" 0");							// First entry is always 0
		for (uint8_t i=1;i<16;i++) {			// Step through bits of the grid
			uint8_t entry=0;
			for (uint8_t index=0;index<avrCamMaxColorMaps;index++) {	// Step through the color map entries
				if (color==0) {									// Red
					high=avrCamColorMap[index].rH;
					low =avrCamColorMap[index].rL;
				} else if (color==1) {							// Green
					high=avrCamColorMap[index].gH;
					low =avrCamColorMap[index].gL;
				} else if (color==2) {							// Blue
					high=avrCamColorMap[index].bH;
					low =avrCamColorMap[index].bL;
				}
				if ((low<=i*16) && (high>=i*16) ) {	// Is value between the high and low?
					entry |= 1<<index;
				}											// Set bit
			}
#ifdef avrCamDebug
			rprintfInit(avrCamSTDOUTSendByte);
			rprintf(" %d",entry);
			rprintfInit(avrCamSendByte);
#endif
			rprintf(" %d",entry);							// Send byte
		}
	}
#ifdef avrCamDebug
	rprintfInit(avrCamSTDOUTSendByte);
	rprintf("\n");
	rprintfInit(avrCamSendByte);
#endif
	return avrCamSendCmd(NULL);					// Send null command to let that routing look for the return codes
}

int avrCamPing(void) {
	// ********************************************
	// Ping the camera and return 1 or 0 as status
	// ********************************************
	return avrCamSendCmd("PG");
}

int avrCamTrackStart(void) {
	// **********************
	// Turn on tracking mode
	// **********************
	uint8_t status;
	if (avrCamTracking) {			// Already tracking
		return 1;
	} else {						// Need to enable
		status=avrCamSendCmd("ET");		// Turn on tracking
		if (status) {					// Check status of command
			avrCamTracking=1;				// If good, set flag
		}
		return status;
	}
}

int avrCamTrackStop(void) {
	// ***********************
	// Turn off tracking mode
	// ***********************
	uint8_t status;
	// Stop tracking mode
	if (! avrCamTracking) {			// Already stopped
		return 1;
	} else {						// Need to disable
		status=avrCamSendCmd("DT");		// Turn off tracking
		if (status) {					// Check status of command
			avrCamTracking=0;				// If good, clear flag
		}
		return status;
	}
}

uint8_t avrCamGetTrackLine (void) {
	// *******************************************************
	// Get next tracing line from the camera and interpret it
	// *******************************************************
	if (avrCamTracking) {								// Are we in tracking mode?
		int16_t len=avrCamGetLine(0x0A,0xFF,0,0);			// Get the next tracking line;
		if (len>0) {											// Is there valid data?
			uint16_t size=0;
			uint8_t ptr=0;
			uint16_t	xSize,ySize;
			avrCamBlobInfo.cnt=avrCamResponse[ptr++];				// Get the number of blobs in the line
			for (uint8_t i=0;i<avrCamBlobInfo.cnt;i++) {			// Step through the blobs and store the results
				avrCamBlob[i].color	=avrCamResponse[ptr++];			// Get color
				avrCamBlob[i].x1	=avrCamResponse[ptr++];			// Get Left
				avrCamBlob[i].y1	=avrCamResponse[ptr++];			// Get Top
				avrCamBlob[i].x2	=avrCamResponse[ptr++];			// Get Right
				avrCamBlob[i].y2	=avrCamResponse[ptr++];			// Get Bottom
				xSize				=avrCamBlob[i].x2
									-avrCamBlob[i].x1;				// Calculate X size
				ySize				=avrCamBlob[i].y2
									-avrCamBlob[i].y1;				// Calculate Y size
				avrCamBlob[i].xDist	=xSize/2
									+avrCamBlob[i].x1-avrCamCenterX;// Calculate X dist from center
				avrCamBlob[i].yDist	=ySize/2
									+avrCamBlob[i].y1-avrCamCenterY;// Calculate Y dist from center
				avrCamBlob[i].yDist *=-1;							// Convert so up is positive and down is negative
				avrCamBlob[i].size	=xSize*ySize;					// Calculate Size (w*h)
				if (avrCamBlob[i].size>size) {						// Keep track of which blob is the biggest
					size=avrCamBlob[i].size;
					avrCamBlobInfo.largest=i;
				}
			}			
			return avrCamBlobInfo.cnt;								// Return number of valid blobs
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

uint8_t avrCamSetCameraReg(uint8_t reg, uint8_t val) {
	// *****************************
	// Set a register in the OV6620
	// *****************************
#ifdef avrCamDebug
	rprintfInit(avrCamSTDOUTSendByte);
	rprintf("Sending command: CR %d %d\n",reg,val);
#endif

	rprintfInit(avrCamSendByte);	// Use Camera UART
	rprintf("CR %d %d",reg,val);	// Send command
	return avrCamSendCmd(NULL);		// Send null command to let that routing look for the return codes
	
}

char *avrCamVersion(void) {
	// ******************************************
	// Return the Version string from the camera
	// ******************************************
	uint8_t status;
	status=avrCamSendCmd("GV");						// Send the command
	if (status) {									// Did it work?
		int16_t len=avrCamGetLine(0,'\r',0,0);			// Get the response
		if (len>0) {									// Is it valid?
			return avrCamResponse;
		} else if (len == avrCamErrTimeout) {			// Time out
			return "Timed out getting version";
		} else {
			return "Unknown error getting version";		// Other error
		}
	} else {										// Command failed
		return "Failed to get version";
	}
}

uint8_t avrCamGetCenterColor(uint8_t index, uint8_t size) {
	// ********************************************************************
	// Set color map entry 0 based on what is in the center of my vision.
	// Dump the current frame, sample a square in the center, and record
	// the high and low values for each R,G, and B.
	// ********************************************************************
	struct _avrCamColorMap cm;					// Temp color map entry
	uint8_t	cnt=72;								// Number of lines expected
	uint8_t line;								// Line number
	uint8_t len;								// Length of line
	uint8_t centerX=avrCamCenterX/2;			// We read two pix at a time, so /2
	uint8_t centerY=avrCamCenterY/2;			// We read two pix at a time, so /2
	uint8_t pix=MIN(size/2,MIN(centerY,centerY));// Number of pixels on each side of center to sample

	if (avrCamSendCmd("DF")) {					// Send dump frame command
#ifdef avrCamDebug
			rprintfInit(avrCamSTDOUTSendByte);
#endif
		delay_ms(1000);							// Wait one second for stream to start
		while(cnt--) {
			len=avrCamGetLine(0x0B,0x0F,2,0);		// Get next line
			line=avrCamResponse[0];					// Get line number (1st byte)
#ifdef avrCamDebug
			rprintf("DF line (%d) length= %d\n",line,len);
#endif
			if(len==177) {							// Make sure it is the right length
				if (line>=(centerY-pix)	&& line<=(centerY+pix)) {	// Is it in the center box?
#ifdef avrCamDebug
					rprintf("Processing line: %d\n",line);
#endif
					for(uint8_t i=(centerX-pix);i<=(centerX+pix);i++) {	// Look at center square
						uint8_t ptr=1+i*2;									// Set pointer to grab next 2 bytes
						uint8_t x1=avrCamResponse[ptr++];					// Get first byte
						uint8_t x2=avrCamResponse[ptr];						// Get second byte
						uint8_t	g1=x1&0xF0;									// Extract green 1
						uint8_t	b=(x1&0x0F)<<4;								// Extract blue
						uint8_t	g2=x2&0xF0;									// Extract green 2
						uint8_t	r=(x2&0x0F)<<4;								// Extract red
						uint8_t gH=MAX(g1,g2);								// Find highest green
						uint8_t gL=MIN(g1,g2);								// Find lowest green
						if (line==(centerY-pix)) {	// If first pixels, set color map
							cm.rH=r;
							cm.rL=r;
							cm.gH=gH;
							cm.gL=gL;
							cm.bH=b;
							cm.bL=b;
						} else {											// If not first pixels, update color map as needed
							cm.rH=MAX(r ,cm.rH);
							cm.rL=MIN(r ,cm.rL);
							cm.gH=MAX(gH,cm.gH);
							cm.gL=MIN(gL,cm.gL);
							cm.bH=MAX(b ,cm.bH);
							cm.bL=MIN(b ,cm.bL);
						}
					}
				}
			} else {
				return 0;
			}
		}
		avrCamSetColorMap(index,cm.rL,cm.rH,cm.gL,cm.gH,cm.bL,cm.bH);			// Set color map entry 0
		delay_ms(200);														// Wait for camera to recover from frame dump
		uartFlushReceiveBuffer(avrCamUART);									// Flush receive buffer
		return avrCamSendColorMap();										// Send color map to the camera
	} else {
		return 0;
	}
}

void avrCamInit(void) {
	// *********************
	// Intialize the camera
	// *********************
	uint8_t cnt=15;	// Number of times to try to ping
#ifdef avrCamShowInitStatus
	char ver[15];
	rprintfInit(avrCamSTDOUTSendByte);
	rprintf("   AVRcam:\t%d.%d\t",avrCamVer,avrCamRev);
#endif
	// The normal settings for 115200 baud do not work with the camera's UART. There
	// is too high of an error percentage. Using double clock settings solve the problem
	if (avrCamUART==0) {
		UCSR0C=0x06;								// UART0 = 8,1,n
		UCSR0A=0x12;								// 15,200 using double clock speed
		UBRR0L=16;									// 15,200 using double clock speed
	} else if (avrCamUART==1) {
		UCSR1C=0x06;								// UART1 = 8,1,n
		UCSR1A=0x12;								// 15,200 using double clock speed
		UBRR1L=16;									// 15,200 using double clock speed
	} else if (avrCamUART==2) {	
		UCSR2C=0x06;								// UART2 = 8,1,n
		UCSR2A=0x12;								// 15,200 using double clock speed
		UBRR2L=16;									// 15,200 using double clock speed
	} else if (avrCamUART==3) {
		UCSR3C=0x06;								// UART3 = 8,1,n
		UCSR3A=0x12;								// 15,200 using double clock speed
		UBRR3L=16;									// 15,200 using double clock speed
	}
	uartFlushReceiveBuffer(avrCamUART);				// Clear out anything in the camera receive buffer
	avrCamInitColorMap();							// Set local color map to all zeros
	avrCamSetColorMap(0,180,255,180,255,180,255);	// Set up color map 0 to pick up light areas
	if (avrCamOn) {
		avrCamTrackStop();							// Turn off tracking if on
	}
	while (! avrCamPing() && cnt--);				// Wait until camera answers a ping
	if (cnt) {
		avrCamOn=1;
		delay_ms(avrCamTimeout);					// Wait for any previous pings to complete
		uartFlushReceiveBuffer(avrCamUART);			// Flush receive buffer
#ifdef avrCamShowInitStatus
		strcpy(ver,avrCamVersion());				// Get the camera version string
#endif
		avrCamSetCameraReg(18,40);					// Turn off auto white balance
		avrCamSetCameraReg(19,0);					// Turn off auto mode adjust
		avrCamSetCameraReg(45,3);					// Turn off florescent light compensation
		avrCamSendColorMap();						// Send the color map to the camera
#ifdef avrCamShowInitStatus
		rprintf("done\tCamera HW ver: ");
		rprintfStr(ver);							// Print out the camera version
		rprintf("\n");
#endif
	} else {
		avrCamOn=0;
#ifdef avrCamShowInitStatus
		rprintf("timed out\n");
#endif
	}
}
