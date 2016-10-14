/*
 * TFT_Interface.cpp
 *
 * Created: 08.09.2016 14:38:30
 *  Author: Florian
 */ 
#include <Arduino.h>


// UTFT & URTouch Library
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/

#include <UTFT.h>
#include <URTouch.h>
extern "C"
{
	#include "esp8266.h"
}

//#include "esp8266.c"



//Defines
#define MAXIMUM_MENU_ENTRYS 50

#define WHITE 255,255,255
#define BLUE  0,0,255
#define RED   255,0,0
#define GREEN 0,255,0
#define GRAY  64,64,64
#define BLACK 0,0,0


// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t arial_bold[];
extern uint8_t various_symbols[];
extern uint8_t SevenSegNumFont[];


//constructors
UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);
URTouch  myTouch( 6, 5, 4, 3, 2);

//Type definitions

typedef void (*BoolCallBackFunc)(int8_t ID, bool state);
typedef void (*IntCallBackFunc)(int8_t ID, int Number);

//structures
struct MenuEntry {
	char* Text;
	bool State;
	BoolCallBackFunc CallBack;
};

struct MenuEntryIntNumber {
	char* Text;
	int InitNum;
	int incrNum;
	IntCallBackFunc CallBack;
};

struct Entry {
	int Type;//0-->normal;1-->int number;2-->float number
	MenuEntry menuEntry;
	MenuEntryIntNumber menuEntryIntNumber;
};

//variable definitions
String Log[15];
uint8_t LogLength=0;
int NumberOfMenuEntries = 0;
int ScrollPosition = 0;
int ActiveScreen = 0; //0-->splash Screen;1-->main Screen;2-->log screen
bool Connected = true;
int x = 0;
int y = 0;
Entry allEntries[MAXIMUM_MENU_ENTRYS] = {0};
	


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

void DrawMenuEntry(char Text[], bool state, int MenuEntryIndex)
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

void DrawMenuEntryIntNumber(int InitNumber, char Text[], int MenuEntryIndex)
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

void addMenuEntry(char Text[],bool initState, BoolCallBackFunc CB)
{
  allEntries[NumberOfMenuEntries].Type = 0;
  allEntries[NumberOfMenuEntries].menuEntry.Text = Text;
  allEntries[NumberOfMenuEntries].menuEntry.State = initState;
  allEntries[NumberOfMenuEntries].menuEntry.CallBack = CB;
  NumberOfMenuEntries++;

}

void addMenuEntryIntNum(char Text[], int initNum, int numIncr, IntCallBackFunc CB )
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

void drawMsgBox()
{
  ActiveScreen = 3;

  myGLCD.setColor(BLUE);
  myGLCD.drawRoundRect (30, 50, 280, 200);
  myGLCD.setColor(BLACK);
  myGLCD.fillRoundRect (31, 51, 279, 199);
  if (Connected)
    myGLCD.setColor(GREEN);
  else
    myGLCD.setColor(RED);
  //xB, yB, xE, yE
  myGLCD.fillRoundRect (175, 0, 145, 20);
  myGLCD.fillRect (175, 0, 145, 1);
  myGLCD.setColor(WHITE);
  myGLCD.setFont(arial_bold);
  myGLCD.setBackColor(BLACK);
  myGLCD.print("Connect to Robot", CENTER, 104);
}

void drawButton(int x1, int y1, int x2, int y2, char Text[], bool font)
{

  myGLCD.setColor(BLACK);
  myGLCD.fillRoundRect(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
  myGLCD.setColor(WHITE);
  myGLCD.setBackColor(BLACK);
  if (font)
  {
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.print(Text, x1 + 22, (y2 - y1) / 2 + y1 - 24);
  } else {
    myGLCD.setFont(BigFont);
    myGLCD.print(Text, x1 + 5, (y2 - y1) / 2 + y1 - 8);
  }
  myGLCD.setColor(BLUE);
  myGLCD.drawRoundRect(x1, y1, x2, y2);

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
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

void EntryTouchCalc(uint8_t no)
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
}



void setup()
{
	randomSeed(analogRead(0));
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
	myGLCD.print("Universal Remote", CENTER, 124);
	myGLCD.setFont(SmallFont);
	myGLCD.setColor(255, 255, 0);
	myGLCD.print("(c)2016 Florian Laschober", CENTER, 227);
	addLog("LOG:");
	/*addLog("testMgs15");
	addLog("testMgs14");
	addLog("testMgs13");
	addLog("testMgs12");
	addLog("testMgs11");
	addLog("testMgs10");
	addLog("testMgs9");
	addLog("testMgs8");
	addLog("testMgs7");
	addLog("testMgs6");
	addLog("testMgs5");
	addLog("testMgs4");
	addLog("testMgs3");
	addLog("testMgs2");
	addLog("testMgs1");
	addLog("This Text is too long for being displayed all in one row, so the print function will add three '.' at the end");
	addLog("testMgs-1");
	addLog("testMgs-2");
	addLog("testMgs-3");*/
	//Serial.begin(9600);
}

void loop()
{
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY();
    if (ActiveScreen == 0)
    {
      ActiveScreen = 1;
      myGLCD.clrScr();
      addMenuEntryIntNum("PT1", 0, 1,IntCallBack);
      addMenuEntryIntNum("PT2", 20, 1,IntCallBack);
      addMenuEntry("Entry 1",1,BoolCallBack);
      addMenuEntry("Option 2",0,BoolCallBack);
      addMenuEntryIntNum("PD", 564, 2,IntCallBack);
      addMenuEntryIntNum("PID", 25, 3,IntCallBack);
      addMenuEntry("Option 3",0,BoolCallBack);
      addMenuEntry("Option 4",1,BoolCallBack);
	  addMenuEntry("Option 5",1,BoolCallBack);
	  addMenuEntry("Option 6",1,BoolCallBack);
      DrawInterface();

    } else if (ActiveScreen == 1) //main screen
    {
      if (x >= 290 && x <= 320 && y >= 0 && y <= 119) //down button
      {
        waitForIt(300, 0, 319, 119);
        if (ScrollPosition > 4)
		{
			ScrollPosition-=4;
			DrawInterface();
		}else if(ScrollPosition!=0){
			ScrollPosition=0;
			DrawInterface();
		}else{
			  myGLCD.setColor(BLUE);
			  myGLCD.drawRoundRect (300, 0, 319, 119);
		}
      } else if (x >= 290 && x <= 320 && y >= 121 && y <= 240) //up button
      {
        waitForIt(300, 121, 319, 239);
        if (ScrollPosition + 6 < NumberOfMenuEntries)
		{
          ScrollPosition += 4;
		  DrawInterface();
		}else if(ScrollPosition != NumberOfMenuEntries-5){
			ScrollPosition = NumberOfMenuEntries-5;
			DrawInterface();
		}else{
			 myGLCD.setColor(BLUE);
			 myGLCD.drawRoundRect (300, 121, 319, 239);
		}
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
        //myGLCD.clrScr();
        DrawInterface();
      }
    }
  } else if (ActiveScreen == 4) //Keyboard
  {
    myGLCD.clrScr();
    DrawInterface();
  }
}
}

