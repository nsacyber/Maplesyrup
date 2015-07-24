#ifndef _CP_15_H
#define _CP_15_H

/* Identification Registers function group */
void do_MIDR_read(unsigned int * midr);
void do_CTR_read(unsigned int * val);
void do_TCMTR_read(unsigned int * val);
void do_TLBTR_read(unsigned int * val);
void do_MPIDR_read(unsigned int * val);
void do_REVIDR_read(unsigned int * val);
void do_PFR0_read(unsigned int * val);
void do_PFR1_read(unsigned int * val);
void do_DFR0_read(unsigned int * val);
void do_AFR0_read(unsigned int * val);
void do_MMFR0_read(unsigned int * val);
void do_MMFR1_read(unsigned int * val);
void do_MMFR2_read(unsigned int * val);
void do_MMFR3_read(unsigned int * val);
void do_MMFR4_read(unsigned int * val);
void do_ISAR0_read(unsigned int * val);
void do_ISAR1_read(unsigned int * val);
void do_ISAR2_read(unsigned int * val);
void do_ISAR3_read(unsigned int * val);
void do_ISAR4_read(unsigned int * val);
void do_ISAR5_read(unsigned int * val);
void do_CCSIDR_read(unsigned int * val);
void do_CLIDR_read(retdatatype * val);
void do_AIDR_read(unsigned int * val);
void do_CSSELR_read(unsigned int * val);

/* Virtual Memory Control functional group */
void do_SCTLR_read(unsigned int * val);
void do_TTBR0_read(retdatatype * val);
void do_TTBR0LPAE_read(unsigned int * low, unsigned int * hi);
void do_TTBR1_read(retdatatype * val);
void do_TTBR1LPAE_read(unsigned int * low, unsigned int * hi);
void do_TTBCR_read(retdatatype * val);
void do_DACR_read(unsigned int * val);
void do_PRRR_read(unsigned int * val);
void do_MAIR_read(unsigned long long * val);
void do_MAIR0_read(unsigned int * val);
void do_MAIR1_read(unsigned int * val);
void do_NMRR_read(unsigned int * val);
void do_CONTEXTIDR_read(unsigned int * val);
void do_AMAIR0_read(unsigned int * val);
void do_AMAIR1_read(unsigned int * val);
void do_AMAIR_read(unsigned long long * val);
void do_DCZID_read(unsigned int * val);

/* Other System Control Registers */
void do_CPACR_read(unsigned int * val);
void do_FCSEIDR_read(unsigned int * val);
void do_RMR_read(unsigned int * val);
void do_RVBAR_read(unsigned int * val);

/* Address Translations */
void do_PAR_read(retdatatype * val);

/* Miscellaneous function group */
void do_TPIDRPRW_read(retdatatype * val);
void do_TPIDRURO_read(retdatatype * val);
void do_TPIDRURW_read(retdatatype * val);

/* Performance Monitors */
void do_PMCCFILTR_read(unsigned int * val);
void do_PMCR_read(unsigned int * val);
void do_PMCNTENSET_read(unsigned int * val);
void do_PMCNTENCLR_read(unsigned int * val);
void do_PMOVSR_read(unsigned int * val);
void do_PMSWINC_read(unsigned int * val);
void do_PMSELR_read(unsigned int * val);
void do_PMCEID0_read(unsigned int * val);
void do_PMCEID1_read(unsigned int * val);
void do_PMCCNTR_read(retdatatype * val);
void do_PMXEVTYPER_read(unsigned int * val);
void do_PMXEVCNTR_read(unsigned int * val);
void do_PMUSERENR_read(unsigned int * val);
void do_PMINTENSET_read(unsigned int * val);
void do_PMINTENCLR_read(unsigned int * val);
void do_PMOVSSET_read(unsigned int * val);
void do_PMOVSCLR_read(unsigned int * val);
 
/* Security Extensions functional group */
void do_SCR_read(unsigned int * val);
void do_SDER_read(unsigned int * val);
void do_NSACR_read(unsigned int * val);
void do_VBAR_read(retdatatype * val);
void do_MVBAR_read(unsigned int * val);
void do_ISR_read(unsigned int * val);

/* Fault Handling functional group */
void do_IFAR_read(unsigned int * val);
void do_IFSR_read(unsigned int * val);
void do_ADFSR_read(unsigned int * val);
void do_AIFSR_read(unsigned int * val);
void do_DFAR_read(unsigned int  * val);
void do_DFSR_read(unsigned int * val);
void do_AFSR0_read(unsigned int * val1);
void do_AFSR1_read(unsigned int * val1);
void do_ESR_read(unsigned int * val);
void do_FAR_read(unsigned long long * val);

/* Generic Timer Register */
void do_CNTFRQ_read(unsigned int * val);
void do_CNTHCTL_read(unsigned int * val);
void do_CNTHP_CTL_read(unsigned int * val); //todo
void do_CNTHP_CVAL_read(unsigned int * val); //todo
void do_CNTHP_TVAL_read(unsigned int * val); //todo
void do_CNTKCTL_read(unsigned int * val);
void do_CNTP_CTL_read(unsigned int * val); //todo
void do_CNTP_CVAL_read(unsigned int * val); //todo
void do_CNTP_TVAL_read(unsigned int * val); //todo
void do_CNTPCT_read(unsigned int * val); //todo
void do_CNTV_CTL_read(unsigned int * val); //todo
void do_CNTV_CVAL_read(unsigned int * val); //todo
void do_CNTV_TVAL_read(unsigned int * val); //todo
void do_CNTVCT_read(unsigned int * val); //todo
void do_CNTVOFF_read(unsigned int * val); //todo

/* Implementation defined registers */
void do_L2ACTLR_read(unsigned int * val);
void do_CPUMERRSR_read(unsigned int * val1, unsigned int * val2);
void do_L2MERRSR_read(unsigned int * val1, unsigned int * val2);
void do_L2PFR_read(unsigned int * val);
void do_ACTLR_read(retdatatype * val);
void do_ACTLR2_read(unsigned int * val);
void do_CPUACTLR_read(unsigned long long * val);
void do_L2CTLR_read(unsigned int * val);
void do_L2ECTLR_read(unsigned int * val);
void do_IL1DATA0_read(unsigned int * val);
void do_IL1DATA1_read(unsigned int * val);
void do_IL1DATA2_read(unsigned int * val);
void do_IL1DATA3_read(unsigned int * val);
void do_DL1DATA0_read(unsigned int * val);
void do_DL1DATA1_read(unsigned int * val);
void do_DL1DATA2_read(unsigned int * val);
void do_DL1DATA3_read(unsigned int * val);
void do_CDBGDR0_read(unsigned int * val);
void do_CDBGDR1_read(unsigned int * val);
void do_CDBGDR2_read(unsigned int * val);
void do_CDBGDCT_read(unsigned int * val);
void do_CDBGICT_read(unsigned int * val);
void do_CDBGDCD_read(unsigned int * val);
void do_CDBGICD_read(unsigned int * val);
void do_CDBGTD_read(unsigned int * val);
void do_CBAR_read(retdatatype * val);
void do_CPUECTLR_read(unsigned long long * val);

/* Debug */
void do_DBGDIDR_read(unsigned int * val);
void do_DBGAUTHSTATUS_read(unsigned int * val);
void do_DBGBCR_read(unsigned int * val);
void do_DBGBVR_read(unsigned int * val);
void do_DBGBXVR_read(unsigned int * val);
void do_DBGCID0_read(unsigned int * val);
void do_DBGCID1_read(unsigned int * val);
void do_DBGCID2_read(unsigned int * val);
void do_DBGCID3_read(unsigned int * val);
void do_DBGCIDSR_read(unsigned int * val);
void do_DBGCLAIMCLR_read(unsigned int * val);
void do_DBGCLAIMSET_read(unsigned int * val);
void do_DBGDEVID_read(unsigned int * val);
void do_DBGDEVID1_read(unsigned int * val);
void do_DBGDEVTYPE_read(unsigned int * val);
void do_DBGDRAR_read(unsigned long long * val);
void do_DBGDRCR_read(unsigned int * val);
void do_DBGDSAR_read(unsigned int * val);
void do_DBGDSCCR_read(unsigned int * val);
void do_DBGDSCR_read(unsigned int * val);
void do_DBGDSMCR_read(unsigned int * val);
void do_DBGDTRRX_read(unsigned int * val);
void do_DBGDTRTX_read(unsigned int * val);
void do_DBGEACR_read(unsigned int * val);
void do_DBGECR_read(unsigned int * val);
void do_DBGITCTRL_read(unsigned int * val);
void do_DBGITR_read(unsigned int * val);
void do_DBGLAR_read(unsigned int * val);
void do_DBGLSR_read(unsigned int * val);
void do_DBGOSDLR_read(unsigned int * val);
void do_DBGOSLSR_read(unsigned int * val);
void do_DBGOSSRR_read(unsigned int * val);
void do_DBGPCSR_read(unsigned int * val);
void do_DBGPID0_read(unsigned int * val);
void do_DBGPID1_read(unsigned int * val);
void do_DBGPID2_read(unsigned int * val);
void do_DBGPID3_read(unsigned int * val);
void do_DBGPID4_read(unsigned int * val);
void do_DBGPRCR_read(unsigned int * val);
void do_DBGPRSR_read(unsigned int * val);
void do_DBGVCR_read(unsigned int * val);
void do_DBGVIDSR_read(unsigned int * val);
void do_DBGWCR_read(unsigned int * val);
void do_DBGWFAR_read(unsigned int * val);
void do_DBGWVR_read(unsigned int * val);

/* Thumb */
void do_TEECR_read(unsigned int * val);
void do_TEEHBR_read(unsigned int * val);

/* Jazelle Registers */
void do_JIDR_read(unsigned int * val);
void do_JMCR_read(unsigned int * val);
void do_JOSCR_read(unsigned int * val);


/* DMA Support */
/* Implementation Defined */

/* TCM Control */
/* Implementation Defined */

/* Data Memory Barrier Operations */
/* Cache & Branch Predictor Maintenance Operations */
/* TLB Maintenance Operations - not in hyp mode */
/* TLB Maintenance Operations - hyp mode */



#endif


