/****************************************************************************//**
 * @file     ssp_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 * @note
 * Copyright (C) 2012 Marvell Technology Group Ltd. All rights reserved.
 *
 * @par
 * Marvell is supplying this software which provides customers with programming
 * information regarding the products. Marvell has no responsibility or 
 * liability for the use of the software. Marvell not guarantee the correctness 
 * of this software. Marvell reserves the right to make changes in the software 
 * without notification. 
 * 
 *******************************************************************************/

#ifndef _SSP_REG_H
#define _SSP_REG_H

struct ssp_reg {
    /* 0x00: SSP Control Register 0 */
    union {
        struct {
            uint32_t DSS            :  4;  /* [3:0]   r/w */
            uint32_t FRF            :  2;  /* [5:4]   r/w */
            uint32_t RESERVED_6     :  1;  /* [6]     r/w */
            uint32_t SSE            :  1;  /* [7]     r/w */
            uint32_t RESERVED_19_8  : 12;  /* [19:8]  rsvd */
            uint32_t EDSS           :  1;  /* [20]    r/w */
            uint32_t RESERVED_21    :  1;  /* [21]    rsvd */
            uint32_t RIM            :  1;  /* [22]    r/w */
            uint32_t TIM            :  1;  /* [23]    r/w */
            uint32_t FRDC           :  3;  /* [26:24] r/w */
            uint32_t MCRT           :  1;  /* [27]    r/w */
            uint32_t RHCD           :  1;  /* [28]    r/w */
            uint32_t FPCKE          :  1;  /* [29]    r/w */
            uint32_t RESERVED_30    :  1;  /* [30]    rsvd */
            uint32_t MOD            :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } SSCR0;

    /* 0x04: SSP Control Register 1 */
    union {
        struct {
            uint32_t RIE            :  1;  /* [0]     r/w */
            uint32_t TIE            :  1;  /* [1]     r/w */
            uint32_t LBM            :  1;  /* [2]     r/w */
            uint32_t SPO            :  1;  /* [3]     r/w */
            uint32_t SPH            :  1;  /* [4]     r/w */
            uint32_t RESERVED_5     :  1;  /* [5]     rsvd */
            uint32_t TFT            :  4;  /* [9:6]   r/w */
            uint32_t RFT            :  4;  /* [13:10] r/w */
            uint32_t EFWR           :  1;  /* [14]    r/w */
            uint32_t STRF           :  1;  /* [15]    r/w */
            uint32_t IFS            :  1;  /* [16]    r/w */
            uint32_t RESERVED_18_17 :  2;  /* [18:17] rsvd */
            uint32_t TINTE          :  1;  /* [19]    r/w */
            uint32_t RSRE           :  1;  /* [20]    r/w */
            uint32_t TSRE           :  1;  /* [21]    r/w */
            uint32_t RESERVED_22    :  1;  /* [22]    rsvd */
            uint32_t RWOT           :  1;  /* [23]    r/w */
            uint32_t SFRMDIR        :  1;  /* [24]    r/w */
            uint32_t SCLKDIR        :  1;  /* [25]    r/w */
            uint32_t RESERVED_27_26 :  2;  /* [27:26] rsvd */
            uint32_t SCFR           :  1;  /* [28]    r/w */
            uint32_t EBCEI          :  1;  /* [29]    r/w */
            uint32_t TTE            :  1;  /* [30]    r/w */
            uint32_t TTELP          :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } SSCR1;

    /* 0x08: SSP Status Register */
    union {
        struct {
            uint32_t RESERVED_1_0   :  2;  /* [1:0]   rsvd */
            uint32_t TNF            :  1;  /* [2]     r/o */
            uint32_t RNE            :  1;  /* [3]     r/o */
            uint32_t BSY            :  1;  /* [4]     r/o */
            uint32_t TFS            :  1;  /* [5]     r/o */
            uint32_t RFS            :  1;  /* [6]     r/o */
            uint32_t ROR            :  1;  /* [7]     r/w */
            uint32_t TFL            :  4;  /* [11:8]  r/o */
            uint32_t RFL            :  4;  /* [15:12] r/o */
            uint32_t RESERVED_18_16 :  3;  /* [18:16] rsvd */
            uint32_t TINT           :  1;  /* [19]    r/w */
            uint32_t RESERVED_20    :  1;  /* [20]    rsvd */
            uint32_t TUR            :  1;  /* [21]    r/w */
            uint32_t CSS            :  1;  /* [22]    r/o */
            uint32_t BCE            :  1;  /* [23]    r/w */
            uint32_t RESERVED_29_24 :  6;  /* [29:24] rsvd */
            uint32_t TX_OSS         :  1;  /* [30]    r/o */
            uint32_t OSS            :  1;  /* [31]    r/o */
        } BF;
        uint32_t WORDVAL;
    } SSSR;

    /* 0x0c: SSP Interrupt Test Register */
    union {
        struct {
            uint32_t RESERVED_4_0   :  5;  /* [4:0]   rsvd */
            uint32_t TTFS           :  1;  /* [5]     r/w */
            uint32_t TRFS           :  1;  /* [6]     r/w */
            uint32_t TROR           :  1;  /* [7]     r/w */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SSITR;

    /* 0x10: SSP Data Register */
    union {
        struct {
            uint32_t DATA           : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } SSDR;

    uint8_t zReserved0x14[20];  /* pad 0x14 - 0x27 */

    /* 0x28: SSP Time Out Register */
    union {
        struct {
            uint32_t TIMEOUT        : 24;  /* [23:0]  r/w */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SSTO;

    /* 0x2c: SSP Programmable Serial Protocol Register */
    union {
        struct {
            uint32_t SCMODE         :  2;  /* [1:0]   r/w */
            uint32_t SFRMP          :  1;  /* [2]     r/w */
            uint32_t ETDS           :  1;  /* [3]     r/w */
            uint32_t STRTDLY        :  3;  /* [6:4]   r/w */
            uint32_t DMYSTRT        :  2;  /* [8:7]   r/w */
            uint32_t SFRMDLY        :  7;  /* [15:9]  r/w */
            uint32_t SFRMWDTH       :  6;  /* [21:16] r/w */
            uint32_t RESERVED_22    :  1;  /* [22]    rsvd */
            uint32_t DMYSTOP        :  2;  /* [24:23] r/w */
            uint32_t FSRT           :  1;  /* [25]    r/w */
            uint32_t EDMYSTRT       :  2;  /* [27:26] r/w */
            uint32_t EDMYSTOP       :  3;  /* [30:28] r/w */
            uint32_t RESERVED_31    :  1;  /* [31]    rsvd */
        } BF;
        uint32_t WORDVAL;
    } SSPSP;

    /* 0x30: SSP TX Time Slot Active Register */
    union {
        struct {
            uint32_t TTSA           :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SSTSA;

    /* 0x34: SSP RX Time Slot Active Register */
    union {
        struct {
            uint32_t RTSA           :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8  : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SSRSA;

    /* 0x38: SSP Time Slot Status Register */
    union {
        struct {
            uint32_t TSS            :  3;  /* [2:0]   r/o */
            uint32_t RESERVED_30_3  : 28;  /* [30:3]  rsvd */
            uint32_t NMBSY          :  1;  /* [31]    r/o */
        } BF;
        uint32_t WORDVAL;
    } SSTSS;

};

typedef volatile struct ssp_reg ssp_reg_t;

#ifdef SSP_IMPL
BEGIN_REG_SECTION(ssp_registers)
ssp_reg_t SSPREG;
END_REG_SECTION(ssp_registers)
#else
extern ssp_reg_t SSPREG;
#endif

#endif /* _SSP_REG_H */

