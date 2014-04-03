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



PROCESS(data_incoming_process,"data incoming process");


/*PROCESS(data_outgoing_process, "data outgoing data");*/

/*AUTOSTART_PROCESSES(&data_outgoing_process);*/
AUTOSTART_PROCESSES(&data_incoming_process);





/*PROCESS_THREAD(data_outgoing_process, ev, data)*/
/*{*/
/*//static struct etimer et;*/
/*PROCESS_BEGIN();*/
/*//etimer_set(&et,CLOCK_CONF_SECOND/10);*/
/*//event_temperature_send = process_alloc_event();*/
/*//event_uart_send = process_alloc_event();*/
/*//process_post(&test_send_process,event_uart_send,NULL);*/
/*//get_self_addr();*/
/*sensor_incomingPktProcessing();*/
/*while(1)*/
/*{*/
/*[>PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));<]*/
/*//PROCESS_WAIT_EVENT_UNTIL(ev == event_uart_send);*/
/*PROCESS_YIELD(); */
/*//testReceivedData();*/
/*[>etimer_reset(&et);<]*/
/*}*/
/*PROCESS_END();*/
/*}*/

PROCESS_THREAD(data_incoming_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    etimer_set(&et, CLOCK_CONF_SECOND/5);
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        if(getpacketbufListLength() != 0)
        {
            sensor_popAndSendItemOfList();
        }
        etimer_reset(&et);
    }
    PROCESS_END();
}




