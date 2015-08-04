#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "maplesyrup.h"

#define MAX_ARCH_BUFFER 16
#define MAX_IMPL_BUFFER 16
#define MAX_CALC_BUFFER 32

#define FLAG_SHOW_GROUPS 1
#define FLAG_SHOW_BITFIELDS 2
#define FLAG_SHOW_REGISTERS 3

typedef struct option_wrapper
{
    struct option * name;
    const char * usage;
} option_wrapper;

int flag_fb_group = 0;
int flag_fb_register = 0;
int flag_fb_bitfield = 0;
int flag_fb_all = 0;
int flag_fb_el0 = 0;
int flag_fb_el1 = 0;
int flag_fb_show_info = 0;
int flag_fb_showerrors = 0;
int flag_fb_noparse = 0;
int flag_fb_calc = 0;

static struct option long_options[] = 
{       
        /* arguments */     
        { "arch", required_argument, 0, 'a' },
        { "impl", required_argument, 0, 'i' },
        { "core", required_argument, 0, 'c' },
        { "calc", required_argument, 0, 'v' },
        { "help", no_argument, 0, 'h' },
        
        /* flags */
        { "group", no_argument, &flag_fb_group, 1 },
        { "register", no_argument, &flag_fb_register, 1 },
        { "bitfield", no_argument, &flag_fb_bitfield, 1 },
        { "all", no_argument, &flag_fb_all, 1 },
        { "noparse", no_argument, &flag_fb_noparse, 1 },
        { "showfail", no_argument, &flag_fb_showerrors, 1 },        
        { "el0", no_argument, &flag_fb_el0, 1 },
        { "el1", no_argument, &flag_fb_el1, 1 },        
        { "show_groups", no_argument, &flag_fb_show_info, FLAG_SHOW_GROUPS },
        { "show_bitfields", no_argument, &flag_fb_show_info, FLAG_SHOW_BITFIELDS },
        { "show_registers", no_argument, &flag_fb_show_info, FLAG_SHOW_REGISTERS },
        { 0, 0, 0 , 0 },
};

static
void
print_usage(void)
{   
    int i = 0;
    option_wrapper usage_struct[] = 
    {
        { long_options, "armv7a or armv8a" },
        { long_options, "cortex-a7, cortex-a15, cortex-a53, cortex-a57, or all" },
        { long_options, "core number, or -1 for all cores" },
        { long_options, "supply register value. Use with --register <regname>" },
        { long_options, "show this menu" },
        
        { long_options, "search by group" },
        { long_options, "search by register" },
        { long_options, "search by bitfield" },
        { long_options, "search all available bitfields" },
        { long_options, "don't parse the results into bitfields" },
        { long_options, "show all results regardless of success" },
        { long_options, "force execution at pl0" },
        { long_options, "force execution at pl1" },
        { long_options, "shows all valid functional groups" },  
        { long_options, "shows all valid bitfields" },  
        { long_options, "shows all valid register" },   
    };
    
    printf("Usage: maplesyrup --<register|group|bitfield|all> <NAME or ID> --arch=<ARCH> --impl=<IMPL or all> [--core=<CORE or -1>] [OTHER OPTIONS]\n");
    printf("\n");
    
    for (i = 0; i < sizeof(usage_struct) / sizeof(option_wrapper); ++i)
    {
        printf("  --%s - %s\n", usage_struct[i].name[i].name, usage_struct[i].usage);
    }
    
    printf("\nExamples:\n\t(1) maplesyrup --register MIDR --arch=armv7a --impl=all\n\t\tto display the MIDR register\n");
    printf("\t(2) maplesyrup --all --arch=armv7a --impl=all\n\t\tto display the all registers\n");
    printf("\t(3) maplesyrup --register MIDR --arch=armv7a --impl=all --calc=0x410fc073\n\t\tsupply a value\n");
    
    printf("\nThe kernel module must be loaded before running.\n");    
    
    return;
}

static
void
show_banner(unsigned int val1,
            unsigned int valid_elements,
            unsigned int elements)
{
    printf("\n");
    printf("============================\n");
    printf("|| Maplesyrup 1.0         ||\n");    
#ifdef __gnu_linux__
    printf("||    -linux              ||\n");
#endif
#ifdef __ANDROID__
    printf("||    -android            ||\n");
#endif
#ifdef __aarch64__
    printf("||    -aarch64            ||\n");
#else
    printf("||    -aarch32            ||\n");
#endif    
    
    if (val1)
    {
        printf("============================\n");
        printf("\n");
        printf("\t--help for options\n");
    }
    else
    {       
        printf("||                        ||\n");
        printf("|| %06d/%06d entries  ||\n", valid_elements, elements);
        printf("============================\n");
    }
}

static
void
throw_table_static(void)
{
    char * old_locale = NULL;
    char * saved_locale = NULL;    
    wchar_t * table = L"RAWWR(╯°□°）╯︵ ┻━┻";    
    
    old_locale = setlocale(LC_CTYPE, NULL);
    
    if (old_locale == NULL)
    {
        return;
    }
    
    saved_locale = strdup(old_locale);
    if (saved_locale == NULL)
    {
        return;
    }
    
    setlocale(LC_CTYPE, "");  
        
    fprintf(stdout, "%ls\n", table);
    
    setlocale(LC_CTYPE, saved_locale);
    if (saved_locale != NULL)
    {
        free(saved_locale);
    }

    return;
};

static
int
get_ppl_from_flag(
    unsigned int el0,
    unsigned int el1)
{
    unsigned int result = 0;
    if ((el0 > 0) && (el1 == 0))
    {   
        throw_table_static();
        return NS_PRIVILEGE_LEVEL_0;
    }
    else if ((el0 == 0) && (el1 > 0))
    {
        throw_table_static();
        return NS_PRIVILEGE_LEVEL_1;
    }
    else if ((el0 > 0) && (el1 > 0))
    {
        throw_table_static();
        return NS_PRIVILEGE_LEVEL_0 | NS_PRIVILEGE_LEVEL_1;
    }
    else
    {
        return PREFERRED_PL;
    }
}

static
int
no_results(ms_bitfield_info * output)
{
    int elements = -1;
    int i = 0;
    int j = 0;
    int valid_elements = 0;
    
    if (output == NULL)
    {
        return -1;
    }
    elements = ms_get_size(output);     
    
    for (i = 0; i < elements; ++i)
    {       
        if (ms_get_status(output, i) == 0)      
        {
            break;
        }       
    }
    for (j = 0; j < elements; ++j)
    {       
        if (ms_get_status(output, j) == 0)      
        {
            ++valid_elements;
        }       
    }
    if ((i < elements) || (flag_fb_showerrors == 1))
    {
        /* at least one valid entry */
        show_banner(0, valid_elements, elements);
        return 0;
    }
    else
    {
        printf("No results\n");
        return -1;
    }   
}

static
void
show_results(ms_bitfield_info * output)
{
    int elements = ms_get_size(output); 
    int i = 0;
    
        
    /* Show banner */
    if ((no_results(output) != 0) && (flag_fb_showerrors == 0))
    {
        goto done;
    }   
    
    /* Show results */
    for (i = 0; i < elements; ++i)
    {       
        if ((ms_get_status(output, i) == 0) || (flag_fb_showerrors == 1))
        {   
            printf("[cpu%d/%s/%s/%d/%s/%s/%s/%s]: 0x%llx (%s) (%d)\n", 
            ms_get_cpu(output, i),
            ms_get_partnumber_string(output, i),
            ms_get_decode_string(output, i),
            ms_get_epl(output, i),
            ms_get_group_string(output, i),
            ms_get_register_string(output, i),                          
            ms_get_stamp(output, i),            
            ms_get_bitfield_string(output, i),
            ms_get_value(output, i),
            ms_get_description_string(output, i),
            ms_get_status(output, i));
        }
    }
done:
    
    return;
}

int
main(int argc, char ** argv)
{
    int c;
    int result = -1;    
    int i = 0;
    char arg_arch[MAX_ARCH_BUFFER];
    char arg_impl[MAX_IMPL_BUFFER];   
    char arg_calc[MAX_CALC_BUFFER]; 
    ms_bitfield_info * output = NULL;
    ms_bitfield_info * toshow = NULL;   
    int arg_core = -1;    
    int option_index = 0;   
    int ppl = 0;    
    int m = 0;
    
    if (argc == 1)
    {
        show_banner(1, 0, 0);
    }

    while ((c = getopt_long(argc, argv, "t:a:i:c:v:", long_options, &option_index)) != -1)
    {           
        switch(c)
        {   
            case 0:
                break;
            case 'a':               
                strncpy(arg_arch, optarg, MAX_ARCH_BUFFER - 1);             
                break;
            case 'i':                           
                strncpy(arg_impl, optarg, MAX_ARCH_BUFFER - 1);
                break;
            case 'c':               
                arg_core = atoi(optarg);
                break;
            case 'v':               
                strncpy(arg_calc, optarg, MAX_CALC_BUFFER - 1);
                flag_fb_calc = 1;
                break;
            case '?':
            case 'h':               
                print_usage();
                break;
            default:
                show_banner(1, 0, 0);
                return -1;
        }
    }
    
    if (flag_fb_show_info)
    {
        switch(flag_fb_show_info)
        {
            case FLAG_SHOW_GROUPS:
                ms_get_groups(&toshow);
                no_results(toshow);
                for (i = 0; i < ms_get_size(toshow); ++i)
                {       
                    printf("%s\n", ms_get_group_string(toshow, i));         
                }               
                ms_free_result(&toshow);
                break;
            case FLAG_SHOW_BITFIELDS:
                ms_get_bitfields(&toshow);
                no_results(toshow);
                for (i = 0; i < ms_get_size(toshow); ++i)
                {       
                    printf("0x%08x (%s.%s.%s)\n", 
                                            ms_get_uid(toshow, i),
                                            ms_get_decode_string(toshow, i),
                                            ms_get_register_string(toshow, i),
                                            ms_get_bitfield_string(toshow, i));         
                }
                ms_free_result(&toshow);
                break;
            case FLAG_SHOW_REGISTERS:
                ms_get_registers(&toshow);
                no_results(toshow);
                for (i = 0; i < ms_get_size(toshow); ++i)
                {       
                    printf("%s\n", ms_get_register_string(toshow, i));          
                }
                ms_free_result(&toshow);
                break;
            default:
                break;
        }       
        goto done;
    }
    
    ppl = get_ppl_from_flag(flag_fb_el0, flag_fb_el1);
    if (flag_fb_bitfield)
    {
        if (optind < argc)
        {   
            if (ms_find_by_bitfield(arg_arch,
                                    arg_impl,
                                    strtol(argv[optind], NULL, 16),
                                    arg_core,
                                    ppl,
                                    flag_fb_noparse,
                                    &output) != 0)
            {               
                goto done;
            }               
        }
    }
    else if (flag_fb_group)
    {
        if (optind < argc)
        {           
            if (ms_find_by_group(arg_arch,
                                 arg_impl,
                                 argv[optind],
                                 arg_core,
                                 ppl,
                                 flag_fb_noparse,
                                 &output) != 0)
            {               
                goto done;
            }   
        }
    }
    else if (flag_fb_register)
    {
        if (optind < argc)
        {
            if (flag_fb_calc)
            {
                if (ms_find_by_register(arg_arch,
                                        arg_impl,
                                        argv[optind],
                                        arg_core,
                                        ppl,
                                        flag_fb_noparse,
                                        arg_calc,
                                        &output) != 0)
                {               
                    goto done;
                }   
            }
            else
            {
                if (ms_find_by_register(arg_arch,
                                        arg_impl,
                                        argv[optind],
                                        arg_core,
                                        ppl,
                                        flag_fb_noparse,
                                        NULL,
                                        &output) != 0)
                {               
                    goto done;
                }   
            }
        }
    }   
    else if (flag_fb_all)
    {
        ms_get_groups(&toshow);
        for (i = 0; i < ms_get_size(toshow); ++i)
        {                       
            if (ms_find_by_group(arg_arch,
                                 arg_impl,
                                 ms_get_group_string(toshow, i),
                                 arg_core,
                                 ppl,
                                 flag_fb_noparse,
                                 &output) != 0)
            {               
                goto done;
            }           
            show_results(output);
            ms_free_result(&output);
        }       
    }   
    else
    {           
        goto done;
    }   

    show_results(output);
    
    result = 0;
    
done:
    ms_free_result(&toshow);
    ms_free_result(&output);
    printf("\n# Leaving the kernel module installed compromises the security of your system.\n# Use 'rmmod maplesyrup_mod' to unload the module.\n");
    return result;
}

