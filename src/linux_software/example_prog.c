#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#define _BSD_SOURCE

#define GPIO_DIP_OFFSET 2
#define GPIO_LED_OFFSET 0 
#define DIPS_AND_LEDS_BASEADDR 0x41200000

#define UPDATE_INTERVAL_MICROSECONDS 500 // needs to be a factor of 500,000 (0.5 s)
#define COUNTER_INTERVAL_MICROSECONDS 500000

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr) {
	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}

int main() {
	// first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    volatile unsigned int *dips_and_leds_ptr = get_a_pointer(DIPS_AND_LEDS_BASEADDR);	
    printf("Lab 9 - Jonathan Piland\r\n(press Ctrl-C to exit)\r\n");
	fflush(stdout);
	unsigned long int elapsed_microseconds = 0;
	unsigned int counter_value = 0;
    while (1) {
		if (elapsed_microseconds == COUNTER_INTERVAL_MICROSECONDS) {
			elapsed_microseconds = 0;
			counter_value += 1;
			counter_value &= 0b1111;
		}
		*(dips_and_leds_ptr + GPIO_LED_OFFSET) = ((*(dips_and_leds_ptr + GPIO_DIP_OFFSET) & 0b111) << 4) | counter_value;
	    usleep(UPDATE_INTERVAL_MICROSECONDS);
		elapsed_microseconds += UPDATE_INTERVAL_MICROSECONDS;
    }
    return 0;
}
