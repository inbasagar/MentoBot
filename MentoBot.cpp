#include <Key.h>
#include <Keypad.h>

// motor pin config
//motor 1 correspoonds to Y axis - column wise traversal
//motor 2 corresponds to X axis - row wise traversal
#define mot1a 2
#define mot1b 3
#define mot2a 4
#define mot2b 5
#define motspeed 0.94
#define en 20


// Keypad config
//defining no of rows and columns
const byte ROWS = 4; 
const byte COLS = 3;
// defining an array for mapping each key with
char keys[ROWS][COLS] = {
  {'3','2','1'},
  {'6','5','4'},
  {'9','8','7'},
  {'#','0','*'}
};
//defining controling pins
byte rowPins[ROWS] = {10,9,8,7};
byte colPins[COLS] = {13,12,11};
//Defining the keypad object itself
Keypad keypad = Keypad( makeKeymap(keys),rowPins, colPins, ROWS, COLS);

// frame size in x and y axes
float boxX = 1, boxY = 1;
//position of the end effector/attachment
float posX = 0, posY = 0;
//motor motion direction state variable, true -> left to right, false -> right to left
bool mstate = true;
//state variable to indicate if the attachment reached end of the row
bool endReached = false;
//increment distance/ distance between the plants
float inc = 0.2;
//state variable to denote what operation is done
bool sow = false; // sowing operation
bool spray = false; // spraying water or insecticide
bool enable = false; //specifies whether an operation is enabled
bool iptake = false; //specifies whether the keypad is taking input
bool bx = false;
bool configOK = false; // specifies whether the configurations are successfully done

char key = ' ';

void setup() {
  
  // motor controller config
  pinMode(mot1a,OUTPUT); //Motor 1
  pinMode(mot1b,OUTPUT); //Motor 1
  pinMode(mot2a,OUTPUT); //Motor 2
  pinMode(mot2b,OUTPUT); //Motor 2
  pinMode(en,OUTPUT); //motor speed percentage control

  // setting up serial monitor
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(en,10);
  key=keypad.getKey();

  
  if(key!=NO_KEY){  //Looking for input from keypad
    if(key=='#'){
      Serial.println("into if key equal #");
      iptake = !iptake;
      Serial.println("Input mode changed");
      bx = !bx;
      if(iptake){
      boxX=0;
      boxY=0;
      }
    }else if(iptake){
      getDim();
    }
    }
  
  if((key=='1' || key=='2') && !iptake){
    // printing pressed value for debug purpose
    enable = true;
    Serial.println(key);
    

    // setting the operation state based on the key
    if(key=='1'){
      sow = true;
      Serial.println("Sowing Selected");
    }else{
      spray = true;
      Serial.println("Spraying Selected");
    }
    Serial.println("End coordinates:");
    Serial.println("X Y");

  }

    //Running both the motors
    if(enable && !iptake){
      while(posX<boxX-inc){
        displaceRight();
      }
      if(posY < boxY - inc){
        displaceTop();
        while(posX > inc){
          displaceLeft();
        }
        displaceTop();
      }else{
        restorePos();
        enable = false;
      }
    }
    
}

void getDim(){
    if(key=='*'){
      Serial.println("variable changed");
      bx = !bx;
    }else{
      int intForm  = 0;
      switch(key){
        case '0':
          intForm = 0;
          break;
        case '1':
          intForm = 1;
          break;
        case '2':
          intForm = 2;
          break;
        case '3':
          intForm = 3;
          break;
        case '4':
          intForm = 4;
          break;
        case '5':
          intForm = 5;
          break;
        case '6':
          intForm = 6;
          break;
        case '7':
          intForm = 7;
          break;
        case '8':
          intForm = 8;
          break;
        case '9':
          intForm = 9;
          break;
      }
      if(bx){
        Serial.println("changing x: ");
        boxX = boxX*10 + intForm;
        Serial.println(boxX);
        Serial.print(" ");
        Serial.println("meters");
      }else{
        Serial.println("changing y: ");
        boxY = boxY*10 + intForm;
        Serial.print(boxY);
        Serial.print(" ");
        Serial.println("meters");
      }
    }
}

void displaceRight(){
    digitalWrite(mot2a,1);  //motor 2 on
    digitalWrite(mot2b,0);
    delay((inc/motspeed)*1000);//motor 2 spins upto 20cm
    digitalWrite(mot2a,0);  //motor 2 off
    if(sow){
      sowing();
    }else{
      spraying();
    }
    posX += inc;
    Serial.print(posX);
    Serial.print("  ");
    Serial.println(posY); 
    if(posX + inc >= boxX){
      mstate = false;
      endReached = true;
    }

}

void displaceLeft(){
    digitalWrite(mot2a,0);  //motor 2 on
    digitalWrite(mot2b,1);
    delay((inc/motspeed)*1000);//motor 2 spins upto 20cm
    digitalWrite(mot2b,0);  //motor 2 off
    if(sow){
      sowing();
    }else{
      spraying();
    }
    posX -= inc;
    Serial.print(posX);
    Serial.print("  ");
    Serial.println(posY);
    if(posX - inc >= boxX){
      mstate = false;
      endReached = true;
    }

}

void displaceTop(){
  digitalWrite(mot1a,1);  //motor 1 on
  digitalWrite(mot1b,0);
  delay((inc/motspeed)*1000); //motor 1 spins upto 20cm
  digitalWrite(mot1a,0); //motor 1 off
  posY += inc;
  
  Serial.print(posX);
  Serial.print("  ");
  Serial.println(posY);
}

void spraying(){
  delay(1000);
}

void sowing(){
  delay(1000);
}

void restorePos(){
  Serial.println("Restoring position to origin");
  digitalWrite(mot1b,1);  //motor 1 on in reverse
  digitalWrite(mot1a,0);
  delay((boxY/motspeed)*1000); //motor 1 spins upto 1cm
  digitalWrite(mot1b,0);  //motor 1 off
  digitalWrite(mot2b,1);  //motor 2 on in reverse
  digitalWrite(mot2a,0);
  delay((boxX/motspeed)*1000); //motor 1 spins upto 1m
  digitalWrite(mot2b,0);  //motor 2 off
  posX=0;
  posY=0;
  Serial.print(posX);
  Serial.print("  ");
  Serial.println(posY);  
}