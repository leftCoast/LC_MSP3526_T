#include <MSP3526_T.h>
#include <ST7796_t3.h>
#include <LC_SPI.h>
#include <mask.h>

// **********************************************************
//								maskableMSP3526_T
//	ST7796_t3 with the ability to have a mask added.	
// **********************************************************


maskableMSP3526_T::maskableMSP3526_T(int cs, int rst)
	: ST7796_t3(cs,LC_DC,rst) { }

	
maskableMSP3526_T::~maskableMSP3526_T(void) {  }
	
	
void maskableMSP3526_T::drawPixel(int16_t x, int16_t y, uint16_t color) {

	if (gMask) {
		if (gMask->checkPixel(x,y)) {
			ST7796_t3::drawPixel(x,y,color);
		}
	} else {
		ST7796_t3::drawPixel(x,y,color);
	}
}


void maskableMSP3526_T::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

	int	lastX;
	int	lastY;
	
	if (gMask) {
		switch(gMask->checkRect(x,y,w,h)) {
			case unMasked			: ST7796_t3::fillRect(x,y,w,h,color); break;
			case totalMasked		: return;
			case partialMasked	:
				lastX = x + w;
				lastY = y + h;
				for(int i=y;i<lastY;i++) {
					for(int j=x;j<=lastX;j++) {
						drawPixel(j,i,color);
					}
				}
			break;
		}
	} else {
		ST7796_t3::fillRect(x,y,w,h,color);
	}
}

// **********************************************************
//								MSP3526_T	
// **********************************************************



MSP3526_T::MSP3526_T(byte inCS,byte inRst)
  :displayObj(true,true,true,true,false) {

	cTS			= NULL;
	theTFT		= NULL;
	cs				= inCS;
	rst			= inRst;
	lastTouch	= false;
	touchNum		= 0;
}


MSP3526_T::~MSP3526_T(void) {

	if (theTFT!=NULL) {
		delete theTFT;
		theTFT = NULL;
	}
	if (cTS!=NULL) {
		delete cTS;
		cTS = NULL;
	}
}


bool MSP3526_T::begin(void) { 

	theTFT = new maskableMSP3526_T(cs,rst);	// Have a go at allocating the display object.
	if (theTFT != NULL) {						// If we got one?
		cTS = new Adafruit_FT6206();			// Have a go at allocating a touch object.
		if (cTS != NULL) {						// We got both now?
			theTFT->init(320, 480);				// Do the init() like the example.
   		theTFT->invertDisplay(true);		// This LCD requires colors to be inverted.
			if (cTS->begin(40)) {				// 40 comes from example code.
				return true;						// If everything checks out? We exit with a success.
			}											//
		}												//
	}													// At this point something went wrong. Recycle everything.
	if (theTFT!=NULL) {							// Have a TFT thing?
		delete theTFT;								// Delete it.
		theTFT = NULL;								// Note it.
	}													// 
	if (cTS!=NULL) {								// Have a touch object?
		delete cTS;									// Delete it.
		cTS = NULL;									// Note it.
	}													//
	return false;									// Return false, we failed.
}


int MSP3526_T::width(void)																							{ return theTFT->width(); }
int MSP3526_T::height(void)																							{ return theTFT->height(); }
void MSP3526_T::setRotation(byte inRotation)																	{ theTFT->setRotation((uint8_t)inRotation); }
void MSP3526_T::setTextColor(colorObj* inColor)																{ theTFT->setTextColor(inColor->getColor16()); }
void MSP3526_T::setTextColor(colorObj* tColor,colorObj* bColor) 										{ theTFT->setTextColor(tColor->getColor16(),bColor->getColor16()); }
void MSP3526_T::setTextSize(byte inSize)																		{ theTFT->setTextSize(inSize); }
void MSP3526_T::setTextWrap(boolean wrap)																		{ theTFT->setTextWrap(wrap); }
void MSP3526_T::setFont(const GFXfont* font)																	{ theTFT->setFont(font); }
void MSP3526_T::setCursor(int x,int y)																			{ theTFT->setCursor(gX(x),gY(y)); }
void MSP3526_T::drawText(const char* inText)																	{ theTFT->print(inText); }
void MSP3526_T::fillScreen(colorObj* inColor)																	{ theTFT->fillScreen(inColor->getColor16()); }
void MSP3526_T::fillRect(int x,int y,int width,int height,colorObj* inColor)						{ theTFT->fillRect(gX(x),gY(y),width, height,inColor->getColor16()); }
void MSP3526_T::drawRect(int x,int y,int width,int height,colorObj* inColor)						{ theTFT->drawRect(gX(x),gY(y),width, height,inColor->getColor16()); }
void MSP3526_T::fillRect(rect* inRect,colorObj* inColor)													{ theTFT->fillRect(gX(inRect->x),gY(inRect->y),inRect->width, inRect->height,inColor->getColor16()); }
void MSP3526_T::drawRect(rect* inRect,colorObj* inColor)													{ theTFT->drawRect(gX(inRect->x),gY(inRect->y),inRect->width, inRect->height,inColor->getColor16()); }
void MSP3526_T::fillRoundRect(int x,int y,int width,int height,int radius,colorObj* inColor)	{ theTFT->fillRoundRect(gX(x),gY(y),width,height,radius,inColor->getColor16()); }
void MSP3526_T::drawRoundRect(int x,int y,int width,int height,int radius,colorObj* inColor)	{ theTFT->drawRoundRect(gX(x),gY(y),width,height,radius,inColor->getColor16()); }
void MSP3526_T::drawCircle(int x,int y,int diam, colorObj* inColor)									{ drawRoundRect(gX(x),gY(y),diam,diam,diam/2,inColor); }
void MSP3526_T::drawCircleHelper(int x,int y,int rad,byte corner,colorObj* inColor)				{ theTFT->drawCircleHelper(gX(x),gY(y),rad,corner,inColor->getColor16()); }
void MSP3526_T::fillCircle(int x,int y,int diam, colorObj* inColor)									{ fillRoundRect(gX(x),gY(y),diam,diam,diam/2,inColor); }
void MSP3526_T::drawTriangle(point* pt0,point* pt1,point* pt2,colorObj* inColor) 					{ theTFT->drawTriangle(gP(pt0).x,gP(pt0).y,gP(pt1).x,gP(pt1).y,gP(pt2).x,gP(pt2).y,inColor->getColor16()); }
void MSP3526_T::fillTriangle(point* pt0,point* pt1,point* pt2,colorObj* inColor)					{ theTFT->fillTriangle(gP(pt0).x,gP(pt0).y,gP(pt1).x,gP(pt1).y,gP(pt2).x,gP(pt2).y,inColor->getColor16()); }
void MSP3526_T::drawVLine(int x,int y,int height,colorObj* inColor)									{ theTFT->drawFastVLine(gX(x),gY(y),height,inColor->getColor16()); }
void MSP3526_T::drawHLine(int x,int y,int width,colorObj* inColor)										{ theTFT->drawFastHLine(gX(x),gY(y),width,inColor->getColor16()); }
void MSP3526_T::drawLine(int x,int y,int x2,int y2,colorObj* inColor)									{ theTFT->drawLine(gX(x),gY(y),gX(x2),gY(y2),inColor->getColor16()); }
void MSP3526_T::drawPixel(int x,int y,colorObj* inColor)													{ theTFT->drawPixel(gX(x),gY(y),inColor->getColor16()); }


point MSP3526_T::getPoint(void) {

	TS_Point	adaPoint;
	point		lcPoint;

	adaPoint = cTS->getPoint();								// Grab a point from the hardware.
	if (!adaPoint.z) {											// If the z value is zero..
		return lastTouchPt;										// Its a bogus touch! Return the last point.
	}																	//
	switch (theTFT->getRotation()) {							// For each type of rotation..
		case PORTRAIT:												// PORTRAIT On this screen it's wires down.
			lcPoint.x = adaPoint.x;								// Load the x value into our lcPoint.		
			lcPoint.y = adaPoint.y;								// Load the y value into our lcPoint.
		break;														// And we waltz off.
		case INV_LANDSCAPE:										// INV_LANDSCAPE..
			lcPoint.x = adaPoint.y;								// x gets old y.
			lcPoint.y = theTFT->height() - adaPoint.x;	// Load the new y flipped around the y axis.
		break;														// Time to go. Better test this later, looks odd to me.
		case INV_PORTRAIT:										// INV_PORTRAIT ON this screen, wires up.
			lcPoint.x = theTFT->width() - adaPoint.x;		// Flip around the x axis.
			lcPoint.y = theTFT->height() - adaPoint.y;	// Flip around the y axis.
		break;														// And go!
		case LANDSCAPE:											// LANDSCAPE Another one I need to recheck. Its got some weird stuff in there too!
			lcPoint.x = theTFT->width() - adaPoint.y;		// Flip around the new x axis.
			lcPoint.y = adaPoint.x;								// y gets old x.
		break;														// Pack your bags and move on!
	}																	//
	lastTouchPt = lcPoint;										// Save it for later. (We cover bogus values with this.) I say "we" like anyone else is ever here..
	return lcPoint;												// And return our result!
}


bool MSP3526_T::touched(void) { return cTS->touched(); }

