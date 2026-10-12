#include "PicP.h"

void confINT0(octet EN_TMR0,octet EN_INT0){
    INTCONbits.GIE    = 1;// Global Interrupt Enable bit
    INTCONbits.PEIE   = 1;// Peripheral Interrupt Enable bit
    INTCONbits.TMR0IE = EN_TMR0==1;// TMR0 Overflow Interrupt Enable bit
    INTCONbits.INT0IE = EN_INT0==1;// INT0 External Interrupt Enable bit
    INTCONbits.RBIE   = 0;// RB Port Change Interrupt Enable bit
    INTCONbits.TMR0IF = 0;// TMR0 Overflow Interrupt Flag bit
    INTCONbits.INT0IF = 0;// INT0 External Interrupt Flag bit
    INTCONbits.RBIF   = 0;// RB Port Change Interrupt Flag bit 
}

void perte_temps(int tempo){
    for(;tempo>0;tempo--);
}

#ifdef analog_module

double gain_ATmV;
double len_ATmv = 5063.7;
double offset_ATmV = 21.8;

void confAnalogTomV(double min,double max){
    offset_ATmV = min; 
    len_ATmv = max-min;
    gain_ATmV = len_ATmv/(ADFM?1023.:255.);
}

int analogTomV(int analog){
    return analog*gain_ATmV + offset_ATmV;
}

void confAnalog(octet mode_10bit,octet PCFG){
    ADCON0 = 0b01000001;
    ADCON1 = (0b10000000*mode_10bit)|PCFG;
    gain_ATmV = len_ATmv/(ADFM?1023.:255.);
}

int analogRead(octet Pin){
    int ret;
    ADCON0 = byteMerge(ADCON0,Pin<<3,0b00111000);
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO==1);
    if(ADCON1bits.ADFM){
        //mode 10bit
        ret = ADRES;
    }else{
        //mode 8bit
        ret = ADRESH;
    }
    return ret;
}
#endif

#ifdef TRM0_module
unsigned short reset_T0=0;

void resetTMR0(){
    TMR0 = reset_T0;
    TMR0IF = 0;
}

void confTMR0(octet PS,unsigned short reset_delay){
    T0CON = byteMerge(0b10000000,PS,0b00001111);
    reset_T0 = reset_delay;
}

void confTMR0ms(int ms){
    if(ms>16000) ms = 16000;
    long calc = (long)(((long)(ms)*1000l)/65535l);
    octet PS = 1,PS_bin=0;
    while(calc>0){
        calc = (calc&(~1))>>1;
        PS = PS<<1;
        PS_bin++;
    }
    if(PS_bin==0){
        PS_bin = NPS_0;
    }else{
        PS_bin--;
    }
    confTMR0(PS_bin,65535- ((long)(ms)*1000l)/PS);
}
#endif

#ifdef aff7seg_module
// 7seg config save
volatile unsigned char *port_7seg;
volatile unsigned char *dis_7seg;
unsigned char mask_dis0_7seg;
unsigned char mask_dis1_7seg;
unsigned char mask_dis2_7seg;
unsigned char mask_dis3_7seg;

octet convert7seg(octet chiffre ){
    chiffre = chiffre & 0b00001111;
    switch(chiffre){
        case 0 : return 0b00000011; 
        case 1 : return 0b10011111;
        case 2 : return 0b00100101;
        case 3 : return 0b00001101;
        case 4 : return 0b10011001;
        case 5 : return 0b01001001;
        case 6 : return 0b01000001;
        case 7 : return 0b00011111;
        case 8 : return 0b00000001;
        case 9 : return 0b00001001;
        case 10 : return 0b00010001;
        case 11 : return 0b11000001;
        case 12 : return 0b01100011;
        case 13 : return 0b10000101;
        case 14 : return 0b01100001;
        case 15 : return 0b01110001;
        default : return 0b11111111;
    }
}

void conf7seg(t_port *port,t_port *dis,octet pos_dis0){
    port_7seg=port;
    dis_7seg=dis;
    mask_dis0_7seg = (unsigned char)(0b00000001 << pos_dis0);
    mask_dis1_7seg = (unsigned char)(0b00000001 << (pos_dis0+1));
    mask_dis2_7seg = (unsigned char)(0b00000001 << (pos_dis0+2));
    mask_dis3_7seg = (unsigned char)(0b00000001 << (pos_dis0+3));
}

void write7seg(int val,unsigned char points){
    // DIS3 (le plus à droite)
    
    *port_7seg = byteMerge(convert7seg(val%10),~points,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis3_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis3_7seg;// set disN a 1
    val= val/10;
    
    // DIS2 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),(~points)>>1,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis2_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis2_7seg;// set disN a 1
    val= val/10;
    
    // DIS1 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),(~points)>>2,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis1_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis1_7seg;// set disN a 1
    val= val/10;
    
    // DIS0 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),(~points)>>3,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis0_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis0_7seg;// set disN a 1
}
#endif

#ifdef KeyPad_module

t_key_bitmap keyPadRead(t_port *port){
    unsigned int ret=0;
    char i;
    *port = 0b00010000;
    ret=(ret|(*port & 0b00001111));
    for(i=1;i<4;i++){
        *port = (unsigned char)(0b00010000<<i);
        ret=ret<<4;
        ret=ret|(*port & 0b00001111);
    }
    return ret;
}

octet isKeySet(t_key_bitmap scan,t_key_bitmap key){
    return (scan & key)>0;
}

t_key_bitmap keyBitmapChange(t_key_bitmap scan){
    static t_key_bitmap lastScan = 0;
    t_key_bitmap ret = scan^lastScan;
    lastScan = scan;
    return ret;
}

octet keyBitmapToInt(t_key_bitmap val){
    switch(val){
        case KEY_0 :       return 0;
        case KEY_1 :       return 1;
        case KEY_2 :       return 2;
        case KEY_3 :       return 3;
        case KEY_4 :       return 4;
        case KEY_5 :       return 5;
        case KEY_6 :       return 6;
        case KEY_7 :       return 7;
        case KEY_8 :       return 8;
        case KEY_9 :       return 9;
        case KEY_A :       return 10;
        case KEY_B :       return 11;
        case KEY_C :       return 12;
        case KEY_D :       return 13;
        case KEY_Star :    return 14;
        case KEY_Hashtag : return 15;
        default:           return Default_KBTI;
    }
}
#endif

#ifdef Sequancial_module
void initSequance(t_sequance *seq){
    seq->len=0;
    seq->pos=0;
    seq->tab[0]=0;
}

void addSequanceStep(t_sequance *seq,t_Step Step){
    if(seq->len<maxSeqStep){
        seq->tab[seq->len++]= Step;
    }
}

t_Step SequanceStep(t_sequance *seq){
    if(seq->pos >= seq->len) seq->pos=0;
    return seq->tab[seq->pos++];
}

void SequanceSetPos(t_sequance* seq, octet pos){
    seq->pos = pos;
}
#endif