#ifndef _MS_INTC_H
#define _MS_INTC_H

typedef struct ms_gic_map
{
    /* GIC CPU Interface */
    void * gicc_phys;
    void * gicc_virt;
    unsigned int gicc_size;
    
    /* GIC Distributor Interface */
    void * gicd_phys;
    void * gicd_virt;
    unsigned int gicd_size;
    
    /* Virtual Interface */
    void * gich_phys;
    void * gich_virt;
    unsigned int gich_size;
    
    /* Virtual CPU */
    void * gicv_phys;
    void * gicv_virt;
    unsigned int gicv_size;
    
} ms_gic_map;


#endif
