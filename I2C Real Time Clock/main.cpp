#include "mbed.h"
#include "math.h"

DigitalOut CLK(p27);	// clock is at p27
DigitalOut CLR(p28);	// clear is at p28
DigitalOut DATA(p30);	// data is at p30

// Rows are inputs, we will be checking for the button to be pressed (inverted logic)
DigitalIn R1(p9);
DigitalIn R2(p10);
DigitalIn R3(p11);
DigitalIn R4(p12);

// Columns are outputs, we will be manually setting them high and low.
DigitalOut C1(p5);
DigitalOut C2(p6);
DigitalOut C3(p7);
DigitalOut C4(p8);

// for the temp sensor (SDA, SCL)
I2C i2c(p32,p31);

void loadBits(int bits[8]);

//lab 6 a
int rowScan(int colNum);
int rowPressed();
int getNum(int rowNum, int colNum);
int get2DigNum();

//lab 6 b
void calc();
int getOpNum();
void clearScreen();
void displayResult(int result);

//lab 7
void tempSensor();
void convertTemp(int temp);
void displayTemp(int result, int type);
int round(float num);

//lab 8
#define decode_bcd(x) ((x >> 4) * 10 + (x & 0x0F)) 
#define encode_bcd(x) ((((x / 10) & 0x0F) << 4) + (x % 10)) 
void RTC();
void init_temp();
void init_RTC();
void displayDay(int Day);
void displayHrs(int time, int type, int ampm);
void displayTime(int time, int type, int ampm);

int main()
{
	CLK = 0; // initially disable load
	CLR = 1; // first make clear = 1
	CLR = 0; // then clear = 0 to reset
	CLR = 1; // then back to clear = 1

	// setting the columns high (we are doing row scanning)
	C1 = 1;
	C2 = 1;
	C3 = 1;
	C4 = 1;

	int colNum = 1;
	int rowNum = 0;

	init_temp();
	init_RTC();

	//calc();
	while(1)
	{
		RTC();
		tempSensor();
	}
}

// lab 8
void init_RTC()
{
	// clearing the register values
	i2c.start(); /* The following Enables the Oscillator */ 
	i2c.write(0xD0); /* address the part to write */ 
	i2c.write(0x00); /* position the address pointer to 0 */ 
	i2c.write(0x00); /* write 0 to the secs register, clear the CH bit */ 
	i2c.stop();

	// entering the values
	i2c.start();
	i2c.write(0xD0);
	i2c.write(0x00); // write register address 1st clock register
	i2c.write(encode_bcd(0)); // starts at 0 seconds
	i2c.write(encode_bcd(57)); // starts at 59 minutes
	i2c.write( 0x60 | encode_bcd(11)); // the 3rd hour pm
	i2c.write(encode_bcd(6)); // the 6th day of week (Friday)
	i2c.write(encode_bcd(28)); // the 28th day of month
	i2c.write(encode_bcd(2)); // the 2nd month of the year (February)
	i2c.write(encode_bcd(20)); // the year is 2000 + 21 (not a leap year)
	i2c.start();
	i2c.write(0xD0); 
	i2c.write(0x0e); // write register address control register
	i2c.write(0x20); // enable osc, bbsqi
	i2c.write(0); // clear the osf, alarm flags
	i2c.stop();

	// after doing this, the ds1337 will start to update time every sec
}

void RTC()
{
	int Sec, Min, Hrs, Day, Dte, Mon, Yr;
	int ampm, tenHr;

	// now we will read values
	i2c.start(); // start i2c 
	i2c.write(0xD0); // the address of the slave
	i2c.write(0x00); // the starting address to read (will increment automatically after each ACK)
	i2c.start(); // repeated i2c start
	i2c.write(0xD1); // we want to read from the slave
	Sec = i2c.read(1); // read the seconds register, send ACK
	Min = i2c.read(1); // read the minutes register, send ACK
	Hrs = i2c.read(1); // read the hour register, send ACK
	Day = i2c.read(1); // read the day register, send ACK
	Dte = i2c.read(1); // read the Date register, send ACK
	Mon = i2c.read(1); // read the month register, send ACK
	Yr  = i2c.read(0); // read the year register, send NACK to end the communication
	i2c.stop(); // stop the i2c 
	ampm = (Hrs & 0x20) >> 5;			 // get the ampm bit
	Sec = decode_bcd(Sec);
	Min = decode_bcd(Min);
	Hrs = Hrs & 0x1F;
	Hrs = decode_bcd(Hrs);
	Day = decode_bcd(Day);
	Dte = decode_bcd(Dte);
	Mon = decode_bcd(Mon);
	Yr = decode_bcd(Yr);

	// time should be displayed in 12 hr format
	// use lsb dot as PM indicator
	// bit 6 of hours register is the 12 or 24 hour format, 1 indicates 12 hour format
	// bit 5 is the am pm bit, 1 indicates PM

	// first display the day of week
	// then the hour
	// then the minute
	// and then the second
	// display each of them for 3 seconds
	// use dot to indiacate pm

	printf("Day = %d\n", Day);
	displayDay(Day);
	//displayDay(6);
	wait(1);
	displayHrs(Hrs, 0, ampm);
	//displayTime(11,0,1);
	wait(1);
	displayTime(Min, 1, ampm);
	//displayTime(57,1,1);
	wait(1);
	displayTime(Sec, 2, ampm);
	//displayTime(45,2,1);
	wait(1);
}

void displayDay(int Day)
{
	int Days[7][3][8] =
	{
		{{0,1,0,0,1,0,0,1}, {1,0,0,0,0,0,1,1}, {1,1,0,1,0,1,0,1}},	// sun
		{{0,1,1,0,0,0,0,1}, {0,0,0,0,0,0,1,1}, {1,1,0,1,0,1,0,1}},	// mon
		{{0,1,1,1,0,0,1,1}, {1,0,0,0,0,0,1,1}, {0,1,1,0,0,0,0,1}},	// tue
		{{0,0,0,0,1,1,0,1}, {0,1,1,0,0,0,0,1}, {1,0,0,0,0,1,0,1}},	// wed
		{{0,1,1,1,0,0,1,1}, {1,0,0,1,0,0,0,1}, {1,0,0,0,0,0,1,1}},	// thu
		{{0,1,1,1,0,0,0,1}, {1,1,1,1,0,1,0,1}, {1,1,1,1,0,0,1,1}},	// fri
		{{0,1,0,0,1,0,0,1}, {0,0,0,1,0,0,0,1}, {0,1,1,1,0,0,1,1}}	// sat
	};

	for(int i = 2; i >= 0; i--)
	{
		loadBits(Days[Day-1][i]);
	}
}

void displayTime(int time, int type, int ampm)
{
	int first = 0;
	int second = 0;
	int dec[10][8] =		// numbers
	{
		{0,0,0,0,0,0,1,1},	//0
		{1,0,0,1,1,1,1,1},	//1
		{0,0,1,0,0,1,0,1},	//2
		{0,0,0,0,1,1,0,1},	//3
		{1,0,0,1,1,0,0,1},	//4
		{0,1,0,0,1,0,0,1},	//5
		{0,1,0,0,0,0,0,1},	//6
		{0,0,0,1,1,1,1,1},	//7
		{0,0,0,0,0,0,0,1},	//8
		{0,0,0,1,1,0,0,1}	//9
	};
	int symbols[2][3][8] =	// symbols
	{
		{{1,0,0,1,0,0,0,1}, {1,0,1,1,1,1,1,1}, {1,0,1,1,1,0,1,1}},
		{{1,0,0,1,0,0,0,0}, {1,0,1,1,1,1,1,0}, {1,0,1,1,1,0,1,0}}
	};

	loadBits(symbols[ampm][type]);
	
	first	= time / 10;
	second	= time - first*10;										
	
	loadBits(dec[second]);
	loadBits(dec[first]);
}

void displayHrs(int time, int type, int ampm)
{
	int first = 0;
	int second = 0;
	int dec[10][8] =		// numbers
	{
		{0,0,0,0,0,0,1,1},	//0
		{1,0,0,1,1,1,1,1},	//1
		{0,0,1,0,0,1,0,1},	//2
		{0,0,0,0,1,1,0,1},	//3
		{1,0,0,1,1,0,0,1},	//4
		{0,1,0,0,1,0,0,1},	//5
		{0,1,0,0,0,0,0,1},	//6
		{0,0,0,1,1,1,1,1},	//7
		{0,0,0,0,0,0,0,1},	//8
		{0,0,0,1,1,0,0,1}	//9
	};
	int symbols[2][3][8] =	// symbols
	{
		{{1,0,0,1,0,0,0,1}, {1,0,1,1,1,1,1,1}, {1,0,1,1,1,0,1,1}},
		{{1,0,0,1,0,0,0,0}, {1,0,1,1,1,1,1,0}, {1,0,1,1,1,0,1,0}}
	};

	loadBits(symbols[ampm][type]);
	
	if (time > 9)
	{
		first = 1;
		second = time-10;
	}
	else
	{
		first = 0;
		second = time;
	}
			
	loadBits(dec[second]);
	loadBits(dec[first]);
}

// lab 7 (will be reused for lab 8)
void init_temp()
{
	i2c.start();
	i2c.write(0x90);
	i2c.write(0xac); 
	i2c.write(2); 
	i2c.stop();
}

void tempSensor()
{
	int temperature;
	int upperByte;
	int lowerByte;

	i2c.start();
	i2c.write(0x90);
	i2c.write(0x51);
	i2c.stop();

	i2c.start();
	i2c.write(0x90);
	i2c.write(0xaa);

	i2c.start(); 
	i2c.write(0x91); 

	upperByte = i2c.read(1); // send an ACK after read
	lowerByte = i2c.read(0); // send a NACK after read
	temperature = (upperByte << 8) + lowerByte;
	i2c.stop();
	convertTemp(temperature);
}

void displayTemp(int result, int type)
{
	int sequence[16][8] = 
	{
		{0,0,0,0,0,0,1,1},//0
		{1,0,0,1,1,1,1,1},//1
		{0,0,1,0,0,1,0,1},//2
		{0,0,0,0,1,1,0,1},//3
		{1,0,0,1,1,0,0,1},//4
		{0,1,0,0,1,0,0,1},//5
		{0,1,0,0,0,0,0,1},//6
		{0,0,0,1,1,1,1,1},//7
		{0,0,0,0,0,0,0,1},//8
		{0,0,0,1,1,0,0,1},//9
		{0,0,0,1,0,0,0,1},//A
		{1,1,0,0,0,0,0,1},//b
		{0,1,1,0,0,0,1,1},//C
		{1,0,0,0,0,1,0,1},//d
		{0,1,1,0,0,0,0,1},//E
		{0,1,1,1,0,0,0,1} //F
	};

	int first = result/10;
	int second = result - first*10;
	if (type == 1)  // display in F
	{ 
		loadBits(sequence[15]);
	}
	else			// display in C
	{
		loadBits(sequence[12]);
	}
	loadBits(sequence[second]);
	loadBits(sequence[first]);
}





void convertTemp(int temp){

	int tempF;

	int tempC;

	

	tempC = temp >> 4;

	if (tempC > 0x7FF){ // if negative

		tempC = round(-1 * ( ((int) (2 << 12) - tempC) * 0.0625) ); // 2s comp convert

	}else{

		tempC = round(tempC * 0.0625);

	}

	

	tempF = (int) round((tempC * 1.8) + 32); // conversion from C to F

	

	displayTemp(tempC, 0);

	wait(1);

	displayTemp(tempF, 1);

	wait(1);

	

	

}



int round(float num){

	float temp = num;

	int isNegative = 0;

	

	if (temp < 0.0){

		temp = -1 * temp;

		isNegative = 1;

	}

	

	if(temp - ((int) temp) < 0.5){

		if(isNegative){

			return -1 * (int) temp;

		}

		else{

			return (int) temp;

		}

	}

	else{

		if(isNegative){

			return -1 * ((int) temp + 1);

		}

		else{

			return (int) temp + 1;

		}

	}

}





// load the bits into the shift register

void loadBits(int bits[8]){

	for(int i = 7; i >= 0; i--){

		DATA = bits[i]; // load the data first

		CLK = 1; // then set the clock high to enable the load into register

		CLK = 0; // then set clock low to disable load into register

	}

}











































// lab 6 a



int getNum(int rowNum, int colNum){

		int numOrder[4][4] = {{1,2,3,10}, 

											 {4,5,6,11}, 

											 {7,8,9,12}, 

											 {0,15,14,13}};

		return numOrder[rowNum-1][colNum-1];

}





int rowScan(int colNum){

	int rowNum = 0;

	if(colNum == 1){

		C1 = 0;

	}

	else if(colNum == 2){

		C2 = 0;

	}

	else if(colNum == 3){

		C3 = 0;

	}

	else if(colNum == 4){

		C4 = 0;

	}

	

	rowNum = rowPressed();

	

	C1 = 1;

	C2 = 1;

	C3 = 1;

	C4 = 1;

	return rowNum;

}



int rowPressed(){

	int num = 0;

	if(R1 == 0){

		num = 1;

	}

	else if(R2 == 0){

		num = 2;

	}

	else if(R3 == 0){

		num = 3;

	}

	else if(R4 == 0){

		num = 4;

	}

	return num;

}











































// lab 6 b





void calc(){

	int op1;

	int op2;

	int opNum;

	int result = 0;

  int num = 0;

  int colNum = 1;

  clearScreen();

	

	while(1){

		op1 = get2DigNum();

		result = op1;

		

		opNum = getOpNum();

		

		while(opNum != 14 && opNum != 15){



			op2 = get2DigNum();



			switch(opNum){

					case 10: result = result + op2; break;// add

					case 11: result = result - op2; break;// subtract

					case 12: result = result & op2; break;// and

					case 13: result = result | op2; break;// OR

			}



			opNum = getOpNum();

		}

		

		if (opNum == 15){

				clearScreen();

				continue;

		}

		else if(opNum == 14){

				displayResult(result);

				wait(0.1);

				while(rowScan(colNum) == 0){

						if(colNum == 4){

								colNum = 1;

						}

						else{

								colNum++;

						}

						wait(0.05);

				}

				clearScreen();

				continue;

		}

		

	}

}



void displayResult(int result){

	int sequence[16][8] = {{0,0,0,0,0,0,1,1},//0

							 {1,0,0,1,1,1,1,1},//1

							 {0,0,1,0,0,1,0,1},//2

							 {0,0,0,0,1,1,0,1},//3

							 {1,0,0,1,1,0,0,1},//4

							 {0,1,0,0,1,0,0,1},//5

							 {0,1,0,0,0,0,0,1},//6

							 {0,0,0,1,1,1,1,1},//7

							 {0,0,0,0,0,0,0,1},//8

							 {0,0,0,1,1,0,0,1},//9

							 {0,0,0,1,0,0,0,1},//A

							 {1,1,0,0,0,0,0,1},//b

							 {0,1,1,0,0,0,1,1},//C

							 {1,0,0,0,0,1,0,1},//d

							 {0,1,1,0,0,0,0,1},//E

							 {0,1,1,1,0,0,0,1}};//F

	int first = result/100;

  int second = (result - first * 100)/10;

  int third = result - first*100 - second*10;

  

	loadBits(sequence[third]);

  loadBits(sequence[second]);

	loadBits(sequence[first]);

							 

}



void clearScreen(){

	int off[8] = {1,1,1,1,1,1,1,1};

	loadBits(off);

	loadBits(off);

	loadBits(off);

}



int getOpNum(){

	int op = 0;

	int colNum = 1;

	while(op < 10){

		op = rowScan(colNum);

		if(op != 0){

			op = getNum(op, colNum);

		}

		

		if(colNum == 4){ 

			colNum = 1;

		}

		else{

			colNum++;

		}

		wait(0.03);

	}

	return op;

}



int get2DigNum(){

	int count = 0;

	int first = 0;

	int second = 0;

	int num = 0;

	int colNum = 1;

	int sequence[16][8] = {{0,0,0,0,0,0,1,1},//0

							 {1,0,0,1,1,1,1,1},//1

							 {0,0,1,0,0,1,0,1},//2

							 {0,0,0,0,1,1,0,1},//3

							 {1,0,0,1,1,0,0,1},//4

							 {0,1,0,0,1,0,0,1},//5

							 {0,1,0,0,0,0,0,1},//6

							 {0,0,0,1,1,1,1,1},//7

							 {0,0,0,0,0,0,0,1},//8

							 {0,0,0,1,1,0,0,1},//9

							 {0,0,0,1,0,0,0,1},//A

							 {1,1,0,0,0,0,0,1},//b

							 {0,1,1,0,0,0,1,1},//C

							 {1,0,0,0,0,1,0,1},//d

							 {0,1,1,0,0,0,0,1},//E

							 {0,1,1,1,0,0,0,1}};//F

	

	int off[8] = {1,1,1,1,1,1,1,1};

	

	while(count < 2){

		if(count == 0){

			first = rowScan(colNum);

			if(first != 0){

				first = getNum(first, colNum);

				if(first < 10){

					count++;

					loadBits(off);

					loadBits(sequence[first]);

					loadBits(off);

				}

			}

		}

		else if(count == 1){

			second = rowScan(colNum);

			if(second != 0){

				second = getNum(second, colNum);

				if(second < 10){

					count++;

					loadBits(sequence[second]);

					loadBits(sequence[first]);

					loadBits(off);

				}

			}

		}

		

		if(colNum == 4){ 

			colNum = 1;

		}

		else{

			colNum++;

		}

		wait(0.03);

		

	}

	

	num = first*10 + second;

	return num;

	

}







