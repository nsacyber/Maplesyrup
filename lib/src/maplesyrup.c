#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "maplesyrup.h"
#include "support.h"
#include "ms_debug.h"
#include "open_device.h"
#include "parse_breakout.h"

static
int
flatten_results(
    ms_list_head * listin,
    ms_bitfield_info ** flat_array)
{
    int result = -1;
    char tempStamp[PARSE_MAX_STAMPSIZE];
    char tempPartnumber[PARSE_MAX_PARTNUMBER];
    char tempPartnumber_t[PARSE_MAX_PARTNUMBER];
    int i = 0;
    ms_bitfield_info * temp_ptr = NULL;
    bitfield_info_populated * list_ptr = NULL;
    
    /* Convert list of results to an array of structures.
     * Caller must cleanup */    
      
      if ((listin == NULL) || (flat_array == NULL))
      {
          TRACE("param error\n", 0);
          goto done;          
      }
      
      if (listin->size == 0)
      {
          TRACE("No results\n", 0);
          goto done;
      }
      
      temp_ptr = (ms_bitfield_info *)calloc(listin->size, sizeof(ms_bitfield_info));
      if (temp_ptr == NULL)
      {
          TRACE("allocation error\n", 0);
          goto done;
      }   
      
      list_ptr = (bitfield_info_populated *)listin->head;
      for (i = 0; i < listin->size; ++i)
      {
          if (list_ptr == NULL)             
          {
              TRACE("Bad list size\n", 0);
              goto error;
          }       

            memset(tempStamp, 0x0, sizeof(tempStamp));
            if (strncmp(list_ptr->bitfield, "Unparsed", PARSE_MAX_BITFIELD - 1) != 0)
            {
                get_bitfieldstamp(list_ptr->uid, tempStamp);
            }
            
            memset(tempPartnumber, 0x0, sizeof(tempPartnumber));
            get_partnumber_from_midr(list_ptr->midr, tempPartnumber);
            
            memset(tempPartnumber_t, 0x0, sizeof(tempPartnumber_t));
            get_decode_table(list_ptr->uid, tempPartnumber_t);

            temp_ptr[i].cpu = list_ptr->cpu;
            temp_ptr[i].epl = list_ptr->epl;
            temp_ptr[i].uid = list_ptr->uid;
            temp_ptr[i].value = list_ptr->value;
            temp_ptr[i].status = list_ptr->status;
            temp_ptr[i].array_elements = listin->size;

            strncpy(temp_ptr[i].bitfield, list_ptr->bitfield, PARSE_MAX_BITFIELD - 1);
            strncpy(temp_ptr[i].functional_group, list_ptr->functional_group, PARSE_MAX_FUNCTIONAL - 1);
            strncpy(temp_ptr[i].register_name, list_ptr->register_name, PARSE_MAX_REGISTER - 1);
            strncpy(temp_ptr[i].description, list_ptr->description, PARSE_MAX_DESCRIPTION - 1);
            strncpy(temp_ptr[i].stamp, tempStamp, PARSE_MAX_STAMPSIZE - 1);
            strncpy(temp_ptr[i].part_number, tempPartnumber, PARSE_MAX_PARTNUMBER - 1);
            strncpy(temp_ptr[i].part_number_table, tempPartnumber_t, PARSE_MAX_PARTNUMBER - 1);

            list_ptr = (bitfield_info_populated *)list_ptr->list.next;
      }
      
      
      *flat_array = temp_ptr;
      result = 0;
      goto done;
      
error:
    if (temp_ptr != NULL)
    {
        free(temp_ptr);
        temp_ptr = NULL;
    }
    
done:
    return result;
}

static
int
set_match_info(
    match_info * match,
    char * arch,
    char * impl,
    int cpu,
    unsigned int pl,
    unsigned int noparse,
    char * calc_value)
{
    int result = -1;
    if ((match == NULL) || (arch == NULL) || (impl == NULL))
    {
        goto done;
    }
    
    memset(match, 0x0, sizeof(match_info)); 
    match->pl = pl;
    match->cpu = (unsigned int)cpu;
    match->noparse = noparse;
    
    /* Architecture */
    if (strncmp(arch, "armv7a", PARSE_MAX_ARCHITECTURE) == 0)
    {
        match->architecture = ARCH_ARMV7A;
    }
    else if (strncmp(arch, "armv8a", PARSE_MAX_ARCHITECTURE) == 0)
    {
        match->architecture = ARCH_ARMV8A;
    }
    else
    {
        printf("Supported architectures are \"armv7a\" and \"armv8a\"\n");
        goto done;
    }
    
    /* Implementation */
    if (strncmp(impl, "cortex-a7", PARSE_MAX_IMPLEMENTATION) == 0)
    {
        match->implementation = IMPL_CORTEX_A7;
    }
    else if (strncmp(impl, "cortex-a9", PARSE_MAX_IMPLEMENTATION) == 0)
    {
        match->implementation = IMPL_CORTEX_A7;
    }
    else if (strncmp(impl, "cortex-a15", PARSE_MAX_IMPLEMENTATION) == 0)
    {
        match->implementation = IMPL_CORTEX_A15;
    }
    else if (strncmp(impl, "cortex-a53", PARSE_MAX_IMPLEMENTATION) == 0)
    {
        match->implementation = IMPL_CORTEX_A53;
    }
    else if (strncmp(impl, "cortex-a57", PARSE_MAX_IMPLEMENTATION) == 0)
    {
        match->implementation = IMPL_CORTEX_A57;
    }
    else if (strncmp(impl, "all", PARSE_MAX_IMPLEMENTATION) == 0)
    {
        match->implementation = IMPL_ALL;
    }
    else
    {
        printf("Unknown implementation. Use \"all\" if unsure.\n");
        goto done;
    }
    
    /* Calc value supplied */
    if (calc_value == NULL)
    {
        match->calc = 0; 
    }
    else
    {
        match->calc = 1;
        match->calc_value = strtoll(calc_value, NULL, 16);
        if (errno == ERANGE)
        {
            match->calc_value = ~0ULL;
        }
    }
    
    result = 0;
    
done:
    return result;
}


int
ms_get_groups(
    ms_bitfield_info ** list_out,
    int include_devices)
{
    int result = -1;
    ms_list_head list;
    
    if (list_out == NULL)
    {
        goto done;
    }
    ms_list_init(&list);
    
    if (read_tables_to_memory(include_devices) != 0)
    {
        TRACE("Failed to expand table to memory\n", 0);
    }
       
    if (get_groups(&list) != 0)
    {
        TRACE("Failed to get_groups\n", 0);
        goto done;
    }
    
    if (flatten_results(&list, list_out) != 0)
    {
        TRACE("Failed to flatten_results\n", 0);
        goto done;
    }
    
    result = 0;
done:
    ms_list_free(&list);
    erase_tables();
    return result;
}


int
ms_get_registers(
    ms_bitfield_info ** list_out,
    int include_devices)
{
    int result = -1;
    ms_list_head list;
    
    if (list_out == NULL)
    {
        goto done;
    }
    ms_list_init(&list);        
    
    if (read_tables_to_memory(include_devices) != 0)
    {
        TRACE("Failed to expand table to memory\n", 0);
    }
    
    if (get_registers(&list) != 0)
    {
        TRACE("Failed to get_groups\n", 0);
        goto done;
    }
    
    if (flatten_results(&list, list_out) != 0)
    {
        TRACE("Failed to flatten_results\n", 0);
        goto done;
    }
    
    result = 0;
done:
    ms_list_free(&list);
    erase_tables();
    return result;
}

int
ms_get_bitfields(
    ms_bitfield_info ** list_out,
    int include_devices)
{
    int result = -1;
    ms_list_head list;
    
    if (list_out == NULL)
    {
        goto done;
    }
    ms_list_init(&list);    
    
    if (read_tables_to_memory(include_devices) != 0)
    {
        TRACE("Failed to expand table to memory\n", 0);
    }
    
    if (get_bitfields(&list) != 0)
    {
        TRACE("Failed to get_groups\n", 0);
        goto done;
    }
    
    if (flatten_results(&list, list_out) != 0)
    {
        TRACE("Failed to flatten_results\n", 0);
        goto done;
    }
    
    result = 0;
done:
    ms_list_free(&list);
    erase_tables();
    return result;
}


int
ms_find_by_bitfield(
    char * arch,
    char * impl,
    unsigned int bitfield,
    int cpu,
    unsigned int pl,
    unsigned int noparse,
    int include_devices,
    ms_bitfield_info ** results_out)
{
    int result = -1;
    match_info match;
    ms_list_head list;
    
    if (results_out == NULL)
    {
        goto done;
    }
    
    ms_list_init(&list);    
    
    if (start_test() != 0)
    {
        printf("Failed to initialize\n");
        goto done;
    }
    
    if (set_match_info(&match, arch, impl, cpu, pl, noparse, NULL) != 0)
    {
        TRACE("Failed to set_match_info\n", 0);
        goto done;
    }
    match.uid = bitfield;     
    
    if (read_tables_to_memory(include_devices) != 0)
    {
        TRACE("Failed to expand table to memory\n", 0);
    }
    
    if (find_bitfields(&match, &list) != 0)
    {
        TRACE("Failed to find_bitfields\n", 0);
        goto done;
    }   
    
    if (flatten_results(&list, results_out) != 0)
    {
        TRACE("Failed to flatten_results\n", 0);
        goto done;
    }
    
        
    result = 0;
done:
    stop_test();    
    ms_list_free(&list);
    erase_tables();
    return result;
}


int
ms_find_by_register(
    char * arch,
    char * impl,
    char * regname,
    int cpu,
    unsigned int pl,
    unsigned int noparse,
    char * calc_value,
    int include_devices,
    ms_bitfield_info ** results_out)
{
    int result = -1;
    match_info match;
    ms_list_head list;  
        
    if (results_out == NULL)
    {
        goto done;
    }
    
    ms_list_init(&list);    
    
    /* If there is no calc value, then try to open driver */
    if (calc_value == NULL)
    {
        if (start_test() != 0)
        {
            printf("Failed to initialize\n");
            goto done;
        }
    }
        
    if (set_match_info(&match, arch, impl, cpu, pl, noparse, calc_value) != 0)
    {
        TRACE("Failed to set_match_info\n", 0);
        goto done;
    }
    strncpy(match.register_name, regname, PARSE_MAX_REGISTER - 1);                  
    
    if (read_tables_to_memory(include_devices) != 0)
    {
        TRACE("Failed to expand table to memory\n", 0);
    }
    
    if (find_bitfields(&match, &list) != 0)
    {
        TRACE("Failed to find_bitfields\n", 0);
        goto done;
    }   
    
    if (flatten_results(&list, results_out) != 0)
    {
        TRACE("Failed to flatten_results\n", 0);
        goto done;
    }
    
        
    result = 0;
done:
    if (calc_value == NULL)
    {
        stop_test();
    }    
    ms_list_free(&list);
    erase_tables();
    return result;
}

int
ms_find_by_group(
    char * arch,
    char * impl,
    char * groupname,   
    int cpu,
    unsigned int pl, 
    unsigned int noparse,
    int include_devices,
    ms_bitfield_info ** results_out)
{
    int result = -1;
    match_info match;
    ms_list_head list;  
        
    if (results_out == NULL)
    {
        goto done;
    }
    
    ms_list_init(&list);    
    
    if (start_test() != 0)
    {
        printf("Failed to initialize\n");
        goto done;
    }       
    if (set_match_info(&match, arch, impl, cpu, pl, noparse, NULL) != 0)
    {
        TRACE("Failed to set_match_info\n", 0);
        goto done;
    }
    strncpy(match.functional_group, groupname, PARSE_MAX_FUNCTIONAL - 1);               
    
    if (read_tables_to_memory(include_devices) != 0)
    {
        TRACE("Failed to expand table to memory\n", 0);
    }
    
    if (find_bitfields(&match, &list) != 0)
    {
        TRACE("Failed to find_bitfields\n", 0);
        goto done;
    }   
    
    if (flatten_results(&list, results_out) != 0)
    {
        TRACE("Failed to flatten_results\n", 0);
        goto done;
    }
    
        
    result = 0;
done:    
    stop_test();
    ms_list_free(&list);
    erase_tables();
    return result;
}

int
ms_get_size(ms_bitfield_info * size)
{
    if (size != NULL)
    {
        return size[0].array_elements;
    }
    return -1;
}

int
ms_free_result(ms_bitfield_info ** tofree)
{
    if ((tofree == NULL) || (*tofree == NULL))
    {
        return -1;
    }
    
    free(*tofree);
    *tofree = NULL;
    return 0;
}

unsigned long long
ms_get_value(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return -1;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].value;
    }
    
    return -1;
}

char *
ms_get_register_string(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].register_name;
    }
    
    return NULL;
}

char *
ms_get_group_string(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].functional_group;
    }
    
    return NULL;
}

char *
ms_get_description_string(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].description;
    }
    
    return NULL;
}

char *
ms_get_partnumber_string(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].part_number;
    }
    
    return NULL;
}

char *
ms_get_decode_string(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].part_number_table;
    }
    
    return NULL;
}

int
ms_get_cpu(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return -1;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].cpu;
    }
    
    return -1;
}

static
int
translate_epl(unsigned int epl)
{
    switch(epl)
    {
        // non-secure user
        case NS_PRIVILEGE_LEVEL_0:
            return 0;
        // non-secure kernel
        case NS_PRIVILEGE_LEVEL_1:
            return 1;
        // non-secure hyp
        case NS_PRIVILEGE_LEVEL_2:
            return 2;
        // secure PL0
        case S_PRIVILEGE_LEVEL_0:
            return 3;
        // secure PL1
        case S_PRIVILEGE_LEVEL_1:
            return 4;
        default:
            return -1;
    }
}

int
ms_get_epl(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return -1;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return translate_epl(results_in[index].epl);
    }
    
    return -1;
}


int
ms_get_uid(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return -1;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].uid;
    }
    
    return -1;
}

int
ms_get_status(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return -1;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].status;
    }
    
    return -1;
}

char * 
ms_get_status_string(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return get_error_string(results_in[index].status);
    }
    
    return NULL;
}

char *
ms_get_bitfield_string(
    ms_bitfield_info * results_in,
    unsigned int index)
{

    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].bitfield;
    }
    
    return NULL;
}

char *
ms_get_stamp(
    ms_bitfield_info * results_in,
    unsigned int index)
{
    if (results_in == NULL)
    {
        return NULL;
    }
    
    if (index <= ms_get_size(results_in))
    {
        return results_in[index].stamp;
    }
    
    return NULL;
}
