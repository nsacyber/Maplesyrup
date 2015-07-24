#ifndef _MS_AARCH64_H
#define _MS_AARCH64_H

/* ARM V8-A */
void do_AA64AFR0_read(unsigned long long * val1);
void do_AA64AFR1_read(unsigned long long * val1);
void do_AA64DFR0_read(unsigned long long * val1);
void do_AA64DFR1_read(unsigned long long * val1);
void do_AA64ISAR0_read(unsigned long long * val1);
void do_AA64ISAR1_read(unsigned long long * val1);
void do_AA64MMFR0_read(unsigned long long * val1);
void do_AA64MMFR1_read(unsigned long long * val1);
void do_AA64PFR0_read(unsigned long long * val1);
void do_AA64PFR1_read(unsigned long long * val1);

/* Special purpose registers */
void do_CURRENTEL_read(unsigned int * val1);
void do_DAIF_read(unsigned int * val1);
void do_FPCR_read(unsigned int * val1);
void do_FPSR_read(unsigned int * val1);
void do_NZCV_read(unsigned int * val1);
void do_SPSEL_read(unsigned int * val1);


#endif

