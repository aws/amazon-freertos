#ifndef __TEMPLATE_I2C_REGISTERS_H__
#define __TEMPLATE_I2C_REGISTERS_H__

typedef struct i2c_regs {
   __I2C1CONbits_t I2CxCON;
   volatile unsigned int I2CxCONCLR;
   volatile unsigned int I2CxCONSET;
   volatile unsigned int I2CxCONINV;
   __I2C1STATbits_t I2CxSTAT;
   volatile unsigned int I2CxSTATCLR;
   volatile unsigned int I2CxSTATSET;
   volatile unsigned int I2CxSTATINV;
   volatile unsigned int I2CxADD;
   unsigned int DONTUSE1[3];
   volatile unsigned int I2CxMSK;
   unsigned int DONTUSE2[3];
   volatile unsigned int I2CxBRG;
   unsigned int DONTUSE3[3];
   volatile unsigned int I2CxTRN;
   unsigned int DONTUSE4[3];
   volatile unsigned int I2CxRCV;
   unsigned int DONTUSE5[3];
} i2c_register_t;

#define I2CxCON_SEN_MASK	_I2C1CON_SEN_MASK    /* start */
#define I2CxCON_RSEN_MASK	_I2C1CON_RSEN_MASK   /* repeat-start*/
#define I2CxCON_PEN_MASK	_I2C1CON_PEN_MASK    /* stop */
#define I2CxCON_RCEN_MASK	_I2C1CON_RCEN_MASK   /* receive */
#define I2CxCON_ACKEN_MASK	_I2C1CON_ACKEN_MASK  /* ack */
#define I2CxCON_ACKDT_MASK	_I2C1CON_ACKDT_MASK  /* ack data */
#define I2CxCON_STREN_MASK	_I2C1CON_STREN_MASK  /* clock stretch */
#define I2CxCON_A10M_MASK	_I2C1CON_A10M_MASK   /* 10-bit address */
#define I2CxCON_SCLREL_MASK _I2C1CON_SCLREL_MASK /* hold clock */
#define I2CxCON_ON_MASK     _I2C1CON_ON_MASK     /* Enable */
#define I2CxCON_SIDL_MASK   _I2C1CON_SIDL_MASK   /* idle */
#define I2CxCON_STRICT_MASK _I2C1CON_STRICT_MASK /* Strict */
#define I2CxCON_DISSLW_MASK _I2C1CON_DISSLW_MASK /* Slew */
#define I2CxCON_SMEN_MASK   _I2C1CON_SMEN_MASK   /* SMBus */
#define I2CxCON_GCEN_MASK   _I2C1CON_GCEN_MASK   /* General Call */
#define I2CxCON_PCIE_MASK   _I2C1CON_PCIE_MASK   /* Interrupt on Stop */
#define I2CxCON_SCIE_MASK   _I2C1CON_SCIE_MASK   /* Interrupt on Start */
#define I2CxCON_BOEN_MASK   _I2C1CON_BOEN_MASK   /* Interrupt on Buffer Over */
#define I2CxCON_SBCDE_MASK  _I2C1CON_SBCDE_MASK  /* Int on Bus collision */
#define I2CxCON_AHEN_MASK   _I2C1CON_AHEN_MASK   /* Address Hold Enable */
#define I2CxCON_DHEN_MASK   _I2C1CON_DHEN_MASK   /* Hold enable */

/* Status */
#define I2CxSTAT_I2COV_MASK _I2C1STAT_I2COV_MASK /* overflow */
#define I2CxSTAT_IWCOL_MASK _I2C1STAT_IWCOL_MASK /* Write Collision */
#define I2CxSTAT_BCL_MASK   _I2C1STAT_BCL_MASK   /* Bus Collision */
#define I2CxSTAT_P_MASK     _I2C1STAT_P_MASK     /* stop */
#define I2CxSTAT_S_MASK     _I2C1STAT_S_MASK     /* start */

#endif /* __TEMPLATE_I2C_REGISTERS_H__ */
