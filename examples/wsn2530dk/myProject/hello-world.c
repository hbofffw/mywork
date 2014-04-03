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


 

//static process_event_t event_temperature_send;
//static process_event_t event_uart_send;


extern rimeaddr_t rimeaddr_node_addr;

static unsigned char rxbuf[128];
static int bufpos=0;
static int buflen=0;

static char *integerValue;

void *getintvalue(void)
{
	return (void*)integerValue;
}
void puttemperature(void);

//added by dongbo huang, this is to set the short address of this node
static void
 set_rime_addr(void)
 {
	signed char i;
  
	#if CC2530_CONF_MAC_FROM_PRIMARY
		unsigned char __xdata *macp = &X_IEEE_ADDR;
	#else
		unsigned char __code *macp = (__code unsigned char *)0xFFE8;
	#endif


//#if CC2530_CONF_MAC_FROM_PRIMARY
//  	putstring("Reading MAC from Info Page\r\n");
//#else
//  	putstring("Reading MAC from flash\r\n");

  /*
   * The MAC is always stored in 0xFFE8 of the highest BANK of our flash. This
   * maps to address 0xFFF8 of our CODE segment, when this BANK is selected.
   * Load the bank, read 8 bytes starting at 0xFFE8 and restore last BANK.
   * Since we are called from main(), this MUST be BANK1 or something is very
   * wrong. This code can be used even without a bankable firmware.
   */

  /* Don't interrupt us to make sure no BANK switching happens while working */
//  DISABLE_INTERRUPTS();

  /* Switch to the BANKn,
   * map CODE: 0x8000 - 0xFFFF to FLASH: 0xn8000 - 0xnFFFF */
//  FMAP = CC2530_LAST_FLASH_BANK;
//#endif

  for(i = (RIMEADDR_SIZE - 1); i >= 0; --i) {
    rimeaddr_node_addr.u8[i] = *macp;
    macp++;
  }

#if !CC2530_CONF_MAC_FROM_PRIMARY
  /* Remap 0x8000 - 0xFFFF to BANK1 */
  FMAP = 1;
  ENABLE_INTERRUPTS();
#endif

  /* Now the address is stored MSB first */
#if STARTUP_CONF_VERBOSE
  putstring("Rime configured with address ");
  for(i = 0; i < RIMEADDR_SIZE - 1; i++) {
    puthex(rimeaddr_node_addr.u8[i]);
    putchar(':');
  }
  puthex(rimeaddr_node_addr.u8[i]);
  putstring("\r\n");
#endif
  cc2530_rf_set_addr(IEEE802154_PANID);
  return;
}



//PROCESS(sensors_test_process,"Sensor test process");
PROCESS(test_send_process, "Testing send data");
PROCESS(test_receive_process, "Testing receive data");
AUTOSTART_PROCESSES(&test_send_process,&test_receive_process);
//AUTOSTART_PROCESSES(&sensors_test_process);


int uart_rx_callback(unsigned char c)
{
	rxbuf[bufpos] = c;
	bufpos++;
	buflen++;
	return 1;
}


/*

PROCESS_THREAD(sensors_test_process,ev,data)
{
	
	
	// Sensor Values 

	//static struct etimer et;	
	static int rv;
	static struct sensors_sensor * sensor;
	static float sane = 0;
	static int dec;
	static float frac;

	
	PROCESS_BEGIN();
	//etimer_set(&et, CLOCK_SECOND * 2);
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == event_temperature_send);
		
		
		//PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		printf("Starting senor Example......\r\n");
		sensor = (struct sensors_sensor *) sensors_find(ADC_SENSOR);
		if(sensor)
		{
			rv = sensor->value(ADC_SENSOR_TYPE_TEMP);
			if(rv != -1)
			{
				//sane = 25 + ((rv - 1480) / 4.5);
				sane = ((rv-1367.5)/4.5)-4;
				dec = (int)sane;
				frac = sane - dec;
				printf("Temp=%d.%d C (%d)\r\n", dec, (unsigned int)(frac*100), rv);
				//process_post(&test_send_process,event_uart_send,NULL);
			}
			
		}
		else
			putstring("get ADC failed!\r\n");
		//etimer_reset(&et);

	}
	
	PROCESS_END();
}
*/

PROCESS_THREAD(test_send_process, ev, data)
{
	static struct etimer et;
	static char hello[5];
	static int i;
	static char *testvalue;
	PROCESS_BEGIN();
	
	etimer_set(&et,CLOCK_CONF_SECOND/100);
	//event_temperature_send = process_alloc_event();
	//event_uart_send = process_alloc_event();
	//process_post(&test_send_process,event_uart_send,NULL);
	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		//PROCESS_WAIT_EVENT_UNTIL(ev == event_uart_send);
		if(strncmp(rxbuf,"hello",5) == 0)
		{
			//putstring("instruction indicated");
			strcpy(rxbuf,"");
			bufpos = 0;
			buflen = 0;
			unsigned char __code *dst = (__code unsigned char *)FRAME802154_BROADCASTADDR;
			puthex(*dst);
			puthex(*dst++);
			testvalue = getintvalue();
			strcpy(testvalue,"hello");
			
			printf("\r\ntest value is %s, function value = %d, initial value = %d\r\n", testvalue, (char*)getintvalue(),integerValue);
			//strcpy(hello,"hello");
			//now I'm trying to test the basic cc2530rf functionality
			/*cc2530_rf_driver.init();
			if(cc2530_rf_driver.send((void*)hello,sizeof("hello")) == RADIO_TX_OK)
			{
				strcpy(hello,"");
				putstring("some packets sent");
			}
			else
				putstring("send failed");
			*/
			//edited by dognbo huang, the codes commented below is used for to analyse the ADC module working procedure
			//puttemperature();
			//putstring("getting temperature...\r\n");
			//process_post(&sensors_test_process,event_temperature_send,NULL);
		}
		else
		{
			strcpy(rxbuf,"");
			bufpos = 0;
			buflen = 0;
		}
		
		etimer_reset(&et);
	}
	PROCESS_END();
}


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





void puttemperature()
{
	int rv;
	struct sensors_sensor * sensor;
	
	float sane = 0;
	int dec;
	float frac;
	sensor = (struct sensors_sensor *) sensors_find(ADC_SENSOR);
	if(sensor)
	{
		rv = sensor->value(ADC_SENSOR_TYPE_TEMP);
		if(rv != -1)
		{
			sane = 11 + ((rv - 1480) / 4.5);
			//sane = ((rv-1367.5)/4.5)-4;
			dec = (int)sane;
			frac = sane - dec;
			printf("Temp=%d.%d C (%d)\r\n", dec, (unsigned int)(frac*100), rv);
		}
	}
	else
		putstring("get ADC value failed\r\n");
	
}


