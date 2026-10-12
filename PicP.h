/* 
 * File:   PicP.h
 * Author: Baptiste LEGRIX
 *
 * Created on 5. octobre 2026, 15:54
 * Pic+ est une librairie de fonction pour accélérer le dev pour la microinfo en C
 */

#ifndef PICP_H
#define	PICP_H

#include <xc.h>

#define enable 1
#define disable 0

#define byteMerge(A,B,Mask) ((A & ~Mask)|(B & Mask)) // fusione binairemant 2 variables en fonction du mask 0:A 1:B

typedef volatile unsigned char t_port;
typedef unsigned char octet; 

void perte_temps(int tempo); // pert des tempo processeur
void confINT0(octet EN_TMR0,octet EN_INT0); //configure les intéruption 0 soit enable ou disable


#define analog_module
#define TRM0_module
#define aff7seg_module
#define KeyPad_module
#define Sequancial_module



#ifdef analog_module 
// port analogiques dispo
#define AN0 0b000
#define AN1 0b001
#define AN2 0b010
#define AN3 0b011
#define AN4 0b100
#define AN5 0b101
#define AN6 0b110
#define AN7 0b111

#define PCFG_ALL 0b0000
#define PCFG_A0_A4 0b0010
#define PCFG_A0_1_3 0b0100
#define PCFG_no_port 0b011x
#define PCFG_A0toA5 0b1001
#define PCFG_A0 0b1110



#define defaultConfAnalogTomV 21.8,5085.5

void confAnalogTomV(double min,double max);// re définie la config de analogTomV en fonction des tentions min et max
int analogTomV(int analog); // convertie le binaire du analogRead en tention en mV en fonction de la config
void confAnalog(octet mode_10bit,octet PCFG); // config analog input
int analogRead(octet Pin); // renvoie le binaire de la lecture analogique du Pin, les paramétres de lectures sont réglé dans confAnalog
#endif

#ifdef TRM0_module
// Préscaleurs disponibles
#define PS2_0   0b0000
#define PS4_0   0b0001
#define PS8_0   0b0010
#define PS16_0  0b0011
#define PS32_0  0b0100
#define PS64_0  0b0101
#define PS128_0 0b0110
#define PS256_0 0b0111
#define NPS_0   0b1000

void confTMR0ms(int ms); // configure le TMR0 en ms
void confTMR0(octet PS,unsigned short reset_delay); // configure le TMR0 et le lance:
void resetTMR0(void);// ré initialise le TMR0
#endif

#ifdef aff7seg_module
void conf7seg(t_port *port,t_port *dis,octet pos_dis0);// configure write7seg port etant le port des led dis le port des dis et le pos dis0 donne la pos du bit dis0 dans dis
octet convert7seg(octet chiffre ); // renvoie le code led d'un 7seg
void write7seg(int val,octet points);// affiche val sur les 7 seg configuré, il y ajoute les points au formas 0b0000xxxx 1:étind 0: alummé
#endif

#ifdef KeyPad_module
#define Default_KBTI 16

// key du keypad
#define KEY_Star    0b0000000000000001
#define KEY_0       0b0000000000000010
#define KEY_Hashtag 0b0000000000000100
#define KEY_D       0b0000000000001000
#define KEY_7       0b0000000000010000
#define KEY_8       0b0000000000100000
#define KEY_9       0b0000000001000000
#define KEY_C       0b0000000010000000
#define KEY_4       0b0000000100000000
#define KEY_5       0b0000001000000000
#define KEY_6       0b0000010000000000
#define KEY_B       0b0000100000000000
#define KEY_1       0b0001000000000000
#define KEY_2       0b0010000000000000
#define KEY_3       0b0100000000000000
#define KEY_A       0b1000000000000000
#define all_KEY     0b1111111111111111


typedef unsigned int t_key_bitmap;


t_key_bitmap keyPadRead(t_port *port); // effectue une lecture d'un keypad et l'ecrit au format 0bxxxxxxxxxxxxxxxx le tris du port doit etre TRISX = 0x0F;
octet isKeySet(t_key_bitmap scan,t_key_bitmap key);
t_key_bitmap keyBitmapChange(t_key_bitmap scan);
octet keyBitmapToInt(t_key_bitmap val);
#endif

#ifdef Sequancial_module

typedef octet t_Step;
#define maxSeqStep 16
typedef struct {
    octet pos;
    octet len;
    t_Step tab[maxSeqStep];
}t_sequance;

void initSequance(t_sequance *seq);
void addSequanceStep(t_sequance *seq,t_Step Step);
t_Step SequanceStep(t_sequance *seq);
void SequanceSetPos(t_sequance* seq, octet pos);
#endif

#endif	/* PICP_H */

