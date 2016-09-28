#include <Arduino.h>


// UTFT & URTouch Library
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/

#include <UTFT.h>
#include <URTouch.h>
#include <avr/pgmspace.h>
extern "C"
{
	#include "esp8266.h"
}

//#include "esp8266.c"



//Defines
#define MAXIMUM_MENU_ENTRYS 50

#define WHITE 255,255,255
#define BLUE  255,0,0
#define RED   0,0,255
#define GREEN 0,255,0
#define GRAY  64,64,64
#define BLACK 0,0,0


//uncomment for testing, because upload is faster
//#define USE_GRAPHICS


// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t arial_bold[];
extern uint8_t various_symbols[];
extern uint8_t SevenSegNumFont[];

// Declare Icons
#ifdef USE_GRAPHICS
	extern unsigned int caution_sign[0x898];
	extern unsigned int BB8[0x3908];
#endif



//constructors
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);
URTouch  myTouch( 6, 5, 4, 3, 2);


//Type definitions

typedef void (*BoolCallBackFunc)(int8_t ID, bool state);
typedef void (*IntCallBackFunc)(int8_t ID, int Number);

//structures
struct MenuEntry {
	String Text;
	bool State;
	BoolCallBackFunc CallBack;
};

struct MenuEntryIntNumber {
	String Text;
	int InitNum;
	int incrNum;
	IntCallBackFunc CallBack;
};

struct Entry {
	int Type;//0-->normal;1-->int number;2-->float number
	bool wasChanged;
	MenuEntry menuEntry;
	MenuEntryIntNumber menuEntryIntNumber;
};

//variable definitions
String Log[15];
uint8_t LogLength=0;
int NumberOfMenuEntries = 0;
int ScrollPosition = 0;
int ActiveScreen = 0; //0-->splash Screen;1-->main Screen;2-->log screen;3-->msg box
bool Connected = true;
int x = 0;
int y = 0;
Entry allEntries[MAXIMUM_MENU_ENTRYS] = {0};
bool DisplayLocked=false;
bool InitSucsessfull=false;
	


//prototypes
void printLog();
void DrawMenuEntry(char Text, bool state, int MenuEntryIndex);
void DrawMenuEntryIntNumber(int InitNumber, char Text, int MenuEntryIndex);
void addMenuEntry(char Text[],bool initState, BoolCallBackFunc CB);
void addMenuEntryIntNum(char Text[], int initNum, int numIncr, IntCallBackFunc CB );
void DrawInterface();
void drawLog();
void drawMsgBox();
void drawButton(int x1, int y1, int x2, int y2, char Text, bool font);
bool check(int x, int y, int x1, int y1, int x2, int y2);
int drawKeyboardScreen(int Number);
void waitForIt(int x1, int y1, int x2, int y2);
void addLog(String Text);

//functions

void addLog(String Text)
{
	if(LogLength<15)
	{
		Log[LogLength]=Text;
		LogLength++;
		}else{
		for (uint8_t i = 1; i < 15; i++)
		{
			Log[i-1]=Log[i];
		}
		Log[14]=Text;
	}
}

void printLog()
{
	myGLCD.setColor(WHITE);
	myGLCD.setFont(SmallFont);
	myGLCD.setBackColor(BLACK);
	for (uint8_t i = 0; i < LogLength; i++)
	{
		if(Log[i].length()<=40)
			myGLCD.print(Log[i], LEFT, 16*(i));
		else
			myGLCD.print(Log[i].substring(0,37) + "...", LEFT, 16*(i));
	}
}

void DrawMenuEntry(String Text, bool state, int MenuEntryIndex)
{
  myGLCD.setColor(BLUE);
  myGLCD.drawLine (0, 1 + (MenuEntryIndex * 47), 289, 1 + (MenuEntryIndex * 47));
  myGLCD.setFont(arial_bold);
  myGLCD.setColor(GRAY);
  myGLCD.fillRect(0, 2 + (MenuEntryIndex * 47), 289, 47 + (MenuEntryIndex * 47));
  myGLCD.setColor(WHITE);
  myGLCD.setBackColor(GRAY);
  myGLCD.setFont(arial_bold);
  myGLCD.print(Text, LEFT, 17 + (MenuEntryIndex * 47));
  myGLCD.setColor(BLUE);
  myGLCD.drawLine (0, 47 + (MenuEntryIndex * 47), 289, 47 + (MenuEntryIndex * 47));
  myGLCD.drawRect(255, 34 + (MenuEntryIndex * 47),275, 14 + (MenuEntryIndex * 47));
  
  if(state)
    myGLCD.setColor(BLUE);
  else
    myGLCD.setColor(GRAY);
  myGLCD.fillRect(258, 31 + (MenuEntryIndex * 47),272, 17 + (MenuEntryIndex * 47));
}

void DrawMenuEntryIntNumber(int InitNumber, String Text, int MenuEntryIndex)
{
  myGLCD.setColor(BLUE);
  myGLCD.drawLine (0, 1 + (MenuEntryIndex * 47), 289, 1 + (MenuEntryIndex * 47));
  myGLCD.setFont(arial_bold);
  myGLCD.setColor(GRAY);
  myGLCD.fillRect(0, 2 + (MenuEntryIndex * 47), 289, 47 + (MenuEntryIndex * 47));
  myGLCD.setColor(WHITE);
  myGLCD.setBackColor(GRAY);
  myGLCD.setFont(arial_bold);
  myGLCD.print(Text, LEFT, 17 + (MenuEntryIndex * 47));
  myGLCD.setColor(BLUE);
  myGLCD.drawLine (0, 47 + (MenuEntryIndex * 47), 289, 47 + (MenuEntryIndex * 47));

  myGLCD.setColor(BLUE);
  myGLCD.fillRoundRect(260, 12 + (MenuEntryIndex * 47), 280, 36 + (MenuEntryIndex * 47));
  myGLCD.setColor(BLACK);
  myGLCD.fillRoundRect(261, 13 + (MenuEntryIndex * 47), 279, 35 + (MenuEntryIndex * 47));
  myGLCD.setColor(WHITE);
  myGLCD.setBackColor(BLACK);
  myGLCD.setFont(arial_bold);
  myGLCD.print("+", 262, 15 + (MenuEntryIndex * 47));
  myGLCD.setColor(BLUE);
  myGLCD.fillRoundRect(230, 12 + (MenuEntryIndex * 47), 259, 36 + (MenuEntryIndex * 47));
  myGLCD.setColor(BLACK);
  myGLCD.fillRoundRect(231, 13 + (MenuEntryIndex * 47), 258, 35 + (MenuEntryIndex * 47));
  myGLCD.setColor(WHITE);
  myGLCD.setBackColor(BLACK);
  myGLCD.setFont(SmallFont);
  myGLCD.printNumI(InitNumber, 233, 19 + (MenuEntryIndex * 47), 3);
  //myGLCD.print("12", 233, 19+(MenuEntryIndex*47));
  myGLCD.setColor(BLUE);
  myGLCD.fillRoundRect(210, 12 + (MenuEntryIndex * 47), 229, 36 + (MenuEntryIndex * 47));
  myGLCD.setColor(BLACK);
  myGLCD.fillRoundRect(211, 13 + (MenuEntryIndex * 47), 228, 35 + (MenuEntryIndex * 47));
  myGLCD.setColor(WHITE);
  myGLCD.setBackColor(BLACK);
  myGLCD.setFont(arial_bold);
  myGLCD.print("-", 212, 15 + (MenuEntryIndex * 47));
}

void addMenuEntry(String Text,bool initState, BoolCallBackFunc CB)
{
  allEntries[NumberOfMenuEntries].Type = 0;
  allEntries[NumberOfMenuEntries].menuEntry.Text = Text;
  allEntries[NumberOfMenuEntries].menuEntry.State = initState;
  allEntries[NumberOfMenuEntries].menuEntry.CallBack = CB;
  NumberOfMenuEntries++;

}

void addMenuEntryIntNum(String Text, int initNum, int numIncr, IntCallBackFunc CB )
{
  allEntries[NumberOfMenuEntries].Type = 1;
  allEntries[NumberOfMenuEntries].menuEntryIntNumber.Text = Text;
  allEntries[NumberOfMenuEntries].menuEntryIntNumber.InitNum = initNum;
  allEntries[NumberOfMenuEntries].menuEntryIntNumber.incrNum = numIncr;
  allEntries[NumberOfMenuEntries].menuEntryIntNumber.CallBack = CB;
  NumberOfMenuEntries++;
}

void DrawInterface()
{
  ActiveScreen = 1;
  //myGLCD.clrScr();
  for (int i = 0; i < NumberOfMenuEntries; i++)
  {
    if (i < 5)
    {
      if (allEntries[i + ScrollPosition].Type == 0)
      {
        DrawMenuEntry(allEntries[i + ScrollPosition].menuEntry.Text,allEntries[i + ScrollPosition].menuEntry.State, i);
      } else if (allEntries[i + ScrollPosition].Type == 1)
      {
        DrawMenuEntryIntNumber(allEntries[i + ScrollPosition].menuEntryIntNumber.InitNum, allEntries[i + ScrollPosition].menuEntryIntNumber.Text, i);
      }
    }
  }
  if (ESP8266_isConnected())
    myGLCD.setColor(GREEN);
  else
    myGLCD.setColor(RED);
  //xB, yB, xE, yE
  myGLCD.fillRoundRect (175, 0, 145, 20);
  myGLCD.fillRect (175, 0, 145, 1);
  myGLCD.setColor(BLUE);
  myGLCD.drawRoundRect (300, 0, 319, 119);
  myGLCD.drawRoundRect (300, 121, 319, 239);
  myGLCD.setColor(WHITE);
  myGLCD.setBackColor(BLACK);
  myGLCD.setFont(various_symbols);
  myGLCD.print("f", 303, 60);
  myGLCD.print("g", 303, 180);


	
	myGLCD.setColor(BLUE);
	myGLCD.drawRoundRect (138, 210, 179, 242);
	myGLCD.setColor(GRAY);
	myGLCD.fillRoundRect (139, 211, 178, 241);
	myGLCD.setColor(WHITE);
	myGLCD.setFont(SmallFont);
	myGLCD.setBackColor(GRAY);
	myGLCD.print("LOG", CENTER, 224);
	
  myGLCD.setColor(BLUE);
  myGLCD.drawRect(290, 0, 298, 240);
  myGLCD.setColor(BLACK);
  myGLCD.fillRect(291, 1, 297, 239);
  if (NumberOfMenuEntries > 5)
  {
    myGLCD.setColor(200, 200, 200);
    int Length = (238 / NumberOfMenuEntries) * 5;
    int temp = (((int)(238 / NumberOfMenuEntries)) * ScrollPosition);
    myGLCD.fillRoundRect(291, 1 + ((238 / NumberOfMenuEntries)*ScrollPosition), 297, 1 + Length + ((238 / NumberOfMenuEntries)*ScrollPosition));
  }

}

void drawLog()
{
  ActiveScreen = 2;
  myGLCD.clrScr();
  printLog();
  myGLCD.setColor(BLUE);
  myGLCD.drawRoundRect (280, 210, 319, 242);
  myGLCD.drawRect (290, 210, 319, 242);
  myGLCD.setColor(GRAY);
  myGLCD.fillRoundRect (281, 211, 319, 241);
  myGLCD.fillRect(300, 211, 319, 241);
  myGLCD.setColor(WHITE);
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(GRAY);
  myGLCD.print("BACK", RIGHT, 224);
}

void drawButton(int x1, int y1, int x2, int y2, char Text[], int8_t font)
{

	myGLCD.setColor(BLACK);
	myGLCD.fillRoundRect(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	myGLCD.setColor(WHITE);
	myGLCD.setBackColor(BLACK);
	if (font==1)
	{
		myGLCD.setFont(SevenSegNumFont);
		myGLCD.print(Text, x1 + 22, (y2 - y1) / 2 + y1 - 24);
	} else if(font==0){
		myGLCD.setFont(BigFont);
		myGLCD.print(Text, x1 + 5, (y2 - y1) / 2 + y1 - 8);
	}else if(font == 2){
		myGLCD.setFont(SmallFont);
		myGLCD.print(Text, x1+7, (y2 - y1) / 2 + y1 - 5);
	}
	myGLCD.setColor(BLUE);
	myGLCD.drawRoundRect(x1, y1, x2, y2);

}

void applyChanges()
{
	for (int i=0;i<NumberOfMenuEntries;i++)
	{
		if(allEntries[i].wasChanged==true)
		{
			String temp="Applied changes for Entry ";
			addLog(temp + i);
			//TODO: transmit changes via esp library to robot
			allEntries[i].wasChanged=false;
		}
	}
}


void drawMsgBox()
{
	ActiveScreen = 3;
	
	if (ESP8266_isConnected())
		myGLCD.setColor(GREEN);
	else
		myGLCD.setColor(RED);
	//xB, yB, xE, yE
	myGLCD.fillRoundRect (175, 0, 145, 20);
	myGLCD.fillRect (175, 0, 145, 1);
	
	myGLCD.setColor(BLUE);
	myGLCD.drawRoundRect (30, 50, 280, 200);
	myGLCD.setColor(BLACK);
	myGLCD.fillRoundRect (31, 51, 279, 199);
	
	
	myGLCD.setColor(WHITE);
	myGLCD.setBackColor(BLACK);
	myGLCD.setFont(SmallFont);
	myGLCD.print("Status: ", 50, 70);
	myGLCD.print("Connection details: ", 50, 90);
	if(ESP8266_isConnected())
	{
		myGLCD.setColor(GREEN);
		myGLCD.print("1 Active connection", 50, 110);
	}else{
		myGLCD.setColor(RED);
		myGLCD.print("No active connection", 50, 110);
	}
	
	
	if(InitSucsessfull==true)
	{
		drawButton(122,170,188,190," Apply",2);
		myGLCD.setColor(GREEN);
		myGLCD.print("Server running", 120, 70);
	}else{
		drawButton(122,170,188,190," Start",2);
		myGLCD.setColor(RED);
		myGLCD.print("Server stopped", 120, 70);
	}
	
}

int drawKeyboardScreen(int Number)
{
  ActiveScreen = 4;
  myGLCD.setColor(GREEN);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(BLACK);
  myGLCD.printNumI(Number, 0, 0);
  drawButton(0, 17, 75, 67, "1", 1);
  drawButton(80, 17, 155, 67, "2", 1);
  drawButton(160, 17, 235, 67, "3", 1);
  drawButton(240, 17, 315, 67, "4", 1);

  drawButton(0, 72, 75, 122, "5", 1);
  drawButton(80, 72, 155, 122, "6", 1);
  drawButton(160, 72, 235, 122, "7", 1);
  drawButton(240, 72, 315, 122, "8", 1);

  drawButton(0, 127, 75, 177, "DEL", 0);
  drawButton(80, 127, 155, 177, "0", 1);
  drawButton(160, 127, 235, 177, "9", 1);
  drawButton(240, 127, 315, 177, "  ,", 0);

  drawButton(0, 182, 155, 232, "   ESC", 0);
  drawButton(160, 182, 319, 232, "   OK", 0);

  myGLCD.setColor(BLUE);
  while (1)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();

      if (check(x, y, 0, 17, 75, 67)) //1
      {
        waitForIt(0, 17, 75, 67);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(0, 17, 75, 67);
        if (Number < 100)
          Number = Number * 10 + 1;
      } else if (check(x, y, 80, 17, 155, 67)) //2
      {
        waitForIt(80, 17, 155, 67);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(80, 17, 155, 67);
        if (Number < 100)
          Number = Number * 10 + 2;
      } else if (check(x, y, 160, 17, 235, 67)) //3
      {
        waitForIt(160, 17, 235, 67);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(160, 17, 235, 67);
        if (Number < 100)
          Number = Number * 10 + 3;
      } else if (check(x, y, 240, 17, 315, 67)) //4
      {
        waitForIt(240, 17, 315, 67);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(240, 17, 315, 67);
        if (Number < 100)
          Number = Number * 10 + 4;
      } else if (check(x, y, 0, 72, 75, 122)) //5
      {
        waitForIt(0, 72, 75, 122);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(0, 72, 75, 122);
        if (Number < 100)
          Number = Number * 10 + 5;
      } else if (check(x, y, 80, 72, 155, 122)) //6
      {
        waitForIt(80, 72, 155, 122);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(80, 72, 155, 122);
        if (Number < 100)
          Number = Number * 10 + 6;
      } else if (check(x, y, 160, 72, 235, 122)) //7
      {
        waitForIt(160, 72, 235, 122);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(160, 72, 235, 122);
        if (Number < 100)
          Number = Number * 10 + 7;
      } else if (check(x, y, 240, 72, 315, 122)) //8
      {
        waitForIt(240, 72, 315, 122);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(240, 72, 315, 122);
        if (Number < 100)
          Number = Number * 10 + 8;
      } else if (check(x, y, 160, 127, 235, 177)) //9
      {
        waitForIt(160, 127, 235, 177);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(160, 127, 235, 177);
        if (Number < 100)
          Number = Number * 10 + 9;
      } else if (check(x, y, 80, 127, 155, 177)) //0
      {
        waitForIt(80, 127, 155, 177);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(80, 127, 155, 177);
        if (Number < 100)
          Number = Number * 10;
      } else if (check(x, y, 240, 127, 315, 177)) //,
      {
        waitForIt(240, 127, 315, 177);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(240, 127, 315, 177);
      } else if (check(x, y, 0, 127, 75, 177)) //DEL
      {
        waitForIt(0, 127, 75, 177);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(0, 127, 75, 177);
        if (Number >= 1)
          Number = Number / 10;
      } else if (check(x, y, 0, 182, 155, 232)) //ESC
      {
        waitForIt(0, 182, 155, 232);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(0, 182, 155, 232);
        break;
      } else if (check(x, y, 160, 182, 319, 232)) //OK
      {
        waitForIt(160, 182, 319, 232);
        myGLCD.setColor(BLUE);
        myGLCD.drawRoundRect(160, 182, 319, 232);
        break;
      }
      myGLCD.setColor(BLACK);
      myGLCD.fillRect(0, 0, 60, 16);
      myGLCD.setColor(GREEN);
      myGLCD.setFont(BigFont);
      myGLCD.setBackColor(BLACK);
      myGLCD.printNumI(Number, 0, 0);
    }

  }
  myGLCD.clrScr();
  DrawInterface();
  return Number;
}

bool check(int x, int y, int x1, int y1, int x2, int y2)
{
	return x >= x1 && y >= y1 && x <= x2 && y <= y2;
}

void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(RED);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(WHITE);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

void EntryTouchCalc(uint8_t no)
{
	if(NumberOfMenuEntries>no)
	{
		if (allEntries[ScrollPosition+no].Type==1)
		{
			if(x >= 210 && x <= 229)//-
			{
				waitForIt(210, 12+(47*no), 229, 36+(47*no));
				allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum-=allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum;
				DrawMenuEntryIntNumber(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum, allEntries[ScrollPosition+no].menuEntryIntNumber.Text, 0+no);
				allEntries[ScrollPosition+no].menuEntryIntNumber.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
			}else if(x >= 260 && x <= 280)//+
			{
				waitForIt(260, 12+(47*no), 280, 36+(47*no));
				allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum+=allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum;
				DrawMenuEntryIntNumber(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum, allEntries[ScrollPosition+no].menuEntryIntNumber.Text, 0+no);
				allEntries[ScrollPosition+no].menuEntryIntNumber.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
			}else if(x >= 230 && x <= 259)//num
			{
				waitForIt(230, 12+(47*no), 259, 36+(47*no));
				myGLCD.clrScr();
				allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum=drawKeyboardScreen(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
				DrawMenuEntryIntNumber(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum, allEntries[ScrollPosition+no].menuEntryIntNumber.Text, 0+no);
				allEntries[ScrollPosition+no].menuEntryIntNumber.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
			}
		}else if(allEntries[ScrollPosition+no].Type==0)
		{
			waitForIt(255, 34+(47*no), 275, 14+(47*no));
			allEntries[ScrollPosition+no].menuEntry.State= !allEntries[ScrollPosition+no].menuEntry.State;
			DrawMenuEntry(allEntries[ScrollPosition+no].menuEntry.Text,allEntries[ScrollPosition+no].menuEntry.State, 0+no);
			allEntries[ScrollPosition+no].menuEntry.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntry.State);
		}
	}
}

void IntToString_(uint16_t number,char String[])
{
	uint8_t zt, t, h, z, e, help;
	zt=number/10000;
	t=number/1000-zt*10;
	h=number/100-(t*10+zt*100);
	help = number%100;
	z= help/10;
	e = help%10;
	String[0]=h+'0';
	String[1]=z+'0';
	String[2]=e+'0';
	String[3]='\0';
}

void BoolCallBack(int8_t ID, bool state)
{
	char temp[26]="Entry ";
	temp[6]=ID+'0';
	temp[7]='\0';
	if(state)
		strcat(temp," changed to: true");
	else
		strcat(temp," changed to: false");
	addLog(temp);
	allEntries[ID].wasChanged=true;
}

void IntCallBack(int8_t ID, int Number)
{
	char temp[26]="Entry ";
	char temp2[4];
	temp[6]=ID+'0';
	temp[7]='\0';
	strcat(temp," changed to: ");
	IntToString_(Number,temp2);
	strcat(temp,temp2);
	addLog(temp);
	allEntries[ID].wasChanged=true;
	
}

void TouchEventManagement()
{
	if (myTouch.dataAvailable() && DisplayLocked!=true)
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();
		if (ActiveScreen == 0)
		{
			ActiveScreen = 1;
			myGLCD.clrScr();
			DrawInterface();

		} else if (ActiveScreen == 1) //main screen
		{
			if (x >= 290 && x <= 320 && y >= 0 && y <= 119) //down button
			{
				waitForIt(300, 0, 319, 119);
				if(ScrollPosition > 0)
				{
					ScrollPosition--;
					DrawInterface();
				}else{
					myGLCD.setColor(BLUE);
					myGLCD.drawRoundRect (300, 121, 319, 239);
				}
				/*if (ScrollPosition > 4)
				{
					ScrollPosition-=4;
					DrawInterface();
				}else if(ScrollPosition!=0){
					ScrollPosition=0;
					DrawInterface();
				}else{*/
					myGLCD.setColor(BLUE);
					myGLCD.drawRoundRect (300, 0, 319, 119);
				//}
			} else if (x >= 290 && x <= 320 && y >= 121 && y <= 240) //up button
			{
				waitForIt(300, 121, 319, 239);
				if(ScrollPosition + 5 <  NumberOfMenuEntries)
				{
					ScrollPosition++;
					DrawInterface();
				}else{
					myGLCD.setColor(BLUE);
					myGLCD.drawRoundRect (300, 121, 319, 239);
				}
				/*if (ScrollPosition + 6 < NumberOfMenuEntries)
				{
					ScrollPosition += 4;
					DrawInterface();
				}else if(ScrollPosition != NumberOfMenuEntries-5){
					ScrollPosition = NumberOfMenuEntries-5;
					DrawInterface();
				}else{
					myGLCD.setColor(BLUE);
					myGLCD.drawRoundRect (300, 121, 319, 239);
				}*/
			} else if (x >= 110 && x <= 190 && y >= 200 && y <= 242) //log button
			{
				waitForIt(138, 210, 179, 242);
				drawLog();
				//myGLCD.drawRoundRect (145, 210, 200, 242);
				//myGLCD.drawRoundRect (90, 210, 145, 242);
				
			} else if (x >= 145 && x <= 175 && y >= 0 && y <= 20) //Connect Button
			{
				waitForIt(175, 0, 145, 20);
				drawMsgBox();
				
				//--------------------------------------------------------------------------------------------------------------------------menu entry touch management
				} else if (y >= 0 && y <= 46) {
					EntryTouchCalc(0);                                                                                                                       //menu Entry No. 1
					//redrawing connection button
					if (ESP8266_isConnected())
						myGLCD.setColor(GREEN);
					else
						myGLCD.setColor(RED);
					myGLCD.fillRoundRect (175, 0, 145, 20);
				} else if (y >= 45 && y <= 91) {                                                                                                                        //menu Entry No. 2
					EntryTouchCalc(1);
				} else if (y >= 90 && y <= 136) {                                                                                                                        //menu Entry No. 3
					EntryTouchCalc(2);
				} else if (y >= 135 && y <= 181) {																														//menu Entry No. 4
					EntryTouchCalc(3);
				} else if (y >= 180 && y <= 226) {                                                                                                                        //menu Entry No. 5
					EntryTouchCalc(4);
				//redrawing LOG button
				myGLCD.setColor(BLUE);
				myGLCD.drawRoundRect (138, 210, 179, 242);
				myGLCD.setColor(GRAY);
				myGLCD.fillRoundRect (139, 211, 178, 241);
				myGLCD.setColor(WHITE);
				myGLCD.setFont(SmallFont);
				myGLCD.setBackColor(GRAY);
				myGLCD.print("LOG", CENTER, 224);
			}
		} else if (ActiveScreen == 2) //log Screen
		{
			if (x >= 280 && x <= 319 && y >= 210 && y <= 242) //back button
			{
				waitForIt(280, 210, 319, 242);
				myGLCD.clrScr();
				DrawInterface();
			}
			} else if (ActiveScreen == 3) { //Msg Box
			{
				if (x >= 145 && x <= 175 && y >= 0 && y <= 20) //Connect Button
				{
					waitForIt(175, 0, 145, 20);
					if(NumberOfMenuEntries<5)
						myGLCD.clrScr();
					DrawInterface();
				}else if(x >= 122 && x <= 188 && y >= 170 && y <= 190)//apply or connect button
				{
					//drawButton(122,170,188,190," Apply",2);
					waitForIt(122, 170, 188, 190);
					if(InitSucsessfull==false)
					{
						//InitSucsessfull=ERROR_handeling(1,ESP8266_init(1,"server"));
						drawMsgBox();
					}else{
						applyChanges();
						DrawInterface();
					}
					
				}
			}
		} else if (ActiveScreen == 4) //Keyboard
		{
			myGLCD.clrScr();
			DrawInterface();
		}
	}
}

void LockDisplay(bool State)//1--> Unlocked
{
	if(State)
	{
		DisplayLocked=false;
		ActiveScreen = 1;
		myGLCD.clrScr();
		DrawInterface();
	}else{
		DisplayLocked=true;
		myGLCD.clrScr();
		#ifdef USE_GRAPHICS
			myGLCD.drawBitmap (110, 54, 50,44, caution_sign,2);
		#endif
		myGLCD.setColor(WHITE);
		myGLCD.setBackColor(BLACK);
		myGLCD.setFont(BigFont);
		myGLCD.print("Screen is Locked!", CENTER, 150);
	}
}

bool timeoutDetectionRunning=false;
uint8_t Int_count=0;
bool waitingForPong=false;

void EventFunc(int8_t ID,bool event)//1 connected; 0 disconnected
{
	if(event==1)
	{
		if(ID!=-1)
		{
			timeoutDetectionRunning=true;
			String temp="Connection established with ";
			addLog(temp + ID);
		}else{
			addLog("Weird Error");
		}
	}else{
		if(ID!=-1)
		{
			timeoutDetectionRunning=false;
			String temp="Connection closed with ";
			addLog(temp + ID);
		}else{
			addLog("Weird Error");
		}
	}
	
	//just grapical stuff
	if(ActiveScreen==2)
		drawLog();
	else if(ActiveScreen==1)
	{
		if (ESP8266_isConnected())
			myGLCD.setColor(GREEN);
		else
			myGLCD.setColor(RED);
		myGLCD.fillRoundRect (175, 0, 145, 20);
	}else if(ActiveScreen==3)
	{
		drawMsgBox();
	}
}

void recMessFunc1(char Message[])
{	
	uint8_t ID=0;
	if(ID!=-1)
	{
		addLog("M");
		if(Message[0]=='A')
		{
			addLog("A");
			waitingForPong=false;
			Int_count=0;
		}else{
		
			String Message1=Message;
			if(Message1.charAt(0)==1)
			{
				addLog("recived control data");
				//addLog(Message);
				//control Char;		type 1 --> int entry;		init value max 3 digits ended with ';';		label ending with ';'
				//control Char;		type 0 --> switch entry;	init value 0 or 1;							label ending with ';'
				if(Message1.charAt(1)=='1')
				{
					//addLog(Message);
					uint16_t num;
					num=((Message1.charAt(2)-'0')*100)+((Message1.charAt(3)-'0')*10)+(Message1.charAt(4)-'0');
					addMenuEntryIntNum(Message1.substring(5),num,1,IntCallBack);
					addLog("added int control entry");
					if(ActiveScreen==1)
						DrawInterface();
				}else if(Message1.charAt(1)=='0')
				{
					addMenuEntry(Message1.substring(3),Message1.charAt(2)-'0',BoolCallBack);
					addLog("added bool entry");
					if(ActiveScreen==1)
						DrawInterface();

				}else{
					addLog("Tried to add unknown menu type");
				}
			}else{
				String temp="Message from ";
				addLog(temp + ID + ": " + Message);
			}
			
		}
	}else{
		addLog("Weird Error");
	}
	
	if(ActiveScreen==2)
		drawLog();
}

void setup()
{
	//randomSeed(analogRead(0));
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);
	// Setup the LCD
	myGLCD.InitLCD();
	myGLCD.setFont(BigFont);


	int buf[318];
	int x, x2;
	int y, y2;
	int r;


	// Clear the screen and draw the frame
	myGLCD.clrScr();
	myGLCD.setColor(WHITE);
	myGLCD.setBackColor(WHITE);
	myGLCD.fillRect(0,0,319,239);
	
	#ifdef USE_GRAPHICS
		myGLCD.drawBitmap (110, 47, 100,146, BB8);
	#endif
	
	
	myGLCD.setColor(BLACK);
	myGLCD.setFont(SmallFont);
	myGLCD.print("Universal Remote", CENTER, 0);
	myGLCD.print("(c)2016 Florian Laschober", CENTER, 227);
	addLog("LOG:");
	
	String test="PT1";
	addMenuEntryIntNum(test, 0, 1,IntCallBack);			//control Char; type; init value max 3 digits ended with ';'; label ending with ';'
	//Serial.begin(9600);
	
	ESP8266_registerMessageCallback(&recMessFunc1);
	
	InitSucsessfull=0;
	ESP8266_Init();
	
	
	TCCR1A &= ~((1<<WGM10) | (1<<WGM11));
	TCCR1B &= ~((1<<WGM13) | (1<<CS12));
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS11);
	TIMSK1 |= (1<<OCIE1A);

	OCR1A = 3124;//12.4ms
	
	sei();
}

void loop()
{
	
	
	/*static bool a=false;
	if(Serial.available())
	{
		Serial.read();
		a=!a;
		LockDisplay(a);
	}*/
  	TouchEventManagement();
}


ISR(TIMER1_COMPA_vect)
{
	
	//ESP8266_sendMessage(0,"")
	if(timeoutDetectionRunning==true)
	{
		Int_count++;
		if(Int_count>40)
		{
			if(waitingForPong==false)
			{
				ESP8266_sendMessage("p");//sending ping
				addLog("P");
				waitingForPong=true;
				Int_count=0;
			}else{
				if(Int_count>45)
				{
					//connection errored
					addLog("T");
					Int_count=0;
				}
			}
		}
	}
}