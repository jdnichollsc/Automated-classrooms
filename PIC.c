#INCLUDE <16F887.h>
#USE DELAY (CLOCK=4000000)
#FUSES XT,NOPROTECT,NOWDT,NOBROWNOUT,PUT,NOLVP,MCLR
#USE RS232(BAUD=9600,PARITY=N,XMIT=PIN_C6,RCV=PIN_C7,BITS=8)
#DEFINE USE_PORTD_LCD TRUE
#DEFINE USE_PORTB_KBD TRUE
#INCLUDE <LCD.C>
#INCLUDE <KBD.C>

#BYTE PORTA = 5
#BYTE PORTC = 7
#BYTE PORTE = 9

#DEFINE ENTER PORTC, 0
#DEFINE SW1 PORTA, 1
#DEFINE SW2 PORTA, 5
#DEFINE SW3 PORTE, 2

#DEFINE LL1 PORTA, 0
#DEFINE LL2 PORTA, 3
#DEFINE LL3 PORTE, 1

#DEFINE SG1 PORTA, 2
#DEFINE SG2 PORTE, 0
#DEFINE SG3 PORTC, 2

#DEFINE ALARMA PORTC, 3

INT TEMP=0;
INT POS=7;
UNSIGNED CHAR DATAKEY, CLAVE[6],INGRESO[6],PC;
INT J;

//METODO ENCARGADO DE CONFIGURAR PUERTOS
void configPic(void){   
   PORT_B_PULLUPS(PIN_B5);
   PORT_B_PULLUPS(PIN_B6);
   PORT_B_PULLUPS(PIN_B7);
   SET_TRIS_A(50);
   SET_TRIS_C(179);
   SET_TRIS_E(4);   
   LCD_INIT();   
   KBD_INIT(); 
   BIT_SET(SG1); 
   BIT_SET(SG2);
   BIT_SET(SG3);
   BIT_CLEAR(LL1); 
   BIT_CLEAR(LL2); 
   BIT_CLEAR(LL3);
   BIT_CLEAR(ALARMA); 
}

//METODO ENCARGADO DE REINICIALIZAR
VOID clear(void)
{
   LCD_PUTC("\f");
   LCD_PUTC("NUMERO AULA=");
   LCD_PUTC("\n");
   LCD_GOTOXY(1,2);
   LCD_PUTC("CLAVE=");
   SET_TIMER0(0);
   POS=7;
   TEMP=0;
}

//METODO ENCARGADO 
INT1 COMPCLAVES(VOID)
{
INT1 RCLAVE;
RCLAVE=1;
FOR (J=0;J<6;J++)
{
IF(CLAVE[J]!=INGRESO[J])
{
RCLAVE=0;
GOTO SALIR;
}
}
SALIR:
RETURN RCLAVE;
}

//METODO ENCARGADO DE RECIBIR EL NUEVO CODIGO DE SEGURIDAD
VOID CLAVEIN(VOID)
{
  IF(KBHIT())
  {
     PC=GETC();
     IF(PC=='C')
     {
        FOR (J=0;J<6;J++)
        {
           INGRESO[J]=GETC();
        }   
     } 
     IF(PC=='A')
     {
        BIT_CLEAR(ALARMA);
     }
     IF(PC=='I')
     {
        PC=GETC();
        IF(PC=='1')
        {
           BIT_SET(LL1);
           BIT_CLEAR(SG1);
        }
        IF(PC=='2')
        {
           BIT_SET(LL2);
           BIT_CLEAR(SG2);
        }
        IF(PC=='3')
        {
           BIT_SET(LL3);
           BIT_CLEAR(SG3);
        }
     }
     IF(PC=='F')
     {
        PC=GETC();
        IF(PC=='1')
        {
           BIT_SET(SG1);
           BIT_CLEAR(LL1);
        }
        IF(PC=='2')
        {
           BIT_SET(SG2);
           BIT_CLEAR(LL2);
        }
        IF(PC=='3')
        {
           BIT_SET(SG3);
           BIT_CLEAR(LL3);
        }
     }
  }
}

//METODO ENCARGADO PARA APAGAR LA ALARMA

//METODO ENCARGADO DE ALARMAR AL PC
VOID ALARMAR(VOID)
{

   IF(BIT_TEST(SW1)==0)
   {
      PUTC('X');
   }
   IF(BIT_TEST(SW2)==0)
   {
      PUTC('Y');
   }
   IF(BIT_TEST(SW3)==0)
   {
      PUTC('Z');
   }   
   IF(BIT_TEST(SG1) && BIT_TEST(SW1)==0)
   {
      PUTC('A');
      PUTC('1');
      BIT_SET(ALARMA);   
      WHILE(!BIT_TEST(SW1))
      {
      DELAY_MS(20);  
      } 
   }
   IF(BIT_TEST(SG1) && BIT_TEST(SW1)==0)
   {
      PUTC('A');
      PUTC('1');
      BIT_SET(ALARMA);   
      WHILE(!BIT_TEST(SW1))
      {
      DELAY_MS(20);  
      } 
   }
   IF(BIT_TEST(SG2) && BIT_TEST(SW2)==0)
   {
      PUTC('A');
      PUTC('2');
      BIT_SET(ALARMA); 
      WHILE(!BIT_TEST(SW2))
      {
      DELAY_MS(20);  
      } 
   }
   IF(BIT_TEST(SG3) && BIT_TEST(SW3)==0)
   {
      PUTC('A');
      PUTC('3');
      BIT_SET(ALARMA); 
      WHILE(!BIT_TEST(SW3))
      {
      DELAY_MS(20);     
      } 
   }   
}
VOID main()
{ 
   configPic();
   clear();
   WHILE(TRUE)
   {        
      RETORNAR:
      WHILE(BIT_TEST(ENTER))
      {      
         DATAKEY=KBD_GETC();
         WHILE((DATAKEY=='\0'))
         {  
            CLAVEIN();
            ALARMAR();
            DATAKEY=KBD_GETC();
            IF(GET_TIMER0()> 3)
            {                    
               SET_TIMER0(1);        
            }
            IF(GET_TIMER0()!=TEMP)
            {
               TEMP=GET_TIMER0();
               LCD_GOTOXY(13,1);
               LCD_PUTC(TEMP+48);
            }            
            IF(BIT_TEST(ENTER)==0)
            {
               GOTO VALIDAR;
            }
         }
         IF(POS<13)
         {
            LCD_GOTOXY(POS,2);
            CLAVE[POS-7]=DATAKEY;
            LCD_PUTC('*');         
            POS++;
         }
      }  
      VALIDAR:
      IF((POS!=13) || (GET_TIMER0()==0)){GOTO RETORNAR;}         
      LCD_PUTC("\f");
      IF(COMPCLAVES()==1)
      {
         PUTC('H');
         PUTC(TEMP+48);
         LCD_PUTC("CLAVE ACEPTADA");
         DELAY_MS(3000); 
         
      }
      ELSE
      {
         LCD_PUTC("CLAVE ERRADA");
         DELAY_MS(3000);   
      }       
      clear();
      WHILE(!BIT_TEST(ENTER))
      {
         DELAY_MS(20);            
      } 
   }
}
