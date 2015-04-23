#include <msp430.h>

#include <libemb/serial/serial.h>
#include <libemb/conio/conio.h>
void spigot(int[], int *);
void totals(int[], int *, int *);

/*
 * QDSP-6064
 * 4-Digit Micro Numeric Indicator
 * (7 Segment Monolithic)
 *
 * Pinout:
 *  1:  Cathode 1
 *  2:  Anode e
 *  3:  Anode c
 *  4:  Cathode 3
 *  5:  Anode dp
 *  6:  Cathode 4
 *  7:  Anode g
 *  8:  Anode d
 *  9:  Anode f
 * 10:  Cathode 2
 * 11:  Anode b
 * 12:  Anode a

 * LEDS:
 * GND WHO CARES
 * P2.4
 * P2.5
 * P2.6
 */
 
/*
 * P2.0: Cathode 1
 * P2.1: Cathode 2
 * P2.2: Cathode 3
 * P2.3: Cathode 4
 */
#define CATHODES 4
int cathode = 0;
int lowest = 0;
//int pi[10]={3,1,4,1,5,9,2,6,5,3};             
//{3,5,6,2,9,5,1,4,1,3};
int tally[10];
int size = 10;
int eNumbers[10];
int digitsToDisplay = 1;
int pisize = (sizeof(eNumbers) / sizeof(int));
int i = 0;
int j = 0;
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
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
 
    // P1DIR = 0xff;
    // P1OUT = 0x00;
    int temp[10];
    spigot(eNumbers, &size);
    // for(j=size-1; j>-1; j--){
    //   temp[i]=eNumbers[j];
    //   i++;
    // }
    // for(i=0; i>size; i++){
    //   eNumbers[i]=temp[i];
    // }
    P1DIR  =  BIT0 | BIT1 | BIT2|BIT4 | BIT5 | BIT6| BIT7;
    P1REN |= BIT3; //aka 0b00001000
    P1OUT |= BIT3; //Set pull-up resistor initial state to 1
    P1DIR |= BIT0; // Prepare P1 for LED for output
    
    P1IE  |= BIT3;
    P1IES |= BIT3;
    P1IFG &= ~BIT3;
 
    // This changes the function of XIN and XOUT to act as P2.6 and P2.7
    P2SEL    &= ~(BIT6 | BIT7);
    //initial P2DIR, need to make it change with the numbers but having trouble getting both working at once
    //P2DIR |= output[8];
  
    P2OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);
 
    TA0CTL = MC_1 | ID_3 | TASSEL_2;
    TA0CCTL0 = CCIE;
    TA0CCR0 = 0xff;
 
    TA1CTL = MC_1 | ID_3 | TASSEL_2;
    TA1CCTL0 = CCIE;
    TA1CCR0 = 0xffff;
 
    _BIS_SR(GIE | LPM1_bits);
}
//Put this in just to try for calling the colors, no luck yet
void spigot(int answer[], int *places){
  answer[0] =2;
  int size = 5;
  int rowtally[5];
  int temptop[5];
  int i = 0;
  for(i = 0; i < size; i++){
    temptop[i]=1;
  }
  int j = 5;
  int k =0;
  for(i=1; i<*places; i++){
    int temp =0;
    for(j=size-1; j>-1; j--){
      temptop[j] = temptop[j]*10;
      if(j==4){
      rowtally[j]=temptop[j]%6;
        }
        if(j==3){
        temp=(((temptop[4]/6)+temptop[3])/(j+2));
        rowtally[j]=(((temptop[4]/6)+temptop[3])%(j+2));
        }
        if(j<3){
        rowtally[j] = ((temp+temptop[j])%(j+2));  
        temp =((temp+temptop[j])/(j+2));
        if(j==0){
          if(temp==0){
            temp=2;
          }
          answer[i]=temp;
          totals(tally, &size, &answer[i]);
        }
        }
    }
    for(k=0; k<size; k++){
          temptop[k]=rowtally[k];
          rowtally[k]=0;
          temp=0;
    }
  }
}
void totals(int tally[], int *size, int *number){

  tally[*number] +=1;

}
/*
 * P1.0: Anode a
 * P1.1: Anode b
 * P1.2: Anode c
 * P1.7: Anode d
 * P1.4: Anode e
 * P1.5: Anode f
 * P1.6: Anode g
 */
enum anode {
    ANODE_A = BIT0,
    ANODE_B = BIT1,
    ANODE_C = BIT2,
    ANODE_D = BIT7,
    ANODE_E = BIT4,
    ANODE_F = BIT5,
    ANODE_G = BIT6
    //,
    //ANODE_DP = BIT7
};
 
/*
 * Values for the anodes (segments), indexed by the cathode (digit).
 */
int numbers[10] = {
/** ZERO **/  ANODE_A | ANODE_B | ANODE_C | ANODE_D | ANODE_E | ANODE_F,
/** ONE **/   ANODE_B | ANODE_C,
/** TWO **/   ANODE_A | ANODE_B | ANODE_G | ANODE_E | ANODE_D,
/** THREE **/ ANODE_A | ANODE_B | ANODE_G | ANODE_C | ANODE_D,
/** FOUR **/  ANODE_F | ANODE_G | ANODE_B | ANODE_C,
/** FIVE **/  ANODE_A | ANODE_F | ANODE_G | ANODE_C | ANODE_D,
/** SIX **/   ANODE_F | ANODE_E | ANODE_D | ANODE_C | ANODE_G,
/** SEVEN **/ ANODE_A | ANODE_B | ANODE_C,
/** EIGHT **/ ANODE_A | ANODE_B | ANODE_C | ANODE_D | ANODE_E | ANODE_F | ANODE_G,
/** NINE **/  ANODE_A | ANODE_B | ANODE_G | ANODE_F | ANODE_C
};
 
int bubbleDisplay[4] = {
  0,
  0,
  0,
  0
};
 
//ALOT
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void display(void) {
    cathode = (cathode + 1) % CATHODES;
    P2OUT = ~(1 << cathode);
    // P1OUT = 0x00;
    // P1OUT = bubbleDisplay[cathode];
    //Set pull-up resistor initial state to 1
    P1OUT = BIT3 | 0x00;
    P1OUT = BIT3 | bubbleDisplay[cathode];
}
 

__attribute__((interrupt(TIMER1_A0_VECTOR)))
void scan(void) {
if (digitsToDisplay == 1) {
    P2DIR = output[0];
      __delay_cycles(10000);
    bubbleDisplay[0] = numbers[eNumbers[0]];
    digitsToDisplay++;
  } else if (digitsToDisplay == 2) {
     P2DIR = output[1];
      __delay_cycles(10000);
    bubbleDisplay[1] = numbers[eNumbers[0]];
    bubbleDisplay[0] = numbers[eNumbers[1]];
    digitsToDisplay++;
  } else if (digitsToDisplay == 3) {
     P2DIR = output[2];
      __delay_cycles(10000);
    bubbleDisplay[2] = numbers[eNumbers[0]];
    bubbleDisplay[1] = numbers[eNumbers[1]];
    bubbleDisplay[0] = numbers[eNumbers[2]];
    digitsToDisplay++;
  } else if (digitsToDisplay == 4) {
     P2DIR = output[3];
      __delay_cycles(10000);
    bubbleDisplay[3] = numbers[eNumbers[0]];
    bubbleDisplay[2] = numbers[eNumbers[1]];
    bubbleDisplay[1] = numbers[eNumbers[2]];
    bubbleDisplay[0] = numbers[eNumbers[3]];
    digitsToDisplay++;
  } else if ((digitsToDisplay > 4) && (i < (pisize - 4))) {
    P2DIR = output[5];
    __delay_cycles(10000);
    bubbleDisplay[3] = numbers[eNumbers[i+1]];
    bubbleDisplay[2] = numbers[eNumbers[i+2]];
    bubbleDisplay[1] = numbers[eNumbers[i+3]];
    bubbleDisplay[0] = numbers[eNumbers[i+4]];
    i++;
  } else {
    P2DIR = output[0];
    __delay_cycles(10000);
    bubbleDisplay[3] = numbers[eNumbers[i+1]];
    bubbleDisplay[2] = numbers[eNumbers[i+2]];
    bubbleDisplay[1] = numbers[eNumbers[i+3]];
    bubbleDisplay[0] = numbers[0];
  }
}

  // int i, j=0, index=0;
  // if(j>9){
  //   j=0;
  // }
  // for(i=0; i < sizeof(bubbleDisplay); i++ ){
  //   index = i + lowest;
  //   if( index > 9 ) {
  //     index -= 10;
  //   }
  //   bubbleDisplay[i] = numbers[eNumbers[index]]; 
  // }
  // lowest++;
  // if(lowest>9) { lowest = 0; }
//}
// #pragma vector=PORT1_VECTOR
// __interrupt void Port_1(void) {
//   while ( !(BIT3 & P1IN) ) {} // wait till state change
//   __delay_cycles(32000);
//   P1IFG &= ~BIT3;
//    int i, index=0;
//   for(i=0; i < sizeof(bubbleDisplay); i++ ){
//     index = i + lowest;
//     if( index > 9 ) {
//       index -= 10;
//     }
//     bubbleDisplay[i] = numbers[index];
//   }

//   lowest++;
//   if(lowest>9) { lowest = 0; }
// }
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
  while (!(BIT3 & P1IN)) {} // wait till state change
  __delay_cycles(32000);
  P1OUT = 0xff;
  __delay_cycles(3200000);
  P1IFG &= ~BIT3;

  lowest++;
  if( lowest > 9 ) { lowest = 0; }
}

