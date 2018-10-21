/*
 * Arquivo:   main.c
 * Autor: Maria Teresa
 * Criado em 06 de Junho de 2017
 * Obejetivo: Projeto final da disciplina
 */

#include <main.h>
#include <lcd.c>
#include <string.h>

//Quando o sensor � digital 0 � ativo e 1 � inativo

//variaveis de entrada 
//DIGITAL
#define i_SENSOR_NIVEL          pin_b2 //sensor nivel da agua 


//variaveis de saida
#define o_VALVULA_CL           pin_c0 //valvula do cloro
#define o_LED_PH               pin_c1 //LED da ALCALINIDADE
#define o_VALVULA_AGUA         pin_c2 //valvula de agua
#define o_FILTRA               pin_c3 //Motor do filtro
#define o_LED_FILTRA           pin_c4 //LED do filtro
#define o_ALGICIDA             pin_c5 //Motor do algicida
#define o_LED_ALGICIDA         pin_c6 //LED do algicida
#define o_LED_AT               pin_c7 //LED da ALCALINIDADE
#define o_LED_CL               pin_d3 //LED do CLORO


    
byte i=0; //Vari�vel contadora do timer1
byte estado=0; //Vari�vel indicativa da m�quina de estados
char c; //Vari�vel de escrita de caractere no LCD
int at=0, ph=0, cl=0, cont=0; //Entradas Anal�gicas




//Leitura da porta RA0 - Sensor de Alcalinidade total
int le_A0(){  
   restart_wdt();  
   int analogico, at;
   set_adc_channel(0);
   delay_ms(1);
   analogico = read_adc();
   at = (analogico * 0.0390625);
   return at;
}



//Leitura da porta RA1 - Sensor de PH
   int le_A1(){  
   restart_wdt();  
   int analogico, ph;
   set_adc_channel(1);
   delay_ms(1);
   analogico = read_adc();
   ph = (analogico * 0.0390625);
   return ph;
}



//Leitura da porta RA2 - Sensor de cloro
int le_A2(){  
   restart_wdt();   
   int analogico, cl;
   set_adc_channel(2);
   delay_ms(1);
   analogico = read_adc();
   cl = (analogico * 0.0390625);
   return cl;
}


//limpa o LCD
void limpar(){
   c='\f';
   lcd_putc(c);
}
  
  
void inicializar(){
   output_low(o_VALVULA_AGUA);
   output_low(o_FILTRA);
   output_low(o_LED_FILTRA);
   output_low(o_ALGICIDA);
   output_low(o_LED_ALGICIDA);
   output_low(o_LED_AT);
   output_low(o_LED_PH);
   output_low(o_VALVULA_CL);
   output_low(o_LED_CL);
}  
  
  
  
#int_timer1
void timer1_isr(){
   //S� ira incrementar se no 2 ou 3
   if(estado==2||estado==3){
      clear_interrupt(INT_TIMER1);
      set_timer1(3036);
      i++;
   }
}  
  


void main(){


   //Configura��es      
   setup_adc_ports(AN0_AN1_AN3);
   setup_adc(ADC_CLOCK_INTERNAL);
   Port_b_pullups(true);
   enable_interrupts(global);
   enable_interrupts(int_ext);
   enable_interrupts(INT_TIMER1);
   clear_interrupt(INT_TIMER1);
   setup_timer_1 ( T1_INTERNAL | T1_DIV_BY_8 );
   set_timer1(3036);
   ext_int_edge(H_to_L);
   lcd_init();

   while(TRUE){
    
    switch(estado){
            //Estado 0 - Inicializa��o
            case 0:
               
               i=0;
               inicializar();
               limpar();
               estado=1;
            break;
            
            //Estado 1 - Enchimento da piscina 
            case 1:
                 
                 while((input(i_SENSOR_NIVEL)==1)&&(estado==1)){
                  printf(LCD_PUTC, "Enchendo      \n");
                  
                  delay_ms(150);
                  output_high(o_VALVULA_AGUA);
                  delay_ms(150);
                  output_low(o_VALVULA_AGUA);
                  limpar();
                 }
                 limpar();
                 output_high(o_VALVULA_AGUA);
                 printf(LCD_PUTC, "1:OK      \n");
                 delay_ms(500);
                 estado=2;
            break;
            
            //Estado 2 - Filtrar piscina.. liga o motor por x tempo, depois desse x tempo ele desliga o motor e mostra 2:OK
            case 2:
                  limpar();
                  
                  
                  if(cont!=0){//SE NAO FOR A PRIMEIRA VEZ...
                      cont=0;
                      i=0;
                      
                      while((i<7)){ //alguns segundos
                        printf(LCD_PUTC, "Filtrando   %u   \n",i);
                        output_high(o_FILTRA);
                        delay_ms(450);
                        limpar();
                     }
                     
                     limpar();
                     output_low(o_FILTRA);
                     output_high(o_LED_FILTRA); //liga quando termina de filtrar para dizer que a etapa esta concluida
                     printf(LCD_PUTC, "===FIM===     \nAMANHA TEM MAIS");
                     delay_ms(500);
                     estado=0;
                     break;
                  }
                  i=0;
                  while((i<3)){ //alguns segundos
                     printf(LCD_PUTC, "Filtrando  %u   \n",i);
                     output_high(o_FILTRA);
                     delay_ms(450);
                     limpar();
                  }
                  limpar();
                  output_low(o_FILTRA);
                  output_high(o_LED_FILTRA); //liga quando termina de filtrar para dizer que a etapa esta concluida
                  printf(LCD_PUTC, "1:OK, 2:OK     \n");
                  delay_ms(500);
                  estado=3;
            break;
            
            /*Estado 3: Aplicar o algicida 
            Para evitar o esverdeamento, aplique algicida de manuten��o. A dosagem � 6 ml por m�.
            Use a qualquer hor�rio do mesmo dia ou da noite que usar o cloro, ou qualquer outro produto. Filtre por 1 horas
            */
            case 3:
                  limpar();
                  i=0;
                  while(i<3){
                     printf(LCD_PUTC, "Aplic Algicida  %u   \n",i);
                     output_high(o_ALGICIDA);
                     delay_ms(450);
                     limpar();
                  }
                  limpar();
                  output_low(o_ALGICIDA);
                  output_high(o_LED_ALGICIDA);//liga o led pra dizer que a etapa esta concluida
                  printf(LCD_PUTC, "1:OK, 2:OK, 3:OK    \n");
                  delay_ms(500);
                  
                  estado=4;
            break;
           
           
           /*Estado 4: Verifica alcalinidade, se for baixa aplica elevador de alcalinidade, 
           se for alta aplica redutor de alcalinidade, se estiver ok vai para proximo estado*/
           case 4:
               limpar();
               printf(LCD_PUTC, "LENDO SENSOR AT  \n"); 
               delay_ms(500);
               at = le_A0();
              
               
               while((at!=7)||(at!=8)){
               
                   if((at==7)||(at==8)){  // alcalinidade ideal
                      limpar();
                      printf(LCD_PUTC, "AT IDEAL:  %u   \n",at);
                      output_high(o_LED_AT); 
                      delay_ms(500);
                      limpar();
                      printf(LCD_PUTC, "1:OK, 2:OK, 3:OK,  \n 4:OK");
                      delay_ms(500);
                      estado=5;
                      break;
               
                   }else if((at>=0)&&(at<=6)){  // alcalinidade baixa pisca 6 vezes
                            limpar();
                            printf(LCD_PUTC, "AT BAIXA:  %u   \n",at);
                            output_high(o_LED_AT); 
                            delay_ms(100);
                            output_low(o_LED_AT); 
                            delay_ms(100);
                   
                         } else{// alcalinidade alta
                              limpar();
                              printf(LCD_PUTC, "AT ALTA:  %u   \n",at);
                              output_high(o_LED_AT); 
                              delay_ms(100);
                              output_low(o_LED_AT); 
                              delay_ms(100);
                           }
                at = le_A0();
               }
           limpar();
           break;
           
          
           //Estado 5 - Verifica o ph da agua, o teor ideal de pH � entre 7,2 e 7,6. Caso esteja abaixo, adicione Elevador de pH. Caso esteja acima, adicione Redutor de pH.
           
           case 5:
                
              printf(LCD_PUTC, "LENDO SENSOR PH  \n");
              delay_ms(500);
              ph = le_A1();
              limpar();
              while((ph!=5)||(ph!=6)||(ph!=7)){
               
                if((ph>=0)&&(ph<=4)){  // PH baixo
                     limpar();
                     printf(LCD_PUTC, "PH BAIXO:  %u   \n",ph);
                     output_high(o_LED_PH); 
                     delay_ms(100);
                     output_low(o_LED_PH); 
                     delay_ms(100);
                     estado=6;
                
               }else if((ph==5)||(ph==6)||(ph==7)){ // ph ideal
                        limpar();
                        printf(LCD_PUTC, "PH IDEAL:  %u   \n",ph);
                        output_high(o_LED_PH); 
                        delay_ms(500);
                        limpar();
                        printf(LCD_PUTC, "1:OK, 2:OK, 3:OK,   \n  4:OK, 5:OK ");
                        delay_ms(500);
                        estado=6;
                        break; 
                     
                      } else{// PH alto
                            limpar();
                            printf(LCD_PUTC, "PH ALTO:  %u   \n",ph);
                            output_high(o_LED_PH); 
                            delay_ms(100);
                            output_low(o_LED_PH); 
                            delay_ms(100);
                            estado=6;
                     }
               
               ph = le_A1();
                
              }
           
            limpar();
            break;
           
           
           // Estado 6: Verifica a porcentagem de cloro. O teor ideal de cloro � entre 1 e 3 ppm. Caso esteja abaixo aplica o produto na piscina.
            case 6:
               cl = le_A2();
               limpar();
               while((cl!=8)||(cl!=9)){
               
               // CLORO ideal
               if((cl==8)||(cl==9)){
                   printf(LCD_PUTC, "CLORO IDEAL:  %u   \n",cl);
                   delay_ms(500);
                   limpar();
                   printf(LCD_PUTC, "1:OK, 2:OK, 3:OK,   \n4:OK, 5:OK, 6:OK ");
                   delay_ms(500);
                   output_high(o_LED_CL);
                   output_low( o_VALVULA_CL);
                   delay_ms(200);
                   cont++;
                   estado=2;
                   break;
               }else{
                   printf(LCD_PUTC, "APLIQUE MAIS CLORO:  %u   \n",cl);
                   delay_ms(500);
                   limpar();
                   output_high( o_VALVULA_CL);
               }
            cl = le_A2();
         }
   
   
   
   
         
         }
  }
 }





