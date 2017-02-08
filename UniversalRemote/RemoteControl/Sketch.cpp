#include <Arduino.h>


// UTFT & URTouch Library
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/

#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_Geometry.h>
#include <avr/pgmspace.h>
#include "Joystick.h"
#include <stdlib.h>
extern "C"
{
	#include "uart1.h"
}



//Defines
#define MAXIMUM_MENU_ENTRYS 50

#define WHITE 255,255,255
#define BLUE  0,0,255
#define RED   255,0,0
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
UTFT_Geometry geo(&myGLCD);
Joystick myJoystick(10);


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
	int InitNum;//current value
	int incrNum;
	IntCallBackFunc CallBack;
};

struct Entry {
	int Type;//0-->normal;1-->int number;2-->Label;3-->float number
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
bool Connected = false;
int x = 0;
int y = 0;
Entry allEntries[MAXIMUM_MENU_ENTRYS] = {0};
bool DisplayLocked=false;
bool InitSucsessfull=false;
unsigned long before=millis();
bool recOngoing=false;


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
void LockDisplay(bool State);

//functions

String NumToString(unsigned int num, int spacing)
{
	String temp;
	char tempString[spacing]="";
	String numString=String(itoa(num,tempString,10));
	for (int i =0;i<spacing-numString.length();i++)
	{
		temp+="0";
	}
	return temp+numString;
}

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

void DrawMenuEntryLabel(String Text, int MenuEntryIndex)
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

void DrawMenuEntryFloatNumber(int InitNumber, String Text, int MenuEntryIndex)
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
	myGLCD.printNumF(InitNumber/100.0, 1, 233, 19 + (MenuEntryIndex * 47), '.', 3);
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

void addMenuEntry(String Text, bool initState, BoolCallBackFunc CB)
{
  allEntries[NumberOfMenuEntries].Type = 0;
  allEntries[NumberOfMenuEntries].menuEntry.Text = Text;
  allEntries[NumberOfMenuEntries].menuEntry.State = initState;
  allEntries[NumberOfMenuEntries].menuEntry.CallBack = CB;
  NumberOfMenuEntries++;

}

void addMenuEntryLabel(String Text)
{
	allEntries[NumberOfMenuEntries].Type = 2;
	allEntries[NumberOfMenuEntries].menuEntry.Text = Text;
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

void addMenuEntryFloatNum(String Text, int initNum, int numIncr, IntCallBackFunc CB )
{
	allEntries[NumberOfMenuEntries].Type = 3;
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
      } else if (allEntries[i + ScrollPosition].Type == 2)
	  {
		DrawMenuEntryLabel(allEntries[i + ScrollPosition].menuEntry.Text, i);
	  }else if (allEntries[i + ScrollPosition].Type == 3)
	  {
		  DrawMenuEntryFloatNumber(allEntries[i + ScrollPosition].menuEntryIntNumber.InitNum, allEntries[i + ScrollPosition].menuEntryIntNumber.Text, i);
	  }
    }
  }
	myGLCD.setColor(GREEN);
	myGLCD.setBackColor(GREEN);

  //xB, yB, xE, yE
  myGLCD.fillRoundRect (175, 0, 145, 20);
  myGLCD.fillRect (175, 0, 145, 1);
  myGLCD.setColor(BLUE);
  myGLCD.drawRoundRect (300, 0, 319, 119);
  myGLCD.drawRoundRect (300, 121, 319, 239);
  myGLCD.setColor(BLACK);
  myGLCD.setFont(various_symbols);
  myGLCD.print("t", CENTER, 3);
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
			char Temp[10];
			Temp[0]='c';
			
			if(i>=10)
			{
				Temp[1]=(i/10)+'0';
				Temp[2]=(i-(i/10)*10)+'0';
			}else if(i<10)
			{
				Temp[1]='0';
				Temp[2]=i+'0';
			}
			Temp[3]=',';
			if(allEntries[i].Type==0)
			{
				Temp[4]='0';
				Temp[5]='0';
				Temp[6]=allEntries[i].menuEntry.State+'0';
			}else if(allEntries[i].Type==1)
			{
				int number=allEntries[i].menuEntryIntNumber.InitNum;
				if(number<10)
				{
					Temp[4]='0';
					Temp[5]='0';
					Temp[6]=number+'0';
				}else if(number<100)
				{
					char numTemp[5]="";
					itoa(number,numTemp,10);
					Temp[4]='0';
					Temp[5]=numTemp[0];
					Temp[6]=numTemp[1];
				}else if(number>=100)
				{
					char numTemp[5]="";
					itoa(number,numTemp,10);
					Temp[4]=numTemp[0];
					Temp[5]=numTemp[1];
					Temp[6]=numTemp[2];
				}
			}else if(allEntries[i].Type==2)
			{
				Temp[4]='0';
				Temp[5]='0';
				Temp[6]='0';
			}else if(allEntries[i].Type==3)
			{
				int number=allEntries[i].menuEntryIntNumber.InitNum;
				if(number<10)
				{
					Temp[4]='0';
					Temp[5]='0';
					Temp[6]=number+'0';
				}else if(number<100)
				{
					char numTemp[5]="";
					itoa(number,numTemp,10);
					Temp[4]='0';
					Temp[5]=numTemp[0];
					Temp[6]=numTemp[1];
				}else if(number>=100)
				{
					char numTemp[5]="";
					itoa(number,numTemp,10);
					Temp[4]=numTemp[0];
					Temp[5]=numTemp[1];
					Temp[6]=numTemp[2];
				}
				//addLog("Float");
				//addLog(String(Temp));
			}
			Temp[7]=';';
			Temp[8]='\n';
			Temp[9]='\0';
			uart1_puts(Temp);
			delay(100);
			allEntries[i].wasChanged=false;
		}
		
	}
	
	myGLCD.clrScr();
	DrawInterface();
}

void drawMsgBox()
{
	ActiveScreen = 3;
	myGLCD.clrScr();
	drawButton(20,20,300,60,"       Calibrate Joysticks",2);
	drawButton(20,70,300,110,"          Joystick Test",2);
	drawButton(20,120,300,160,"        Start Transmission",2);
	drawButton(20,170,300,210,"          Apply Changes",2);
	drawButton(1,200,100,240,"    Back",2);
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
			if(x >= 210 && x <= 229 && allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum+allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum>1)//-
			{
				waitForIt(210, 12+(47*no), 229, 36+(47*no));
				allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum-=allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum;
				DrawMenuEntryIntNumber(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum, allEntries[ScrollPosition+no].menuEntryIntNumber.Text, 0+no);
				allEntries[ScrollPosition+no].menuEntryIntNumber.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
			}else if(x >= 260 && x <= 280 && allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum+allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum<=999)//+
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
		}else if (allEntries[ScrollPosition+no].Type==3)
		{
			if(x >= 210 && x <= 229 && allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum-allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum>=0)//-
			{
				waitForIt(210, 12+(47*no), 229, 36+(47*no));
				allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum-=allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum;
				DrawMenuEntryFloatNumber(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum, allEntries[ScrollPosition+no].menuEntryIntNumber.Text, 0+no);
				allEntries[ScrollPosition+no].menuEntryIntNumber.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
			}else if(x >= 260 && x <= 280 && allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum+allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum<999)//+
			{
				waitForIt(260, 12+(47*no), 280, 36+(47*no));
				allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum+=allEntries[ScrollPosition+no].menuEntryIntNumber.incrNum;
				DrawMenuEntryFloatNumber(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum, allEntries[ScrollPosition+no].menuEntryIntNumber.Text, 0+no);
				allEntries[ScrollPosition+no].menuEntryIntNumber.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
			}else if(x >= 230 && x <= 259)//num
			{
				waitForIt(230, 12+(47*no), 259, 36+(47*no));
				myGLCD.clrScr();
				allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum=drawKeyboardScreen(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
				DrawMenuEntryFloatNumber(allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum, allEntries[ScrollPosition+no].menuEntryIntNumber.Text, 0+no);
				allEntries[ScrollPosition+no].menuEntryIntNumber.CallBack(ScrollPosition+no, allEntries[ScrollPosition+no].menuEntryIntNumber.InitNum);
			}
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

void drawCalibrateInterface()
{
	myGLCD.clrScr();
	ActiveScreen=6;
	drawButton(1,200,100,240,"    Back",2);
	myGLCD.setColor(WHITE);
	myGLCD.setBackColor(BLACK);
	myGLCD.setFont(SmallFont);
	myGLCD.print("Joystick Calibration",CENTER,1);
	drawButton(120,200,319,240,"    Clear all Values",2);
	
	
	myGLCD.setColor(BLACK);
	myGLCD.fillCircle(70,120,60);
	myGLCD.fillCircle(250,120,60);
	myGLCD.setColor(GREEN);
	//L
	myGLCD.drawCircle(70,120,60);
	myGLCD.drawLine(70,120,70,120);
	myGLCD.fillCircle(70,120,5);
	//R
	myGLCD.drawCircle(250,120,60);
	myGLCD.drawLine(250,120,250,120);
	myGLCD.fillCircle(250,120,5);
	//myGLCD.setColor(BLUE);
	//myGLCD.fillRect(10,60,130,180);//left
	//myGLCD.fillRect(190,60,310,180);//right
}

void drawTestInterface()
{
	myGLCD.clrScr();
	ActiveScreen=7;
	drawButton(1,200,100,240,"    Back",2);
	myGLCD.setColor(WHITE);
	myGLCD.setBackColor(BLACK);
	myGLCD.setFont(SmallFont);
	myGLCD.print("Joystick Test",CENTER,1);
	myGLCD.fillCircle(70,120,60);
	
	myGLCD.setColor(BLACK);
	myGLCD.fillCircle(70,120,60);
	myGLCD.fillCircle(250,120,60);
	myGLCD.setColor(GREEN);
	//L
	myGLCD.drawCircle(70,120,60);
	myGLCD.drawLine(70,120,70,120);
	myGLCD.fillCircle(70,120,5);
	//R
	myGLCD.drawCircle(250,120,60);
	myGLCD.drawLine(250,120,250,120);
	myGLCD.fillCircle(250,120,5);
	
	
	
	//myGLCD.setColor(BLUE);
	//myGLCD.fillRect(10,60,130,180);//left
	//myGLCD.fillRect(190,60,310,180);//right
}

void TouchEventManagement()
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
				
			} else if (x >= 130 && x <= 180 && y >= 0 && y <= 40) //Connect Button
			{
				waitForIt(175, 0, 145, 20);
				drawMsgBox();
				
				//--------------------------------------------------------------------------------------------------------------------------menu entry touch management
			} else if (y >= 0 && y <= 46) {
				EntryTouchCalc(0);                                                                                                                       //menu Entry No. 1
				//redrawing connection button

				myGLCD.setColor(GREEN);
				myGLCD.setBackColor(GREEN);
				myGLCD.fillRoundRect (175, 0, 145, 20);
				myGLCD.setColor(BLACK);
				myGLCD.setFont(various_symbols);
				myGLCD.print("t", CENTER, 3);
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
			if (x >= 1 && x <= 100 && y >= 200 && y <= 240) //Back Button
			{
				waitForIt(1,200,100,240);
				myGLCD.clrScr();
				DrawInterface();
			}else if(x >= 20 && x <= 300 && y >= 20 && y <= 60)//calibrate Joysticks button
			{
				waitForIt(20,20,300,60);
				drawCalibrateInterface();
			}else if(x >= 20 && x <= 300 && y >= 70 && y <= 110)//Joystick Test button
			{
				waitForIt(20,70,300,110);
				drawTestInterface();
			}else if(x >= 20 && x <= 300 && y >= 120 && y <= 160)//Start Transmission button
			{
				waitForIt(20,120,300,160);
				applyChanges();
				LockDisplay(0);
			}else if(x >= 20 && x <= 300 && y >= 170 && y <= 210)//Start Transmission button
			{
				waitForIt(20,170,300,210);
				applyChanges();
			}
		} else if (ActiveScreen == 4) //Keyboard
		{
			myGLCD.clrScr();
			DrawInterface();
		}else if(ActiveScreen==5)//Lockscreen
		{
			int duration=2000;
			double before=millis();
			myGLCD.setColor(120,120,120);
			int state=0;
			while (myTouch.dataAvailable())
			{
				myTouch.read();
				if(before+(duration/8)<=millis() && state==0)//1/8
				{
					state=1;
					geo.fillTriangle(160,0,320,0,160,120);
				}
				else if(before+(duration/4)<=millis() && state==1)//1/4
				{
					state=2;
					geo.fillTriangle(160,120,319,1,319,120);
				}else if(before+((duration*3)/8)<=millis() && state==2)//3/8
				{
					state=3;
					geo.fillTriangle(160,120,319,239,319,120);
				}else if(before+(duration/2)<=millis() && state==3)//1/2
				{
					state=4;
					geo.fillTriangle(160,120,319,239,160,239);
				}else if(before+((duration*5)/8)<=millis() && state==4)//5/8
				{
					state=5;
					geo.fillTriangle(160,120,1,239,160,239);
				}else if(before+((duration*3)/4)<=millis() && state==5)//3/4
				{
					state=6;
					geo.fillTriangle(160,120,1,239,1,120);
				}else if(before+((duration*7)/8)<=millis() && state==6)//7/8
				{
					state=7;
					geo.fillTriangle(160,120,1,1,1,120);
				}else if(before+duration<=millis() && state==7)//1
				{
					state=8;
					geo.fillTriangle(160,120,1,1,160,1);
				}
			}
			if(before+duration<=millis())
				LockDisplay(1);
			else if(state!=0)
			{
				myGLCD.clrScr();
				#ifdef USE_GRAPHICS
				myGLCD.drawBitmap (110, 54, 50,44, caution_sign,2);
				#endif
				myGLCD.setColor(WHITE);
				myGLCD.setBackColor(BLACK);
				myGLCD.setFont(BigFont);
				myGLCD.print("Screen is Locked!", CENTER, 150);
			}
		}else if(ActiveScreen==6)//Joystick Calibration
		{
			if (x >= 1 && x <= 100 && y >= 200 && y <= 240) //Back Button
			{
				waitForIt(1,200,100,240);
				myJoystick.setValuesToEEprom();
				myGLCD.clrScr();
				drawMsgBox();
			}else if (x >= 120 && x <= 319 && y >= 200 && y <= 240) //Back Button
			{
				waitForIt(120,200,319,240);
				myJoystick.Calibrate(true);
				addLog("Cleared calibration values");
			}
		}else if(ActiveScreen==7)//Joystick Test
		{
			if (x >= 1 && x <= 100 && y >= 200 && y <= 240) //Back Button
			{
				waitForIt(1,200,100,240);
				myGLCD.clrScr();
				drawMsgBox();
			}
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
		ActiveScreen = 5;
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

//Transmission Function:
void recMessFunc1(char c)
{	
	recOngoing=true;
	char static Buffer[100]={0};
	int static index=0;
	Buffer[index++]=c;
	if(Buffer[index-1]==';')
	{
		//control Char			arguments		end Char
		if(Buffer[0]=='l')
		//l-->Log message		message			;
		{
			Buffer[index-1]='\0';
			addLog(String(&Buffer[1]));//TODO: check if working
		}else if(Buffer[0]=='I')
		//I-->Int menu entry	Label,initValue	;
		{
			String label = String(Buffer);
			int dividerIndex=label.indexOf(',');
			String MenuEntryText=label.substring(1,dividerIndex);
			int InitNum = label.substring(dividerIndex+1,label.indexOf(';')).toInt();
			addMenuEntryIntNum(MenuEntryText,InitNum,1,IntCallBack);
			addLog("Added INT menu entry: "+MenuEntryText+" Init="+InitNum);
		}else if(Buffer[0]=='B')
		//B-->Bool menu entry	Label,initValue	;
		{
			String test = String(Buffer);
			int dividerIndex=test.indexOf(',');
			String MenuEntryText=test.substring(1,dividerIndex);
			int InitNum = test.substring(dividerIndex+1,test.indexOf(';')).toInt();
			addMenuEntry(MenuEntryText,InitNum, BoolCallBack);
			addLog("Added BOOL menu entry: "+MenuEntryText+" Init="+InitNum);
		}else if(Buffer[0]=='L')
		//L-->Label menu entry	Label			;
		{
			Buffer[index-1]='\0';
			String label = String(&Buffer[1]);
			addMenuEntryLabel(label);
			addLog("Added LABEL menu entry: "+label);
		}else if(Buffer[0]=='F')
		//F-->Float menu entry	Label,initValue	;
		{
			Buffer[index-1]='\0';
			String test = String(Buffer);
			int dividerIndex=test.indexOf(',');
			String MenuEntryText=test.substring(1,dividerIndex);
			int InitNum = test.substring(dividerIndex+1,test.indexOf(';')).toInt();
			addMenuEntryFloatNum(MenuEntryText,InitNum,10,IntCallBack);
			addLog("Added FLOAT menu entry: "+MenuEntryText+" Init="+InitNum);
		}else if(Buffer[0]=='d')
		//d--> refresh log screen if visible
		{
			if(ActiveScreen==2)
				drawLog();
		}else if(Buffer[0]=='D')
		//D--> init done --> refresh dynamic interfaces if visible
		{
			if(ActiveScreen==1)
				DrawInterface();
			if(ActiveScreen==2)
				drawLog();
		}else if(Buffer[0]=='r')
		//r--> reset all menu entrys
		{
			addLog("Reseted all menu entrys");
			NumberOfMenuEntries=0;
			for (int i=0;i<MAXIMUM_MENU_ENTRYS;i++)
			{
				allEntries[i].Type=NULL;
				allEntries[i].wasChanged=NULL;
				allEntries[i].menuEntry.State=NULL;
				allEntries[i].menuEntry.Text="";
				allEntries[i].menuEntry.CallBack=NULL;
				allEntries[i].menuEntryIntNumber.incrNum=NULL;
				allEntries[i].menuEntryIntNumber.CallBack=NULL;
				allEntries[i].menuEntryIntNumber.InitNum=NULL;
				allEntries[i].menuEntryIntNumber.Text="";
			}
		}
		index=0;
		recOngoing=false;
	}
}

void setup()
{
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_HI);
	// Setup the LCD
	myGLCD.InitLCD();
	
	myGLCD.setFont(BigFont);

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
	myGLCD.print("(C)2016-2017 Florian Laschober", CENTER, 227);
	addLog("LOG:");	
	uart1_init_x(57600,1,1,0,1);
	uart1_registerCallBack(recMessFunc1);
}

void drawJoystickTest()
{
	static RandL values,oldValues;
	values=myJoystick.Calulate();
	
	if(values.L.x!=oldValues.L.x||values.L.y!=oldValues.L.y)
	{
		myGLCD.setBackColor(BLACK);
		myGLCD.setColor(BLACK);
		myGLCD.fillRect(40,30,100,80);
		myGLCD.setColor(WHITE);
		myGLCD.print("X: ",40,30);
		myGLCD.printNumI(values.L.x,60,30);
		myGLCD.print("Y: ",40,40);
		myGLCD.printNumI(values.L.y,60,40);
		
		myGLCD.setColor(BLACK);
		myGLCD.fillCircle(70,120,60);
		myGLCD.setColor(GREEN);
		myGLCD.drawCircle(70,120,60);
		myGLCD.drawLine(70,120,70+values.L.x,120-values.L.y);
		myGLCD.fillCircle(70+values.L.x,120-values.L.y,5);
		oldValues=values;
	}if(values.R.x!=oldValues.R.x||values.R.y!=oldValues.R.y)
	{		
		myGLCD.setBackColor(BLACK);
		myGLCD.setColor(BLACK);
		myGLCD.fillRect(40,30,100,80);
		myGLCD.setColor(WHITE);
		myGLCD.print("X: ",200,30);
		myGLCD.printNumI(values.L.x,220,30);
		myGLCD.print("Y: ",200,40);
		myGLCD.printNumI(values.L.y,220,40);
		
		myGLCD.setColor(BLACK);
		myGLCD.fillCircle(250,120,60);
		myGLCD.setColor(GREEN);
		myGLCD.drawCircle(250,120,60);
		myGLCD.drawLine(250,120,250+0/*values.R.x*/,120-values.R.y);
		myGLCD.fillCircle(250+0/*values.R.x*/,120-values.R.y,5);
		oldValues=values;
	}
}


void loop()
{
	if(recOngoing==false)
	{
  		TouchEventManagement();
	  
		if(ActiveScreen==6)//calibration
		{
			myJoystick.Calibrate(false);
			drawJoystickTest();
		}else if(ActiveScreen==7)//test
		{
			drawJoystickTest();
		}
	}
	  
	  
	if(DisplayLocked==true)
	{
		RandL joystick;
		if(before+200<millis())
		{
			joystick=myJoystick.Calulate();
			
			int temp=0;
			temp=(int)joystick.L.x+100;
			if(temp>200)
				temp=200;
			if(temp<0)
				temp=0;
			//String transText;
			//transText= "L" + NumToString(temp,3)+",";
			
			uart1_putc('L');
			int h,m,l;
			h=temp/100;
			m=temp/10-h*10;
			l=temp-(h*100+m*10);
			uart1_putc(h+'0');
			uart1_putc(m+'0');
			uart1_putc(l+'0');
			
			uart1_putc(',');
			
			temp=(int)joystick.L.y+100;
			if(temp>200)
				temp=200;
			if(temp<0)
				temp=0;
			/*transText+= NumToString(temp,3)+";\n";
			char text[transText.length()];
			transText.toCharArray(text,transText.length());
			addLog(text);
			uart1_puts(text);*/

			h=temp/100;
			m=temp/10-h*10;
			l=temp-(h*100+m*10);

			uart1_putc(h+'0');
			uart1_putc(m+'0');
			uart1_putc(l+'0');
			uart1_putc(';');
			uart1_putc('\n');
			before=millis();
		}
		
	}
}