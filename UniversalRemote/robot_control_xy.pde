import processing.serial.*;
import controlP5.*;
 
ControlP5 cp5;

Serial myPort;  // Create object from Serial class
controlP5.Button b;
controlP5.Textfield t;
controlP5.Textfield t1;
int before=millis();
int deadZone=10;
int Mode=0;


void setup() 
{
  PFont pfont = createFont("Arial",20,true); // use true/false for smooth/no-smooth
  ControlFont font = new ControlFont(pfont,16);
  smooth();
  size(1500,1000); //make our canvas 1000 x 1000 pixels big
  println(Serial.list()); 
  //String portName = Serial.list()[4]; //change the 0 to a 1 or 2 etc. to match your port
  
  myPort = new Serial(this, "COM6", 57600);
  
  line(0,500-deadZone*5,1000,500-deadZone*5);
  line(0,500+deadZone*5,1000,500+deadZone*5);
  line(500-deadZone*5,0,500-deadZone*5,1000);
  line(500+deadZone*5,0,500+deadZone*5,1000);

  cp5 = new ControlP5(this);
  t=cp5.addTextfield("Index").setPosition(1020, 100).setSize(40, 40).setAutoClear(false).setFont(font);
  t1=cp5.addTextfield("Value").setPosition(1080, 100).setSize(150, 40).setAutoClear(false).setFont(font);
  b=cp5.addButton("Submit").setPosition(1240, 100).setSize(80, 40);
  b.getCaptionLabel().setFont(font);
  t.getCaptionLabel().setColor(127).setFont(font);
  t1.getCaptionLabel().setColor(127).setFont(font);
}

void draw() {
  background(255);
  line(0,500-deadZone*5,1000,500-deadZone*5);
  line(0,500+deadZone*5,1000,500+deadZone*5);
  line(500-deadZone*5,0,500-deadZone*5,1000);
  line(500+deadZone*5,0,500+deadZone*5,1000);
  float x,y;
  float extend=0,angle=0;
  
  if(mouseX!=0&&mouseY!=0)
  {
    int mouseX1=mouseX;
    int mouseY1=mouseY;
    if(mouseX1>1000)
      mouseX1=1000;
    if(mouseX1<0)
      mouseX1=0;
    if(mouseY1>1000)
      mouseY1=1000;
    if(mouseY1<0)
      mouseY1=0;
    x=map(mouseX,0,1000,-100,100);
    y=map(mouseY,0,1000,100,-100);
    
    if(y<=deadZone&&y>=deadZone*-1)
      y=0;
    if(x<=deadZone&&x>=deadZone*-1)
      x=0;
    
  }else{
    x=0;
    y=0;
  }
  if(Mode==0)
  {
    if(mousePressed==true)
    {
      fill(127);
      ellipse(mouseX,mouseY,48,48);
      textSize(32);
      if(x > 100)
        x=100;
      if(y > 100)
        y=100;
      if(x < -100)
        x=-100;
      if(y < -100)
        y=-100;
      String s_X,s_Y;
      s_X=""+(round(x)+100);
      s_Y=""+(round(y)+100);
      if(s_X.length()==1)
        s_X="00"+s_X;
      else if(s_X.length()==2)
        s_X="0"+s_X;
        
      if(s_Y.length()==1)
        s_Y="00"+s_Y;
      else if(s_Y.length()==2)
        s_Y="0"+s_Y;
      String Data=s_X+","+s_Y;
      text(Data, 10, 30); 
      fill(0, 102, 153);
      if(before+200<millis())
      {
        myPort.write("L"+Data+"L;\n");
        before=millis();
      }
      //transmit data
    }else{
      ellipse(500,500,48,48);
      textSize(32);
      text("100,100", 10, 30); 
      fill(0, 102, 153);
      if(before+150<millis())
      {
        myPort.write("L100,100;\n");
        before=millis();
      }
      //transmit stop
    }
  }
  text("Mode:"+Mode, 10, 80); 
}

void keyPressed() {
  if(key=='m'||key=='M')
  {
    Mode^=1;
    myPort.write("L100,100;\n");
  }
}

void Submit() {
  String command="c";
  String temp;
  temp=cp5.get(Textfield.class,"Index").getText();
  if(temp.length()==2)
    command+=temp;
  else if(temp.length()==1)
    command+="0"+temp;
  command+=",";
  temp=cp5.get(Textfield.class,"Value").getText();
  if(temp.length()==3)
    command+=temp;
  else if(temp.length()==2)
    command+="0"+temp;
  else if(temp.length()==1)
    command+="00"+temp;
  command+=";\n";
  myPort.write(command);
  print(command);
}