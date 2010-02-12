#include <pci_vendor.h>
#include <types.h>

#define CONFIG_ADDRESS	0x0CF8
#define CONFIG_DATA	0x0CFC

struct pci_standard_header
{
	// Identifiant du périphérique
	uint16_t device_id;
	// Identifiant du constructeur
	uint16_t vendor_id;
	
	
	uint16_t status;
	uint16_t command;
	
	uint8_t class_code;
	uint8_t subclass;
	uint8_t prog_if;
	uint8_t revision_id;
	
	uint8_t bist;
	uint8_t header_type;
	uint8_t latency_timer;
	uint8_t cache_line_size;
	
	//Base address register
	uint32_t bar0;
	uint32_t bar1;
	uint32_t bar2;
	uint32_t bar3;
	uint32_t bar4;
	uint32_t bar5;
	
	uint32_t cardbus_cis_pointer;
	
	uint16_t subsystem_id;
	uint16_t subsystem_vendor_id;
	
	uint16_t expansion_rom_base_address;
	
	uint8_t capabilities_pointer;
	
	uint8_t max_latency;
	uint8_t min_grant;
	uint8_t interrupt_pin;
	uint8_t interrupt_line;
	
};

/** TODO
struct pci_bridge_header
{
	
};
 */

/** TODO
uint32_t pci_read_register(uint8_t bus,
			   uint8_t slot,
			   uint8_t func,
			   uint8_t reg);
*/
