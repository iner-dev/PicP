#include "PicP.h"

void confINT(octet EN_INT0,octet EN_INT1,octet EN_INT2){
    INTCONbits.GIE    = 1;// Global Interrupt Enable bit
    INTCONbits.PEIE   = 1;// Peripheral Interrupt Enable bit
    INTCONbits.RBIE   = 0;// RB Port Change Interrupt Enable bit
    
    INTCONbits.INT0IE = EN_INT0>0;// INT0 External Interrupt Enable bit
    INTCON3bits.INT1IE = EN_INT1>0;// INT1 External Interrupt Enable bit
    INTCON3bits.INT2IE = EN_INT2>0;// INT2 External Interrupt Enable bit
    
    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT2IF = 0;
    
    INTCON2bits.INTEDG0 = 1&EN_INT0;
    INTCON2bits.INTEDG1 = 1&EN_INT1;
    INTCON2bits.INTEDG2 = 1&EN_INT2;
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
    perte_temps(100);
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

void confTMR0(octet PS,unsigned short reset_delay,octet EN_int){
    if(EN_int>0){
        INTCONbits.GIE    = 1;// Global Interrupt Enable bit
        INTCONbits.PEIE   = 1;// Peripheral Interrupt Enable bit
    }
    INTCONbits.TMR0IE = EN_int>0;
    INTCONbits.TMR0IF = 0;
    T0CON = byteMerge(0b10000000,PS,0b00001111);
    reset_T0 = reset_delay;
}

void confTMR0ms(int ms,octet EN_int){
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
    confTMR0(PS_bin,65535- ((long)(ms)*1000l)/PS,EN_int);
}

void confTMR0us(long us,octet EN_int){
    if(us>16000000) us = 16000000;
    long calc = (long)(us/65535l);
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
    confTMR0(PS_bin,65535- us/PS,EN_int);
}


#endif

#ifdef TRM1_module
unsigned short reset_T1=0;

void resetTMR1(){
    TMR1 = reset_T1;
    TMR1IF = 0;
}

void confTMR1(octet PS,unsigned short reset_delay,octet EN_int){
    if(EN_int>0){
        INTCONbits.GIE    = 1;// Global Interrupt Enable bit
        INTCONbits.PEIE   = 1;// Peripheral Interrupt Enable bit
    }
    
    PIE1bits.TMR1IE = EN_int>0;
    PIR1bits.TMR1IF = 0;
    TMR1IP = 1;
    T1CON = byteMerge(0b10000001,PS<<4,0b00110000);
    reset_T1 = reset_delay;
}

void confTMR1us(long us,octet EN_int){
    if(us>524000) us = 524000;
    long calc = (long)(us/65535l);
    octet PS = 1,PS_bin=0;
    while(calc>0){
        calc = (calc&(~1))>>1;
        PS = PS<<1;
        PS_bin++;
    }
    confTMR1(PS_bin,65535- us/PS,EN_int);
}

unsigned short T1PropTime(octet prop){
    return reset_T1+(unsigned)((long)((65535-reset_T1)*prop)>>8);
}
#endif

#ifdef TRM2_module
octet reset_T2=0;

void confTMR2(octet PRS,octet POS,octet PR_Delay,octet reset_delay,octet EN_int){
    if(EN_int>0){
        INTCONbits.GIE    = 1;// Global Interrupt Enable bit
        INTCONbits.PEIE   = 1;// Peripheral Interrupt Enable bit
    }
    
    PIE1bits.TMR2IE = EN_int>0;
    PIR1bits.TMR2IF = 0;
    TMR2IP = 1;
    T2CON = byteMerge(0b00000100,POS<<3,0b01111000)|(PRS & 0b00000011);
    PR2 = PR_Delay;
    reset_T2 = reset_delay;
}

void confTMR2us(unsigned int us,octet EN_int){
    if(us>65500) us = 65500;
    int calc = us/4096;
    octet PS = 1,PRS_bin=0;
    while(calc>0){
        calc = (calc&(~0b11))>>2;
        PS = PS<<2;
        PRS_bin += 1;
    }
    us = us/PS;
    PS = (octet)(us/256);
    confTMR2(PRS_bin,PS,0xFF,(octet)((256*(PS+1))-us+1),EN_int);
}

void resetTMR2(void){
    TMR2 += reset_T2;
    TMR2IF = 0;
}

#endif

#ifdef TRM3_module
unsigned short reset_T3=0;

void resetTMR3(){
    TMR3 = reset_T3;
    TMR3IF = 0;
}

void confTMR3(octet PS,unsigned short reset_delay,octet EN_int){
    if(EN_int>0){
        INTCONbits.GIE    = 1;// Global Interrupt Enable bit
        INTCONbits.PEIE   = 1;// Peripheral Interrupt Enable bit
    }
    
    TMR3IE = EN_int>0;
    TMR3IF = 0;
    TMR3IP = 1;
    T3CONbits.NOT_T3SYNC = 0;
    T3CONbits.RD16 = 1;
    T3CONbits.TMR3CS = 0;
    T3CONbits.TMR3ON = 1;
    T3CON = byteMerge(T3CON,PS<<4,0b00110000);
    reset_T3 = reset_delay;
}

void confTMR3us(long us,octet EN_int){
    if(us>524000) us = 524000;
    long calc = (long)(us/65535l);
    octet PS = 1,PS_bin=0;
    while(calc>0){
        calc = (calc&(~1))>>1;
        PS = PS<<1;
        PS_bin++;
    }
    confTMR3(PS_bin,65535- us/PS,EN_int);
}
#endif


#ifdef CCP1_module

void stopCCP1(void){
    CCP1CON = 0;
    CCP1IF = 0;
    CCP1IE = 0;
}

void confCapt1(octet captureType,octet TMR_source,octet EN_int){
    CCP1CON = captureType&0b00001111;
    T3CCP2 = TMR_source>0;
    CCP1IF = 0;
    CCP1IP = 1;
    CCP1IE = EN_int>0;
}

void confComp1(octet compareType,octet TMR_source,unsigned int compVal,octet EN_int){
    CCPR1 = compVal;
    confCapt1(compareType,TMR_source,EN_int);
}

#ifdef PWM_modules // zone d'interdiction TMR2
// periode [4;4000]us soit [250Hz;250Khz] min
int confPWM1(int period_us,unsigned int prop){
    if(period_us>4000) period_us = 4000;
    long calc = (period_us&0xFF00)>>8;
    octet PS = 1,PRS_bin=0;
    while(calc>0){
        calc = (calc&(~0b11))>>2;
        PS = PS<<2;
        PRS_bin += 1;
    }
    calc = period_us/PS -1;
    confTMR2(PRS_bin,0,calc,0,disable);
    calc = ((calc*prop)&0xFFFFFF00)>>8;
    CCP1CON = 0b00001100|(octet)((calc<<4)&0b00110000);
    CCPR1L = (octet)(calc>>2);
}

#endif
#endif


#ifdef CCP2_module

void stopCCP2(void){
    CCP2CON = 0;
    CCP2IF = 0;
    CCP2IE = 0;
}

void confCapt2(octet captureType,octet TMR_source,octet EN_int){
    CCP2CON = captureType&0b00001111;
    T3CCP1 = TMR_source>0;
    if(CCP_TMR1_src == T3CCP1) T3CCP2 = CCP_TMR1_src;
    CCP2IF = 0;
    CCP2IP = 1;
    CCP2IE = EN_int>0;
}

void confComp1(octet compareType,octet TMR_source,unsigned int compVal,octet EN_int){
    CCPR2 = compVal;
    confCapt2(compareType,TMR_source,EN_int);
}

#ifdef PWM_modules // zone d'interdiction TMR2
// periode [4;4000]us soit [250Hz;250Khz] min
int confPWM2(int period_us,unsigned int prop){
    if(period_us>4000) period_us = 4000;
    long calc = (period_us&0xFF00)>>8;
    octet PS = 1,PRS_bin=0;
    while(calc>0){
        calc = (calc&(~0b11))>>2;
        PS = PS<<2;
        PRS_bin += 1;
    }
    calc = period_us/PS -1;
    confTMR2(PRS_bin,0,calc,0,disable);
    calc = ((calc*prop)&0xFFFFFF00)>>8;
    CCP2CON = 0b00001100|(octet)((calc<<4)&0b00110000);
    CCPR2L = (octet)(calc>>2);
}

#endif
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
    
    *port_7seg = byteMerge(convert7seg(val%10),points,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis3_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis3_7seg;// set disN a 1
    val= val/10;
    
    // DIS2 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),points>>1,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis2_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis2_7seg;// set disN a 1
    val= val/10;
    
    // DIS1 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),points>>2,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis1_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis1_7seg;// set disN a 1
    val= val/10;
    
    // DIS0 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),points>>3,0b00000001);
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