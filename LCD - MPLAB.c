#define RS LATDbits.LATD5  // Nombrar RS a LATDbits.LATD5
#define EN LATDbits.LATD4  // Nombrar EN a LATDbits.LATD4
#define D4 LATDbits.LATD0  // Nombrar D4 a LATDbits.LATD0
#define D5 LATDbits.LATD1  // Nombrar D3 a LATDbits.LATD1
#define D6 LATDbits.LATD2  // Nombrar D6 a LATDbits.LATD2
#define D7 LATDbits.LATD3  // Nombrar D7 a LATDbits.LATD3



#include "conf_45k22.h"
#include <xc.h>         // libreria de C
#define _XTAL_FREQ 8000000
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


float next = 0;             // vairable para cambiar de umbral
float tmax = 30;            // varibale para umbral de temperatura maxima
float tmin = 10;            // variable para umbral de temperatura minima
float hmax = 60;            // variable para umbral de humedad maxima
float hmin = 30;            // variable para umbral de humedad minima


float time_ = 15;           // variable para tiempo de calefacción
float aum_dis = 0;          // contador para aumentar y disminuir umbrales

int dc = 0;                 // contador para que el pwm siga encendido 2 grados abajo del umbral
int k = 0;                  // contador para que se cumpla el tiempo correcto de caleffación


void __interrupt() nombre(){            // interrupciones
    
    if(INTCONbits.TMR0IF == 1 && INTCONbits.TMR0IE==1){     // interrupciones para el temporizador de calefactor cada vuelta es un segundo
        
        CCPR2L = 124;
        TMR0H=0xC2;
        TMR0L=0xF6;
        
        k = k + 1;
        if(k == (time_ + 1))            // el calefactor se realizará el tiempo que se solicite
        {
           CCPR2L = 0;
           k = 0;
           INTCONbits.TMR0IE=0;
           INTCONbits.GIE = 0;
        }
        INTCONbits.TMR0IF=0;
    }
    
    
    if(INTCONbits.INT0IF==1){       // interrupción para cambiar de umbral
        next++;
        if(next>=5){
            next=0;
        }
        __delay_ms(20);
        INTCONbits.INT0IF=0;
    }
    
    if(INTCON3bits.INT1IF==1){      // interrupcion para aumentar el rango del umbral
        aum_dis++;
        
        __delay_ms(20);
        INTCON3bits.INT1IF=0;
        
    }
    
    if(INTCON3bits.INT2IF==1){      // interrupción para disminuir el rango de umbral
        aum_dis--;
        
        __delay_ms(20);
        INTCON3bits.INT2IF=0;
    }
    
    
}



void main(void) {
    
    TRISB = 0x07;       // declaramos como entrada algunos puerto B para las interrupciones
    RCONbits.IPEN = 0;  // Habilitar niveles de prioridad en interrupciones
    
    INTCONbits.GIE = 0;     // habilitar interrupciones globales
    
    INTCON3bits.INT1IE = 1;     // habilitar interrupciones int1
    INTCON3bits.INT2IE = 1;     // habilitar interrupciones int2
    INTCONbits.INT0IE = 1;     // habilitar interrupciones int0
    
    INTCONbits.INT0IF = 0;
    INTCON2bits.INTEDG0 = 0;    // Interupcion flanco de bajada
    
    INTCON3bits.INT1IF = 0;
    INTCON2bits.INTEDG1 = 1;    // Interupcion flanco de bajada
    
    INTCON3bits.INT2IF = 0;
    INTCON2bits.INTEDG2 = 1;    // Interupcion flanco de bajada
    
    TRISE = 0x00;       // salidas
    TRISD = 0x00;       // salidas
    
    LATBbits.LATB5=1; LATBbits.LATB4=1; LATBbits.LATB3=1; LATDbits.LATD7 = 1;       // encendidos
    
    ADCON0 = 0b00000001; // configuración de la entrada analogicas
    ADCON1 = 0b00001111;
    ADCON2 = 0b00010001;
    
    ANSELE = 0x00;          // configuración de los puertos E para que sean digitales
    TRISE = 0b0000111;      // encendidos    
    
    
    float a, b, c, d, Tk, Tf, temp, Vout, Rt;       //variables flotantes para calculo de temp en C y F
    char temperatura [4];                           // arreglo que guardará los valores flotantes en string
    char new_ [4];                                  // el arreglo que guardará el umbral modificado en el modo programador
    
    char ini;                                       // variable para desplegar los puntos en movimiento del modo programador y guardando
    
    PR2 = 32;       // configuración del pwm
    CCPR1L = 124;     
    CCPR2L = 0;     
    TRISCbits.RC2 = 0;
    TRISCbits.RC1 = 0;
    T2CON = 0b0000010;     // Prescales 16 Timer 2 OFF   
    CCP1CON = 0x0C;   // 0x0C   pwm mode    
    CCP2CON = 0x0C;   // 0x0C
    TMR2 = 0;
    T2CONbits.TMR2ON = 1;
    
    T0CON=0b00000011;       // configuracion para que el temprizador dure 1 segundo
    TMR0H=0xC2;             // se divide el dato hexadecimal de 4 digitos
    TMR0L=0xF6;             // segunda parte del dato hexadecimal
    
    T0CONbits.TMR0ON=1;
    INTCONbits.TMR0IE=0;    
    
    Lcd_Init();         // inicializamos el lcd
    Lcd_Clear();        // limpiamos el lcd
    
    
    
    
    while(1){
        
        if(PORTEbits.RE0 == 1){     // condición para entrar en modo programador con el bot+on de enclavamiento
            
        ini = 2;
        Lcd_Clear();    
        Lcd_Set_Cursor(2,0);
        Lcd_Write_String("  MODO PROGRAMADOR  ");
        while(ini <= 17){ Lcd_Set_Cursor(3,ini); Lcd_Write_String("."); __delay_ms(2); ini++; } // despliega una serie de puntos con un delay de 2 ms
        __delay_ms(10);
        
        while(1){ INTCONbits.GIE = 1;  if(PORTEbits.RE0 == 0){  break;}     // habilitamos que se puedan producir interrupciones
                                                                    // y ponemos condición para salir del while en caso de que se desenclave el bottón
            if(next == 0){ 
                Lcd_Clear();    
                Lcd_Set_Cursor(1,0);
                Lcd_Write_String("  Temperatura max   ");
                while(next==0){ if(PORTEbits.RE0 == 0){ Lcd_Clear(); break;}    // mostramos el umbral de temperatura max

                        if(tmax <= 65 && tmax >= 25){
                            tmax = tmax+(aum_dis);
                        }
                        __delay_ms(10);
                        if(aum_dis != 0){
                            aum_dis=0;
                        }
                        if(tmax>=66){tmax=65;}if(tmax<=24){tmax=25;}
                        sprintf(new_,"%.2f",tmax);
                        Lcd_Set_Cursor(2,6);
                        Lcd_Write_String(new_);
                        Lcd_Set_Cursor(2,12);
                        Lcd_Write_String("C");

                        Lcd_Set_Cursor(4,6);
                        Lcd_Write_String("Salir y Guardar");


                }
            }

            if(next == 1){                  // mostramos el umbral de temperatura min
                Lcd_Clear();    
                Lcd_Set_Cursor(1,0);
                Lcd_Write_String("  Temperatura min   ");
                while(next==1){ if(PORTEbits.RE0 == 0){ Lcd_Clear(); break;}

                    if(tmin <= 20 && tmin >= -20){
                            tmin = tmin+(aum_dis);
                        }
                        __delay_ms(10);
                        if(aum_dis != 0){
                            aum_dis=0;
                        }
                    if(tmin>=21){tmin=20;}if(tmin<=-21){tmin=-20;}
                        
                    sprintf(new_,"%.2f",tmin);
                    __delay_ms(10);
                    Lcd_Set_Cursor(2,6);
                    Lcd_Write_String(new_);
                    Lcd_Set_Cursor(2,12);
                    Lcd_Write_String("C");

                    Lcd_Set_Cursor(4,6);
                    Lcd_Write_String("Salir y Guardar");
                }

            }

            if(next == 2){      // mostramos el umbral de humedad max
                Lcd_Clear();    
                Lcd_Set_Cursor(1,0);
                Lcd_Write_String("    Humedad max     ");
                while(next==2){ if(PORTEbits.RE0 == 0){ Lcd_Clear(); break;}

                    if(hmax <= 100 && hmax >= 45){
                            hmax = hmax+(aum_dis);
                        }
                        __delay_ms(10);
                        if(aum_dis != 0){
                            aum_dis=0;
                        }
                        
                    if(hmax>=101){hmax=100;}if(hmax<=44){hmax=45;}
                    sprintf(new_,"%.2f",hmax);
                    __delay_ms(10);
                    Lcd_Set_Cursor(2,6);
                    Lcd_Write_String(new_);
                    Lcd_Set_Cursor(2,12);
                    Lcd_Write_String("%");

                    Lcd_Set_Cursor(4,6);
                    Lcd_Write_String("Salir y Guardar");
                }
            }

            if(next == 3){                      // mostramos el umbral de humedad min
                Lcd_Clear();    
                Lcd_Set_Cursor(1,0);
                Lcd_Write_String("    Humedad min     ");
                while(next==3){ if(PORTEbits.RE0 == 0){ Lcd_Clear(); break;}

                    if(hmin <= 30 && hmin >= 0){
                            hmin = hmin+(aum_dis);
                        }
                        __delay_ms(10);
                        if(aum_dis != 0){
                            aum_dis=0;
                        }
                    if(hmin>=31){hmin=30;}if(hmin<=-1){hmin=0;}
                    sprintf(new_,"%.2f",hmin);
                    __delay_ms(10);
                    Lcd_Set_Cursor(2,6);
                    Lcd_Write_String(new_);
                    Lcd_Set_Cursor(2,12);
                    Lcd_Write_String("%");

                    Lcd_Set_Cursor(4,6);
                    Lcd_Write_String("Salir y Guardar");
                }
            }

            if(next == 4){                  // mostramos el umbral de tiempo del calefactor
                Lcd_Clear();
                Lcd_Set_Cursor(1,0);
                Lcd_Write_String(" Tiempo calefaccion ");
                while(next==4){ if(PORTEbits.RE0 == 0){ Lcd_Clear(); break;}
                
                
                    if(time_ <= 120 && time_ >= 1){
                            time_ = time_+(aum_dis);
                        }
                        __delay_ms(10);
                        if(aum_dis != 0){
                            aum_dis=0;
                        }
                    if(time_>=121){time_=120;}if(time_<=0){time_=1;}
                    
                    sprintf(new_,"%.2f",time_);
                    Lcd_Set_Cursor(2,5);
                    Lcd_Write_String(new_);
                    Lcd_Set_Cursor(2,11);
                    Lcd_Write_String("seg");

                    Lcd_Set_Cursor(4,6);
                    Lcd_Write_String("Salir y Guardar");
                }
            }
        
                                        }     
        INTCONbits.GIE = 0; next =0; 
        
        ini = 5;
        Lcd_Clear(); Lcd_Set_Cursor(2,0);
        Lcd_Write_String("     GUARDANDO      ");
        while(ini <= 13){ Lcd_Set_Cursor(3,ini); Lcd_Write_String("."); __delay_ms(3); ini++; }
        
        __delay_ms(5); Lcd_Clear();
        }           //     salimos del modo programador
        
        Lcd_Set_Cursor(1,4);
        Lcd_Write_String("Temperatura:");           // desplegamos temperatura
        ADCON0 = 0b00000001;
        ADCON0bits.GO_DONE = 1;
        while(ADCON0bits.GO_DONE == 1){
        }
        Vout = (ADRESH) * 0.0196078;
        Rt = ((5/Vout)-1)*10000;
        a = 0.003354016;
        b = 0.000256173;
        c = 2.13941e-6;
        d = -7.25325e-8;
        Tk = 1/(a + b*(log(Rt/10000)) + (c*(log(Rt/10000))*c*(log(Rt/10000))) + (d*(log(Rt/10000))*d*(log(Rt/10000))*d*(log(Rt/10000))));
        temp = Tk-273; 
        Tf = (temp*1.8)+32;
        
        // se realizaron calculos para calcular la temperatura en C y F
           
        sprintf(temperatura,"%.2f",temp);
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(temperatura);      // desplegar temperatura en C
        Lcd_Set_Cursor(2,6);
        Lcd_Write_String(" C");
        
        sprintf(temperatura,"%.2f",Tf);
        Lcd_Set_Cursor(2,12);
        Lcd_Write_String(temperatura);  // desplegar temperatura en F
        Lcd_Set_Cursor(2,17);
        Lcd_Write_String(" F");
        
        Lcd_Set_Cursor(4,2);
        Lcd_Write_String("Estado(T):");
        
        if (temp>tmax){ Lcd_Set_Cursor(4,13); Lcd_Write_String("Caliente"); LATBbits.LATB5=0; LATBbits.LATB4=1; LATBbits.LATB3=1;
                                                                            }
        
        if (temp<tmin){  Lcd_Set_Cursor(4,13); Lcd_Write_String("Frio    "); LATBbits.LATB5=1; LATBbits.LATB4=1; LATBbits.LATB3=0; 
                                                                            CCPR1L = 124;}
        
        if (temp>(tmin) && temp<(tmax) ){ Lcd_Set_Cursor(4,13); Lcd_Write_String("Normal  ");  LATBbits.LATB5=1; LATBbits.LATB4=0; LATBbits.LATB3=1; 
                                                                                }
        // condiciones para el estado de la temperatura 
        
        if (temp>tmax){  CCPR1L = 0;  dc = 1;   }
        
        if (temp<=(tmax-2) && dc==1){   dc = 0;   }
        
        if (temp>(tmin) && temp<(tmax) && dc == 0){   CCPR1L = 28;   }
        // condicones con el contador del pwm para lograr que se mantenga encendido hasta dos grados abajo
        
        
        __delay_ms(5);
        
        
        Lcd_Set_Cursor(3,1);
        Lcd_Write_String("Humedad:");
        ADCON0 = 0b00000101;
        ADCON0bits.GO_DONE = 1;
        while(ADCON0bits.GO_DONE == 1){
        }
        temp = (25500/161) - ((100*ADRESH)/161);        // calculamos humedad en porcentaje
        if (temp>99){ temp = 100;}              // en caso de que la humedad sea 99, se redonde a 100
        if (temp<2.1){ temp = 0;}               // en caso de que la humedad se quede en 2, se rendonde a 0
        
        sprintf(temperatura,"%.2f",temp);
        Lcd_Set_Cursor(3,12);
        Lcd_Write_String(temperatura);      // reulizamos vaariable para la temperatura y desplegamos humedad
        Lcd_Set_Cursor(3,17);
        Lcd_Write_String(" %");
        
        
        Lcd_Set_Cursor(4,2);
        Lcd_Write_String("Estado(H):");
        
        if (temp>(hmax)){ Lcd_Set_Cursor(4,13); Lcd_Write_String("Ahogado"); LATDbits.LATD7 = 0; INTCONbits.GIE = 1; INTCONbits.TMR0IE=1;  }
        if (temp<hmin){  Lcd_Set_Cursor(4,13); Lcd_Write_String("Seco    "); LATDbits.LATD7 = 1;  }
        if (temp>(hmin) && temp<hmax ){ Lcd_Set_Cursor(4,13); Lcd_Write_String("Humedo  ");  LATDbits.LATD7 = 1; }
        // condiciones para mostrar el estado de la humedad
    }
    return;
}
