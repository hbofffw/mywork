/**
 * \file
 *         
 * \author
 Dongbo Huang-CUMT
 */     



#include "contiki.h"
#include "soc.h"
#include "stack.h"
#include "sys/clock.h"
#include "sys/autostart.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#include "dev/leds.h"
#include "dev/io-arch.h"
#include "dev/dma.h"
#include "dev/cc2530-rf.h"
#include "dev/watchdog.h"
#include "dev/clock-isr.h"
#include "dev/port2.h"
#include "dev/lpm.h"
#include "dev/button-sensor.h"
#include "dev/adc-sensor.h"
#include "dev/leds-arch.h"
#include "net/rime.h"
#include "net/netstack.h"
#include "net/mac/frame802154.h"
#include "debug.h"
#include "cc253x.h"
#include "sfr-bits.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <stdio.h> /* For printf() */
#include <string.h>



#include "dataformat/dataformat.h"


//static process_event_t event_temperature_send;
//static process_event_t event_uart_send;




static unsigned char serial_rxbuf[128];
static int buf_pos=0;
static int buf_len=0;


PROCESS(test_receive_process,"Testing receive process");
PROCESS(test_send_process, "Testing send data");

AUTOSTART_PROCESSES(&test_send_process,&test_receive_process);
//AUTOSTART_PROCESSES(&sensors_test_process);

//uart input handler communication 
int uart_rx_callback(unsigned char c)
{
    serial_rxbuf[buf_pos] = c;
    buf_pos++;
    buf_len++;
    return 1;
}

PROCESS_THREAD(test_send_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();

    etimer_set(&et,CLOCK_CONF_SECOND/100);

    while(1) 
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        
        if(strcmp(serial_rxbuf,"hl") == 0)
        //if(serial_rxbuf == 0x00)
        {
            memset(serial_rxbuf,0,sizeof(serial_rxbuf));
            buf_pos = 0;
            buf_len = 0;
            //cc2530_rf_driver.on();
            if(instructionSend(HELLOMSG))
                putstring("hello data sent\r\n");
            else
                putstring("frame build failed!!!\r\n");
            
        }
        else if(strncmp(serial_rxbuf,"sleep",5) == 0)
        {
            memset(serial_rxbuf,0,sizeof(serial_rxbuf));
            buf_pos = 0;
            buf_len = 0;
            if(instructionSend(SLEEPMSG))
                putstring("sleep data sent\r\n");
            else
                putstring("data sent failed\r\n");
        }
        else
        {
            strcpy(serial_rxbuf,"");
            buf_pos = 0;
            buf_len = 0;
        }
        if(getInformationListLength()!= 0)
        {
            sink_printItemOfList();
        }
        
        etimer_reset(&et);
    }
    PROCESS_END();
}

//serial communication process
PROCESS_THREAD(test_receive_process, ev, data)
{

    PROCESS_BEGIN();

    uart0_set_input(uart_rx_callback);

    while(1) 
    {
        PROCESS_YIELD(); 
    }

    PROCESS_END();
}



