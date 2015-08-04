#ifndef _MAPLESYRUP_H
#define _MAPLESYRUP_H

#include "parse.h"

#define PARSE_MAX_STAMPSIZE 16

typedef struct ms_bitfield_info
{
    unsigned int array_elements;
    unsigned int cpu;
    unsigned int epl;
    unsigned int uid;
    unsigned int status;
    char part_number[PARSE_MAX_PARTNUMBER];
    char part_number_table[PARSE_MAX_PARTNUMBER];
    char stamp[PARSE_MAX_STAMPSIZE];
    char bitfield[PARSE_MAX_BITFIELD];
    char functional_group[PARSE_MAX_FUNCTIONAL];
    char register_name[PARSE_MAX_REGISTER];
    char description[PARSE_MAX_DESCRIPTION];    
    unsigned long long value;
} ms_bitfield_info;

enum
{
    NS_PRIVILEGE_LEVEL_0 = 1, // non-secure user
    NS_PRIVILEGE_LEVEL_1 = 2, // non-secure kernel
    NS_PRIVILEGE_LEVEL_2 = 4, // non-secure hyp
    
    S_PRIVILEGE_LEVEL_0 = 8,  // secure PL0
    S_PRIVILEGE_LEVEL_1 = 16, // secure PL1
    
    PREFERRED_PL        = 32, // preferred
};

enum
{
    ARCH_ARMV7A,
    ARCH_ARMV8A,
    IMPL_CORTEX_A7,
    IMPL_CORTEX_A9,
    IMPL_CORTEX_A15,
    IMPL_CORTEX_A53,
    IMPL_CORTEX_A57,
    IMPL_ALL,
    
    GICC_V2,
    GICD_V2,
    GICH_V2,
    GICV_V2,
};

#define NS_ALL (NS_PRIVILEGE_LEVEL_0 | NS_PRIVILEGE_LEVEL_1 | NS_PRIVILEGE_LEVEL_2)
#define S_ALL (S_PRIVILEGE_LEVEL_0 | S_PRIVILEGE_LEVEL_1)
#define ALL_NS_S (NS_PRIVILEGE_LEVEL_0 | NS_PRIVILEGE_LEVEL_1 | NS_PRIVILEGE_LEVEL_2) | (S_PRIVILEGE_LEVEL_0 | S_PRIVILEGE_LEVEL_1)

/******************************************************
 * Search for values by a given bitfield ID
 *  
 * @param bitfield [in] Input bitfield id
 * @param cpu [in] Requested CPU or -1 for all
 * @param pl [in] Level to execute command
 * @param results_out [out] Results. Caller must free.
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int ms_find_by_bitfield(char * arch,
                        char * impl,
                        unsigned int bitfield,
                        int cpu,
                        unsigned int pl,
                        unsigned int noparse,
                        ms_bitfield_info ** results_out);

/******************************************************
 * Search for values by a given register name
 *  
 * @param regname [in] Name of register
 * @param cpu [in] Requested CPU or -1 for all
 * @param pl [in] Level to execute command
 * @param results_out [out] Results. Caller must free.
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int ms_find_by_register(char * arch,
                        char * impl,
                        char * regname,
                        int cpu,
                        unsigned int pl,
                        unsigned int noparse,
                        char * calc_value,
                        ms_bitfield_info ** list_out);

/******************************************************
 * Search for values by functional group
 *  
 * @param groupname [in] Name of functional group
 * @param cpu [in] Requested CPU or -1 for all
 * @param pl [in] Level to execute command
 * @param results_out [out] Results. Caller must free.
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int ms_find_by_group(char * arch,
                    char * impl,
                    char * groupname,
                     int cpu,
                     unsigned int pl,
                     unsigned int noparse,
                     ms_bitfield_info ** list_out);
                     
                     
/******************************************************
 * Search tables for all entered groups
 *  
 * @param results_out [out] Results. Caller must free.
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int ms_get_groups(ms_bitfield_info ** list_out);

/******************************************************
 * Search tables for all entered register names
 *  
 * @param results_out [out] Results. Caller must free.
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int ms_get_registers(ms_bitfield_info ** list_out);

/******************************************************
 * Search tables for all valid bitfield ids
 *  
 * @param results_out [out] Results. Caller must free.
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int ms_get_bitfields(ms_bitfield_info ** list_out);

/******************************************************
 * Free the memory allocated by find and get
 *  
 * @param [in] tofree Results to free
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int ms_free_result(ms_bitfield_info ** tofree);

int ms_get_size(ms_bitfield_info * size);
unsigned long long ms_get_value(ms_bitfield_info * results_in, unsigned int index);
int ms_get_cpu(ms_bitfield_info * results_in, unsigned int index);
int ms_get_epl(ms_bitfield_info * results_in, unsigned int index);
int ms_get_uid(ms_bitfield_info * results_in, unsigned int index);
int ms_get_status(ms_bitfield_info * results_in, unsigned int index);
char * ms_get_bitfield_string(ms_bitfield_info * results_in, unsigned int index);
char * ms_get_register_string(ms_bitfield_info * results_in, unsigned int index);
char * ms_get_group_string(ms_bitfield_info * results_in, unsigned int index);
char * ms_get_description_string(ms_bitfield_info * results_in, unsigned int index);
char * ms_get_stamp(ms_bitfield_info * results_in, unsigned int index);
char * ms_get_partnumber_string(ms_bitfield_info * results_in, unsigned int index);
char * ms_get_decode_string(ms_bitfield_info * results_in, unsigned int index);

#endif
