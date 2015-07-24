#ifndef _PARSE_BREAKOUT_H
#define _PARSE_BREAKOUT_H

#include "support.h"
#include "parse.h"


#define PPN_CORTEX_A7 0xC07
#define PPN_CORTEX_A9 0xC09
#define PPN_CORTEX_A15 0xC0F
#define PPN_CORTEX_A53 0xD03
#define PPN_CORTEX_A57 0xD07


typedef struct _bitfield_table_entry
{
    unsigned int table_type;
    bitfield_info * table_ptr;
    int (*table_size_ptr)(void);
} bitfield_table_entry;

enum
{
    TYPE_IMPL,
    TYPE_ARCH,
};
    

int get_bitfieldstamp(unsigned int code, char * buffer);
int get_partnumber(unsigned int midr, char * buffer);
int get_decode_table(unsigned int uid, char * buffer);
char * get_error_string(unsigned int errval);

/******************************************************
 * Used internally to add items to a list after
 * bitfield parsing
 *  
 * @param [in] UID
 * @param [out] Index
 * 
 * ***************************************************/
int breakout_add_bitfield_result(ms_list_head * list,
                                unsigned int uid,
                                unsigned int ioctl,
                                unsigned long long value,
                                unsigned long long uvalue,
                                unsigned int cpu,
                                unsigned int epl,
                                unsigned int midr,
                                unsigned int status);
        
/******************************************************
 * Finds matching registers and returns their
 * status
 *  
 * @param [in] Info to match
 * @param [out] Results
 * 
 * ***************************************************/
int find_bitfields(match_info * info, ms_list_head * list);

int get_groups(ms_list_head * listout);
int get_registers(ms_list_head * listout);
int get_bitfields(ms_list_head * listout);

#endif
