/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <FreeRTOS/include/FreeRTOS.h>
#include <stdio.h>
#include <string.h>
#include <FreeRTOS/include/task.h>
#include <FreeRTOS/include/timers.h>

#define USART_SERIAL_EXAMPLE             &USARTC0
#define USART_SERIAL_EXAMPLE_BAUDRATE    9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

static char strbuf[201];
static char reads[100];
int result = 0;
char in = 'x';
long increment = 0;

char *str1 = "atas ";
char *str2 = "bawah ";

static portTASK_FUNCTION_PROTO(freeHeap, p_);
static portTASK_FUNCTION_PROTO(ACK, p_);

void vTimerCallback(){
	increment++;
}

void setUpSerial()
{
    // Baud rate selection
    // BSEL = (2000000 / (2^0 * 16*9600) -1 = 12.0208... ~ 12 -> BSCALE = 0
    // FBAUD = ( (2000000)/(2^0*16(12+1)) = 9615.384 -> mendekati lah ya
   
    USARTC0_BAUDCTRLB = 0; //memastikan BSCALE = 0
    USARTC0_BAUDCTRLA = 0x0C; // 12
	
	//USARTC0_BAUDCTRLB = 0; //Just to be sure that BSCALE is 0
	//USARTC0_BAUDCTRLA = 0xCF; // 207
   
   
    //Disable interrupts, just for safety
    USARTC0_CTRLA = 0;
    //8 data bits, no parity and 1 stop bit
    USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;
   
    //Enable receive and transmit
    USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm; 
}

void sendChar(char c)
{
   
    while( !(USARTC0_STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
   
    USARTC0_DATA = c;
   
}

void sendString(char *text)
{
    while(*text)
    {
        //sendChar(*text++);
		usart_putchar(USART_SERIAL_EXAMPLE, *text++);
    }
}

char receiveChar()
{
    while( !(USARTC0_STATUS & USART_RXCIF_bm) ); //Wait until receive finish
    return USARTC0_DATA;
}

void receiveString()
{
    int i = 0;
    while(1){
        //char inp = receiveChar();
		char inp = usart_getchar(USART_SERIAL_EXAMPLE);
        if(inp=='\n') break;
        else reads[i++] = inp;
    }
	if(strcmp(str1,reads) == 0){
		gpio_set_pin_high(J2_PIN0);
	}else if(strcmp(str2,reads) == 0){
		gpio_set_pin_high(J2_PIN0);
	}else{
		gpio_set_pin_low(J2_PIN0);
	}
}

static portTASK_FUNCTION(freeHeap, p_){
	//ioport_set_pin_level(LCD_BACKLIGHT_ENABLE_PIN, false);
	
	//send free heap in here
		
	gpio_set_pin_high(LED0_GPIO);
	vTaskDelay(100/portTICK_PERIOD_MS);
}

static portTASK_FUNCTION(ACK, p_){
	//ioport_set_pin_level(LCD_BACKLIGHT_ENABLE_PIN, false);
	//send acknowledgement in here
		
	sendString("atas \n");
	vTaskDelay(100/portTICK_PERIOD_MS);
}

int main (void)
{
    // Insert system clock initialization code here (sysclk_init()).

    board_init();
	sysclk_init();
    gfx_mono_init();
   
    gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
   
    PORTC_OUTSET = PIN3_bm; // PC3 as TX
    PORTC_DIRSET = PIN3_bm; //TX pin as output
   
    PORTC_OUTCLR = PIN2_bm; //PC2 as RX
    PORTC_DIRCLR = PIN2_bm; //RX pin as input
   
    setUpSerial();
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	
	usart_init_rs232(USART_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);
	
	ioport_set_pin_dir(J2_PIN0, IOPORT_DIR_OUTPUT);
	
	TimerHandle_t timerPing = xTimerCreate("tPing", 2/portTICK_PERIOD_MS, pdTRUE, (void *) 0, vTimerCallback);
	
	xTaskCreate(freeHeap,"",500,NULL,1,NULL);
	
	xTaskCreate(ACK,"",500,NULL,1,NULL);
	
	xTimerStart(timerPing, 0);
	
	vTaskStartScheduler();
	   
    /*while(1){
       
        if(gpio_pin_is_low(GPIO_PUSH_BUTTON_1) && gpio_pin_is_high(GPIO_PUSH_BUTTON_2)){
            gpio_set_pin_low(LED2_GPIO);
            gpio_set_pin_high(LED3_GPIO);
            //sendChar('u');
            sendString("atas \n");
			//usart_putchar(USART_SERIAL_EXAMPLE,'u');
        }else if(gpio_pin_is_low(GPIO_PUSH_BUTTON_2) && gpio_pin_is_high(GPIO_PUSH_BUTTON_1)){
            gpio_set_pin_low(LED3_GPIO);
            gpio_set_pin_high(LED2_GPIO);
            //sendChar('d');
            sendString("bawah \n");
			//usart_putchar(USART_SERIAL_EXAMPLE,'d');
        }else if(gpio_pin_is_high(GPIO_PUSH_BUTTON_1) && gpio_pin_is_high(GPIO_PUSH_BUTTON_2)){
            gpio_set_pin_high(LED3_GPIO);
            gpio_set_pin_high(LED2_GPIO);
            //sendChar('n');
            sendString("diam \n");
			//usart_putchar(USART_SERIAL_EXAMPLE,'n');
        }
       
        //in = receiveChar();
        receiveString();
		//in = usart_getchar(USART_SERIAL_EXAMPLE);
		//reads[0] = in;
       
        //snprintf(strbuf, sizeof(strbuf), "Read USART : %3d",in);
        gfx_mono_draw_string(reads,0, 0, &sysfont);
        delay_ms(50);
    }*/

}

        /*
        //sendString("haha");
        sendChar('q');
        in = usart_receiveByte();
        delay_ms(1000);
       
        snprintf(strbuf, sizeof(strbuf), "Read USART : %3d",in);
        gfx_mono_draw_string(strbuf,0, 0, &sysfont);
       
        sendChar('a');
        in = usart_receiveByte();
        delay_ms(1000);
       
        snprintf(strbuf, sizeof(strbuf), "Read USART : %3d",in);
        gfx_mono_draw_string(strbuf,0, 0, &sysfont);
       
        */
        //delay_ms(1000);