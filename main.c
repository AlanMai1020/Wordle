/*
 * project5.c
 *
 * Created: 3/20/2023 6:06:51 PM
 * Author : alanm
 */ 

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avr.h"
#include "lcd.h"

int is_pressed(int row, int col){
	DDRC = 0x00; //set all the gpio in c to be n/c
	PORTC = 0x00;
	
	SET_BIT(DDRC, row); //set ddr at the gpio to be 1 for gnd
	CLR_BIT(PORTC, row);
	
	CLR_BIT(PORTC, 4 + col);
	SET_BIT(PORTC, 4 + col); //will be a weak 1
	avr_wait(10);
	if (GET_BIT(PINC, 4 + col) == 0){return 1;} //return 1 if key is pressed
	return 0;
}

int get_keys(){
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			if (is_pressed(i,j)){
			return (i * 4 + j + 1);}
		}
	}return 0;
}

char key_num(int num_pressed, int mode){
	//mode 0 or 15
	int temp = 0;
	switch(num_pressed){
		case 1:
		temp = 65 + mode;
		break;
		case 2:
		temp = 66 + mode;
		break;
		case 3:
		temp = 67 + mode;
		break;
		case 4:
		temp = 68 + mode;
		break;
		case 5:
		temp = 69 + mode;
		break;
		case 6:
		temp = 70 + mode;
		break;
		case 7:
		temp = 71 + mode;
		break;
		case 8:
		temp = 72 + mode;
		break;
		case 9:
		temp = 73 + mode;
		break;
		case 10:
		temp = 74 + mode;
		break;
		case 11:
		temp = 75 + mode;
		break;
		case 12:
		temp = 76 + mode;
		break;
		case 13:
		temp = 77 + mode;
		break;
		case 14:
		temp = 78 + mode;
		break;
		case 15:
		temp = 79 + mode;
		break;
		case 16:
		return 0;
	}
	if (temp > 90){return 0;}
	return temp;
}

void print_mode(int mode){
	char buf[4];
	lcd_pos(1,13);
	if(mode == 0){
		sprintf(buf, "A-O");
	}
	else{sprintf(buf, "P-Z");}
	lcd_puts2(buf);
}

char replace_letter(char word[6], char printed[11], char letter, char tries){
	int success = 0;
	for (int i = 0; i < 5; i++){
		if (word[i] == letter){
			printed[i * 2] = letter;
			success++;
		}
	}
	if(success == 0){tries -= 1; return tries;}
	return tries;
}

void restart(char printed[11]){
	char printed_text[11] = "_ _ _ _ _";
	strcpy(printed, printed_text);
}

void print_tries(int tries){
	lcd_pos(1,0);
	char buf[9];
	sprintf(buf, "Tries: %c", tries + 48);
	lcd_puts2(buf);
}

char win(char text[11]){
	int found = 0;
	for(int i = 0; i < 9; i++){
		if (text[i] == 95){found++;}
	}
	if(found > 0){return 0;}
	return 1;
}

void print_win(){
	lcd_clr();
	lcd_pos(0,0);
	char buf[20] = "You win!!!";
	lcd_puts2(buf);
	avr_wait(3000);
}

void print_lose(){
	lcd_clr();
	lcd_pos(0,0);
	char buf[20] = "You lost!!!";
	lcd_puts2(buf);
	avr_wait(3000);
}

int get_sample(){
	int result;
	ADMUX = 0b01000000;
	ADCSRA = 0b11000000;
	while (GET_BIT(ADCSRA, 6));
	result = GET_BIT(ADCL,0);
	return result;
}


int main(void)
{
    lcd_init();
	int key;
	int mode = 0;
	char word[3][6] = {"HELLO", "WORLD"};
	char printed_text[11] = "_ _ _ _ _";
	char tries = 5;
	int r = get_sample();
	
    while (1) 
    {
		avr_wait(200);
		//game over
		if (tries == 0 || win(printed_text) == 1){
			if (win(printed_text) == 1){print_win();}
			else{print_lose();}
			restart(printed_text); 
			tries = 5;
			r = get_sample();
		}
		key = get_keys();
		if (key != 0){
			//button to switch modes
			if(key == 16){
				if(mode == 0){mode = 15;}
				else{mode = 0;}
			}
			//print out the letter if in the word
			else{
				char temper = key_num(key, mode);
				if(temper != 0){
					tries = replace_letter(word[r], printed_text, temper, tries);
				}
			}
		}
		//print to screen
		lcd_clr();
		lcd_pos(0,0);
		lcd_puts2(printed_text);
		print_tries(tries);
		print_mode(mode);
		lcd_pos(0,14);
		lcd_put(r + 48);
		avr_wait(200);
    }
}

