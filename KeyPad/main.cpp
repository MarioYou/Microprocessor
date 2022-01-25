#include"mbed.h"

DigitalOut clk(p27);
DigitalOut rst(p28);
DigitalOut data(p30); 

DigitalIn row1(p9);
DigitalIn row2(p10);
DigitalIn row3(p11);
DigitalIn row4(p12);

DigitalOut col1(p5);
DigitalOut col2(p6);
DigitalOut col3(p7);
DigitalOut col4(p8);

int main();

int arr[17][8] = {  {1,1,0,0,0,0,0,0}, //0
               {1,1,1,1,1,0,0,1}, //1
               {1,0,1,0,0,1,0,0}, //2
               {1,0,1,1,0,0,0,0}, //3
               {1,0,0,1,1,0,0,1}, //4
               {1,0,0,1,0,0,1,0}, //5
               {1,0,0,0,0,0,1,0}, //6
               {1,1,1,1,1,0,0,0}, //7
               {1,0,0,0,0,0,0,0}, //8
               {1,0,0,1,1,0,0,0}, //9
               {1,0,0,0,1,0,0,0}, //A
               {1,0,0,0,0,0,1,1}, //B
               {1,1,0,0,0,1,1,0}, //C
               {1,0,1,0,0,0,0,1}, //d
               {1,0,0,0,0,1,1,0}, //E
               {1,0,0,0,1,1,1,0}, //F
{1,1,1,1,1,1,1,1}}; //' '

int readPad(int row1, int row2, int row3, int row4, int col1, int col2, int col3, int col4 )
{
if (!row1)
{
if (!col1) return 1;
    else if (!col2) return 2;
    else if (!col3) return 3;
    else if (!col4) return 10;
}
else if (!row2)
{
if (!col1) return 4;
else if (!col2) return 5;
else if (!col3) return 6;
else if (!col4) return 11;
}
else if (!row3)
{
if (!col1) return 7;
    else if (!col2) return 8;
    else if (!col3) return 9;
    else if (!col4) return 12;
  }
  else if (!row4)
{
if (!col1) return 0;
    else if (!col2) return 15;
    else if (!col3) return 14;
    else if (!col4) return 13;
}
  else return 88;
}

void EdgeClock(){
wait(0.000001);
clk = 1;
wait(0.000001);
clk=0;

}

void displayVal(int val1){
   for (int j = 0; j < 8 ; j++) {
       data = arr[val1][j];
       EdgeClock();
   }
   wait(0.2);
}

void clearScreen(){
for (int i = 0; i < 3; i++)
{
displayVal(16);
}
}

void Calculate(int topR, int topL, int Operation, int botR, int botL){
int output = 0;
switch(Operation)
{
 case 10: output = ((topR) + (topL)*10) + ((botR) + (botL)*10); break; // LSB + MSB*10 for all
case 11: output = ((topR) + (topL)*10) - ((botR) + (botL)*10); break;
 case 12: output = ((topR) + (topL)*10) & ((botR) + (botL)*10); break;
case 13: output = ((topR) + (topL)*10) | ((botR) + (botL)*10); break;
default: break;
}
displayVal(output % 10);
wait(0.0001);
displayVal(output / 10);
wait(0.0001);
displayVal(16);
}

void CalculatorInput(int num){
static int first_Operand[2] = {}; //sets the first operand
static int second_Operand[2] = {}; //sets the second operand
static int Operation = 0; //operation selection
static int Equal_Operator = 0; // finish the calculation
static int index_Operand1 = 0; //operand index
static int index_Operand2 = 0;
static int state_of_Operation = 1; //starting at 1
if(num == 15)
{ // resets everything if true
clearScreen();

 Operation = 0;
 Equal_Operator = 0;
 index_Operand1 = 0;
 index_Operand2 = 0;
 state_of_Operation = 1;
main();
}

switch(state_of_Operation)
{
case 1:
first_Operand[index_Operand1] = num; //collects first input LSB
 index_Operand1++; //moves the index
 displayVal(num); //display LSB
 state_of_Operation++; //move to next state
break;
case 2:
first_Operand[index_Operand1] = num; //collect MSB
 displayVal(num); // display MSB
 state_of_Operation++; //move to next state
break;
case 3:
Operation = num; //operation selection
 clearScreen(); //clear screen
state_of_Operation++; //next state
break;
case 4:
second_Operand[index_Operand2] = num; //collects second operand LSB
 index_Operand2++; //move the index
 displayVal(num); //display LSB
 state_of_Operation++; //next state
break;
case 5:
second_Operand[index_Operand2] = num; // MSB for operand 2
 displayVal(num); //display MSB
 state_of_Operation++; //move to next state
 break;
case 6:
clearScreen(); // clear screen
 Calculate(first_Operand[0], first_Operand[1], Operation, second_Operand[0], second_Operand[1]); //calculate
 state_of_Operation++; //move to defualt
break;
default: break;
}
}

int main(){
   rst = 0;
   EdgeClock();
   rst = 1;
   EdgeClock();
   displayVal(16);
   displayVal(16);
   displayVal(16);
   while(1){

       int val;
       col1=0;
       col2=1;
       col3=1;
       col4=1;
       val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);
        if (val!=88)
{
CalculatorInput(val);
    }
wait(0.0001);

       col1=1;
       col2=0;
       col3=1;
       col4=1;
       val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);

       if (val!=88){
           CalculatorInput(val);
       }
wait(0.0001);

       col1=1;
       col2=1;
       col3=0;
       col4=1;
       val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);

       if (val!=88){
           CalculatorInput(val);
       }
wait(0.0001);

       col1=1;
       col2=1;
       col3=1;
       col4=0;
       val=readPad(row1, row2, row3, row4, col1, col2, col3, col4);

       if (val!=88){
           CalculatorInput(val);
       }
 wait(0.0001);


   }
}//end main