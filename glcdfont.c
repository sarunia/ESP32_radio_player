#ifndef FONT5X7_H
#define FONT5X7_H

#ifdef __AVR__
 #include <avr/io.h>
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #define PROGMEM
#endif

// Standard ASCII 5x7 font

static const unsigned char font[] PROGMEM = {
0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,// 0x00							
0b00111110, 0b01011011, 0b01001111, 0b01011011, 0b00111110,// 0x01							
0b00111110, 0b01101011, 0b01001111, 0b01101011, 0b00111110,// 0x02							
0b00011100, 0b00111110, 0b01111100, 0b00111110, 0b00011100,// 0x03							
0b00011000, 0b00111100, 0b01111110, 0b00111100, 0b00011000,// 0x04							
0b00011100, 0b01010111, 0b01111101, 0b01010111, 0b00011100,// 0x05							
0b00011100, 0b01011110, 0b01111111, 0b01011110, 0b00011100,// 0x06							
0b00000000, 0b00011000, 0b00111100, 0b00011000, 0b00000000,// 0x07							
0b11111111, 0b11100111, 0b11000011, 0b11100111, 0b11111111,// 0x08							
0b00000000, 0b00011000, 0b00100100, 0b00011000, 0b00000000,// 0x09							
0b11111111, 0b11100111, 0b11011011, 0b11100111, 0b11111111,// 0x0A							
0b00110000, 0b01001000, 0b00111010, 0b00000110, 0b00001110,// 0x0B							
0b00100110, 0b00101001, 0b01111001, 0b00101001, 0b00100110,// 0x0C							
0b01000000, 0b01111111, 0b00000101, 0b00000101, 0b00000111,// 0x0D							
0b01111100, 0b00001000, 0b00000110, 0b00000101, 0b01111000,// 0x0E	ń							
0b00111000, 0b01000100, 0b01000110, 0b01000101, 0b00111000,// 0x0F	ó							
0b00000000, 0b01010001, 0b01111111, 0b01000100, 0b00000000,// 0x10	ł							
0b00100000, 0b01010100, 0b01010100, 0b11111000, 0b01000000,// 0x11	ą							
0b01001000, 0b01010100, 0b01010110, 0b01010101, 0b00100100,// 0x12	ś						
0b00111000, 0b01010100, 0b01010100, 0b11010100, 0b00011000,// 0x13	ę						
0b00111000, 0b01000100, 0b01000110, 0b01000101, 0b00101000,// 0x14	ć						
0b01000100, 0b01100100, 0b01010101, 0b01001100, 0b01000100,// 0x15	ż						
0b01000100, 0b01100100, 0b01010110, 0b01001101, 0b01000100,// 0x16	ź						
0b01111111, 0b00000100, 0b00001011, 0b00010000, 0b01111111,// 0x17	Ń							
0b00100110, 0b01001001, 0b01001011, 0b01001001, 0b00110010,// 0x18	Ś						
0b01111100, 0b00010010, 0b00010001, 0b10010010, 0b01111100,// 0x19	Ą							
0b01111111, 0b01001001, 0b01001001, 0b11001001, 0b01000001,// 0x1A	Ę							
0b00111000, 0b01000100, 0b01000110, 0b01000101, 0b00101000,// 0x1B	Ć							
0b01111111, 0b01001000, 0b01000100, 0b01000000, 0b01000000,// 0x1C	Ł							
0b01100001, 0b01011011, 0b01001001, 0b01001101, 0b01000011,// 0x1D	Ż							
0b01100101, 0b01011001, 0b01001001, 0b01001101, 0b01010011,// 0x1E	Ź							
0b00111110, 0b01000001, 0b01000011, 0b01000001, 0b00111110,// 0x1F	Ó							
0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,// 0x20	spacja					
0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,// 0x21						
0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,// 0x22							
0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,// 0x23							
0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,// 0x24							
0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,// 0x25							
0b00110110, 0b01001001, 0b01010110, 0b00100000, 0b01010000,// 0x26							
0b00000000, 0b00001000, 0b00000111, 0b00000011, 0b00000000,// 0x27							
0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,// 0x28							
0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,// 0x29							
0b00101010, 0b00011100, 0b01111111, 0b00011100, 0b00101010,// 0x2A							
0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,// 0x2B	+							
0b00000000, 0b10000000, 0b01110000, 0b00110000, 0b00000000,// 0x2C							
0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,// 0x2D							
0b00000000, 0b00000000, 0b01100000, 0b01100000, 0b00000000,// 0x2E							
0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,// 0x2F							
0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,// 0x30	0							
0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,// 0x31	1			 				
0b01110010, 0b01001001, 0b01001001, 0b01001001, 0b01000110,// 0x32	2							
0b00100001, 0b01000001, 0b01001001, 0b01001101, 0b00110011,// 0x33	3							
0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,// 0x34	4							
0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,// 0x35	5							
0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110001,// 0x36	6							
0b01000001, 0b00100001, 0b00010001, 0b00001001, 0b00000111,// 0x37	7							
0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,// 0x38	8							
0b01000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,// 0x39	9							
0b00000000, 0b00000000, 0b00010100, 0b00000000, 0b00000000,//:							
0b00000000, 0b01000000, 0b00110100, 0b00000000, 0b00000000,//;							
0b00000000, 0b00001000, 0b00010100, 0b00100010, 0b01000001,//<							
0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,//=							
0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,//>							
0b00000010, 0b00000001, 0b01011001, 0b00001001, 0b00000110,//?							
0b00111110, 0b01000001, 0b01011101, 0b01011001, 0b01001110,//@							
0b01111100, 0b00010010, 0b00010001, 0b00010010, 0b01111100,//A							
0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,//B							
0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,//C							
0b01111111, 0b01000001, 0b01000001, 0b01000001, 0b00111110,//D							
0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,//E							
0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,//F							
0b00111110, 0b01000001, 0b01000001, 0b01010001, 0b01110011,//G							
0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,//H							
0b00000000, 0b01000001, 0b01111111, 0b01000001, 0b00000000,//I							
0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,//J							
0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,//K							
0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,//L							
0b01111111, 0b00000010, 0b00011100, 0b00000010, 0b01111111,//M							
0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,//N							
0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,//O							
0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,//P							
0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,//Q							
0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,//R							
0b00100110, 0b01001001, 0b01001001, 0b01001001, 0b00110010,//S							
0b00000011, 0b00000001, 0b01111111, 0b00000001, 0b00000011,//T							
0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,//U							
0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,//V							
0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,//W							
0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,//X							
0b00000011, 0b00000100, 0b01111000, 0b00000100, 0b00000011,//Y							
0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,//Z							
0b00000000, 0b01111111, 0b01000001, 0b01000001, 0b01000001,//[							
0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000,//slash							
0b00000000, 0b01000001, 0b01000001, 0b01000001, 0b01111111,//]							
0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,//^							
0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,//_							
0b00000000, 0b00000011, 0b00000111, 0b00001000, 0b00000000,//apostrof							
0b00100000, 0b01010100, 0b01010100, 0b01111000, 0b01000000,//a							
0b01111111, 0b00101000, 0b01000100, 0b01000100, 0b00111000,//b							
0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00101000,//c							
0b00111000, 0b01000100, 0b01000100, 0b00101000, 0b01111111,//d							
0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,//e							
0b00000000, 0b00001000, 0b01111110, 0b00001001, 0b00000010,//f							
0b00011000, 0b10100100, 0b10100100, 0b10011100, 0b01111000,//g							
0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,//h							
0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,//i							
0b00100000, 0b01000000, 0b01000000, 0b00111101, 0b00000000,//j							
0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,//k							
0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,//l							
0b01111100, 0b00000100, 0b01111000, 0b00000100, 0b01111000,//m							
0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,//n							
0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,//o							
0b11111100, 0b00011000, 0b00100100, 0b00100100, 0b00011000,//p							
0b00011000, 0b00100100, 0b00100100, 0b00011000, 0b11111100,//q							
0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,//r							
0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100100,//s							
0b00000100, 0b00000100, 0b00111111, 0b01000100, 0b00100100,//t							
0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,//u							
0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,//v							
0b00111100, 0b01000000, 0b00110000, 0b01000000, 0b00111100,//w							
0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,//x							
0b01001100, 0b10010000, 0b10010000, 0b10010000, 0b01111100,//y							
0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,//z							
0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,//{							
0b00000000, 0b00000000, 0b01110111, 0b00000000, 0b00000000,//| 							
0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,//}							
0b00000010, 0b00000001, 0b00000010, 0b00000100, 0b00000010,//~							
0b01000100, 0b01100100, 0b01010101, 0b01001100, 0b01000100,//ż							
0b01100001, 0b01011011, 0b01001001, 0b01001101, 0b01000011,//Ż							
0b01000100, 0b01100100, 0b01010110, 0b01001101, 0b01000100,//ź							
0b01100101, 0b01011001, 0b01001001, 0b01001101, 0b01010011,//Ź							
0b11111111, 0b00000001, 0b00000111, 0b00000001, 0b11111111,//							
0b11111111, 0b00000001, 0b00000111, 0b00000011, 0b11111111,//							
0b11111111, 0b00000001, 0b00000111, 0b00000111, 0b11111111,//							
0b11111111, 0b00000001, 0b00000111, 0b00001111, 0b11111111,//							
0b11111111, 0b00000001, 0b00000111, 0b00011111, 0b11111111,//							
0b11111111, 0b00000001, 0b00000111, 0b00111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00000111, 0b01111111, 0b11111111,//							
0b00111000, 0b01000100, 0b01000110, 0b01000101, 0b00111000,//						
0b11111111, 0b00000001, 0b00000111, 0b10111111, 0b11111111,//							
0b00100110, 0b01001001, 0b01001011, 0b01001001, 0b00110010,//Ś	zrobione					
0b11111111, 0b00000001, 0b00001111, 0b00000001, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b00000011, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b00000111, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b00001111, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b00011111, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b00111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b01111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b11111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00001111, 0b10111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00011111, 0b00000000, 0b11111111,//							
0b11111111, 0b00000001, 0b00011111, 0b00000001, 0b11111111,//							
0b11111111, 0b00000001, 0b00011111, 0b00000011, 0b11111111,//							
0b11111111, 0b00000001, 0b00011111, 0b00000111, 0b11111111,//							
0b11111111, 0b00000001, 0b00011111, 0b00001111, 0b11111111,//							
0b11111111, 0b00000001, 0b00011111, 0b00011111, 0b11111111,//							
0b01001000, 0b01010100, 0b01010110, 0b01010101, 0b00100100,//ś	zrobione					
0b01001000, 0b01010100, 0b01010110, 0b01010101, 0b00100100,//ś	zrobiony duplikat do wyjaśnienia							
0b00100100, 0b00110110, 0b01111111, 0b00110110, 0b00100100,//							
0b01000100, 0b01100100, 0b01010110, 0b01001101, 0b01000100,//ź	zrobione 9E			
0b01000100, 0b01100100, 0b01010110, 0b01001101, 0b01000100,//				
0b00100110, 0b01001001, 0b01001011, 0b01001001, 0b00110010,//Ś							
0b11111111, 0b00000001, 0b00111111, 0b00000011, 0b11111111,//							
0b00111000, 0b01010100, 0b01010100, 0b11010100, 0b00011000,//ę							
0b01111111, 0b01001001, 0b01001001, 0b11001001, 0b01000001,//Ę							
0b11111111, 0b00000001, 0b00111111, 0b00011111, 0b11111111,//							
0b11111111, 0b00000001, 0b00111111, 0b00111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00111111, 0b01111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00111111, 0b11111111, 0b11111111,//							
0b11111111, 0b00000001, 0b00111111, 0b10111111, 0b11111111,//							
0b00111110, 0b01000001, 0b01000011, 0b01000001, 0b00111110,//Ó							
0b11111111, 0b00000001, 0b01111111, 0b00000001, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b00000011, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b00000111, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b00001111, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b00011111, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b00111111, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b01111111, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b11111111, 0b11111111,//							
0b11111111, 0b00000001, 0b01111111, 0b10111111, 0b11111111,//							
0b00000000, 0b01010001, 0b01111111, 0b01000100, 0b00000000,//ł	zrobione 0xB3							
0b00111110, 0b01000001, 0b01000011, 0b01000001, 0b00100010,//Ć	
0b00111000, 0b01000100, 0b01000110, 0b01000101, 0b00101000,//ć	zrobione 0xB5						
0b00000000, 0b01010001, 0b01111111, 0b01000100, 0b00000000,//ł							
0b01111100, 0b00001000, 0b00000110, 0b00000101, 0b01111000,//ń							
0b01111100, 0b00010010, 0b00010001, 0b10010010, 0b01111100,//Ą							
0b00100000, 0b01010100, 0b01010100, 0b11111000, 0b01000000,//ą	zrobione				
0b01111111, 0b01001000, 0b01000100, 0b01000000, 0b01000000,//Ł							
0b01111111, 0b00000100, 0b00001011, 0b00010000, 0b01111111,//Ń	
0b11111111, 0b00000001, 0b11111111, 0b10111111, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b00000000, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b00000001, 0b11111111,//							
0b01000100, 0b01100100, 0b01010101, 0b01001100, 0b01000100,//ż	zrobione					
0b11111111, 0b00000001, 0b10111111, 0b00000111, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b00001111, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b00011111, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b00111111, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b01111111, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b11111111, 0b11111111,//							
0b11111111, 0b00000001, 0b10111111, 0b10111111, 0b11111111,//					
0b11111111, 0b00000011, 0b00000000, 0b00000000, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b00000001, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b00000011, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b00000111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b00001111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b00011111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b00111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b01111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b11111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000000, 0b10111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b00000000, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b00000001, 0b11111111,//							
0b00111110, 0b01000001, 0b01000011, 0b01000001, 0b00111110,//Ó	zrobione							
0b11111111, 0b00000011, 0b00000001, 0b00000111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b00001111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b00011111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b00111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b01111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b11111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000001, 0b10111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b00000000, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b00000001, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b00000011, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b00000111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b00001111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b00011111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b00111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b01111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b11111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000011, 0b10111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b00000000, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b00000001, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b00000011, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b00000111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b00001111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b00011111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b00111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b01111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b11111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00000111, 0b10111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00001111, 0b00000000, 0b11111111,//							
0b11111111, 0b00000011, 0b00001111, 0b00000001, 0b11111111,//							
0b01111100, 0b00001000, 0b00000110, 0b00000101, 0b01111000,//ń	zrobione						
0b11111111, 0b00000011, 0b00001111, 0b00000111, 0b11111111,//							
0b00111000, 0b01000100, 0b01000110, 0b01000101, 0b00111000,//ó	zrobione						
0b11111111, 0b00000011, 0b00001111, 0b00011111, 0b11111111,//							
0b11111111, 0b00000011, 0b00001111, 0b00111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00001111, 0b01111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00001111, 0b11111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00001111, 0b10111111, 0b11111111,//							
0b11111111, 0b00000011, 0b00011111, 0b00000000, 0b11111111,//							
0b11111111, 0b00000011, 0b00011111, 0b00000001, 0b11111111,//							
0b11111111, 0b00000011, 0b00011111, 0b00000011, 0b11111111,//							
0b11111111, 0b00000011, 0b00011111, 0b00000111, 0b11111111,//							
0b11111111, 0b00000011, 0b00011111, 0b00001111, 0b11111111,//							
0b11111111, 0b00000011, 0b00011111, 0b00011111, 0b11111111,//
};
#endif // FONT5X7_H