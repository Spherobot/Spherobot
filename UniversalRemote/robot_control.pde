import processing.serial.*;

Serial myPort;  // Create object from Serial class

int before=millis();
int deadZone=10;
void setup() 
{
  size(1000,1000); //make our canvas 200 x 200 pixels big
  println(Serial.list()); 
  //String portName = Serial.list()[4]; //change the 0 to a 1 or 2 etc. to match your port
  
  myPort = new Serial(this, "COM9", 57600);
  
  line(0,500-deadZone*5,1000,500-deadZone*5);
  line(0,500+deadZone*5,1000,500+deadZone*5);
  line(500-deadZone*5,0,500-deadZone*5,1000);
  line(500+deadZone*5,0,500+deadZone*5,1000);
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
    x=map(mouseX,0,1000,-100,100);
    y=map(mouseY,0,1000,100,-100);
    
    if(y<=deadZone&&y>=deadZone*-1)
      y=0;
    if(x<=deadZone&&x>=deadZone*-1)
      x=0;
    
    //caluclate polar coordinate
    extend=sqrt(y*y+x*x);
    if(y>=0)
      angle=acos(x/extend);
    else if(y<0)
      angle=(2*PI-acos(x/extend));
      
    if(extend==0)
      angle=0;
    else
      angle=(angle * 4068) / 71;
    
  }else{
    x=0;
    y=0;
    extend=0;
    angle=0;
  }
  if(mousePressed==true)
  {
    fill(127);
    ellipse(mouseX,mouseY,48,48);
    angle+=270;
    if(angle>=360)
      angle-=360;
    if(extend>100)
      extend=100;
    textSize(32);
    if(extend>100)
    {
      extend=100;
    }
    String Data=round(angle)+","+round(extend);
    text(Data, 10, 30); 
    fill(0, 102, 153);
    if(before+200<millis())
    {
      myPort.write(Data+"L;");
      before=millis();
    }
    //transmit data
  }else{
    ellipse(500,500,48,48);
    textSize(32);
    text("0,0", 10, 30); 
    fill(0, 102, 153);
    if(before+200<millis())
    {
      myPort.write("0,0L;");
      before=millis();
    }
    //transmit stop
  }
}