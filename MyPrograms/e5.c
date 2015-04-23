#include <msp430g2553.h>
#include <libemb/serial/serial.h>
#include <libemb/conio/conio.h>
 
#define CATHODES 4
#define places 45
int cathode = 0;
int i = 0, p = 0;
 
//int places = 30; //how much e do you want?
int e[places + 1] = {2}; //put a 2 in index 0
 
int remainderArray[places];
int carryArray[places];
int sumArray[places];
int frequencyArray[10];
 
int digitsToDisplay = 1;
int esize = (sizeof(e) / sizeof(int));
 
void reset(int* arr, int size, int init) {
  //int i;
  for (i = 0; i < size; i++) {
    arr[i] = init;
  }
}
 
void stupid() {
	//int answerArray[places + 1];
	//    answerArray[0] = 2; // put a 2 in the first spot
	//e[0] = 2; // put a 2 in the first spot
 
  //int i, p;
 
  // setup our arrays
  reset(remainderArray, places, 1);
  reset(carryArray, places, 0);
  reset(sumArray, places, 0);
  reset(frequencyArray, 10, 0);
  //frequencyArray[2] = 1;
 
  // the meat of the spigot
  for (p = 0; p < places; p++) {
 
    //multiply every element by 10
    for (i = 0; i < places; i++) {
      remainderArray[i] *= 10;
    }
 
    // this calculation is done right-to-left
    for (i = (places - 1); i >= 0; i--) {
      sumArray[i] = remainderArray[i] + carryArray[i];
 
      if (i > 0) {
        //for some reason, this was able to assign
        // values to a negative array index
        // thanks, c
        carryArray[i-1] = sumArray[i] / (i+2);
      }
      remainderArray[i] = sumArray[i] % (i+2);
    }
 
    // calculate the answer and put it
    // in the next spot in the answerArray
    //answerArray[p+1] = sumArray[0] / 2;
		e[p+1] = sumArray[0] / 2;
 
    //increment the frequency
    //frequencyArray[answerArray[p+1]]++;
		//frequencyArray[answerArray[p+1]]++;
  } // spigot is done
}
 
/***********SETUP***********
	* Cathode 1: P2.3 - Pin1
	* Cathode 2: P2.2 - Pin10
	* Cathode 3: P2.1 - Pin4
	* Cathode 4: P2.0 - Pin6
	*
	**************************
	*
	* Anode A: P1.0 - Pin12
	* Anode B: P1.1 - Pin11
	* Anode C: P1.2 - Pin3
	* Anode D: P1.7 - Pin8
	* Anode E: P1.4 - Pin2
	* Anode F: P1.5 - Pin9
	* Anode G: P1.6 - Pin7
	*
*/
 
/* This anode is this bit sequence*/
enum anode {
	ANODE_A = BIT0,
	ANODE_B = BIT1,
	ANODE_C = BIT2,
	ANODE_D = BIT7,
	ANODE_E = BIT4,
	ANODE_F = BIT5,
	ANODE_G = BIT6,
};
 
/* This number is this combination of anodes */
int numbers[10] = {
	ANODE_A | ANODE_B | ANODE_C | ANODE_D | ANODE_E | ANODE_F,//0
	ANODE_B | ANODE_C, 																				//1
	ANODE_A | ANODE_B | ANODE_G | ANODE_E | ANODE_D,					//2
	ANODE_A | ANODE_B | ANODE_G | ANODE_C | ANODE_D,					//3
	ANODE_B | ANODE_C | ANODE_F | ANODE_G,										//4
	ANODE_A | ANODE_F | ANODE_G | ANODE_C | ANODE_D,					//5
	ANODE_F | ANODE_E | ANODE_D | ANODE_C | ANODE_G,					//6
	ANODE_A | ANODE_B | ANODE_C,															//7
	ANODE_A | ANODE_B | ANODE_C | ANODE_D | ANODE_E | ANODE_F | ANODE_G, //8
	ANODE_A | ANODE_B | ANODE_C | ANODE_F | ANODE_G,					//9
};
int output[10] ={
  BIT0 | BIT1 | BIT2 | BIT3 | BIT6| BIT7,//001 //blue ~0
  BIT0 | BIT1 | BIT2 | BIT3 | BIT5| BIT7,//010 //red ~1
  BIT0 | BIT1 | BIT2 | BIT3 | BIT5| BIT6 | BIT7,//011 //purple ~2
  BIT0 | BIT1 | BIT2 | BIT3 | BIT4| BIT7,//100 //green ~3
  BIT0 | BIT1 | BIT2 | BIT3 | BIT4| BIT6 | BIT7,//101 //weird, blue/green ~4
  BIT0 | BIT1 | BIT2 | BIT3 | BIT4| BIT5 | BIT7,//110 //yellow ~5
  BIT0 | BIT1 | BIT2 | BIT3 | BIT4| BIT5 | BIT6 | BIT7,//111 //white ~6
  BIT0 | BIT1 | BIT2 | BIT3 | BIT6| BIT7,//001 //back to blue ~7
  BIT0 | BIT1 | BIT2 | BIT3 | BIT5| BIT7,//010 //red ~8
  BIT0 | BIT1 | BIT2 | BIT3 | BIT7,//011 //purple ~9
};
int bubbleDisplay[] = {0, 0, 0, 0};
 
/* ??? */
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void display(void) {
  cathode = (cathode + 1) % CATHODES;
  P2OUT = ~(1 << cathode);
  P1OUT = 0b00000000;
  P1OUT = bubbleDisplay[cathode];
}
 
/* ??? */
__attribute__((interrupt(TIMER1_A0_VECTOR)))
void scan(void) {
 
	if (digitsToDisplay == 1) {
		P2DIR = output[e[0]];
      	__delay_cycles(10000);
		bubbleDisplay[0] = numbers[e[0]];
		frequencyArray[e[0]]+=1;
		digitsToDisplay++;
	} else if (digitsToDisplay == 2) {
		P2DIR = output[e[1]];
     	 __delay_cycles(10000);
		bubbleDisplay[1] = numbers[e[0]];
		bubbleDisplay[0] = numbers[e[1]];
		frequencyArray[e[1]]+=1;
		digitsToDisplay++;
	} else if (digitsToDisplay == 3) {
		P2DIR = output[e[2]];
      	__delay_cycles(10000);
		bubbleDisplay[2] = numbers[e[0]];
		bubbleDisplay[1] = numbers[e[1]];
		bubbleDisplay[0] = numbers[e[2]];
		frequencyArray[e[2]]+=1;
		digitsToDisplay++;
	} else if (digitsToDisplay == 4) {
		P2DIR = output[e[3]];
      	__delay_cycles(10000);
		bubbleDisplay[3] = numbers[e[0]];
		bubbleDisplay[2] = numbers[e[1]];
		bubbleDisplay[1] = numbers[e[2]];
		bubbleDisplay[0] = numbers[e[3]];
		frequencyArray[e[3]]+=1;
		digitsToDisplay++;
	} else if ((digitsToDisplay > 4) && (i < (esize - 4))) {
		P2DIR = output[e[i+1]];
    	__delay_cycles(10000);
		bubbleDisplay[3] = numbers[e[i+1]];
		frequencyArray[e[i+1]]+=1;
		P2DIR = output[e[i+2]];
    	__delay_cycles(10000);
		bubbleDisplay[2] = numbers[e[i+2]];
		frequencyArray[e[i+2]]+=1;
		P2DIR = output[e[i+3]];
    	__delay_cycles(10000);
		bubbleDisplay[1] = numbers[e[i+3]];
		frequencyArray[e[i+3]]+=1;
		P2DIR = output[e[i+4]];
    	__delay_cycles(10000);
		bubbleDisplay[0] = numbers[e[i+4]];
		frequencyArray[e[i+4]]+=1;
		i++;
	} else {
		P2DIR = output[e[0]];
    	__delay_cycles(10000);
		bubbleDisplay[3] = numbers[e[i+1]];
		bubbleDisplay[2] = numbers[e[i+2]];
		bubbleDisplay[1] = numbers[e[i+3]];
		bubbleDisplay[0] = numbers[0];
	}
}
 
int main(void) {
	WDTCTL  = WDTPW + WDTHOLD;	// stop watchdog timer
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL  = CALDCO_16MHZ;
 
	stupid();
 
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL  = CALDCO_1MHZ;
	//serial_init(9600);
 
	//black magic
	P2SEL &= ~(BIT6 | BIT7);
 
	P1DIR = 0b11111111;
	P1OUT = 0b00000000;
 
	P2DIR = 0b11111111;
	P2OUT = 0b00001111;
 
	TA0CTL = MC_1 | ID_3 | TASSEL_2;
	TA0CCTL0 = CCIE;
	TA0CCR0 = 0xff;
 
	TA1CTL = MC_1 | ID_3 | TASSEL_2;
	TA1CCTL0 = CCIE;
	TA1CCR0 = 0xffff;
 
	_BIS_SR(GIE | LPM1_bits);
 
	return 0;
}

