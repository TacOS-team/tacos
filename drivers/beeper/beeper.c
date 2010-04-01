#include <beeper.h>
#include <types.h>
#include <ioports.h>

static void set_PIT_2(uint32_t frequency)
{
 	uint32_t div;
    //Set the PIT to the desired frequency
 	div = 1193180 / frequency;
 	outb(0xb6, 0x43);
 	outb((uint8_t) (div), 0x42);
 	outb((uint8_t) (div >> 8), 0x42);
}

//Play sound using built in speaker
static void play_sound(uint32_t nFrequence)
{
 	uint8_t tmp;
 
    set_PIT_2(nFrequence);
 
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
 }
 
 //make it shutup
 static void nosound()
 {
 	uint8_t tmp = (inb(0x61) & 0xFC);
 
 	outb(0x61, tmp);
 }
 
 //Make a beep
 void beep()
 {
	 int i;
 	 play_sound(1000);
 	 for(i=0 ; i<10000000 ; i++);
 	 nosound();
     //set_PIT_2(old_frequency);
 }
