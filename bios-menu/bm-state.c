#include <stdint.h>
#include <sms/console.h>
#include "bios-menu.h"
#include "rom-tools.h"
#include "mapper-test.h"
#include "x-bootloader.h"

#define ON_ENTRY -1
#define ON_EXEC  0
#define ON_EXIT  1

#define STATE_NULL               0
#define STATE_MAIN_MENU          1
#define STATE_BOOT_CARTRIDGE     2
#define STATE_BOOT_CARD_SLOT     3
#define STATE_BOOT_EXPANSION     4
#define STATE_BOOT_BIOS          5
#define STATE_BOOT_GENERIC       6
#define STATE_SYSTEM_INFO        7
#define STATE_CARTRIDGE_ROM_INFO 8 
#define STATE_CARD_ROM_INFO      9
#define STATE_EXPANSION_ROM_INFO 10
#define STATE_BIOS_ROM_INFO      11
#define STATE_GENERIC_ROM_INFO   12
#define STATE_BOOT_GENERIC_CHECK 13
#define STATE_MAPPER_TEST        14
#define STATE_BOOTLOADER         15


#define STATE_INITIAL     STATE_MAIN_MENU

/* --- state variables --- */
static uint8_t state_current;
static uint8_t state_next;
static uint8_t boot_media;

/* --- Helper functions --- */
static uint8_t update_input_and_cursor(){
    uint8_t rv;
    rv = update_input();
    switch(rv){
        case KEY_UP:
        draw_cursor(get_cursor() - 1); break;
        case KEY_DOWN:
        draw_cursor(get_cursor() + 1); break;
        default:
        break;
    }
    return rv;
}


/* --- State implementations --- */
static uint8_t state_main_menu(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw main menu */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("BIOS menu for Master System");
        
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 0);
        con_put("Cartridge");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 1);
        con_put("Card slot");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 2);
        con_put("Expansion port");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 3);
        con_put("BIOS");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 4);
        con_put("Bootloader");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 5);
        con_put("Mapper test");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 6);
        con_put("System info");
        
        con_gotoxy(3, 23);
        con_put("Build type: ");
#if PROGRAM_MEDIA == ROM_BIOS
        con_put("BIOS");
#elif PROGRAM_MEDIA == ROM_CARTRIDGE
        con_put("CARTRIDGE");
#elif PROGRAM_MEDIA == ROM_CARD_SLOT
        con_put("CARD SLOT");
#elif PROGRAM_MEDIA == ROM_EXPANSION
        con_put("EXPANSION");
#else
#error Unknown PROGRAM_MEDIA value
#endif
        
        set_cursor_limits(0,6);
        draw_cursor(0);
    }
    else if(mode == ON_EXIT){
        return STATE_MAIN_MENU;
    }
    else{
        uint8_t key;
        uint8_t cursor;
        /*Scan input, if button 1 is pressed and cursor is on*/
        /*the correct position, transition to a state, if not*/
        /*stay here.*/
        key = update_input_and_cursor();
        cursor = get_cursor();
        
        if(key == KEY_1){
            switch(cursor){
                case 0:
                return STATE_BOOT_CARTRIDGE;
                
                case 1:
                return STATE_BOOT_CARD_SLOT;
                
                case 2:
                return STATE_BOOT_EXPANSION;
                
                case 3:
                return STATE_BOOT_BIOS;
                
                case 4:
                return STATE_BOOTLOADER;
                
                case 5:
                return STATE_MAPPER_TEST;
                
                case 6:
                return STATE_SYSTEM_INFO;
                
                default:
                break;
            }
        }
    }
    return STATE_MAIN_MENU;
}

static uint8_t state_system_info(int8_t mode){
    if(mode == ON_ENTRY){
        /* Draw header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("System info\n\n");
        
        /*Dump info*/
        show_sysinfo();
    }
    else if(mode == ON_EXIT){
        
    }
    else{
        uint8_t key;
        uint8_t cursor;

        key = update_input_and_cursor();
        cursor = get_cursor();
        
        /*If button 2 is pressed, go back to main menu*/
        if(key == KEY_2){
            return STATE_MAIN_MENU;
        }
    }
    return STATE_SYSTEM_INFO;
}

static uint8_t state_boot_cartridge(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw boot header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("Cartridge");
        
        /*Set BOOT mode to CARTRIDGE SLOT*/
        boot_media = ROM_CARTRIDGE;
    }
    return STATE_BOOT_GENERIC;
}

static uint8_t state_boot_card_slot(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw card boot header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("Card slot");
        
        /*Set BOOT mode to CARD SLOT*/
        boot_media = ROM_CARD_SLOT;
    }
    return STATE_BOOT_GENERIC;
}

static uint8_t state_boot_expansion(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw card boot header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("Expansion port");
        
        /*Set BOOT mode to EXPANSION PORT*/
        boot_media = ROM_EXPANSION;
    }
    return STATE_BOOT_GENERIC;
}

static uint8_t state_boot_bios(int8_t mode){
    if(mode == ON_ENTRY){
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("BIOS");
        
        /*Set BOOT mode to BIOS*/
        boot_media = ROM_BIOS;
    }
    return STATE_BOOT_GENERIC;
}

static uint8_t state_boot_generic(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw boot options */
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 0);
        con_put("Boot");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 1);
        con_put("Boot (without tests)");
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 2);
        con_put("ROM info");
        
        set_cursor_limits(0,2);
        draw_cursor(0);
    }
    else if(mode == ON_EXIT){
        
    }
    else{
        uint8_t key;
        uint8_t cursor;
        
        key = update_input_and_cursor();
        cursor = get_cursor();
        
        /*If button 2 is pressed, go back to main menu*/
        if(key == KEY_2){
            return STATE_MAIN_MENU;
        }
        else if(key == KEY_1){
            switch(cursor){
                case 0:
                return STATE_BOOT_GENERIC_CHECK;
                case 1:
                rom_boot(boot_media); break;
                case 2:
                {
                    if(boot_media == ROM_CARTRIDGE) return STATE_CARTRIDGE_ROM_INFO;
                    if(boot_media == ROM_CARD_SLOT) return STATE_CARD_ROM_INFO;
                    if(boot_media == ROM_EXPANSION) return STATE_EXPANSION_ROM_INFO;
                    if(boot_media == ROM_BIOS)      return STATE_BIOS_ROM_INFO;
                }
                default:
                break;
            }
        }
    }
    return STATE_BOOT_GENERIC;
}

uint8_t state_boot_generic_check(int8_t mode){
    if (mode == ON_ENTRY){
        sega_header_t* header;
        volatile uint16_t vi;
        uint16_t checksum;
        
        header = get_sega_header(boot_media);
        /* Draw boot process */
        con_gotoxy(LEFT_MARGIN + 2, TOP_MARGIN + 2 + 4);
        con_put("Checksum: ");
        con_put("****");
        checksum = rom_checksum(boot_media);
        con_relxy(-4,0);
        if(checksum == header->checksum){
            con_put("OK! ");
            /*Boot*/
            rom_boot(boot_media);
        }
        else{
            con_put("Failed!");
        }
        /*Add a small delay*/
        for(vi=0; vi<5000; vi++){
            vi = vi;
        }
    }
    else if(mode == ON_EXIT){
        
    }
    else{
        
    }
    return STATE_BOOT_GENERIC;
}

uint8_t state_cartridge_rom_info(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw cartridge info header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("Cartridge ROM info");
        
        /*Set media to Cartridge*/
        boot_media = ROM_CARTRIDGE;
    }
    return STATE_GENERIC_ROM_INFO;
}

uint8_t state_card_rom_info(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw card info header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("Card slot ROM info");
        
        /*Set media to Card*/
        boot_media = ROM_CARD_SLOT;
    }
    return STATE_GENERIC_ROM_INFO;
}

uint8_t state_expansion_rom_info(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw expansion info header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("Expansion ROM info");
        
        /*Set media to Expansion*/
        boot_media = ROM_EXPANSION;
    }
    return STATE_GENERIC_ROM_INFO;
}

uint8_t state_bios_rom_info(int8_t mode){
    if (mode == ON_ENTRY){
        /* Draw expansion info header */
        con_clear();
        con_gotoxy(1, TOP_MARGIN + 0);
        con_put("BIOS ROM info");
        
        /*Set media to Expansion*/
        boot_media = ROM_BIOS;
    }
    return STATE_GENERIC_ROM_INFO;
}

uint8_t state_generic_rom_info(int8_t mode){
    if (mode == ON_ENTRY){
        /*print ROM info*/
        rom_info(boot_media);
    }
    else if(mode == ON_EXIT){
        
    }
    else{
        uint8_t key;
        uint8_t cursor;
        key = update_input_and_cursor();
        cursor = get_cursor();
        
        /*If button 2 is pressed, go back*/
        if(key == KEY_2){
            if(boot_media == ROM_CARD_SLOT) return STATE_BOOT_CARD_SLOT;
            if(boot_media == ROM_CARTRIDGE) return STATE_BOOT_CARTRIDGE;
            if(boot_media == ROM_EXPANSION) return STATE_BOOT_EXPANSION;
            if(boot_media == ROM_BIOS)      return STATE_BOOT_BIOS;
        }
    }
    return STATE_GENERIC_ROM_INFO;
}

uint8_t state_mapper_test(int8_t mode){
    if(mode == ON_ENTRY){
        mapper_test();
    }
    else if(mode == ON_EXIT){
        
    }
    else{
        uint8_t key;
        key = update_input();
        
        if(key == KEY_2){
            return STATE_MAIN_MENU;
        }
    }
    return STATE_MAPPER_TEST;
}

uint8_t state_bootloader(int8_t mode){
    if(mode == ON_ENTRY){
        xboot();
    }
    return STATE_MAIN_MENU;
}

/* State-update functions */

static uint8_t call_state(uint8_t state_id, int8_t mode){
    switch(state_id){
        case STATE_MAIN_MENU:
        return state_main_menu(mode);
        case STATE_BOOT_CARD_SLOT:
        return state_boot_card_slot(mode);
        case STATE_BOOT_CARTRIDGE:
        return state_boot_cartridge(mode);
        case STATE_BOOT_EXPANSION:
        return state_boot_expansion(mode);
        case STATE_BOOT_BIOS:
        return state_boot_bios(mode);
        case STATE_BOOT_GENERIC:
        return state_boot_generic(mode);
        case STATE_SYSTEM_INFO:
        return state_system_info(mode);
        case STATE_CARTRIDGE_ROM_INFO:
        return state_cartridge_rom_info(mode);
        case STATE_CARD_ROM_INFO:
        return state_card_rom_info(mode);
        case STATE_EXPANSION_ROM_INFO:
        return state_expansion_rom_info(mode);
        case STATE_BIOS_ROM_INFO:
        return state_bios_rom_info(mode);
        case STATE_GENERIC_ROM_INFO:
        return state_generic_rom_info(mode);
        case STATE_BOOT_GENERIC_CHECK:
        return state_boot_generic_check(mode);
        case STATE_MAPPER_TEST:
        return state_mapper_test(mode);
        case STATE_BOOTLOADER:
        return state_bootloader(mode);
        
        default:
        return state_id;
    }
}

void bm_state_init(){
    state_current = STATE_NULL;
    state_next    = STATE_INITIAL;
}

void bm_state_tick(){
    if(state_current != state_next){
        call_state(state_current, ON_EXIT);
        call_state(state_next, ON_ENTRY);
        state_current = state_next;
    }
    state_next = call_state(state_current, ON_EXEC);
}