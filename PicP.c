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

void DisableADConvert(){
    ADCON1bits.PCFG = 0b0110;
    ADCON0bits.ADON = 0;
}

#ifdef Virtual_Port_module

void beginVPort(t_VirtualPort *VPort){
    VPort->len = 0;
}

void addBitsVPort(t_VirtualPort *VPort,t_portID Port,octet startID,signed char lenght){
    VPort->bits[VPort->len].startID = startID;
    if(lenght<0){
        //cas negatif
        VPort->bits[VPort->len].portID = Port+5;
        startID += lenght+1;
        lenght=-lenght;
    }else{
        //cas positif
        VPort->bits[VPort->len].portID = Port;
    }
    VPort->bits[VPort->len].mask = 0b1<<startID;
    VPort->bits[VPort->len].len = lenght;
    for(octet i=1;i<lenght;i++){
        VPort->bits[VPort->len].mask = VPort->bits[VPort->len].mask | (VPort->bits[VPort->len].mask<<1);
    }
    VPort->len++;
}

octet reverse_bits(octet n) {
    // 1. Échange les blocs de 4 bits (nibbles)
    n = ((n >> 4) & 0x0F) | ((n << 4) & 0xF0);
    // 2. Échange les blocs de 2 bits
    n = ((n >> 2) & 0x33) | ((n << 2) & 0xCC);
    // 3. Échange les bits adjacents
    n = ((n >> 1) & 0x55) | ((n << 1) & 0xAA);
    return n;
}

octet readChunk(t_portID portID,octet mask,octet startID,octet len){
    switch(portID){
        case 0  : return (PORTA&mask)>>startID; //PortA
        case 1  : return (PORTB&mask)>>startID; //PortB
        case 2  : return (PORTC&mask)>>startID; //PortC
        case 3  : return (PORTD&mask)>>startID; //PortD
        case 4  : return (PORTE&mask)>>startID; //PortE
        case 5  : return reverse_bits(PORTA&mask)>>(8-startID-len); //RevPortA
        case 6  : return reverse_bits(PORTB&mask)>>(8-startID-len); //RevPortB
        case 7  : return reverse_bits(PORTC&mask)>>(8-startID-len); //RevPortC
        case 8  : return reverse_bits(PORTD&mask)>>(8-startID-len); //RevPortD
        case 9  : return reverse_bits(PORTE&mask)>>(8-startID-len); //RevPortE
        default : return 0;
    }
}

void writeChunk(t_portID portID,octet mask,octet startID,octet len,octet val){
    switch(portID){
        case 0  : PORTA = byteMerge(PORTA,val<<startID,mask); return; //PortA
        case 1  : PORTB = byteMerge(PORTB,val<<startID,mask); return; //PortA
        case 2  : PORTC = byteMerge(PORTC,val<<startID,mask); return; //PortA
        case 3  : PORTD = byteMerge(PORTD,val<<startID,mask); return; //PortA
        case 4  : PORTE = byteMerge(PORTE,val<<startID,mask); return; //PortA
        case 5  : PORTA = byteMerge(PORTA,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 6  : PORTB = byteMerge(PORTB,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 7  : PORTC = byteMerge(PORTC,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 8  : PORTD = byteMerge(PORTD,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 9  : PORTE = byteMerge(PORTE,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        default : return ;
    }
}

void trisChunk(t_portID portID,octet mask,octet startID,octet len,octet val){
    switch(portID){
        case 0  : TRISA = byteMerge(TRISA,val<<startID,mask); return; //PortA
        case 1  : TRISB = byteMerge(TRISB,val<<startID,mask); return; //PortA
        case 2  : TRISC = byteMerge(TRISC,val<<startID,mask); return; //PortA
        case 3  : TRISD = byteMerge(TRISD,val<<startID,mask); return; //PortA
        case 4  : TRISE = byteMerge(TRISE,val<<startID,mask); return; //PortA
        case 5  : TRISA = byteMerge(TRISA,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 6  : TRISB = byteMerge(TRISB,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 7  : TRISC = byteMerge(TRISC,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 8  : TRISD = byteMerge(TRISD,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        case 9  : TRISE = byteMerge(TRISE,reverse_bits(val)>>(8-startID-len),mask); return ; //RevPortA
        default : return ;
    }
}

octet readVPort(t_VirtualPort *VPort){
    octet ret=0,lenCalc=0;
    for(octet i = 0;i<(VPort->len);i++){
        ret += readChunk(
                VPort->bits[i].portID,
                VPort->bits[i].mask,
                VPort->bits[i].startID,
                VPort->bits[i].len)<<lenCalc;
        lenCalc += VPort->bits[i].len;
    }
    return ret;
}

void writeVPort(t_VirtualPort *VPort, octet data){
    octet lenCalc=0;
    for(octet i = 0;i<(VPort->len);i++){
        writeChunk(
                VPort->bits[i].portID,
                VPort->bits[i].mask,
                VPort->bits[i].startID,
                VPort->bits[i].len,
                data>>lenCalc
                );
        lenCalc += VPort->bits[i].len;
    }
}

void trisVPort(t_VirtualPort *VPort, octet tris){
    octet lenCalc=0;
    for(octet i = 0;i<(VPort->len);i++){
        trisChunk(
            VPort->bits[i].portID,
            VPort->bits[i].mask,
            VPort->bits[i].startID,
            VPort->bits[i].len,
            tris>>lenCalc
        );
        lenCalc += VPort->bits[i].len;
    }
}

void writeVPin(t_VirtualPort *VPort, octet data,octet pin){
    octet lenCalc=0,pos = 0;
    for(octet i = 0;i<(VPort->len);i++){
        if((lenCalc+VPort->bits[i].len)>pin){
            //on a le bon chunk
            pos = (pin-lenCalc)*((lenCalc+VPort->bits[i].portID)>=5 ? -1 : 1) + VPort->bits[i].startID;
            switch(VPort->bits[i].portID%5){
                case 0  : PORTA = byteMerge(PORTA,data<<pos,0b1<<pos); return; //PortA
                case 1  : PORTB = byteMerge(PORTB,data<<pos,0b1<<pos); return; //PortB
                case 2  : PORTC = byteMerge(PORTC,data<<pos,0b1<<pos); return; //PortC
                case 3  : PORTD = byteMerge(PORTD,data<<pos,0b1<<pos); return; //PortD
                case 4  : PORTE = byteMerge(PORTE,data<<pos,0b1<<pos); return; //PortE
            }
        }
        lenCalc += VPort->bits[i].len;
    }
}

void trisVPin(t_VirtualPort *VPort, octet tris,octet pin){
    octet lenCalc=0,pos = 0;
    for(octet i = 0;i<(VPort->len);i++){
        if((lenCalc+VPort->bits[i].len)>pin){
            //on a le bon chunk
            pos = (pin-lenCalc)*((lenCalc+VPort->bits[i].portID)>=5 ? -1 : 1) + VPort->bits[i].startID;
            switch(VPort->bits[i].portID%5){
                case 0  : TRISA = byteMerge(TRISA,tris<<pos,0b1<<pos); return; //PortA
                case 1  : TRISB = byteMerge(TRISB,tris<<pos,0b1<<pos); return; //PortB
                case 2  : TRISC = byteMerge(TRISC,tris<<pos,0b1<<pos); return; //PortC
                case 3  : TRISD = byteMerge(TRISD,tris<<pos,0b1<<pos); return; //PortD
                case 4  : TRISE = byteMerge(TRISE,tris<<pos,0b1<<pos); return; //PortE
            }
        }
        lenCalc += VPort->bits[i].len;
    }
}

octet readVPin(t_VirtualPort *VPort,octet pin){
    octet lenCalc=0,pos = 0;
    for(octet i = 0;i<(VPort->len);i++){
        if((lenCalc+VPort->bits[i].len)>pin){
            //on a le bon chunk
            pos = (pin-lenCalc)*((lenCalc+VPort->bits[i].portID)>=5 ? -1 : 1) + VPort->bits[i].startID;
            switch(VPort->bits[i].portID%5){
                case 0  : return PORTA&(0b1<<pos)>0; //PortA
                case 1  : return PORTB&(0b1<<pos)>0; //PortB
                case 2  : return PORTC&(0b1<<pos)>0; //PortC
                case 3  : return PORTD&(0b1<<pos)>0; //PortD
                case 4  : return PORTE&(0b1<<pos)>0; //PortE
            }
        }
        lenCalc += VPort->bits[i].len;
    }
    return 0;
}

#endif

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
#ifdef Virtual_Port_module
t_VirtualPort *Vport_7seg;
t_VirtualPort *Vdis_7seg;
octet modeVP_7seg=0;
#endif
unsigned char mask_dis0_7seg;
unsigned char mask_dis1_7seg;
unsigned char mask_dis2_7seg;
unsigned char mask_dis3_7seg;


#ifdef Virtual_Port_module
void conf7segVP(t_VirtualPort *port,t_VirtualPort *dis){
    modeVP_7seg=1;
    Vport_7seg = port;
    Vdis_7seg = dis;
    trisVPort(Vport_7seg,0x00);
    trisVPort(Vdis_7seg,0x00);
}
#endif

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
    #ifdef Virtual_Port_module
    modeVP_7seg=0;
    #endif
    port_7seg=port;
    dis_7seg=dis;
    mask_dis0_7seg = (unsigned char)(0b00000001 << pos_dis0);
    mask_dis1_7seg = (unsigned char)(0b00000001 << (pos_dis0+1));
    mask_dis2_7seg = (unsigned char)(0b00000001 << (pos_dis0+2));
    mask_dis3_7seg = (unsigned char)(0b00000001 << (pos_dis0+3));
}

void writeExa7seg(unsigned int val,unsigned char points){
    #ifdef Virtual_Port_module
    if(modeVP_7seg==1){// version VPort
        points = ~points;
        for(octet i = 0;i<4;i++){
            writeVPort(Vport_7seg,byteMerge(convert7seg(val&0x000F),points,0b00000001));
            writeVPin(Vdis_7seg,0,3-i);
            perte_temps(100);
            writeVPin(Vdis_7seg,1,3-i);
            val= val>>4;
            points = points>>1;
        }
        return;
    }
    #endif
    *port_7seg = byteMerge(convert7seg((unsigned char)(val&0x000F)),~points,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis3_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis3_7seg;// set disN a 1
    val= val>>4;
    
    *port_7seg = byteMerge(convert7seg((unsigned char)(val&0x000F)),~points>>1,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis2_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis2_7seg;// set disN a 1
    val= val>>4;
    
    *port_7seg = byteMerge(convert7seg((unsigned char)(val&0x000F)),~points>>2,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis1_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis1_7seg;// set disN a 1
    val= val>>4;
    
    *port_7seg = byteMerge(convert7seg((unsigned char)(val&0x000F)),~points>>3,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis0_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis0_7seg;// set disN a 1
}

void write7seg(int val,unsigned char points){
    #ifdef Virtual_Port_module
    if(modeVP_7seg==1){// version VPort
        points = ~points;
        for(octet i = 0;i<4;i++){
            writeVPort(Vport_7seg,byteMerge(convert7seg(val%10),points,0b00000001));
            writeVPin(Vdis_7seg,0,3-i);
            perte_temps(100);
            writeVPin(Vdis_7seg,1,3-i);
            val= val/10;
            points = points>>1;
        }
        return;
    }
    #endif
    //sinon mode normal
    // DIS3 (le plus à droite)
    
    *port_7seg = byteMerge(convert7seg(val%10),~points,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis3_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis3_7seg;// set disN a 1
    val= val/10;
    
    // DIS2 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),~points>>1,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis2_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis2_7seg;// set disN a 1
    val= val/10;
    
    // DIS1 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),~points>>2,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis1_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis1_7seg;// set disN a 1
    val= val/10;
    
    // DIS0 (le plus à droite)
    *port_7seg = byteMerge(convert7seg((unsigned char)(val%10)),~points>>3,0b00000001);
    *dis_7seg = *dis_7seg&(~mask_dis0_7seg);// set disN a 0
    perte_temps(100);
    *dis_7seg = *dis_7seg|mask_dis0_7seg;// set disN a 1
}
#endif

#ifdef KeyPad_module
#ifdef Virtual_Port_module
octet KeyPad_mode = 0;
t_port* KeyPad_VPort;
#endif
t_port* KeyPad_port;


void confKeyPad(t_port *port){
    KeyPad_mode = 0;
    KeyPad_port = port;
}
#ifdef Virtual_Port_module
void confKeyPadVP(t_VirtualPort *port){
    KeyPad_mode = 1;
    KeyPad_VPort = port;
    trisVPort(KeyPad_VPort,0x0F);
}
#endif
t_key_bitmap keyPadRead(){
    unsigned int ret=0;
    char i;
    #ifdef Virtual_Port_module
    if(KeyPad_mode){
        writeVPort(KeyPad_VPort,0b00010000);
        ret=(ret|(readVPort(KeyPad_VPort) & 0b00001111));
        for(i=1;i<4;i++){
            writeVPort(KeyPad_VPort,(octet)(0b00010000<<i));
            ret=ret<<4;
            ret=ret|(readVPort(KeyPad_VPort) & 0b00001111);
        }
    return ret;
    }
    #endif
    *KeyPad_port = 0b00010000;
    ret=(ret|(*KeyPad_port & 0b00001111));
    for(i=1;i<4;i++){
        *KeyPad_port = (unsigned char)(0b00010000<<i);
        ret=ret<<4;
        ret=ret|(*KeyPad_port & 0b00001111);
    }
    return ret;
}

#ifdef Virtual_Port_module
t_key_bitmap keyPadReadVP(t_VirtualPort *port){
    unsigned int ret=0;
    char i;
    
}
#endif

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

octet keyBitmapToIntPL(t_key_bitmap val){
    if(isKeySet(val,KEY_0)) return 0;
    if(isKeySet(val,KEY_1)) return 1;
    if(isKeySet(val,KEY_2)) return 2;
    if(isKeySet(val,KEY_3)) return 3;
    if(isKeySet(val,KEY_4)) return 4;
    if(isKeySet(val,KEY_5)) return 5;
    if(isKeySet(val,KEY_6)) return 6;
    if(isKeySet(val,KEY_7)) return 7;
    if(isKeySet(val,KEY_8)) return 8;
    if(isKeySet(val,KEY_9)) return 9;
    if(isKeySet(val,KEY_A)) return 10;
    if(isKeySet(val,KEY_B)) return 11;
    if(isKeySet(val,KEY_C)) return 12;
    if(isKeySet(val,KEY_D)) return 13;
    if(isKeySet(val,KEY_Star)) return 14;
    if(isKeySet(val,KEY_Hashtag)) return 15;
    return Default_KBTI;
}

octet keyBitmapToIntPG(t_key_bitmap val){
    if(isKeySet(val,KEY_Hashtag)) return 15;
    if(isKeySet(val,KEY_Star)) return 14;
    if(isKeySet(val,KEY_D)) return 13;
    if(isKeySet(val,KEY_C)) return 12;
    if(isKeySet(val,KEY_B)) return 11;
    if(isKeySet(val,KEY_A)) return 10;
    if(isKeySet(val,KEY_9)) return 9;
    if(isKeySet(val,KEY_8)) return 8;
    if(isKeySet(val,KEY_7)) return 7;
    if(isKeySet(val,KEY_6)) return 6;
    if(isKeySet(val,KEY_5)) return 5;
    if(isKeySet(val,KEY_4)) return 4;
    if(isKeySet(val,KEY_3)) return 3;
    if(isKeySet(val,KEY_2)) return 2;
    if(isKeySet(val,KEY_1)) return 1;
    if(isKeySet(val,KEY_0)) return 0;
    return Default_KBTI;
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

#ifdef Virtual_Port_module
#ifdef LCD_module
t_VirtualPort *DataPortLCD;
t_VirtualPort *ControlePortLCD; // 0 -> RS, 1 -> RW, 2 -> E

#define LCD_RS 0
#define LCD_RW 1
#define LCD_E  2

octet LireTrameLCD(octet type){
    octet trame;
    trisVPort(DataPortLCD,0xFF);// lecture
    writeVPin(ControlePortLCD,type,LCD_RS);
    writeVPin(ControlePortLCD,1,LCD_RW);
    writeVPin(ControlePortLCD,1,LCD_E);
    trame = readVPort(DataPortLCD);
    writeVPin(ControlePortLCD,0,LCD_E);
    return trame;
}

void EcrireTrameLCD(octet type,char trame){
    if(type==2){
        writeVPin(ControlePortLCD,LCD_Command,LCD_RS);
    }else{
        busy();
        writeVPin(ControlePortLCD,type,LCD_RS);
    }
    trisVPort(DataPortLCD,0x00);// ecriteure
    writeVPin(ControlePortLCD,0,LCD_RW);
    writeVPort(DataPortLCD,trame);
    writeVPin(ControlePortLCD,1,LCD_E);
    writeVPin(ControlePortLCD,0,LCD_E);
}

void confLCD(t_VirtualPort *DataPort,t_VirtualPort *ControlePort){
    DataPortLCD = DataPort;
    ControlePortLCD = ControlePort;
    trisVPort(ControlePort,0b000);
    EcrireTrameLCD(LCD_First_Command,0b00111011);
    EcrireTrameLCD(LCD_Command,0x0F);
    EcrireTrameLCD(LCD_Command,0b110);
    EcrireTrameLCD(LCD_Command,0x01);
    EcrireTrameLCD(LCD_Command,0x02);
}

void printIntLCD(int val){
    octet chifre,decimales=1 ;
    if(val<0) {
        EcrireTrameLCD(LCD_DATA,'-');
        val = -val;
    }
    if(val>0){
        decimales = 4;
        while(val<1000){
            val = val*10;
            --decimales;
        }
        if(val>=10000){
            EcrireTrameLCD(LCD_DATA,(char)(val/10000+48));
        }
    }
    chifre = (val/1000)%10;
    for(octet i =0;i<decimales;i++){
        EcrireTrameLCD(LCD_DATA,(char)(chifre+48));
        val = 10*(val%1000);
        chifre = (val/1000);
    }
}

void clearLCD(){
    EcrireTrameLCD(LCD_Command,0x01);
    EcrireTrameLCD(LCD_Command,0x02);
}

void gotoLCD(octet ligne,octet pos){
    EcrireTrameLCD(LCD_Command,0b10000000|(ligne*0x40)|pos);
}

void printLCD(char* text){
    for(octet i=0;i<40;i++){
        switch(text[i]){
            case '\0':
                i = 50;
                break;
            case '\n':
                EcrireTrameLCD(LCD_Command,0b11000000);
                break;
            case '\r':
                EcrireTrameLCD(LCD_Command,0b10000000);
                break;
            default:
                EcrireTrameLCD(LCD_DATA,text[i]);
                break;
        };
    }
}

void confCurseurLCD(octet DisplayAll,octet CuseurDisplay,octet Blinking){
    EcrireTrameLCD(LCD_Command,0b1000|((DisplayAll*0b100)&0b100)|((CuseurDisplay*0b10)&0b10)|((Blinking)&0b1));
}

void moveLCD(octet typeMove,octet versLaDroite){
    EcrireTrameLCD(LCD_Command,0b10000|((typeMove*0b1000)&0b1000)|((versLaDroite*0b100)&0b100));
}


#endif
#endif