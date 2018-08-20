/*
the file is in order to define the structs.
*/

#include <iostream>
using namespace std;
#ifndef _STRUCT_DEFINE_
#define _STRUCT_DEFINE_
#define MYDELETE(p) {if(NULL!=p)delete p;p=NULL;}
#define M_STOP 4
typedef struct _FILT
{
	FLOAT fu;    //lower border frequency of filter
	FLOAT fo;    //upper border frequency of filter
	FLOAT fs;     //sample rate;
	FLOAT type;   //filter type 2 - CHEBYSHEV  1 - BUTTERWORTH
	FLOAT order;   //filter order.
}FILT;

typedef struct
{
	FLOAT version;
	char* info;
	
}Device_info;
enum ConnetState{CONNECTED=1,DISCONNECTED,CONNECTING};


#define MYCOLOR_SNOW            RGB(255,250,250)
#define MYCOLOR_GAINSBORO       RGB(220,220,220)
#define MYCOLOR_OLDLACE         RGB(253,245,230)
#define MYCOLOR_BLACK           RGB(0,0,0)
#define MYCOLOR_DARKSLAKEGRAY   RGB(49,79,79)
#define MYCOLOR_CYAN1           RGB(0,255,255)
#define MYCOLOR_CYAN2           RGB(0,238,238)
#define MYCOLOR_CYAN3           RGB(121,205,205)
#define MYCOLOR_DIMGRAY         RGB(105,105,105)
#define MYCOLOR_DARKSEAGREEN    RGB(105,139,105)
#define MYCOLOR_MIDNIGHTBLUE    RGB(25,25,112)
#define MYCOLOR_DARKSLATEBLUE   RGB(72,61,139)
#define MYCOLOR_STEELBLUE       RGB(70,130,180)
#define MYCOLOR_KHAKI           RGB(139,134,78)
#define MYCOLOR_DARKOLIVERGREEN RGB(85,107,47)
#define MYCOLOR_LIGGHTGOLDEN    RGB(205,190,112)
#define MYCOLOR_GOLD            RGB(238,201,0)
#define MYCOLOR_GOLD2           RGB(139,117,0)
#define MYCOLOR_YELLOW          RGB(255,255,0)
#define MYCOLOR_DARKGOLDEN      RGB(205,149,12)
#define MYCOLOR_ROSYBROWN       RGB(188,143,143)
#define MYCOLOR_INDIANRED       RGB(205,92,92)
#define MYCOLOR_SIENNA          RGB(205,104,57)
#define MYCOLOR_FIREBIRCK       RGB(178,34,34)
#define MYCOLOR_BURYWOOD        RGB(139,115,85)
#define MYCOLOR_RED             RGB(255,0,0)
#define MYCOLOR_DEEPPINK        RGB(255,20,147)
#define MYCOLOR_DARKORCHID      RGB(153,50,204)
#define MYCOLOR_LAWGREEN		RGB(134,252,0)
#define MYCOLOR_DARKGREEN		RGB(0,128,0)
#define MYCOLOR_WHITE			RGB(255,255,255)
#define MYCOLOR_GREEN			RGB(0,255,0)
#define MYCOLOR_LIGHTGREY		RGB(208,208,208)

// MODES
#define M_NORMAL		0
#define	M_IMPEDANCE		1
#define	M_CALIBRATE		2
#define M_COUNTER		3

#endif