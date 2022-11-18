#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
// Global variabls
unsigned char g_second1=0 ;      //Counter For First Segment (second)
unsigned char g_second2=0 ;     //Counter For Second segment (second)
unsigned char g_minute1=0 ;    //Counter For Third Segment (Minute)
unsigned char g_minute2=0 ;   //Counter For Fourth Segment (Minute)
unsigned int g_hours1=0   ;  //Counter For Fifth Segment (Hours)
unsigned int g_hours2=0   ; //Counter For sixth Segment (Hours)
unsigned short enable =0;


// INT0
ISR(INT0_vect)
{
	g_second1=0 ;      //RESET FIRST 7_SEGMENT (SECOND)
	g_second2=0 ;     //RESET SECOND 7_SEGMENT (SECOND)
	g_minute1=0 ;    //RESET THIRD 7_SEGMENT (MINUTE)
	g_minute2=0 ;   //RESET FOURTH 7_SEGMENT (MINUTE)
	g_hours1=0 ;   //RESET FIFTH 7_SEGMENT (HOURS)
	g_hours2=0  ; //RESET SIXTH 7_SEGMENT (HOURS)
}

void INT0_RESET (void)
{
	DDRD &=~ (1<<PD2);      // Input Pin for INT0
	PORTD |= (1<<PD2);     // ACTIVATE INTERNAL PULL UP
	MCUCR |=(1<<ISC01);   // Making INT0 With falling Egde
	GICR  |= (1<<INT0);  // Enable INT0
}

// INT1
ISR(INT1_vect)
{
	TCCR1B &=~((1<<CS12)|(1<<CS10));
}

void INT1_paused (void)
{
	DDRD &=~ (1<<PD3);                  // Input Pin for INT1
	PORTD &=~(1<<PD3);                 // Stop Internal pull Up
	MCUCR |= (1<<ISC11)|(1<<ISC10);   // Making INT1 With rising Egde
	GICR  |= (1<<INT1);              // Enable INT1
}

// INT2
ISR(INT2_vect)            // INT Service Routine for INT2
{
	TCCR1B|= (1<<CS12)|(1<<CS10);
}

void INT2_Resumed (void) // External INT2 to Resume Stope watch
{
	DDRB &=~(1<<PB2);         // Input Pin for INT1
	PORTB |=(1<<PB2);        // Enable Internal pull Up
	MCUCSR &=~ (1<<ISC2);   // with falling egd
	GICR |= (1<<INT2);     // Input Pin for INT1
}

// TIMER1 CTC MODE
ISR (TIMER1_COMPA_vect)
{
	Second1 ();
}
void Timer1 (void)
{
	OCR1A=976;                                     // count from 0 to 976
	TCCR1A = (1<<FOC1A)| (1<<FOC1B);              // Using Compare Mode
	TCNT1=0;                                     // Intial Value to Count
	TCCR1B = (1<<CS12)|(1<<CS10)|(1<<WGM12);    // using prescale 1024 and compare mode
	TIMSK |=(1<<OCIE1A);                       // Input matched enable
}


void Second1 ()
{
	if (g_second1 == 9)
	{
		if (g_second2<5)
		{
			g_second1=0;
			g_second2++;
		}
		else
		{
			g_second1=0;
			g_second2=0;
			minutes ();
		}

	}
	else
	{
		g_second1++;
	}

}


void minutes(void)
{

	if (g_minute1 == 9)
	{

		if (g_minute2<5)
		{
			g_minute2++;
			g_minute1=0 ;
		}
		else
		{
			g_minute1=0;
			g_minute2=0;
			Hours();
		}

	}
	else
	{
		g_minute1++;
	}
}


void Hours ()
{
	g_minute2=0;
	if (g_hours1==9)
	{
		g_hours1=0;
		g_hours2++;
	}
	else
	{
		g_hours1++;
	}
}


void switchcase (unsigned char num)
{
	switch (num)
	{
	case 0 :
		PORTC &=~ (1<<PC0);
		PORTC &=~ (1<<PC1);
		PORTC &=~ (1<<PC2);
		PORTC &=~ (1<<PC3);
		break ;

	case 1 :
		PORTC |=  (1<<PC0);
		PORTC &=~ (1<<PC1);
		PORTC &=~ (1<<PC2);
		PORTC &=~ (1<<PC3);
		break ;

	case 2 :
		PORTC &=~ (1<<PC0);
		PORTC |=  (1<<PC1);
		PORTC &=~ (1<<PC2);
		PORTC &=~ (1<<PC3);
		break ;

	case 3:
		PORTC |=  (1<<PC0);
		PORTC |=  (1<<PC1);
		PORTC &=~ (1<<PC2);
		PORTC &=~ (1<<PC3);
		break;

	case 4:
		PORTC &=~ (1<<PC0);
		PORTC &=~ (1<<PC1);
		PORTC |=  (1<<PC2);
		PORTC &=~ (1<<PC3);
		break;

	case 5:
		PORTC |=  (1<<PC0);
		PORTC &=~ (1<<PC1);
		PORTC |=  (1<<PC2);
		PORTC &=~ (1<<PC3);
		break;

	case 6:
		PORTC &=~ (1<<PC0);
		PORTC |=  (1<<PC1);
		PORTC |=  (1<<PC2);
		PORTC &=~ (1<<PC3);
		break;

	case 7:
		PORTC |=  (1<<PC0);
		PORTC |=  (1<<PC1);
		PORTC |=  (1<<PC2);
		PORTC &=~ (1<<PC3);
		break;

	case 8:
		PORTC &=~ (1<<PC0);
		PORTC &=~ (1<<PC1);
		PORTC &=~ (1<<PC2);
		PORTC |=  (1<<PC3);
		break;

	case 9:
		PORTC |=  (1<<PC0);
		PORTC &=~ (1<<PC1);
		PORTC &=~ (1<<PC2);
		PORTC |=  (1<<PC3);
		break;
	}
}


int main (void)
{
	DDRC  |= 0X0F;              // First 4 Pin O/P PINS = 1
	PORTC |= 0XF0;             // Intial Value Equal zero as Anode
	DDRA  |= 0X3F;            // First 6 Pin in PORTC O/P Pins
	PORTA |= 0xC0;           // Intial value Equal to Zero
	SREG |= (1<<7);         // Enable I_bit
	INT0_RESET ();         // INT0 TO RESET 6 7_SEGMENT
	INT1_paused () ;      // INT1 FOR PAUSE 6 7_SEGMENT
	INT2_Resumed ();     // INT2 FOR RESUME 7_SEGMENT
	Timer1 ();          // ACTIVATE TIMER0


	while (1)
	{

		enable=1;
		PORTA= ((PORTA & 0xC0) | (enable & 0x3F));
		switchcase (g_second1);

		_delay_us (3);


		enable=2;
		PORTA= ((PORTA & 0xC0) | (enable & 0x3F));
		switchcase (g_second2);
		_delay_us (3);


		enable=4;
		PORTA= ((PORTA & 0xC0) | (enable & 0x3F));
		switchcase (g_minute1);
		_delay_us (3);


		enable=8;
		PORTA= ((PORTA & 0xC0) | (enable & 0x3F));
		switchcase (g_minute2);
		_delay_us (3);


		enable=0x10;
		PORTA= ((PORTA & 0xC0) | (enable & 0x3F));
		switchcase (g_hours1);
		_delay_us (3);


		enable=0x20;
		PORTA= ((PORTA & 0xC0) | (enable & 0x3F));
		switchcase (g_hours2);
		_delay_us (3);


	}
}

