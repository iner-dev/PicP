/* 
 * File:   PicP.h
 * Author: Baptiste LEGRIX
 *
 * Created on 5. octobre 2026, 15:54
 * Pic+ est une librairie de fonction pour accï¿½lï¿½rer le dev pour la microinfo en C
 */

#ifndef PICP_H
#define	PICP_H

#include <xc.h>


// liste des modules pour l'activation
#define analog_module
#define TRM0_module
//#define TRM1_module
//#define TRM2_module // incompatible avec PWM(les commandes PWM sont desactivï¿½)
//#define TRM3_module
#define CCP1_module
//#define CCP2_module
//#define aff7seg_module
#define KeyPad_module
//#define Sequancial_module
#define Virtual_Port_module
#define LCD_module  // uniquement fonctionelle avec des Virtual port





#define enable 1
#define enable_int_rising 1
#define enable_int_falling 2
#define disable 0

#define byteMerge(A,B,Mask) ((A & ~Mask)|(B & Mask)) // fusione binairemant 2 variables en fonction du mask 0:A 1:B

typedef unsigned char octet; 
typedef volatile octet t_port;

void perte_temps(int tempo); // pert des tempo processeur
void confINT(octet EN_INT0,octet EN_INT1,octet EN_INT2); //configure les intï¿½ruption 0 soit enable ou disable
void DisableADConvert();


// VirtualPort


#ifdef Virtual_Port_module

typedef octet t_portID; // alias

typedef struct{
    octet len;
    struct{
        octet portID;
        octet startID;
        octet mask;
        octet len;
    } bits[8];
} t_VirtualPort;

#define PA 0 //PORTA ID
#define PB 1 //PORTB ID
#define PC 2 //PORTC ID
#define PD 3 //PORTD ID
#define PE 4 //PORTE ID

const t_VirtualPort VPA = {1,{{PA,0,0b01111111,7}}};
const t_VirtualPort VPB = {1,{{PB,0,0b00111111,6}}};
const t_VirtualPort VPC = {1,{{PC,0,0b11111111,8}}};
const t_VirtualPort VPD = {1,{{PD,0,0b11111111,8}}};
const t_VirtualPort VPE = {1,{{PE,0,0b00000111,3}}};

void beginVPort(t_VirtualPort *VPort);
void addBitsVPort(t_VirtualPort *VPort,t_portID Port,octet startID,signed char lenght);
octet readVPort(t_VirtualPort *VPort);
octet readVPin(t_VirtualPort *VPort,octet pin);
void writeVPort(t_VirtualPort *VPort, octet data);
void writeVPin(t_VirtualPort *VPort, octet data,octet pin);
void trisVPort(t_VirtualPort *VPort, octet tris);
void trisVPin(t_VirtualPort *VPort, octet tris,octet pin);
#endif


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
#define PCFG_no_port 0b0110
#define PCFG_A0toA5 0b1001
#define PCFG_A0 0b1110



#define defaultConfAnalogTomV 21.8,5085.5

void confAnalogTomV(double min,double max);// re dï¿½finie la config de analogTomV en fonction des tentions min et max
int analogTomV(int analog); // convertie le binaire du analogRead en tention en mV en fonction de la config
void confAnalog(octet mode_10bit,octet PCFG); // config analog input
int analogRead(octet Pin); // renvoie le binaire de la lecture analogique du Pin, les paramï¿½tres de lectures sont rï¿½glï¿½ dans confAnalog
#endif

#ifdef TRM0_module
// limit ms = 16 000 000

// Prï¿½scaleurs disponibles
#define PS2_0   0b0000
#define PS4_0   0b0001
#define PS8_0   0b0010
#define PS16_0  0b0011
#define PS32_0  0b0100
#define PS64_0  0b0101
#define PS128_0 0b0110
#define PS256_0 0b0111
#define NPS_0   0b1000

void confTMR0ms(int ms,octet EN_int); // configure le TMR0 en ms
void confTMR0us(long us,octet EN_int); // configure le TMR0 en us
void confTMR0(octet PS,unsigned short reset_delay,octet EN_int); // configure le TMR0 et le lance:
void resetTMR0(void);// rï¿½ initialise le TMR0
#define startTMR0() T0CONbits.TMR0ON = 1
#define stopTMR0() T0CONbits.TMR0ON = 0

#endif

#ifdef TRM1_module

// limit us = 524 000

// Prï¿½scaleurs disponibles
#define NPS_1   0b00
#define PS2_1   0b01
#define PS4_1   0b10
#define PS8_1   0b11

void confTMR1us(long us,octet EN_int); // configure le TMR1 en us
void confTMR1(octet PS,unsigned short reset_delay,octet EN_int); // configure le TMR0 et le lance:
void resetTMR1(void);// rï¿½ initialise le TMR0
#define startTMR1() T1CONbits.TMR1ON = 1
#define stopTMR1() T1CONbits.TMR1ON = 0
unsigned short T1PropTime(octet prop);

#endif

#ifdef TRM2_module

// limit us = 65500

// Prï¿½scaleurs disponibles
#define NPRS_2   0b00
#define PRS4_2   0b01
#define PRS16_2   0b10

#define NPOS_2    0b0000
#define P0S2_2    0b0001
#define P0S3_2    0b0010
#define P0S4_2    0b0011
#define P0S5_2    0b0100
#define P0S6_2    0b0101
#define P0S7_2    0b0110
#define P0S8_2    0b0111
#define P0S9_2    0b1000
#define P0S10_2   0b1001
#define P0S11_2   0b1010
#define P0S12_2   0b1011
#define P0S13_2   0b1100
#define P0S14_2   0b1101
#define P0S15_2   0b1110
#define P0S16_2   0b1111

void confTMR2us(unsigned int us,octet EN_int);
void confTMR2(octet PRS,octet POS,octet PR_Delay,octet reset_delay,octet EN_int); 
void resetTMR2(void);
#define startTMR2() T2CONbits.TMR2ON = 1
#define stopTMR2() T2CONbits.TMR2ON = 0

#endif

#ifdef TRM3_module

// limit us = 524 000

// Prï¿½scaleurs disponibles
#define NPS_1   0b00
#define PS2_1   0b01
#define PS4_1   0b10
#define PS8_1   0b11

void confTMR3us(long us,octet EN_int); // configure le TMR3 en us
void confTMR3(octet PS,unsigned short reset_delay,octet EN_int); // configure le TMR3 et le lance:
void resetTMR3(void);// rï¿½ initialise le TMR3
#define startTMR3() T3CONbits.TMR3ON = 1
#define stopTMR3() T3CONbits.TMR3ON = 0


#endif

#ifdef CCP1_module

void stopCCP1(void);
#define resetCCP1IF() CCP1IF = 0

void confComp1(octet compareType,octet TMR_source,unsigned int compVal,octet EN_int);
void confCapt1(octet captureType,octet TMR_source,octet EN_int);
#define readCapt1() CCPR1

#ifndef TRM2_module // zone d'interdiction TMR2
#define PWM_modules
// periode max = 4000us soit 250Hz min
int confPWM1(int period_us,unsigned int prop);
#define confPWM1_8bitmode(period_us,prop) confPWM1(period_us,(int)(prop)<<2);
#endif

#define CCP_module
#endif

#ifdef CCP2_module

void stopCCP2(void);
#define resetCCP2IF() CCP2IF = 0

void confComp2(octet compareType,octet TMR_source,unsigned int compVal,octet EN_int);
void confCapt2(octet captureType,octet TMR_source,octet EN_int);
#define readCapt2() CCPR1

#ifndef TRM2_module // zone d'interdiction TMR2
#define PWM_modules
// periode max = 4000us soit 250Hz min
int confPWM2(int period_us,unsigned int prop);
#define confPWM2_8bitmode(period_us,prop) confPWM2(period_us,(int)(prop)<<2);
#endif

#define CCP_module
#endif

#ifdef CCP_module

#define CCP_TMR1_src 0
#define CCP_TMR3_src 1

#define CCP_capt_falling 0b0100
#define CCP_capt_rising 0b0101
#define CCP_capt_4th_rising 0b0110
#define CCP_capt_16th_rising 0b0111

#define CCP_comp_set_High 0b1001
#define CCP_comp_set_Low 0b1000
#define CCP_comp_toggle 0b0010
#define CCP_comp_stay 0b1010
#define CCP_comp_special 0b1011


#endif

#ifdef aff7seg_module
void conf7seg(t_port *port,t_port *dis,octet pos_dis0);// configure write7seg port etant le port des led dis le port des dis et le pos dis0 donne la pos du bit dis0 dans dis
octet convert7seg(octet chiffre ); // renvoie le code led d'un 7seg
void write7seg(int val,octet points);// affiche val sur les 7 seg configurï¿½, il y ajoute les points au formas 0b0000xxxx 1:ï¿½tind 0: alummï¿½
void writeExa7seg(unsigned int val,unsigned char points);

#ifdef Virtual_Port_module
void conf7segVP(t_VirtualPort *port,t_VirtualPort *dis);
#endif

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

void confKeyPad(t_port *port);
t_key_bitmap keyPadRead(); // effectue une lecture d'un keypad et l'ecrit au format 0bxxxxxxxxxxxxxxxx le tris du port doit etre TRISX = 0x0F;
octet isKeySet(t_key_bitmap scan,t_key_bitmap key);
t_key_bitmap keyBitmapChange(t_key_bitmap scan);
octet keyBitmapToInt(t_key_bitmap val);
octet keyBitmapToIntPL(t_key_bitmap val);
octet keyBitmapToIntPG(t_key_bitmap val);

#ifdef Virtual_Port_module
void confKeyPadVP(t_VirtualPort *port);
#endif
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

#ifdef LCD_module
#ifdef Virtual_Port_module

#define LCD_Command 0b0
#define LCD_First_Command 2
#define LCD_DATA 0b1
#define LCDMoveCurseur 0
#define LCDMoveAffichage 1
#define busy() while((LireTrameLCD(LCD_Command)&0b10000000)>0){}

octet LireTrameLCD(octet type);
void EcrireTrameLCD(octet type,char trame);
void confLCD(t_VirtualPort *DataPort,t_VirtualPort *ControlePort);
void printIntLCD(int val);
void clearLCD();
void gotoLCD(octet ligne,octet pos);
void printLCD(char* text);
void confCurseurLCD(octet DisplayAll,octet CuseurDisplay,octet Blinking);
void moveLCD(octet typeMove,octet versLaDroite);

#endif
#endif

// calcules précompilateurs partie Pic+.c

#ifdef PWM_modules
#define TRM2_module
#endif


#endif	/* PICP_H */
