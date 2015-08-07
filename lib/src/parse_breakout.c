#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "maplesyrup.h"
#include "ms_debug.h"
#include "ms_error.h"
#include "support.h"
#include "parse_breakout.h"
#include "bitfields/bitfield_armv7a.h"
#include "bitfields/bitfield_armv8a.h"
#include "bitfields/bitfield_cortex-a7.h"
#include "bitfields/bitfield_cortex-a15.h"
#include "bitfields/bitfield_cortex-a53.h"
#include "bitfields/bitfield_cortex-a57.h"
#include "bitfields/bitfield_gicv2.h"
#include "raw_results.h"


bitfield_table_entry bitfield_table[] =
{   
    { TYPE_IMPL, IMPL_CORTEX_A7, bitfield_cortex_a7_table, return_bitfield_cortex_a7_size, NULL },    
    { TYPE_IMPL, IMPL_CORTEX_A15, bitfield_cortex_a15_table, return_bitfield_cortex_a15_size, NULL }, 
    { TYPE_IMPL, IMPL_CORTEX_A53, bitfield_cortex_a53_table, return_bitfield_cortex_a53_size, NULL },
    { TYPE_IMPL, IMPL_CORTEX_A57, bitfield_cortex_a57_table, return_bitfield_cortex_a57_size, NULL },
    
    { TYPE_ARCH, ARCH_ARMV7A, bitfield_armv7a_table, return_bitfield_armv7a_size, NULL},
    { TYPE_ARCH, ARCH_ARMV8A, bitfield_armv8a_table, return_bitfield_armv8a_size, NULL},
    
    { TYPE_MM, GICC_V2, bitfield_giccv2_table, return_bitfield_giccv2_size, return_bitfield_giccv2_compsize},
    { TYPE_MM, GICD_V2, bitfield_gicdv2_table, return_bitfield_gicdv2_size, return_bitfield_gicdv2_compsize},
    
};

static bitfield_info_heap * g_bitfield_table_heap = NULL;
static unsigned int g_bitfield_table_size = 0;


static
unsigned int
mscode(unsigned int partnumber,
        unsigned int instruction,
        unsigned int field)
{
    unsigned int code = 0;
    
    code = (partnumber << MSCODE_PARTNUMBERSHIFT);
    code |= (instruction << MSCODE_INSTRUCTIONSHIFT);
    code |= (field << MSCODE_FIELDSHIFT);

    return code;
}

static
unsigned int
create_uid(unsigned int i,
            unsigned int j,
            unsigned int k,
            unsigned int part)
{   
    return mscode(part,
                    bitfield_table[i].table_ptr[j].register_uid,
                    bitfield_table[i].table_ptr[j].register_uid + (k * MS_GICV2_ALIGNMENT));    
}

int
read_tables_to_memory(
    int include_devices)
{
    int result = -1;
    int i = 0;
    int j = 0;
    int k = 0;
    unsigned int buffer_size = 0;
    unsigned int buffer_index = 0;
    char num_buffer[32];
    /* Expand tables into memory
  
    /* Get allocation size & allocate */    
    for (i = 0; i < sizeof(bitfield_table) / sizeof(bitfield_table_entry); ++i)
    {   
        buffer_size += bitfield_table[i].table_size_ptr() * sizeof(bitfield_info_heap);     
    }   
    
    g_bitfield_table_heap = (bitfield_info_heap *)malloc(buffer_size);
    if (g_bitfield_table_heap == NULL)
    {
        TRACE("Failed to allocate %d\n", buffer_size);
        goto done;
    }
    TRACE("Allocated %d bytes for table\n", buffer_size);

    buffer_index = 0;
    /* Copy & expand table entries */
    for (i = 0; i < sizeof(bitfield_table) / sizeof(bitfield_table_entry); ++i)
    {   
        if ((bitfield_table[i].table_type == TYPE_ARCH) ||
            (bitfield_table[i].table_type == TYPE_IMPL))
        {
            /* System Register access */
            for (j = 0; j < bitfield_table[i].table_size_ptr(); ++j)
            {
                g_bitfield_table_heap[buffer_index].table_type =        bitfield_table[i].table_type;
                g_bitfield_table_heap[buffer_index].info.range =        bitfield_table[i].table_ptr[j].range;
                
                /* Generate uid */
                g_bitfield_table_heap[buffer_index].info.uid =    mscode(bitfield_table[i].part_number,
                                                                                bitfield_table[i].table_ptr[j].register_uid,
                                                                                bitfield_table[i].table_ptr[j].uid);
                
                g_bitfield_table_heap[buffer_index].info.register_uid =       bitfield_table[i].table_ptr[j].register_uid;
                g_bitfield_table_heap[buffer_index].info.ppl =              bitfield_table[i].table_ptr[j].ppl;
                g_bitfield_table_heap[buffer_index].info.rightshift =       bitfield_table[i].table_ptr[j].rightshift;
                g_bitfield_table_heap[buffer_index].info.mask =                 bitfield_table[i].table_ptr[j].mask;
                g_bitfield_table_heap[buffer_index].info.bitfield =             bitfield_table[i].table_ptr[j].bitfield;
                g_bitfield_table_heap[buffer_index].info.functional_group =     bitfield_table[i].table_ptr[j].functional_group;
                
                g_bitfield_table_heap[buffer_index].info.description =      bitfield_table[i].table_ptr[j].description;
                strncpy(g_bitfield_table_heap[buffer_index].register_name_expanded, bitfield_table[i].table_ptr[j].register_name, PARSE_MAX_REGISTER - 1);
                ++buffer_index;
            }           
        }
        if ((include_devices == 1) && (bitfield_table[i].table_type == TYPE_MM))
        {
            /* Memory mapped access */
            for (j = 0; j < bitfield_table[i].table_compsize_ptr(); ++j)
            {
                /* Create an entry for each index in range
                 * Allocate unique code based off offset */
                for (k = 0; k < bitfield_table[i].table_ptr[j].range; k++)
                {
                    g_bitfield_table_heap[buffer_index].table_type =        bitfield_table[i].table_type;
                    g_bitfield_table_heap[buffer_index].info.range =        bitfield_table[i].table_ptr[j].range;
                    g_bitfield_table_heap[buffer_index].info.ppl =              bitfield_table[i].table_ptr[j].ppl;
                    g_bitfield_table_heap[buffer_index].info.rightshift =       bitfield_table[i].table_ptr[j].rightshift;
                    g_bitfield_table_heap[buffer_index].info.mask =                 bitfield_table[i].table_ptr[j].mask;
                    g_bitfield_table_heap[buffer_index].info.bitfield =             bitfield_table[i].table_ptr[j].bitfield;
                    g_bitfield_table_heap[buffer_index].info.functional_group =     bitfield_table[i].table_ptr[j].functional_group;                    
                    g_bitfield_table_heap[buffer_index].info.description =      bitfield_table[i].table_ptr[j].description;
                    
                    if (bitfield_table[i].table_ptr[j].range > 1)
                    {
                        /* create register_uid */                        
                        g_bitfield_table_heap[buffer_index].info.register_uid = create_uid(i, j, k, bitfield_table[i].part_number);
                        g_bitfield_table_heap[buffer_index].info.uid = mscode(bitfield_table[i].part_number, 
                                                                                bitfield_table[i].table_ptr[j].register_uid,
                                                                                buffer_index);
                        
                        /* expand register name */
                        snprintf(num_buffer, 31, "%d", k);
                        strncpy(g_bitfield_table_heap[buffer_index].register_name_expanded, bitfield_table[i].table_ptr[j].register_name, PARSE_MAX_REGISTER - 1);
                        strncat(g_bitfield_table_heap[buffer_index].register_name_expanded, num_buffer, PARSE_MAX_REGISTER - 1);                        
                    }
                    else
                    {
                        g_bitfield_table_heap[buffer_index].info.uid = mscode(bitfield_table[i].part_number,
                                                                                bitfield_table[i].table_ptr[j].register_uid,
                                                                                bitfield_table[i].table_ptr[j].uid);
                        g_bitfield_table_heap[buffer_index].info.register_uid = create_uid(i, j, k, bitfield_table[i].part_number);
                        strncpy(g_bitfield_table_heap[buffer_index].register_name_expanded, bitfield_table[i].table_ptr[j].register_name, PARSE_MAX_REGISTER - 1);
                    }
                    ++buffer_index;
                }
            }           
        }
    }
    
    g_bitfield_table_size = buffer_index;
    result = 0;
done:
    return result;
}

void
print_table()
{
    int i = 0;
    for (i = 0; i < g_bitfield_table_size; i++)
    {
        printf("UID: %08x REG_UID: %08x: REG_EXPANDED: %s FIELDNAME: %s\n",
                    g_bitfield_table_heap[i].info.uid,
                    g_bitfield_table_heap[i].info.register_uid,
                    g_bitfield_table_heap[i].register_name_expanded,
                    g_bitfield_table_heap[i].info.bitfield);
    }       
}

void
erase_tables()
{
    if (g_bitfield_table_heap != NULL)
    {
        free(g_bitfield_table_heap);
        g_bitfield_table_heap = NULL;
    }
}

static
long long
power(int x, int n)
{
    long long m;    
    if (n == 0) return 1;
    if ((n % 2) == 0)
    {
        m = power(x, n / 2);
        return m * m;
    }
    else
    {
        return x * power(x, n - 1);
    }
}

#define MASK_MAX_BITS 64

static
unsigned long long
get_mask(unsigned int bits)
{
    if (bits <= MASK_MAX_BITS)
    {   
        return (unsigned long long)(power(2, bits) - 1);
    }
    
    return 0;
}

static
unsigned long long
breakout_applymask(unsigned long long value,
                    unsigned int start,
                    unsigned int mask)
{   
    return ((value >> start) & get_mask(mask));
}



/******************************************************
 * Searches all bitfield tables for the specified
 * uid, and returns the table index
 *  
 * @param [in] UID
 * @param [out] Index 
 * 
 * ***************************************************/
static
int
breakout_get_index(
    unsigned int code,  
    unsigned int * index)
{
    unsigned int i = 0; 
    unsigned int table_size = 0;
    int result = -1;
    
    for (i = 0; i < g_bitfield_table_size; ++i)
    {       
        if (g_bitfield_table_heap[i].info.uid == code)
        {
            *index = i;             
            result = 0;
            goto done;
        } 
    }
    result = -1;
done:
    return result;  
}

static
int
get_ioctl(
    unsigned int code)
{
    unsigned int index = -1;
    if (breakout_get_index(code, &index) == 0)
    {
        return g_bitfield_table_heap[index].info.register_uid;
    }
    return -1;
}

static
const char *
get_register_name(
    unsigned int code)
{   
    unsigned int index = -1;
    if (breakout_get_index(code, &index) == 0)
    {
        return g_bitfield_table_heap[index].register_name_expanded;
    }
    return NULL;
}

static
const char *
get_functional_group(
    unsigned int code)
{   
    unsigned int index = -1;
    if (breakout_get_index(code, &index) == 0)
    {
        return g_bitfield_table_heap[index].info.functional_group;
    }
    return NULL;
}

static
const char *
get_description(
    unsigned int code)
{   
    unsigned int index = -1;
    if (breakout_get_index(code, &index) == 0)
    {
        return g_bitfield_table_heap[index].info.description;
    }
    return NULL;
}

static
const char *
get_bitfield(
    unsigned int code)
{   
    unsigned int index = -1;
    if (breakout_get_index(code, &index) == 0)
    {
        return g_bitfield_table_heap[index].info.bitfield;
    }
    return NULL;
}

static
int
get_arch_from_impl(unsigned int impl)
{
    switch(impl)
    {
        case IMPL_CORTEX_A15:
            return ARCH_ARMV7A;
        case IMPL_CORTEX_A7:
            return ARCH_ARMV7A;
        case IMPL_CORTEX_A9:
            return ARCH_ARMV7A;
        case IMPL_CORTEX_A53:
            return ARCH_ARMV8A;
        case IMPL_CORTEX_A57:
            return ARCH_ARMV8A;
        default:
            return -1;
    }
}


static
int
get_arch_from_midr(unsigned int midr)
{
    unsigned int part_number = ((midr >> 4) & 0xFFF);   
    switch(part_number)
    {
        case PPN_CORTEX_A7:
            return get_arch_from_impl(IMPL_CORTEX_A7);
        case PPN_CORTEX_A9:
            return get_arch_from_impl(IMPL_CORTEX_A9);
        case PPN_CORTEX_A15:
            return get_arch_from_impl(IMPL_CORTEX_A15);
        case PPN_CORTEX_A53:
            return get_arch_from_impl(IMPL_CORTEX_A53);
        case PPN_CORTEX_A57:
            return get_arch_from_impl(IMPL_CORTEX_A57);
        default:
            return -1;
    }   
}

static
int
get_impl_from_midr(unsigned int midr)
{
    unsigned int part_number = ((midr >> 4) & 0xFFF);   
    switch(part_number)
    {
        case PPN_CORTEX_A7:
            return IMPL_CORTEX_A7;
        case PPN_CORTEX_A9:
            return IMPL_CORTEX_A9;
        case PPN_CORTEX_A15:
            return IMPL_CORTEX_A15;
        case PPN_CORTEX_A53:
            return IMPL_CORTEX_A53;
        case PPN_CORTEX_A57:
            return IMPL_CORTEX_A57;
        default:
            return -1;          
    }
}

int
get_partnumber_from_midr(unsigned int midr, 
                char * buffer)
{
    int result = -1;
    unsigned int part_number = ((midr >> 4) & 0xFFF);
    
    if (buffer == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    
    switch(part_number)
    {
        case PPN_CORTEX_A7:
            strncpy(buffer, "cortex-a7", PARSE_MAX_PARTNUMBER - 1);
            break;
        case PPN_CORTEX_A9:
            strncpy(buffer, "cortex-a9", PARSE_MAX_PARTNUMBER - 1);
            break;
        case PPN_CORTEX_A15:
            strncpy(buffer, "cortex-a15", PARSE_MAX_PARTNUMBER - 1);
            break;
        case PPN_CORTEX_A53:
            strncpy(buffer, "cortex-a53", PARSE_MAX_PARTNUMBER - 1);
            break;
        case PPN_CORTEX_A57:
            strncpy(buffer, "cortex-a57", PARSE_MAX_PARTNUMBER - 1);
            break;          
        default:
            goto done;
            break;
    }
    
    result = 0;
done:
    return result;
}

int
get_decode_table(unsigned int uid,
                        char * buffer)
{
    unsigned int index = MSCODE_PARTNUMBER(uid);
    int status = -1;
    
    if (buffer == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    switch (index)
    {       
        case ARCH_ARMV7A:
            strncpy(buffer, "armv7a", PARSE_MAX_PARTNUMBER - 1);
            break;
        case ARCH_ARMV8A:
            strncpy(buffer, "armv8a", PARSE_MAX_PARTNUMBER - 1);
            break;
        case IMPL_CORTEX_A7:
            strncpy(buffer, "cortex-a7", PARSE_MAX_PARTNUMBER - 1);
            break;
        case IMPL_CORTEX_A15:
            strncpy(buffer, "cortex-a15", PARSE_MAX_PARTNUMBER - 1);
            break;
        case IMPL_CORTEX_A53:
            strncpy(buffer, "cortex-a53", PARSE_MAX_PARTNUMBER - 1);
            break;
        case IMPL_CORTEX_A57:
            strncpy(buffer, "cortex-a57", PARSE_MAX_PARTNUMBER - 1);
            break;
        case GICC_V2:
        case GICD_V2:
        case GICH_V2:
        case GICV_V2:
            strncpy(buffer, "gicv2", PARSE_MAX_PARTNUMBER - 1);
            break;
        default:
            strncpy(buffer, "unknown", PARSE_MAX_PARTNUMBER - 1);
            break;
    }
    status = 0;
done:
    return status;
}

char *
get_error_string(unsigned int errval)
{
    switch(errval)
    {
        case MS_ERROR_SUCCESS:
            return "Success";
        case MS_ERROR_ILLINST:
            return "Illegal Instruction";
        case MS_ERROR_UNDEFINST:
            return "Undefined Instruction";
        case MS_ERROR_SMP:
            return "SMP error";
        case MS_ERROR_TABLE:
            return "Error reading table";
        case MS_ERROR_SMP_SINGLE:
            return "Error scheduling thread";
        default:
            return "Internal error";
    }
}

/******************************************************
 * Formats the bitfield mask into [MSB:LSB] format
 * for printing
 *  
 * @param [in] code     UID
 * @param [out] buffer  User supplied buffer
 * 
 * ***************************************************/
int
get_bitfieldstamp(
    unsigned int code,
    char * buffer)
{
    unsigned int index = -1;
    unsigned int msb = 0;
    unsigned int lsb = 0;
    unsigned int status = -1;
    if (buffer == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    if (breakout_get_index(code, &index) != 0)
    {
        TRACE("Failed to get index\n", 0);
        goto done;
        
    }
    lsb = g_bitfield_table_heap[index].info.rightshift;
    msb = g_bitfield_table_heap[index].info.mask - 1 + lsb;
    

    sprintf(buffer, "%02d:%02d", msb, lsb); 
    
    status = 0;
done:
    return status;
}

static
int
breakout_add_match(
    ms_list_head * list,
    unsigned int ppl,
    unsigned int cpu,
    unsigned int pl,
    unsigned int uid,
    unsigned int ioctl,
    unsigned int noparse,
    unsigned int part_number,
    unsigned int calc,
    unsigned long long calc_value)
{
    int result = -1;
    results_info * temp_match_info = NULL;
    
    if (list == NULL)
    {
        TRACE("Param error\n", 0);
        goto done;
    }
    
    temp_match_info = (results_info *)calloc(1, sizeof(results_info));
    if (temp_match_info == NULL)
    {
        TRACE("allocation error\n", 0);
        goto done;
    }   
    
    //TRACE("Adding match: IOCTL: 0x%x UID: 0x%x pcpu: %d Part Number: 0x%x\n", ioctl, uid, cpu, part_number);
    temp_match_info->register_uid = ioctl;
    temp_match_info->uid = uid;
    temp_match_info->ppl = ppl;
    temp_match_info->cpu = cpu;
    temp_match_info->pl = pl;
    temp_match_info->noparse = noparse;
    temp_match_info->part_number = part_number;
    temp_match_info->calc = calc;
    temp_match_info->calc_value = calc_value;
    
    ms_list_add(list, (list_element *)temp_match_info);
    
    
    result = 0;
done:
    return result;
}

static
int
breakout_add_ioctl(
    ms_list_head * list,    
    unsigned int ioctl,
    unsigned int cpu,
    unsigned int partnumber)
{
    int result = -1;
    used_ioctl * temp = NULL;
    
    if (list == NULL)
    {
        TRACE("Param error\n", 0);
        goto done;
    }
    
    temp = (used_ioctl *)calloc(1, sizeof(used_ioctl));
    if (temp == NULL)
    {
        TRACE("allocation error\n", 0);
        goto done;
    }   
    
    temp->register_uid = ioctl;
    temp->cpu = cpu;
    temp->partnumber = partnumber;
    
    TRACE("Adding: ioctl:%x cpu:%d partnumber:%x\n", ioctl, cpu, partnumber);
    
    ms_list_add(list, (list_element *)temp);    
    
    result = 0;
done:
    return result;
}

static
int
breakout_add_ioctl_result(ms_list_head * list_in,
                            unsigned int register_uid,
                            unsigned long long val64,
                            unsigned int low,
                            unsigned int high,
                            unsigned int cpu,
                            unsigned int midr,
                            unsigned int status,
                            unsigned int epl)
{
    int result = -1;
    ioctl_result * ioctl_result_p = NULL;
    if (list_in == NULL)
    {
        goto done;
    }
    
    ioctl_result_p = calloc(1, sizeof(ioctl_result));
    if (ioctl_result_p == NULL)
    {
        TRACE("Allocation error\n", 0);
        goto done;
    }
    
    ioctl_result_p->register_uid = register_uid;
    ioctl_result_p->val64 = val64;
    ioctl_result_p->low = low;
    ioctl_result_p->high = high;
    ioctl_result_p->cpu = cpu;
    ioctl_result_p->midr = midr;
    ioctl_result_p->epl = epl;
    ioctl_result_p->status = status;
    
    //TRACE("Adding result: ioctl: %x value_64: %llx low: %x high: %x\n", register_uid, val64, low, high);
    
    ms_list_add(list_in, (list_element *)ioctl_result_p);
    
    result = 0; 
    
done:
    return result;
}


int
breakout_add_noparse_result(ms_list_head * list,
                                unsigned int uid,
                                unsigned int ioctl,
                                unsigned long long value,
                                unsigned long long uvalue,
                                unsigned int cpu,
                                unsigned int epl,
                                unsigned int midr,
                                unsigned int status)
{
    int result = -1;
    bitfield_info_populated * temp_populated = NULL;
    if (list == NULL)
    {
        goto done;
    }
    
    temp_populated = calloc(1, sizeof(bitfield_info_populated));
    if (temp_populated == NULL)
    {
        TRACE("Allocation error\n", 0);
        goto done;
    }    
    
    strncpy(temp_populated->description, "None", PARSE_MAX_DESCRIPTION - 1);
    strncpy(temp_populated->register_name, get_register_name(uid), PARSE_MAX_REGISTER - 1);
    strncpy(temp_populated->functional_group, get_functional_group(uid), PARSE_MAX_FUNCTIONAL - 1);
    strncpy(temp_populated->bitfield, "Unparsed", PARSE_MAX_BITFIELD - 1);
    temp_populated->value = uvalue;
    temp_populated->uvalue = uvalue;
    temp_populated->ioctl = ioctl;
    temp_populated->uid = uid;
    temp_populated->cpu = cpu;
    temp_populated->epl = epl;
    temp_populated->midr = midr;
    temp_populated->status = status;
    
    ms_list_add(list, (list_element *)temp_populated);
    
    result = 0; 
    
done:
    return result;
}

int
breakout_add_bitfield_result(ms_list_head * list,
                                unsigned int uid,
                                unsigned int ioctl,
                                unsigned long long value,
                                unsigned long long uvalue,
                                unsigned int cpu,
                                unsigned int epl,
                                unsigned int midr,
                                unsigned int status)
{
    int result = -1;
    bitfield_info_populated * temp_populated = NULL;
    if (list == NULL)
    {
        goto done;
    }
    
    temp_populated = calloc(1, sizeof(bitfield_info_populated));
    if (temp_populated == NULL)
    {
        TRACE("Allocation error\n", 0);
        goto done;
    }   
    
    
    strncpy(temp_populated->description, get_description(uid), PARSE_MAX_DESCRIPTION - 1);
    strncpy(temp_populated->register_name, get_register_name(uid), PARSE_MAX_REGISTER - 1);
    strncpy(temp_populated->functional_group, get_functional_group(uid), PARSE_MAX_FUNCTIONAL - 1);
    strncpy(temp_populated->bitfield, get_bitfield(uid), PARSE_MAX_BITFIELD - 1);
    temp_populated->value = value;
    temp_populated->uvalue = uvalue;
    temp_populated->ioctl = ioctl;
    temp_populated->uid = uid;
    temp_populated->cpu = cpu;
    temp_populated->epl = epl;
    temp_populated->midr = midr;
    temp_populated->status = status;
    
    
    /*TRACE("Adding masked value to list: %s %s 0x%x CPU: %d Part: %d\n", temp_populated->bitfield, 
                                                        temp_populated->register_name,
                                                        value,
                                                        cpu,
                                                        MSCODE_PARTNUMBER(uid));*/
    
    ms_list_add(list, (list_element *)temp_populated);
    
    result = 0; 
    
done:
    return result;
}

static
int
get_find(ms_list_head * listin,
            char * register_name,
            char * functional_group)
{
    int result = -1;
    bitfield_info_populated * temp = NULL;
    
    if (listin == NULL)
    {
        goto done;
    }
    
    temp = (bitfield_info_populated *)listin->head;
    while (temp != NULL)
    {
        if (register_name != NULL)
        {
            /* search register field in list */
            if (strncmp(register_name, temp->register_name, PARSE_MAX_REGISTER) == 0)
            {
                result = 1;
                goto done;
            }
        }       
        else if (functional_group != NULL)
        {
            /* search functional group field in list */
            if (strncmp(functional_group, temp->functional_group, PARSE_MAX_FUNCTIONAL) == 0)
            {
                result = 1;
                goto done;
            }
        }       
        temp = (bitfield_info_populated *)temp->list.next;
    }
    
    /* nothing found */
    result = 0;
done:
    return result;
}
int
get_groups(ms_list_head * listout)
{
    int result = -1;
    int i = 0;
    
    if (listout == NULL)
    {
        goto done;
    }
     for (i = 0; i < g_bitfield_table_size; ++i)
     { 
         if (get_find(listout, NULL, g_bitfield_table_heap[i].info.functional_group) <= 0)
         {
            if (breakout_add_bitfield_result(listout,
                                    g_bitfield_table_heap[i].info.uid,
                                    0, 0, 0, 0, 0, 0, 0) != 0)
            {
                goto done;
            }
        }   
     }   
     
     result = 0;
 done:
    return result;
    
}   
    
int get_registers(ms_list_head * listout)
{
    int result = -1;
    int i = 0;
    
    if (listout == NULL)
    {
        goto done;
    }
     for (i = 0; i < g_bitfield_table_size; ++i)
     {
         if (get_find(listout, g_bitfield_table_heap[i].register_name_expanded, NULL) <= 0)
         {
            if (breakout_add_bitfield_result(listout,
                                    g_bitfield_table_heap[i].info.uid,
                                    0, 0, 0, 0, 0, 0, 0) != 0)
            {
                goto done;
            }
        }    
     }   
     
     result = 0;
 done:
    return result;
    
}   

int get_bitfields(ms_list_head * listout)
{
    int result = -1;
    int i = 0;
    
    if (listout == NULL)
    {
        goto done;
    }
     for (i = 0; i < g_bitfield_table_size; ++i)
     {         
        if (breakout_add_bitfield_result(listout,
                                g_bitfield_table_heap[i].info.uid,
                                0, 0, 0, 0, 0, 0, 0) != 0)
        {
            goto done;
        }
     }   
     
     result = 0;
 done:
    return result;
}


static
int
breakout_add_table_index(ms_list_head * list,
                        unsigned int index)
{
    int result = -1;
    table_index * temp_index = NULL;
    
    if (list == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    temp_index = calloc(1, sizeof(table_index));
    if (temp_index == NULL)
    {
        TRACE("malloc error\n", 0);
        goto done;
    }    
    temp_index->index = index;
    
    ms_list_add(list, (list_element *)temp_index);
    
    result = 0;
done:
    return result;
    
}

static
int
breakout_construct_bitfield_list(
    unsigned int ioctl,
    ms_list_head * index_list_out)
{
    /* Based off ioctl number, record all indices */
    int result = -1;
    unsigned int i = 0;
    unsigned int table_size = 0;
    if (index_list_out == NULL)
    {
        TRACE("param error", 0);
        goto done;
    }    
     
    /* For all items */
    for (i = 0; i < g_bitfield_table_size; ++i)
    {
        if (g_bitfield_table_heap[i].info.register_uid == ioctl)
        {
            /* Add this register to list */
            //TRACE("Adding table index to list: bitfield: %s part: %d, table: %d, index: %d, ioctl: 0x%x\n", bitfield_table[i].table_ptr[j].bitfield, MSCODE_PARTNUMBER(bitfield_table[i].table_ptr[j].uid), i, j, ioctl);
            if (breakout_add_table_index(index_list_out, i) != 0)
            {
                TRACE("Failed to add item to list\n", 0);
            }
        }
    }   
    result = 0;
done:
    return result;
}

static
void
value_to_long(unsigned long long * cat_value,
                unsigned long long val_in_64,
                unsigned int low,
                unsigned int high)
{   
    if ((val_in_64 != 0) && ((low == 0) && (high == 0)))
    {
        /* 64 bit value */
        *cat_value = val_in_64; 
    }
    else
    {
        /* 32 bit values */
        *cat_value = (((unsigned long long)high << 32) | low);      
    }   
    //TRACE("val64: %llx, low: %x, high: %x cat_value: %llx\n", val_in_64, low, high, *cat_value);
}

static
int
breakout_apply_masks_to_list(
    unsigned long long val_in_64,
    unsigned int low,
    unsigned int high,
    unsigned int cpu_in,
    unsigned int epl,   
    unsigned int midr,
    unsigned int status,                
    ms_list_head * indices,
    ms_list_head * list_out)
{
    int result = -1;
    table_index * temp_index = NULL;
    unsigned long long masked_value = 0;
    unsigned long long cat_value = 0;
    if ((indices == NULL) || (list_out == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }   
    /*
     * 1) For each index in list,
     * 2) Apply the mask to the value
     * 3) Add masked value to list
     * 
     * */
     value_to_long(&cat_value, val_in_64, low, high);
     
     temp_index = (table_index *)indices->head;
     while (temp_index != NULL)
     {       
         /* Apply mask */
        masked_value = breakout_applymask(cat_value,
                    g_bitfield_table_heap[temp_index->index].info.rightshift,
                    g_bitfield_table_heap[temp_index->index].info.mask); 

         
         /* Save result */
         result = breakout_add_bitfield_result(list_out,
                                        g_bitfield_table_heap[temp_index->index].info.uid, 
                                        g_bitfield_table_heap[temp_index->index].info.register_uid, 
                                        masked_value,
                                        cat_value,
                                        cpu_in,
                                        epl,
                                        midr,
                                        status);
        if (result != 0)
        {
            TRACE("Failed to add bitfield to list\n", 0);
            goto done;
        }
         
         /* go to next index */
         temp_index = (table_index *)temp_index->list.next;
     } 
     
    
    result = 0;
done:
    return result;
}

/*************
 * Support function to add to list
 *
 * @return -1 fail
 *          0 list not searched
 *          1 added
 * */
static
int
breakout_add_noparse(
    ms_list_head * list,
    results_info * requested,
    bitfield_info_populated * all)
{
    int result = -1;
    bitfield_info_populated * temp_ptr = NULL;
    if ((list == NULL) || (requested == NULL) || (all == NULL))
    {
        goto done;
    }
     
    if (requested->noparse == 1)
    {
        /* search existing list to see if register is already added */
        temp_ptr = (bitfield_info_populated *)list->head;
        while (temp_ptr != NULL)
        {
            if ((temp_ptr->ioctl == all->ioctl) && (temp_ptr->cpu == all->cpu)
                        && (temp_ptr->status == all->status))
            {
                /* already added */
                result = 1;
                goto done;
            }
            
            /* next list item */
            temp_ptr = (bitfield_info_populated *)temp_ptr->list.next;
        }
        breakout_add_noparse_result(list,
                                    all->uid,   // bitfield #define
                                    all->ioctl,
                                    0,          // value of bitfield
                                    all->uvalue,// unparsed value
                                    all->cpu,   // cpu number
                                    all->epl,   // executed privilege level
                                    all->midr,  // MIDR of cpu
                                    all->status);  // non-null 
        result = 1;
        goto done;    
    }
    else
    {
        result = 0;
        goto done;
    }
    
done:
    return result;
}
                                 
static
int
breakout_filter_results(ms_list_head * all_bitfields,
                    ms_list_head * requested_bitfields,
                    ms_list_head * list_out)
{
    int result = -1;
    int impl = 0;
    int arch = 0;
    bitfield_info_populated * all = NULL;
    results_info * requested = NULL;
    unsigned int part_number = 0;
    if ((all_bitfields == NULL) || (requested_bitfields == NULL) ||
        (list_out == NULL))
    {
        goto done;
        TRACE("param error\n", 0);
    }
    
    /* 1) Iterate through requested
     * 2) Iterate through all bitfields
     * 3) Match & return */
     requested = (results_info *)requested_bitfields->head;
     while (requested != NULL)
     {
         all = (bitfield_info_populated *)all_bitfields->head;
         while (all != NULL)
         {
             /* If requested by user and 
              * the returned cortex number matches the current register */
             if (requested->uid == all->uid)                
             {
                 impl = get_impl_from_midr(all->midr);
                 arch = get_arch_from_midr(all->midr);
                 //TRACE("Checking arch == part_number: %d == %d\n", arch, requested->part_number);
                 //TRACE("Checking impl == part_number: %d == %d\n", impl, requested->part_number);
                 if ((impl == requested->part_number) || (arch == requested->part_number)
                     || (all->midr == 0))
                 {               
                     /* Add to list */
                    if (breakout_add_noparse(list_out, requested, all) != 1)
                    {
                        breakout_add_bitfield_result(list_out,
                                                    all->uid,   // bitfield #define
                                                    all->ioctl, // ioctl code
                                                    all->value, // value of bitfield
                                                    0,
                                                    all->cpu,   // cpu number
                                                    all->epl,   // executed privilege level
                                                    all->midr,  // MIDR of cpu
                                                    all->status);  // non-null indicates failure
                    }
                 }
             }           
             
             /* Go to next all bitfields */
             all = (bitfield_info_populated *)all->list.next;
         }       
         /* Go to next requested bitfield */
         requested = (results_info *)requested->list.next;
     }
    
    result = 0;
done:
    return result;
}

static
int
breakout_value_validate(
    ms_list_head * list_in_matches,
    unsigned int * ioctl_return,
    unsigned long long * calc_return)
{
    int result = -1;
    unsigned int current_ioctl = 0;
    unsigned long long calc_value = 0;  
    results_info * validate = NULL;
    
    if ((list_in_matches == NULL) || (ioctl_return == NULL) ||
        (calc_return == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    validate = (results_info *)list_in_matches->head;
    if (validate != NULL)
    {
        current_ioctl = validate->register_uid;
        calc_value = validate->calc_value;
    }
    else
    {
        TRACE("No entries in list\n", 0);
        goto done;
    }   

    while (validate != NULL)
    {           
        if ((current_ioctl != validate->register_uid) ||
            (calc_value != validate->calc_value))
        {
            /* fail */
            result = -1;
            goto done;
        }
        
        validate = (results_info *)validate->list.next;
    }
    
    *calc_return = calc_value;
    *ioctl_return = current_ioctl;
    result = 0;
    
done:
    return result;
}

/******************************************************
 * From the given value parse the results
 * 
 * @param [in] Items matched from table 
 * @param [out] List of bitfield_info_populated
 * 
 * ***************************************************/
static
int
breakout_parse_results_value(
    ms_list_head * list_in_matches,
    ms_list_head * list_out)
{
    int result = -1;
    unsigned int common_ioctl = 0;
    unsigned long long calc_value = 0;
    ms_list_head register_bitfield_list;
    ms_list_head masked_register_list;
    
    if ((list_in_matches == NULL) || (list_out == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    ms_list_init(&register_bitfield_list);
    ms_list_init(&masked_register_list);
    
    /* Validate that all bitfields belong to the same ioctl */
    if (breakout_value_validate(list_in_matches, &common_ioctl, &calc_value) != 0)
    {
        TRACE("Failed to validate supplied calc value\n", 0);
        goto done;
    }
        
    /* From the given ioctl/register, construct a list of table
     * indices from all the lists */        
    if (breakout_construct_bitfield_list(common_ioctl,
                                            &register_bitfield_list) != 0)
    {
        TRACE("Failed to construct bitfield list\n", 0);
        goto done;
    }
    
    /* Apply all Cortex implementation masks to value */
    result = breakout_apply_masks_to_list(calc_value,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            0,
                                            &register_bitfield_list, 
                                            &masked_register_list);
    if (result != 0)
    {
        TRACE("Failed to apply masks to result\n", 0);
        goto done;
    }
    
   /* From the list of all masked values, only return the 
     * requested bitfield & valid cortex number */      
    if (breakout_filter_results(&masked_register_list,
                                    list_in_matches,
                                    list_out) != 0)
    {
        TRACE("Failed to filter bitfields\n", 0);
        goto done;
    }

    result = 0;
    
done:
    ms_list_free(&register_bitfield_list);
    ms_list_free(&masked_register_list);

    return result;
}


/******************************************************
 * From the condensed ioctl results,
 * parse the results to match the requested data
 *  
 * @param [in] IOCTL results
 * @param [in] Items matched from table 
 * @param [out] List of bitfield_info_populated
 * 
 * ***************************************************/
static
int
breakout_parse_results(
    ms_list_head * list_in_ioctls,
    ms_list_head * list_in_matches,
    ms_list_head * list_out)
{
    int result = -1;
    ioctl_result * ioctl_result_element = NULL;
    ms_list_head register_bitfield_list;
    ms_list_head masked_register_list;
    
    if ((list_in_ioctls == NULL) || (list_in_matches == NULL) ||
        (list_out == NULL))
    {
        goto done;
        TRACE("param error\n", 0);
    }   
    
    /* 1) Construct list of all parsed bitfield values
     * 2) Only return the requested bitfields */
     
     /* For each ioctl result */
    ioctl_result_element = (ioctl_result *)list_in_ioctls->head;
    while (ioctl_result_element != NULL)
    {       
        ms_list_init(&register_bitfield_list);
        ms_list_init(&masked_register_list);
        
        /* From the given ioctl/register, construct a list of table
         * indices from all the lists */        
        if (breakout_construct_bitfield_list(ioctl_result_element->register_uid,
                                                &register_bitfield_list) != 0)
        {
            TRACE("Failed to construct bitfield list\n", 0);
            goto done;
        }
        
        /* Apply all Cortex implementation masks to value */
        result = breakout_apply_masks_to_list(ioctl_result_element->val64,
                                                ioctl_result_element->low,
                                                ioctl_result_element->high,
                                                ioctl_result_element->cpu,
                                                ioctl_result_element->epl,
                                                ioctl_result_element->midr,
                                                ioctl_result_element->status,
                                                &register_bitfield_list, 
                                                &masked_register_list);
        if (result != 0)
        {
            TRACE("Failed to apply masks to result\n", 0);
            goto done;
        }
        
        /* From the list of all masked values, only return the 
         * requested bitfield & valid cortex number */      
        if (breakout_filter_results(&masked_register_list,
                                        list_in_matches,
                                        list_out) != 0)
        {
            TRACE("Failed to filter bitfields\n", 0);
            goto done;
        }
        
        /* Clear temporary lists */
        ms_list_free(&register_bitfield_list);
        ms_list_free(&masked_register_list);
        
        /* Go to next ioctl result in list */
        ioctl_result_element = (ioctl_result *)ioctl_result_element->list.next;
    }
    
    result = 0;
    
done:

    ms_list_free(&register_bitfield_list);
    ms_list_free(&masked_register_list);
    
    return result;
}
    


static
int
breakout_call_driver(ms_list_head * list_in,
                    ms_list_head * list_out)
{
    int result = -1;
    used_ioctl * ioctl_list = NULL;
    unsigned int cpu_iter = 0;
    
    ms_data_cpu local_result;
    if ((list_in == NULL) || (list_out == NULL))
    {
        goto done;
    }
    
    memset(&local_result, 0x0, sizeof(ms_data_cpu));
    
    /* Traverse ioctl list,
    * call ioctls
    * save ioctls to list 
    * return list */
    ioctl_list = (used_ioctl *)list_in->head;

    while(ioctl_list != NULL)
    {
        /* Set requested cpu code & send request to driver */
        local_result.code = ioctl_list->register_uid;
        local_result.pcpu = ioctl_list->cpu;  
        local_result.part_number = ioctl_list->partnumber;
                
        switch(ioctl_list->partnumber)
        {
            case (GICC_V2):
            case (GICD_V2):
            case (GICH_V2):
            case (GICV_V2):
                result = driver_results(MS_IOCTL_GIC, &local_result);
                break;
            default:
                result = driver_results(MS_IOCTL_CO, &local_result);
                break;          
        }
        if (result != 0)
        {
            TRACE("Failed to send ioctl\n", 0);
        }
        else
        {
            /* Save results - for each cpu */
            for (cpu_iter = 0; cpu_iter < MS_MAX_CPUS; ++cpu_iter)
            {
                result = breakout_add_ioctl_result(list_out,
                                                    ioctl_list->register_uid,
                                                    local_result.cpu[cpu_iter].out64,
                                                    local_result.cpu[cpu_iter].out1,
                                                    local_result.cpu[cpu_iter].out2,
                                                    local_result.cpu[cpu_iter].cpu,
                                                    local_result.cpu[cpu_iter].midr,
                                                    local_result.cpu[cpu_iter].status,
                                                    NS_PRIVILEGE_LEVEL_1);
                if (result != 0)
                {
                    TRACE("Problem saving results: cpu: %d\n", cpu_iter);
                }
            }
        }
        ioctl_list = (used_ioctl *)ioctl_list->list.next;
    }

    result = 0;
done:

    return result;
}

static
int
breakout_call_usermode(ms_list_head * list_in,
                    ms_list_head * list_out)
{
    int result = -1;
    used_ioctl * ioctl_list = NULL;
    unsigned int cpu_iter = 0;
    
    ms_data local_result;
    if ((list_in == NULL) || (list_out == NULL))
    {
        goto done;
    }
    
    memset(&local_result, 0x0, sizeof(ms_data));
    
    
    ioctl_list = (used_ioctl *)list_in->head;

    while(ioctl_list != NULL)
    {
        if (usermode_results(ioctl_list->register_uid, &local_result) != 0)
        {
            TRACE("Failed to send ioctl\n", 0);
        }
        else
        {
            /* Save results */
            result = breakout_add_ioctl_result(list_out,
                                                ioctl_list->register_uid,
                                                local_result.out64,
                                                local_result.out1,
                                                local_result.out2,
                                                0, /* todo -- get cpu number? */
                                                0, /* todo -- get midr? */
                                                local_result.status,
                                                NS_PRIVILEGE_LEVEL_0);
        }
        ioctl_list = (used_ioctl *)ioctl_list->list.next;
    }

    result = 0;
done:

    return result;
}

static
int
breakout_sort_ioctls(ms_list_head * internal_list,
                    ms_list_head * list_out)
{
    int status = -1;
    unsigned int flag = 0;
    ms_list_head to_driver;
    ms_list_head to_usermode;
    used_ioctl * temp_used_ioctls = NULL;
    results_info * temp_result_info = NULL;
    
    if ((internal_list == NULL) || (list_out == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    ms_list_init(&to_driver);
    ms_list_init(&to_usermode);
    
    /* Generate todo list -- Either call driver (PL1) or usermode (PL0)  */
    temp_result_info = (results_info *)internal_list->head;
    while (temp_result_info != NULL)
    {   
         /* search to_driver list to see if its already added */
         temp_used_ioctls = (used_ioctl *)to_driver.head;
         while (temp_used_ioctls != NULL)
         {   
             if (temp_used_ioctls->register_uid == temp_result_info->register_uid)
             {
                 /* found, don't add */
                 flag = 1;
                 break;
             }
             /* go to next item on called ioctl list */
             temp_used_ioctls = (used_ioctl *)temp_used_ioctls->list.next;
         }
         
         /* search to_usermode list to see if its already added */
         temp_used_ioctls = (used_ioctl *)to_usermode.head;
         while (temp_used_ioctls != NULL)
         {   
             if (temp_used_ioctls->register_uid == temp_result_info->register_uid)
             {
                 /* found, don't add */
                 flag = 1;
                 break;
             }
             /* go to next item on called ioctl list */
             temp_used_ioctls = (used_ioctl *)temp_used_ioctls->list.next;
         }
         
         /* evaluate flag result */  
         if (flag == 0)
         {
             /* Not found on list, add to list
              * Determine what pl to use
              *  - if pl_default requested, determine if pl0 or pl1 and add to either to_driver or to_usermode
              *  - if pl specified not default then add to either to_driver, or to_usermode
              *  - secure mode paths not supported  * */              
              
              if (temp_result_info->pl & PREFERRED_PL)
              {
                  //TRACE("Preferred PL: 0x%x & 0x%x: 0x%x (pcpu: %d)\n", temp_result_info->pl, PREFERRED_PL, temp_result_info->pl & PREFERRED_PL, temp_result_info->cpu);                
                  if (temp_result_info->ppl & NS_PRIVILEGE_LEVEL_0)
                  {
                      //TRACE("Adding to PL0 list\n", 0);
                        breakout_add_ioctl(&to_usermode,
                                            temp_result_info->register_uid,
                                            temp_result_info->cpu,
                                            temp_result_info->part_number);
                  }
                  if (temp_result_info->ppl & NS_PRIVILEGE_LEVEL_1)
                  {
                      //TRACE("Adding to PL1 list\n", 0);
                        breakout_add_ioctl(&to_driver,
                                            temp_result_info->register_uid,
                                            temp_result_info->cpu,
                                            temp_result_info->part_number);
                  }
              }
              else 
              {
                  //TRACE("Specified PL\n", 0); 
                  if (temp_result_info->pl & NS_PRIVILEGE_LEVEL_0)
                  {
                        //TRACE("Adding to PL0 list\n", 0);
                        breakout_add_ioctl(&to_usermode,
                                            temp_result_info->register_uid,
                                            temp_result_info->cpu,
                                            temp_result_info->part_number);
                  }
                  if (temp_result_info->pl & NS_PRIVILEGE_LEVEL_1)
                  {
                        //TRACE("Adding to PL1 list\n", 0);
                        breakout_add_ioctl(&to_driver,
                                            temp_result_info->register_uid,
                                            temp_result_info->cpu,
                                            temp_result_info->part_number);
                  }
              }
         }
         else
         {
             flag = 0;
         }       
        
        /* go to next item on found list */
        temp_result_info = (results_info *)temp_result_info->list.next;
    }
    
    if (breakout_call_driver(&to_driver, list_out) != 0)
    {
        TRACE("Failed to call PL1 ioctls\n", 0);            
    }
    
    if (breakout_call_usermode(&to_usermode, list_out) != 0)
    {
        TRACE("Failed to call PL0 ioctls\n", 0);
    }
            
    status = 0;
done:

    ms_list_free(&to_driver);   
    ms_list_free(&to_usermode);
    return status;
}

static
int
breakout_search_table(
    match_info * info,
    ms_list_head * list_out)
{
    int i = 0;
    int result = -1;
    int match = 0;   
    if ((info == NULL) || (list_out == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    /* For all tables */
    for (i = 0; i < g_bitfield_table_size; ++i)
    {                           
         /* if the bitfield code, functional group, or register name matches */
        if ((g_bitfield_table_heap[i].info.uid == info->uid)  ||
            (strncmp(g_bitfield_table_heap[i].info.functional_group,
                    (const char *)info->functional_group,
                    PARSE_MAX_FUNCTIONAL - 1) == 0)             ||
            (strncmp(g_bitfield_table_heap[i].register_name_expanded,
                    (const char *)info->register_name,
                    PARSE_MAX_REGISTER - 1) == 0))
        {                   
            if (g_bitfield_table_heap[i].table_type == TYPE_IMPL)
            {
                //add if (arch matches && (all || requested))
                if (info->architecture == get_arch_from_impl(MSCODE_PARTNUMBER(g_bitfield_table_heap[i].info.uid))
                        && ((info->implementation == IMPL_ALL) || (info->implementation == MSCODE_PARTNUMBER(g_bitfield_table_heap[i].info.uid))))
                {                   
                    if (breakout_add_match(list_out,
                                        g_bitfield_table_heap[i].info.ppl,
                                        info->cpu,
                                        info->pl,
                                        g_bitfield_table_heap[i].info.uid,
                                        g_bitfield_table_heap[i].info.register_uid,
                                        info->noparse,
                                        MSCODE_PARTNUMBER(g_bitfield_table_heap[i].info.uid),
                                        info->calc,
                                        info->calc_value) != 0)
                    {
                        TRACE("Failed to save result to list\n", 0);
                    }
                }
            }
            else if (g_bitfield_table_heap[i].table_type == TYPE_ARCH)
            {
                //add if (arch matches)             
                if ((MSCODE_PARTNUMBER(g_bitfield_table_heap[i].info.uid) == info->architecture))
                {                   
                    if (breakout_add_match(list_out,
                                        g_bitfield_table_heap[i].info.ppl,
                                        info->cpu,
                                        info->pl,
                                        g_bitfield_table_heap[i].info.uid,
                                        g_bitfield_table_heap[i].info.register_uid,
                                        info->noparse,
                                        MSCODE_PARTNUMBER(g_bitfield_table_heap[i].info.uid),
                                        info->calc,
                                        info->calc_value) != 0)
                    {
                        TRACE("Failed to save result to list\n", 0);
                    }
                }               
            }
            else if (g_bitfield_table_heap[i].table_type == TYPE_MM)
            {
                // add always -- assume component exists on platform
                if (breakout_add_match(list_out,
                                    g_bitfield_table_heap[i].info.ppl,
                                    info->cpu,
                                    info->pl,
                                    g_bitfield_table_heap[i].info.uid,
                                    g_bitfield_table_heap[i].info.register_uid,
                                    info->noparse,
                                    MSCODE_PARTNUMBER(g_bitfield_table_heap[i].info.uid),
                                    info->calc,
                                    info->calc_value) != 0)
                {
                    TRACE("Failed to save result to list\n", 0);
                }            
            }
        }   
       
    }

    result = 0;
    
done:
    return result;
}

/******************************************************
 * Finds matching registers and returns their
 * status
 *  
 * @param [in] Info to match
 * @param [out] Results
 * 
 * ***************************************************/
int
find_bitfields(
    match_info * info,
    ms_list_head * list)
{

    int result = -1;
    
    ms_list_head requested_bitfields_list;
    ms_list_head ioctl_results_list;   
     
    if ((info == NULL) || (list == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
     
    ms_list_init(&requested_bitfields_list);
    ms_list_init(&ioctl_results_list);
    TRACE("Looking for: %x %s %s\n", info->uid, info->functional_group, info->register_name);
     
     
    /* Search tables for matching bitfields -- 
    * Only add if we can parse the result,
    * save some search request fields in output list */
    if (breakout_search_table(info, &requested_bitfields_list) != 0)
    {
        TRACE("Failed to create bitfield search list\n", 0);
        goto done;
    }
    
    /* If no calc value suppplied, call relevant ioctls and save results */
    if (info->calc == 0)
    {
        /* Call ioctls, save results */
        if (breakout_sort_ioctls(&requested_bitfields_list, &ioctl_results_list) != 0)
        {
            TRACE("Failed to get results\n", 0);
            goto done;
        }
        
        /* From ioctl results, get final list */   
        if (breakout_parse_results(&ioctl_results_list, &requested_bitfields_list, list) != 0)
        {
            TRACE("Failed to parse results\n", 0);
            goto done;
        }  
    }
    
    /* Use supplied calc value */
    else
    {
        if (breakout_parse_results_value(&requested_bitfields_list, list) != 0)
        {
            TRACE("Failed to parse provided value\n", 0);
            goto done;
        }
    }
    
     
    result = 0; 
   
     
done:
    ms_list_free(&requested_bitfields_list);
    ms_list_free(&ioctl_results_list);
    return result;
}
