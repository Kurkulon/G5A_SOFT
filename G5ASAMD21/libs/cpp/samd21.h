#ifndef SAMD21_H__09_04_2019__08_14
#define SAMD21_H__09_04_2019__08_14

#ifndef CORETYPE_SAMD21
#error  Must #include "core.h"
#endif 

#include "types.h"
#include "cm0.h"

#define MCK 48000000


#ifndef WIN32
#define MK_PTR(n,p)  T_HW::S_##n * const n = ((T_HW::S_##n*)(p))
#else
extern byte core_sys_array[0x100000]; 
#define MK_PTR(n,p)  T_HW::S_##n * const n = ((T_HW::S_##n*)(core_sys_array-0x40000000+p))
#endif

#define MKPID(n,i) n##_M=(1UL<<(i&31)), n##_I=i

  /******  SAMD21J17A-specific Interrupt Numbers ***********************/
#define	PM_IRQ                  0 	/**<  0 SAMD21J17A Power Manager (PM) */
#define	SYSCTRL_IRQ             1 	/**<  1 SAMD21J17A System Control (SYSCTRL) */
#define	WDT_IRQ                 2 	/**<  2 SAMD21J17A Watchdog Timer (WDT) */
#define	RTC_IRQ                 3 	/**<  3 SAMD21J17A Real-Time Counter (RTC) */
#define	EIC_IRQ                 4 	/**<  4 SAMD21J17A External Interrupt Controller (EIC) */
#define	NVMCTRL_IRQ             5 	/**<  5 SAMD21J17A Non-Volatile Memory Controller (NVMCTRL) */
#define	DMAC_IRQ                6 	/**<  6 SAMD21J17A Direct Memory Access Controller (DMAC) */
#define	USB_IRQ                 7 	/**<  7 SAMD21J17A Universal Serial Bus (USB) */
#define	EVSYS_IRQ               8 	/**<  8 SAMD21J17A Event System Interface (EVSYS) */
#define	SERCOM0_IRQ             9 	/**<  9 SAMD21J17A Serial Communication Interface 0 (SERCOM0) */
#define	SERCOM1_IRQ            10 	/**< 10 SAMD21J17A Serial Communication Interface 1 (SERCOM1) */
#define	SERCOM2_IRQ            11 	/**< 11 SAMD21J17A Serial Communication Interface 2 (SERCOM2) */
#define	SERCOM3_IRQ            12 	/**< 12 SAMD21J17A Serial Communication Interface 3 (SERCOM3) */
#define	SERCOM4_IRQ            13 	/**< 13 SAMD21J17A Serial Communication Interface 4 (SERCOM4) */
#define	SERCOM5_IRQ            14 	/**< 14 SAMD21J17A Serial Communication Interface 5 (SERCOM5) */
#define	TCC0_IRQ               15 	/**< 15 SAMD21J17A Timer Counter Control 0 (TCC0) */
#define	TCC1_IRQ               16 	/**< 16 SAMD21J17A Timer Counter Control 1 (TCC1) */
#define	TCC2_IRQ               17 	/**< 17 SAMD21J17A Timer Counter Control 2 (TCC2) */
#define	TC3_IRQ                18 	/**< 18 SAMD21J17A Basic Timer Counter 3 (TC3) */
#define	TC4_IRQ                19 	/**< 19 SAMD21J17A Basic Timer Counter 4 (TC4) */
#define	TC5_IRQ                20 	/**< 20 SAMD21J17A Basic Timer Counter 5 (TC5) */
#define	TC6_IRQ                21 	/**< 21 SAMD21J17A Basic Timer Counter 6 (TC6) */
#define	TC7_IRQ                22 	/**< 22 SAMD21J17A Basic Timer Counter 7 (TC7) */
#define	ADC_IRQ                23 	/**< 23 SAMD21J17A Analog Digital Converter (ADC) */
#define	AC_IRQ                 24 	/**< 24 SAMD21J17A Analog Comparators (AC) */
#define	DAC_IRQ                25 	/**< 25 SAMD21J17A Digital Analog Converter (DAC) */
#define	PTC_IRQ                26 	/**< 26 SAMD21J17A Peripheral Touch Controller (PTC) */
#define	I2S_IRQ                27 	/**< 27 SAMD21J17A Inter-IC Sound Interface (I2S) */


#pragma anon_unions

namespace T_HW
{
	typedef volatile unsigned int AT91_REG;// Hardware register definition
	typedef volatile void * AT91_PTR;// Hardware register definition

	typedef volatile void * RW_PTR;	

	typedef void(*AT91_IHP)() __irq;	// Interrupt handler pointer

	typedef volatile const byte RO8, RoReg8;
	typedef volatile const u16 RO16, RoReg16;
	typedef volatile const u32 RO32, RoReg;

	typedef volatile byte RW8, RwReg8, WoReg8;
	typedef volatile u16 RW16, RwReg16;
	typedef volatile u32 RW32, RwReg;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_GCLK
	{
		RW8		CTRL;              //(*(RwReg8 *)0x40000C00U) /**< \brief (GCLK) Control */
		RO8		STATUS;            //(*(RoReg8 *)0x40000C01U) /**< \brief (GCLK) Status */
		RW16	CLKCTRL;           //(*(RwReg16*)0x40000C02U) /**< \brief (GCLK) Generic Clock Control */
		RW32	GENCTRL;           //(*(RwReg  *)0x40000C04U) /**< \brief (GCLK) Generic Clock Generator Control */
		RW32	GENDIV;            //(*(RwReg  *)0x40000C08U) /**< \brief (GCLK) Generic Clock Generator Division */

		void	GenDivCtrl(byte genid, u16 div, u32 ctrl) { GENDIV = genid|((u32)div<<8); GENCTRL = genid|ctrl; }
	};

	/* ========== Instance parameters for GCLK peripheral ========== */
	#define GCLK_GENDIV_BITS            16
	#define GCLK_GEN_NUM                9
	#define GCLK_NUM                    37
	#define GCLK_SOURCE_DFLL48M         7
	#define GCLK_SOURCE_FDPLL           8
	#define GCLK_SOURCE_GCLKGEN1        2
	#define GCLK_SOURCE_GCLKIN          1
	#define GCLK_SOURCE_NUM             9
	#define GCLK_SOURCE_OSCULP32K       3
	#define GCLK_SOURCE_OSC8M           6
	#define GCLK_SOURCE_OSC32K          4
	#define GCLK_SOURCE_XOSC            0
	#define GCLK_SOURCE_XOSC32K         5

	#define	GCLK_SRC_XOSC       		(0x0<<8)   		/**< \brief (GCLK_GENCTRL) XOSC oscillator output */
	#define	GCLK_SRC_GCLKIN     		(0x1<<8)   		/**< \brief (GCLK_GENCTRL) Generator input pad */
	#define	GCLK_SRC_GCLKGEN1   		(0x2<<8)   		/**< \brief (GCLK_GENCTRL) Generic clock generator 1 output */
	#define	GCLK_SRC_OSCULP32K  		(0x3<<8)   		/**< \brief (GCLK_GENCTRL) OSCULP32K oscillator output */
	#define	GCLK_SRC_OSC32K     		(0x4<<8)   		/**< \brief (GCLK_GENCTRL) OSC32K oscillator output */
	#define	GCLK_SRC_XOSC32K    		(0x5<<8)   		/**< \brief (GCLK_GENCTRL) XOSC32K oscillator output */
	#define	GCLK_SRC_OSC8M      		(0x6<<8)   		/**< \brief (GCLK_GENCTRL) OSC8M oscillator output */
	#define	GCLK_SRC_DFLL48M    		(0x7<<8)   		/**< \brief (GCLK_GENCTRL) DFLL48M output */
	#define	GCLK_SRC_FDPLL      		(0x8<<8)   		/**< \brief (GCLK_GENCTRL) FDPLL output */

	#define GCLK_GENEN					(1UL<<16)       /**< \brief (GCLK_GENCTRL) Generic Clock Generator Enable */
	#define GCLK_IDC        			(1UL<<17)		/**< \brief (GCLK_GENCTRL) Improve Duty Cycle */
	#define GCLK_OOV        			(1UL<<18)		/**< \brief (GCLK_GENCTRL) Output Off Value */
	#define GCLK_OE         			(1UL<<19)		/**< \brief (GCLK_GENCTRL) Output Enable */
	#define GCLK_DIVSEL     			(1UL<<20)		/**< \brief (GCLK_GENCTRL) Divide Selection */
	#define GCLK_RUNSTDBY   			(1UL<<21)		/**< \brief (GCLK_GENCTRL) Run in Standby */

	#define GCLK_DIV(value)				(((value) & 0xFFFF) << 8)

	#define GCLK_ID_DFLL48				0x00   	
	#define GCLK_ID_FDPLL       		0x01
	#define GCLK_ID_FDPLL32K    		0x02
	#define GCLK_ID_WDT         		0x03
	#define GCLK_ID_RTC         		0x04
	#define GCLK_ID_EIC         		0x05
	#define GCLK_ID_USB         		0x06
	#define GCLK_ID_EVSYS_0     		0x07
	#define GCLK_ID_EVSYS_1     		0x08
	#define GCLK_ID_EVSYS_2     		0x09
	#define GCLK_ID_EVSYS_3     		0x0A
	#define GCLK_ID_EVSYS_4     		0x0B
	#define GCLK_ID_EVSYS_5     		0x0C
	#define GCLK_ID_EVSYS_6     		0x0D
	#define GCLK_ID_EVSYS_7     		0x0E
	#define GCLK_ID_EVSYS_8     		0x0F
	#define GCLK_ID_EVSYS_9     		0x10
	#define GCLK_ID_EVSYS_10    		0x11
	#define GCLK_ID_EVSYS_11    		0x12
	#define GCLK_ID_SERCOMX_SLOW 		0x13
	#define GCLK_ID_SERCOM0_CORE 		0x14
	#define GCLK_ID_SERCOM1_CORE 		0x15
	#define GCLK_ID_SERCOM2_CORE 		0x16
	#define GCLK_ID_SERCOM3_CORE 		0x17
	#define GCLK_ID_SERCOM4_CORE 		0x18
	#define GCLK_ID_SERCOM5_CORE 		0x19
	#define GCLK_ID_TCC0_TCC1   		0x1A
	#define GCLK_ID_TCC2_TC3    		0x1B
	#define GCLK_ID_TC4_TC5     		0x1C
	#define GCLK_ID_TC6_TC7     		0x1D
	#define GCLK_ID_ADC         		0x1E
	#define GCLK_ID_AC_DIG      		0x1F
	#define GCLK_ID_AC_ANA      		0x20
	#define GCLK_ID_DAC         		0x21
	#define GCLK_ID_PTC         		0x22
	#define GCLK_ID_I2S_0       		0x23
	#define GCLK_ID_I2S_1       		0x24
	#define GCLK_ID_TCC3				0x25

	#define GCLK_GEN(value)				(((value)&0xF) << 8)	/**< \brief (GCLK_CLKCTRL) Generic Clock Generator */
	#define GCLK_CLKEN					(1<<14)					/**< \brief (GCLK_CLKCTRL) Clock Enable */
	#define GCLK_WRTLOCK				(1<<15)					/**< \brief (GCLK_CLKCTRL) Write Lock */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PM
	{
		RwReg8	CTRL;                /*	(*(RwReg8 *)0x40000400U) *< \brief (PM) Control */
		RwReg8 	SLEEP;               /*	(*(RwReg8 *)0x40000401U) *< \brief (PM) Sleep Mode */

		RoReg8	z_reserved1[6];

		RwReg8 	CPUSEL;              /*	(*(RwReg8 *)0x40000408U) *< \brief (PM) CPU Clock Select */
		RwReg8 	APBASEL;             /*	(*(RwReg8 *)0x40000409U) *< \brief (PM) APBA Clock Select */
		RwReg8 	APBBSEL;             /*	(*(RwReg8 *)0x4000040AU) *< \brief (PM) APBB Clock Select */
		RwReg8 	APBCSEL;             /*	(*(RwReg8 *)0x4000040BU) *< \brief (PM) APBC Clock Select */

		RoReg	z_reserved2[2];

		RwReg  	AHBMASK;             /*	(*(RwReg  *)0x40000414U) *< \brief (PM) AHB Mask */
		RwReg  	APBAMASK;            /*	(*(RwReg  *)0x40000418U) *< \brief (PM) APBA Mask */
		RwReg  	APBBMASK;            /*	(*(RwReg  *)0x4000041CU) *< \brief (PM) APBB Mask */
		RwReg  	APBCMASK;            /*	(*(RwReg  *)0x40000420U) *< \brief (PM) APBC Mask */

		RoReg	z_reserved3[4];

		RwReg8 	INTENCLR;            /*	(*(RwReg8 *)0x40000434U) *< \brief (PM) Interrupt Enable Clear */
		RwReg8 	INTENSET;            /*	(*(RwReg8 *)0x40000435U) *< \brief (PM) Interrupt Enable Set */
		RwReg8 	INTFLAG;             /*	(*(RwReg8 *)0x40000436U) *< \brief (PM) Interrupt Flag Status and Clear */

		RoReg8	z_reserved4;

		RoReg8 	RCAUSE;              /*	(*(RoReg8 *)0x40000438U) *< \brief (PM) Reset Cause */
	};

	/* ========== Instance parameters for PM peripheral ========== */
	#define PM_CTRL_MCSEL_DFLL48M       3
	#define PM_CTRL_MCSEL_GCLK          0
	#define PM_CTRL_MCSEL_OSC8M         1
	#define PM_CTRL_MCSEL_XOSC          2
	#define PM_PM_CLK_APB_NUM           2

	#define PM_APBA_PAC0        		(1<<0)        	/**< \brief (PM_APBAMASK) PAC0 APB Clock Enable */
	#define PM_APBA_PM          		(1<<1)        	/**< \brief (PM_APBAMASK) PM APB Clock Enable */
	#define PM_APBA_SYSCTRL     		(1<<2)        	/**< \brief (PM_APBAMASK) SYSCTRL APB Clock Enable */
	#define PM_APBA_GCLK        		(1<<3)        	/**< \brief (PM_APBAMASK) GCLK APB Clock Enable */
	#define PM_APBA_WDT         		(1<<4)        	/**< \brief (PM_APBAMASK) WDT APB Clock Enable */
	#define PM_APBA_RTC         		(1<<5)        	/**< \brief (PM_APBAMASK) RTC APB Clock Enable */
	#define PM_APBA_EIC         		(1<<6)        	/**< \brief (PM_APBAMASK) EIC APB Clock Enable */

	#define PM_APBB_PAC1        		(1<<0)           /**< \brief (PM_APBBMASK) PAC1 APB Clock Enable */
	#define PM_APBB_DSU         		(1<<1)           /**< \brief (PM_APBBMASK) DSU APB Clock Enable */
	#define PM_APBB_NVMCTRL     		(1<<2)           /**< \brief (PM_APBBMASK) NVMCTRL APB Clock Enable */
	#define PM_APBB_PORT        		(1<<3)           /**< \brief (PM_APBBMASK) PORT APB Clock Enable */
	#define PM_APBB_DMAC        		(1<<4)           /**< \brief (PM_APBBMASK) DMAC APB Clock Enable */
	#define PM_APBB_USB         		(1<<5)           /**< \brief (PM_APBBMASK) USB APB Clock Enable */
	#define PM_APBB_HMATRIX     		(1<<6)           /**< \brief (PM_APBBMASK) HMATRIX APB Clock Enable */

	#define PM_APBC_PAC2    			(1<<0)        	/**< \brief (PM_APBCMASK) PAC2 APB Clock Enable */
	#define PM_APBC_EVSYS   			(1<<1)        	/**< \brief (PM_APBCMASK) EVSYS APB Clock Enable */
	#define PM_APBC_SERCOM0 			(1<<2)        	/**< \brief (PM_APBCMASK) SERCOM0 APB Clock Enable */
	#define PM_APBC_SERCOM1 			(1<<3)        	/**< \brief (PM_APBCMASK) SERCOM1 APB Clock Enable */
	#define PM_APBC_SERCOM2 			(1<<4)        	/**< \brief (PM_APBCMASK) SERCOM2 APB Clock Enable */
	#define PM_APBC_SERCOM3 			(1<<5)        	/**< \brief (PM_APBCMASK) SERCOM3 APB Clock Enable */
	#define PM_APBC_SERCOM4 			(1<<6)        	/**< \brief (PM_APBCMASK) SERCOM4 APB Clock Enable */
	#define PM_APBC_SERCOM5 			(1<<7)        	/**< \brief (PM_APBCMASK) SERCOM5 APB Clock Enable */
	#define PM_APBC_TCC0    			(1<<8)        	/**< \brief (PM_APBCMASK) TCC0 APB Clock Enable */
	#define PM_APBC_TCC1    			(1<<9)        	/**< \brief (PM_APBCMASK) TCC1 APB Clock Enable */
	#define PM_APBC_TCC2    			(1<<10)       	/**< \brief (PM_APBCMASK) TCC2 APB Clock Enable */
	#define PM_APBC_TC3     			(1<<11)       	/**< \brief (PM_APBCMASK) TC3 APB Clock Enable */
	#define PM_APBC_TC4     			(1<<12)       	/**< \brief (PM_APBCMASK) TC4 APB Clock Enable */
	#define PM_APBC_TC5     			(1<<13)       	/**< \brief (PM_APBCMASK) TC5 APB Clock Enable */
	#define PM_APBC_TC6     			(1<<14)       	/**< \brief (PM_APBCMASK) TC6 APB Clock Enable */
	#define PM_APBC_TC7     			(1<<15)       	/**< \brief (PM_APBCMASK) TC7 APB Clock Enable */
	#define PM_APBC_ADC     			(1<<16)       	/**< \brief (PM_APBCMASK) ADC APB Clock Enable */
	#define PM_APBC_AC      			(1<<17)       	/**< \brief (PM_APBCMASK) AC APB Clock Enable */
	#define PM_APBC_DAC     			(1<<18)       	/**< \brief (PM_APBCMASK) DAC APB Clock Enable */
	#define PM_APBC_PTC     			(1<<19)       	/**< \brief (PM_APBCMASK) PTC APB Clock Enable */
	#define PM_APBC_I2S     			(1<<20)       	/**< \brief (PM_APBCMASK) I2S APB Clock Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SYSCTRL
	{
		struct SR_Type
		{
			u32 XOSCRDY:1;        	/*!< bit:      0  XOSC Ready                         */
			u32 XOSC32KRDY:1;     	/*!< bit:      1  XOSC32K Ready                      */
			u32 OSC32KRDY:1;      	/*!< bit:      2  OSC32K Ready                       */
			u32 OSC8MRDY:1;       	/*!< bit:      3  OSC8M Ready                        */
			u32 DFLLRDY:1;        	/*!< bit:      4  DFLL Ready                         */
			u32 DFLLOOB:1;        	/*!< bit:      5  DFLL Out Of Bounds                 */
			u32 DFLLLCKF:1;       	/*!< bit:      6  DFLL Lock Fine                     */
			u32 DFLLLCKC:1;       	/*!< bit:      7  DFLL Lock Coarse                   */
			u32 DFLLRCS:1;        	/*!< bit:      8  DFLL Reference Clock Stopped       */
			u32 BOD33RDY:1;       	/*!< bit:      9  BOD33 Ready                        */
			u32 BOD33DET:1;       	/*!< bit:     10  BOD33 Detection                    */
			u32 B33SRDY:1;        	/*!< bit:     11  BOD33 Synchronization Ready        */
			u32 :3;               	/*!< bit: 12..14  Reserved                           */
			u32 DPLLLCKR:1;       	/*!< bit:     15  DPLL Lock Rise                     */
			u32 DPLLLCKF:1;       	/*!< bit:     16  DPLL Lock Fall                     */
			u32 DPLLLTO:1;        	/*!< bit:     17  DPLL Lock Timeout                  */
			u32 :14;              	/*!< bit: 18..31  Reserved                           */
			
			operator u32() { return *((u32*)this); }
			u32 operator=(u32 v) { return *((u32*)this) = v; }
		};								/*!< Structure used for bit  access                  */
			
		volatile SR_Type  		INTENCLR;       /*	(*(RwReg  *)0x40000800U) *< \brief (SYSCTRL) Interrupt Enable Clear */
		volatile SR_Type		INTENSET;       /*	(*(RwReg  *)0x40000804U) *< \brief (SYSCTRL) Interrupt Enable Set */
		volatile SR_Type		INTFLAG;       	/*	(*(RwReg  *)0x40000808U) *< \brief (SYSCTRL) Interrupt Flag Status and Clear */
		volatile const SR_Type	PCLKSR;       	/*	(*(RoReg  *)0x4000080CU) *< \brief (SYSCTRL) Power and Clocks Status */

		RwReg16	XOSC;       	/*	(*(RwReg16*)0x40000810U) *< \brief (SYSCTRL) External Multipurpose Crystal Oscillator (XOSC) Control */

		RoReg8	z_reserved1[2];

		RwReg16	XOSC32K;       	/*	(*(RwReg16*)0x40000814U) *< \brief (SYSCTRL) 32kHz External Crystal Oscillator (XOSC32K) Control */

		RoReg8	z_reserved2[2];

		RwReg  	OSC32K;       	/*	(*(RwReg  *)0x40000818U) *< \brief (SYSCTRL) 32kHz Internal Oscillator (OSC32K) Control */
		RwReg8 	OSCULP32K; 		/*	(*(RwReg8 *)0x4000081CU) *< \brief (SYSCTRL) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */

		RoReg8	z_reserved3[3];

		RwReg  	OSC8M;			/*	(*(RwReg  *)0x40000820U) *< \brief (SYSCTRL) 8MHz Internal Oscillator (OSC8M) Control */
		RwReg16	DFLLCTRL;		/*	(*(RwReg16*)0x40000824U) *< \brief (SYSCTRL) DFLL48M Control */

		RoReg8	z_reserved4[2];

		RwReg  	DFLLVAL;		/*	(*(RwReg  *)0x40000828U) *< \brief (SYSCTRL) DFLL48M Value */
		RwReg  	DFLLMUL;		/*	(*(RwReg  *)0x4000082CU) *< \brief (SYSCTRL) DFLL48M Multiplier */
		RwReg8 	DFLLSYNC;		/*	(*(RwReg8 *)0x40000830U) *< \brief (SYSCTRL) DFLL48M Synchronization */

		RoReg8	z_reserved5[3];

		RwReg  	BOD33;			/*	(*(RwReg  *)0x40000834U) *< \brief (SYSCTRL) 3.3V Brown-Out Detector (BOD33) Control */

		RoReg	z_reserved6;

		RwReg16	VREG;			/*	(*(RwReg  *)0x4000083CU) *< \brief (SYSCTRL) Voltage Regulator System (VREG) Control */

		RoReg8	z_reserved7[2];

		RwReg  	VREF;			/*	(*(RwReg  *)0x40000840U) *< \brief (SYSCTRL) Voltage References System (VREF) Control */
		RwReg8 	DPLLCTRLA;		/*	(*(RwReg8 *)0x40000844U) *< \brief (SYSCTRL) DPLL Control A */

		RoReg8	z_reserved8[3];

		RwReg  	DPLLRATIO;		/*	(*(RwReg  *)0x40000848U) *< \brief (SYSCTRL) DPLL Ratio Control */
		RwReg  	DPLLCTRLB;		/*	(*(RwReg  *)0x4000084CU) *< \brief (SYSCTRL) DPLL Control B */
		RoReg8 	DPLLSTATUS;		/*	(*(RoReg8 *)0x40000850U) *< \brief (SYSCTRL) DPLL Status */

	};

	/* ========== Instance parameters for SYSCTRL peripheral ========== */
	#define SYSCTRL_BGAP_CALIB_MSB      	11
	#define SYSCTRL_BOD33_CALIB_MSB     	5
	#define SYSCTRL_DFLL48M_COARSE_MSB  	5
	#define SYSCTRL_DFLL48M_FINE_MSB    	9
	#define SYSCTRL_GCLK_ID_DFLL48      	0
	#define SYSCTRL_GCLK_ID_FDPLL       	1
	#define SYSCTRL_GCLK_ID_FDPLL32K    	2
	#define SYSCTRL_OSC32K_COARSE_CALIB_MSB 6
	#define SYSCTRL_POR33_ENTEST_MSB    	1
	#define SYSCTRL_ULPVREF_DIVLEV_MSB  	3
	#define SYSCTRL_ULPVREG_FORCEGAIN_MSB 	1
	#define SYSCTRL_ULPVREG_RAMREFSEL_MSB 	2
	#define SYSCTRL_VREF_CONTROL_MSB    	48
	#define SYSCTRL_VREF_STATUS_MSB     	7
	#define SYSCTRL_VREG_LEVEL_MSB      	2
	#define SYSCTRL_BOD12_VERSION       	0x111
	#define SYSCTRL_BOD33_VERSION       	0x111
	#define SYSCTRL_DFLL48M_VERSION     	0x300
	#define SYSCTRL_FDPLL_VERSION       	0x110
	#define SYSCTRL_OSCULP32K_VERSION   	0x111
	#define SYSCTRL_OSC8M_VERSION       	0x120
	#define SYSCTRL_OSC32K_VERSION      	0x110
	#define SYSCTRL_VREF_VERSION        	0x200
	#define SYSCTRL_VREG_VERSION        	0x201
	#define SYSCTRL_XOSC_VERSION        	0x111
	#define SYSCTRL_XOSC32K_VERSION     	0x111

	#define XOSC_ENABLE     		(1<<1)            		/**< \brief (SYSCTRL_XOSC) Oscillator Enable */
	#define XOSC_XTALEN     		(1<<2)            		/**< \brief (SYSCTRL_XOSC) Crystal Oscillator Enable */
	#define XOSC_RUNSTDBY   		(1<<6)            		/**< \brief (SYSCTRL_XOSC) Run in Standby */
	#define XOSC_ONDEMAND   		(1<<7)            		/**< \brief (SYSCTRL_XOSC) On Demand Control */
	#define XOSC_GAIN_0     		(0<<8)
	#define XOSC_GAIN_1     		(1<<8)
	#define XOSC_GAIN_2     		(2<<8)
	#define XOSC_GAIN_3     		(3<<8)
	#define XOSC_GAIN_4     		(4<<8)
	#define XOSC_AMPGC      		(1<<11)					/**< \brief (SYSCTRL_XOSC) Automatic Amplitude Gain Control */
	#define XOSC_STARTUP(value) 	(((value)&0xF) << 12)

	#define XOSC32K_ENABLE  		(1<<1)            		/**< \brief (SYSCTRL_XOSC32K) Oscillator Enable */
	#define XOSC32K_XTALEN  		(1<<2)            		/**< \brief (SYSCTRL_XOSC32K) Crystal Oscillator Enable */
	#define XOSC32K_EN32K   		(1<<3)            		/**< \brief (SYSCTRL_XOSC32K) 32kHz Output Enable */
	#define XOSC32K_EN1K    		(1<<4)            		/**< \brief (SYSCTRL_XOSC32K) 1kHz Output Enable */
	#define XOSC32K_AAMPEN  		(1<<5)            		/**< \brief (SYSCTRL_XOSC32K) Automatic Amplitude Control Enable */
	#define XOSC32K_RUNSTDBY 		(1<<6)            		/**< \brief (SYSCTRL_XOSC32K) Run in Standby */
	#define XOSC32K_ONDEMAND 		(1<<7)            		/**< \brief (SYSCTRL_XOSC32K) On Demand Control */
	#define XOSC32K_STARTUP(value)	(((value)&7) << 8)
	#define XOSC32K_WRTLOCK			(1<<12)					/**< \brief (SYSCTRL_XOSC32K) Write Lock */

	#define OSC32K_ENABLE   		(1<<1)            		/**< \brief (SYSCTRL_OSC32K) Oscillator Enable */
	#define OSC32K_EN32K    		(1<<2)            		/**< \brief (SYSCTRL_OSC32K) 32kHz Output Enable */
	#define OSC32K_EN1K     		(1<<3)            		/**< \brief (SYSCTRL_OSC32K) 1kHz Output Enable */
	#define OSC32K_RUNSTDBY 		(1<<6)            		/**< \brief (SYSCTRL_OSC32K) Run in Standby */
	#define OSC32K_ONDEMAND 		(1<<7)            		/**< \brief (SYSCTRL_OSC32K) On Demand Control */
	#define OSC32K_STARTUP(value)	(((value)&7) << 8)
	#define OSC32K_WRTLOCK			(1<<12)					/**< \brief (SYSCTRL_OSC32K) Write Lock */
	#define OSC32K_CALIB(value)		(((value)&0x7F) << 16)

	#define OSCULP32K_CALIB(value)	(((value)&0x1F) << 0)	/**< \brief (SYSCTRL_OSCULP32K) Oscillator Calibration */
	#define OSCULP32K_WRTLOCK		(1<<7)					/**< \brief (SYSCTRL_OSCULP32K) Write Lock */

	#define OSC8M_ENABLE_Pos    	(1<<1)        				/**< \brief (SYSCTRL_OSC8M) Oscillator Enable */
	#define OSC8M_RUNSTDBY_Pos  	(1<<6)        				/**< \brief (SYSCTRL_OSC8M) Run in Standby */
	#define OSC8M_ONDEMAND_Pos  	(1<<7)        				/**< \brief (SYSCTRL_OSC8M) On Demand Control */
	#define OSC8M_PRESC_1       	(0<<8)						/**< \brief (SYSCTRL_OSC8M) 1 */
	#define OSC8M_PRESC_2       	(1<<8)						/**< \brief (SYSCTRL_OSC8M) 2 */
	#define OSC8M_PRESC_4       	(2<<8)						/**< \brief (SYSCTRL_OSC8M) 4 */
	#define OSC8M_PRESC_8       	(3<<8)						/**< \brief (SYSCTRL_OSC8M) 8 */
	#define OSC8M_CALIB(value)  	(((value)&0xFFF)<<16)		/**< \brief (SYSCTRL_OSC8M) Oscillator Calibration */
	#define OSC8M_FRANGE_4_6  		(0<<30)						/**< \brief (SYSCTRL_OSC8M) 4 to 6MHz */
	#define OSC8M_FRANGE_6_8  		(1<<30)						/**< \brief (SYSCTRL_OSC8M) 6 to 8MHz */
	#define OSC8M_FRANGE_8_11   	(2<<30)						/**< \brief (SYSCTRL_OSC8M) 8 to 11MHz */
	#define OSC8M_FRANGE_11_15  	(3<<30)						/**< \brief (SYSCTRL_OSC8M) 11 to 15MHz */

	#define DFLL_ENABLE 			(1<<1) 	/**< \brief (SYSCTRL_DFLLCTRL) DFLL Enable */
	#define DFLL_MODE   			(1<<2) 	/**< \brief (SYSCTRL_DFLLCTRL) Operating Mode Selection */
	#define DFLL_STABLE 			(1<<3) 	/**< \brief (SYSCTRL_DFLLCTRL) Stable DFLL Frequency */
	#define DFLL_LLAW   			(1<<4) 	/**< \brief (SYSCTRL_DFLLCTRL) Lose Lock After Wake */
	#define DFLL_USBCRM 			(1<<5) 	/**< \brief (SYSCTRL_DFLLCTRL) USB Clock Recovery Mode */
	#define DFLL_RUNSTDBY 			(1<<6) 	/**< \brief (SYSCTRL_DFLLCTRL) Run in Standby */
	#define DFLL_ONDEMAND 			(1<<7) 	/**< \brief (SYSCTRL_DFLLCTRL) On Demand Control */
	#define DFLL_CCDIS  			(1<<8) 	/**< \brief (SYSCTRL_DFLLCTRL) Chill Cycle Disable */
	#define DFLL_QLDIS  			(1<<9) 	/**< \brief (SYSCTRL_DFLLCTRL) Quick Lock Disable */
	#define DFLL_BPLCKC 			(1<<10)	/**< \brief (SYSCTRL_DFLLCTRL) Bypass Coarse Lock */
	#define DFLL_WAITLOCK			(1<<11)	/**< \brief (SYSCTRL_DFLLCTRL) Wait Lock */

	#define DFLL_FINE(value)		(((value)&0x3FF)<<0)	/**< \brief (SYSCTRL_DFLLVAL) Fine Value */
	#define DFLL_COARSE(value)		(((value)&0x3F)<<10)	/**< \brief (SYSCTRL_DFLLVAL) Coarse Value */
	#define DFLL_DIFF(value)		((value)<<16)			/**< \brief (SYSCTRL_DFLLVAL) Multiplication Ratio Difference */

	#define DFLL_MUL(value)			(((value)&0xFFFF)<<0)	/**< \brief (SYSCTRL_DFLLMUL) DFLL Multiply Factor */
	#define DFLL_FSTEP(value)		(((value)&0x3FF)<<16)	/**< \brief (SYSCTRL_DFLLMUL) Fine Maximum Step */
	#define DFLL_CSTEP(value)		(((value))<<26)			/**< \brief (SYSCTRL_DFLLMUL) Coarse Maximum Step */

	#define DFLLSYNC_READREQ		(1<<7)					/**< \brief (SYSCTRL_DFLLSYNC) Read Request */

	#define BOD33_ENABLE    		(1<<1)    				/**< \brief (SYSCTRL_BOD33) Enable */
	#define BOD33_HYST      		(1<<2)    				/**< \brief (SYSCTRL_BOD33) Hysteresis */
	#define BOD33_ACTION_NONE   	(0<<3)					/**< \brief (SYSCTRL_BOD33) No action */
	#define BOD33_ACTION_RESET  	(1<<3)					/**< \brief (SYSCTRL_BOD33) The BOD33 generates a reset */
	#define BOD33_ACTION_INTERRUPT	(2<<3)					/**< \brief (SYSCTRL_BOD33) The BOD33 generates an interrupt */
	#define BOD33_RUNSTDBY  		(1<<6)    				/**< \brief (SYSCTRL_BOD33) Run in Standby */
	#define BOD33_MODE      		(1<<8)    				/**< \brief (SYSCTRL_BOD33) Operation Mode */
	#define BOD33_CEN       		(1<<9)    				/**< \brief (SYSCTRL_BOD33) Clock Enable */
	#define BOD33_DIV2     			(0x0<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 2 */
	#define BOD33_DIV4     			(0x1<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 4 */
	#define BOD33_DIV8     			(0x2<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 8 */
	#define BOD33_DIV16    			(0x3<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 16 */
	#define BOD33_DIV32    			(0x4<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 32 */
	#define BOD33_DIV64    			(0x5<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 64 */
	#define BOD33_DIV128   			(0x6<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 128 */
	#define BOD33_DIV256   			(0x7<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 256 */
	#define BOD33_DIV512   			(0x8<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 512 */
	#define BOD33_DIV1K    			(0x9<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 1024 */
	#define BOD33_DIV2K    			(0xA<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 2048 */
	#define BOD33_DIV4K    			(0xB<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 4096 */
	#define BOD33_DIV8K    			(0xC<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 8192 */
	#define BOD33_DIV16K   			(0xD<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 16384 */
	#define BOD33_DIV32K   			(0xE<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 32768 */
	#define BOD33_DIV64K   			(0xF<<12)				/**< \brief (SYSCTRL_BOD33) Divide clock by 65536 */
	#define BOD33_LEVEL(value)  	(((value)&0x3F)<<16)	/**< \brief (SYSCTRL_BOD33) BOD33 Threshold Level */

	#define VREG_RUNSTDBY   		(1<<6)    				/**< \brief (SYSCTRL_VREG) Run in Standby */
	#define VREG_FORCELDO   		(1<<13)   				/**< \brief (SYSCTRL_VREG) Force LDO Voltage Regulator */

	#define VREF_TSEN       		(1<<1)          		/**< \brief (SYSCTRL_VREF) Temperature Sensor Enable */
	#define VREF_BGOUTEN    		(1<<2)          		/**< \brief (SYSCTRL_VREF) Bandgap Output Enable */
	#define VREF_CALIB(value)   	(((value)&0x7FF)<<16)	/**< \brief (SYSCTRL_VREF) Bandgap Voltage Generator Calibration */

	#define DPLL_ENABLE				(1<<1)    				/**< \brief (SYSCTRL_DPLLCTRLA) DPLL Enable */
	#define DPLL_RUNSTDBY			(1<<6)    				/**< \brief (SYSCTRL_DPLLCTRLA) Run in Standby */
	#define DPLL_ONDEMAND			(1<<7)    				/**< \brief (SYSCTRL_DPLLCTRLA) On Demand Clock Activation */

	#define DPLL_LDR(value)			(((value)&0xFFF)<<0)	/**< \brief (SYSCTRL_DPLLRATIO) Loop Divider Ratio */
	#define DPLL_LDRFRAC(value)		(((value)&0xF)<<16)		/**< \brief (SYSCTRL_DPLLRATIO) Loop Divider Ratio Fractional Part */


	#define DPLL_FILTER_DEFAULT		(0<<0)					/**< \brief (SYSCTRL_DPLLCTRLB) Default filter mode */
	#define DPLL_FILTER_LBFILT 		(1<<0)					/**< \brief (SYSCTRL_DPLLCTRLB) Low bandwidth filter */
	#define DPLL_FILTER_HBFILT 		(2<<0)					/**< \brief (SYSCTRL_DPLLCTRLB) High bandwidth filter */
	#define DPLL_FILTER_HDFILT 		(3<<0)					/**< \brief (SYSCTRL_DPLLCTRLB) High damping filter */
	#define DPLL_LPEN  				(1<<2)    				/**< \brief (SYSCTRL_DPLLCTRLB) Low-Power Enable */
	#define DPLL_WUF   				(1<<3)    				/**< \brief (SYSCTRL_DPLLCTRLB) Wake Up Fast */
	#define DPLL_REFCLK_XOSC32 		(0<<4)					/**< \brief (SYSCTRL_DPLLCTRLB) CLK_DPLL_REF0 clock reference */
	#define DPLL_REFCLK_XOSC 		(1<<4)					/**< \brief (SYSCTRL_DPLLCTRLB) CLK_DPLL_REF1 clock reference */
	#define DPLL_REFCLK_GCLK 		(2<<4)					/**< \brief (SYSCTRL_DPLLCTRLB) GCLK_DPLL clock reference */
	#define DPLL_LTIME_DEFAULT 		(0<<8)					/**< \brief (SYSCTRL_DPLLCTRLB) No time-out */
	#define DPLL_LTIME_8MS			(4<<8)					/**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 8 ms */
	#define DPLL_LTIME_9MS			(5<<8)					/**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 9 ms */
	#define DPLL_LTIME_10MS			(6<<8)					/**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 10 ms */
	#define DPLL_LTIME_11MS			(7<<8)					/**< \brief (SYSCTRL_DPLLCTRLB) Time-out if no lock within 11 ms */
	#define DPLL_LBYPASS			(1<<12)     			/**< \brief (SYSCTRL_DPLLCTRLB) Lock Bypass */
	#define DPLL_DIV(value)			(((value)&0x7FF)<<16)	/**< \brief (SYSCTRL_DPLLCTRLB) Clock Divider */

	#define DPLLSTATUS_LOCK			(1<<0)  /**< \brief (SYSCTRL_DPLLSTATUS) DPLL Lock Status */
	#define DPLLSTATUS_CLKRDY		(1<<1)  /**< \brief (SYSCTRL_DPLLSTATUS) Output Clock Ready */
	#define DPLLSTATUS_ENABLE		(1<<2)  /**< \brief (SYSCTRL_DPLLSTATUS) DPLL Enable */
	#define DPLLSTATUS_DIV			(1<<3)  /**< \brief (SYSCTRL_DPLLSTATUS) Divider Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_WDT
	{
		RwReg8	CTRL     ;          /*	(*(RwReg8 *)0x40001000U) *< \brief (WDT) Control */
		RwReg8	CONFIG   ;          /*	(*(RwReg8 *)0x40001001U) *< \brief (WDT) Configuration */
		RwReg8	EWCTRL   ;          /*	(*(RwReg8 *)0x40001002U) *< \brief (WDT) Early Warning Interrupt Control */

		RoReg8	z_reserved;

		RwReg8	INTENCLR ;          /*	(*(RwReg8 *)0x40001004U) *< \brief (WDT) Interrupt Enable Clear */
		RwReg8	INTENSET ;          /*	(*(RwReg8 *)0x40001005U) *< \brief (WDT) Interrupt Enable Set */
		RwReg8	INTFLAG  ;          /*	(*(RwReg8 *)0x40001006U) *< \brief (WDT) Interrupt Flag Status and Clear */
		RoReg8	STATUS   ;          /*	(*(RoReg8 *)0x40001007U) *< \brief (WDT) Status */
		WoReg8	CLEAR    ;          /*	(*(WoReg8 *)0x40001008U) *< \brief (WDT) Clear */

		void Reset() { CLEAR = 0xA5; }
	};

	/* ========== Instance parameters for WDT peripheral ========== */
	#define WDT_GCLK_ID                 3

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_PORT
	{
		RW32	DIR;         		/**< \brief Offset: 0x00 (R/W 32) Data Direction */
		RW32	DIRCLR;      		/**< \brief Offset: 0x04 (R/W 32) Data Direction Clear */
		RW32    DIRSET;      		/**< \brief Offset: 0x08 (R/W 32) Data Direction Set */
		RW32    DIRTGL;      		/**< \brief Offset: 0x0C (R/W 32) Data Direction Toggle */
		RW32    OUT;         		/**< \brief Offset: 0x10 (R/W 32) Data Output Value */
		RW32    OUTCLR;      		/**< \brief Offset: 0x14 (R/W 32) Data Output Value Clear */
		RW32    OUTSET;      		/**< \brief Offset: 0x18 (R/W 32) Data Output Value Set */
		RW32    OUTTGL;      		/**< \brief Offset: 0x1C (R/W 32) Data Output Value Toggle */
		RO32    IN;          		/**< \brief Offset: 0x20 (R/  32) Data Input Value */
		RW32    CTRL;        		/**< \brief Offset: 0x24 (R/W 32) Control */
		RW32    WRCONFIG;    		/**< \brief Offset: 0x28 ( /W 32) Write Configuration */

		RO8		z__Reserved1[0x4];

		RW8     PMUX[16];    		/**< \brief Offset: 0x30 (R/W  8) Peripheral Multiplexing n */
		RW8     PINCFG[32];  		/**< \brief Offset: 0x40 (R/W  8) Pin Configuration n */

		RO8		z__Reserved2[0x20];

		void 	SET(u32 m) 			{ OUTSET = m; }
		void 	CLR(u32 m) 			{ OUTCLR = m; }
		void 	NOT(u32 m) 			{ OUTTGL = m; }
		void 	WBIT(u32 m, bool c) { if (c) SET(m); else CLR(m); }
		void 	BSET(u16 b) 		{ OUTSET = 1 << b; }
		void 	BCLR(u16 b) 		{ OUTCLR = 1 << b; }
		void 	BTGL(u16 b) 		{ OUTTGL = 1 << b; }

		bool 	TBSET(u16 b) 		{ return IN & (1<<b); }
		bool 	TBCLR(u16 b) 		{ return (IN & (1<<b)) == 0; }

		void 	SetPinMux(byte pinnum, byte value) { byte sh = (pinnum&1)<<2; pinnum >>= 1; PMUX[pinnum] = (PMUX[pinnum] & ~(0xF<<sh)) | (value<<sh); };
		
		void 	SetCfgMux(byte cfg, byte mux, u32 mask) 
		{ 
			u32 t = (((u32)(0x50 | (mux&0xF))<<8) | cfg) << 16; 
			WRCONFIG = t | (mask & 0xFFFF); 
			WRCONFIG = t | (mask >> 16) | (1uL<<31); };
	};										
											
	typedef S_PORT S_PORTA, S_PORTB, S_PIOA, S_PIOB;
											
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_NVMCTRL
	{
		RW16    CTRLA;			/**< \brief Offset: 0x00 (R/W 16) Control A */

		RO8     z__Reserved1[0x2];

		RW32    CTRLB;       	/**< \brief Offset: 0x04 (R/W 32) Control B */
		RW32    PARAM;       	/**< \brief Offset: 0x08 (R/W 32) NVM Parameter */
		RW8		INTENCLR;    	/**< \brief Offset: 0x0C (R/W  8) Interrupt Enable Clear */

		RO8     z__Reserved2[0x3];

		RW8		INTENSET;		/**< \brief Offset: 0x10 (R/W  8) Interrupt Enable Set */

		RO8     z__Reserved3[0x3];

		RW8		INTFLAG;		/**< \brief Offset: 0x14 (R/W  8) Interrupt Flag Status and Clear */
		
		RO8     z__Reserved4[0x3];

		RW16	STATUS;			/**< \brief Offset: 0x18 (R/W 16) Status */

		RO8     z__Reserved5[0x2];

		RW32    ADDR;        	/**< \brief Offset: 0x1C (R/W 32) Address */
		RW16	LOCK;        	/**< \brief Offset: 0x20 (R/W 16) Lock Section */
	};

	#define NVMCTRL_RWS(value)  				(((value)&0xF) << 1)	/**< \brief (NVMCTRL_CTRLB) NVM Read Wait States */
	#define NVMCTRL_RWS_SINGLE  				(0 << 1)
	#define NVMCTRL_RWS_HALF    				(1 << 1)
	#define NVMCTRL_RWS_DUAL    				(2 << 1)
	#define NVMCTRL_MANW						(1<<7)					/**< \brief (NVMCTRL_CTRLB) Manual Write */
	#define NVMCTRL_SLEEPPRM(value)				(((value)&3)<<8)        /**< \brief (NVMCTRL_CTRLB) Power Reduction Mode during Sleep */
	#define NVMCTRL_SLEEPPRM_WAKEONACCESS		(0 << 8)
	#define NVMCTRL_SLEEPPRM_WAKEUPINSTANT		(1 << 8)
	#define NVMCTRL_SLEEPPRM_DISABLED			(3 << 8)
	#define NVMCTRL_READMODE_Pos(value)			(((value)&3) << 16)     /**< \brief (NVMCTRL_CTRLB) NVMCTRL Read Mode */
	#define NVMCTRL_READMODE_NO_MISS_PENALTY	(0 << 16)
	#define NVMCTRL_READMODE_LOW_POWER			(1 << 16)
	#define NVMCTRL_READMODE_DETERMINISTIC		(2 << 16)
	#define NVMCTRL_CACHEDIS					(1<<18)					/**< \brief (NVMCTRL_CTRLB) Cache Disable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_RTC
	{
		RW16	CTRL;        /**< \brief Offset: 0x00 (R/W 16) MODE0 Control */
		RW16	READREQ;     /**< \brief Offset: 0x02 (R/W 16) Read Request */
		RW16	EVCTRL;      /**< \brief Offset: 0x04 (R/W 16) MODE0 Event Control */
		RW8		INTENCLR;    /**< \brief Offset: 0x06 (R/W  8) MODE0 Interrupt Enable Clear */
		RW8		INTENSET;    /**< \brief Offset: 0x07 (R/W  8) MODE0 Interrupt Enable Set */
		RW8		INTFLAG;     /**< \brief Offset: 0x08 (R/W  8) MODE0 Interrupt Flag Status and Clear */

		RO8     z__Reserved1[0x1];

		RW8		STATUS;      /**< \brief Offset: 0x0A (R/W  8) Status */
		RW8		DBGCTRL;     /**< \brief Offset: 0x0B (R/W  8) Debug Control */
		RW8		FREQCORR;    /**< \brief Offset: 0x0C (R/W  8) Frequency Correction */

		RO8     z__Reserved2[0x3];

		RW32    COUNT;				/**< \brief Offset: 0x10 (R/W 32) MODE0 Counter Value */

		RO8     z__Reserved3[0x4];

		RW32    COMP[1];			/**< \brief Offset: 0x18 (R/W 32) MODE0 Compare n Value */
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_I2C
	{
		RW32    CTRLA;       		/**< \brief Offset: 0x00 (R/W 32) I2CM Control A */
		RW32    CTRLB;       		/**< \brief Offset: 0x04 (R/W 32) I2CM Control B */
		RO8     z__Reserved1[0x4];
		RW32    BAUD;				/**< \brief Offset: 0x0C (R/W 32) I2CM Baud Rate */
		RO8     z__Reserved2[0x4];
		RW8		INTENCLR;			/**< \brief Offset: 0x14 (R/W  8) I2CM Interrupt Enable Clear */
		RO8     z__Reserved3[0x1];
		RW8		INTENSET;			/**< \brief Offset: 0x16 (R/W  8) I2CM Interrupt Enable Set */
		RO8     z__Reserved4[0x1];
		RW8		INTFLAG;			/**< \brief Offset: 0x18 (R/W  8) I2CM Interrupt Flag Status and Clear */
		RO8     z__Reserved5[0x1];

		struct
		{
			RW16 BUSERR		:1;     /*!< bit:      0  Bus Error                          */
			RW16 ARBLOST	:1;     /*!< bit:      1  Arbitration Lost                   */
			RW16 RXNACK		:1;     /*!< bit:      2  Received Not Acknowledge           */
			RW16			:1;     /*!< bit:      3  Reserved                           */
			RW16 BUSSTATE	:2;     /*!< bit:  4.. 5  Bus State                          */
			RW16 LOWTOUT	:1;     /*!< bit:      6  SCL Low Timeout                    */
			RW16 CLKHOLD	:1;     /*!< bit:      7  Clock Hold                         */
			RW16 MEXTTOUT	:1;     /*!< bit:      8  Master SCL Low Extend Timeout      */
			RW16 SEXTTOUT	:1;     /*!< bit:      9  Slave SCL Low Extend Timeout       */
			RW16 LENERR		:1;     /*!< bit:     10  Length Error                       */
			//u16				:5;     /*!< bit: 11..15  Reserved                           */

			operator u16() { return *((u16*)this); }
			u16 operator=(u16 v) { return *((u16*)this) = v; }

			u16 Bits() { return *((u16*)this); } 

		} STATUS;	//RW16	STATUS;	/**< \brief Offset: 0x1A (R/W 16) I2CM Status */

		RO32	SYNCBUSY;    		/**< \brief Offset: 0x1C (R/  32) I2CM Syncbusy */
		RO8		z__Reserved6[0x4];
		RW32    ADDR;        		/**< \brief Offset: 0x24 (R/W 32) I2CM Address */
		RW8     DATA;        		/**< \brief Offset: 0x28 (R/W  8) I2CM Data */
		RO8     z__Reserved7[0x7];
		RW8		DBGCTRL;			/**< \brief Offset: 0x30 (R/W  8) I2CM Debug Control */
	};

	typedef S_I2C S_I2C0, S_I2C1, S_I2C2, S_I2C3, S_I2C4, S_I2C5;

	#define BUSSTATE_UNKNOWN	0
	#define BUSSTATE_IDLE		1
	#define BUSSTATE_OWNER		2
	#define BUSSTATE_BUSY		3

	#define	SERCOM_MODE_USART_EXT_CLK 	(0x0<<2)   /**< \brief USART mode with external clock */
	#define	SERCOM_MODE_USART_INT_CLK 	(0x1<<2)   /**< \brief USART mode with internal clock */
	#define	SERCOM_MODE_SPI_SLAVE		(0x2<<2)   /**< \brief SPI mode with external clock */
	#define	SERCOM_MODE_SPI_MASTER		(0x3<<2)   /**< \brief SPI mode with internal clock */
	#define	SERCOM_MODE_I2C_SLAVE		(0x4<<2)   /**< \brief I2C mode with external clock */
	#define	SERCOM_MODE_I2C_MASTER		(0x5<<2)   /**< \brief I2C mode with internal clock */

	#define	I2C_SWRST			(1<<0)            /**< \brief (SERCOM_I2CS_CTRLA) Software Reset */
	#define	I2C_ENABLE			(1<<1)            /**< \brief (SERCOM_I2CS_CTRLA) Enable */
	#define	I2C_RUNSTDBY		(1<<7)            /**< \brief (SERCOM_I2CM_CTRLA) Run in Standby */
	#define	I2C_PINOUT			(1<<16)           /**< \brief (SERCOM_I2CM_CTRLA) Pin Usage */
	#define	I2C_SDAHOLD_DIS		(0<<20)           /**< \brief (SERCOM_I2CM_CTRLA) SDA Hold Time */
	#define	I2C_SDAHOLD_75NS	(1<<20)           /**< \brief (SERCOM_I2CM_CTRLA) SDA Hold Time */
	#define	I2C_SDAHOLD_450NS	(2<<20)           /**< \brief (SERCOM_I2CM_CTRLA) SDA Hold Time */
	#define	I2C_SDAHOLD_600NS	(3<<20)           /**< \brief (SERCOM_I2CM_CTRLA) SDA Hold Time */
	#define	I2C_MEXTTOEN		(1<<22)           /**< \brief (SERCOM_I2CM_CTRLA) Master SCL Low Extend Timeout */
	#define	I2C_SEXTTOEN		(1<<23)           /**< \brief (SERCOM_I2CM_CTRLA) Slave SCL Low Extend Timeout */
	#define	I2C_SPEED_SM		(0<<24)           /**< \brief (SERCOM_I2CM_CTRLA) Transfer Speed */
	#define	I2C_SPEED_FM		(1<<24)           /**< \brief (SERCOM_I2CM_CTRLA) Transfer Speed */
	#define	I2C_SPEED_HSM		(2<<24)           /**< \brief (SERCOM_I2CM_CTRLA) Transfer Speed */
	#define	I2C_SCLSM			(1<<27)           /**< \brief (SERCOM_I2CM_CTRLA) SCL Clock Stretch Mode */
	#define	I2C_INACTOUT_DIS	(0<<28)           /**< \brief (SERCOM_I2CM_CTRLA) Inactive Time-Out */
	#define	I2C_INACTOUT_55US	(1<<28)           /**< \brief (SERCOM_I2CM_CTRLA) Inactive Time-Out */
	#define	I2C_INACTOUT_105US	(2<<28)           /**< \brief (SERCOM_I2CM_CTRLA) Inactive Time-Out */
	#define	I2C_INACTOUT_205US	(3<<28)           /**< \brief (SERCOM_I2CM_CTRLA) Inactive Time-Out */
	#define	I2C_LOWTOUTEN		(1<<30)           /**< \brief (SERCOM_I2CM_CTRLA) SCL Low Timeout Enable */

	#define I2C_MB		(1<<0)					/**< \brief (SERCOM_I2CM_INTFLAG) Master On Bus Interrupt */
	#define I2C_SB		(1<<1)					/**< \brief (SERCOM_I2CM_INTFLAG) Slave On Bus Interrupt */
	#define I2C_ERROR	(1<<7)					/**< \brief (SERCOM_I2CM_INTFLAG) Combined Error Interrupt */

	#define I2C_SMEN		(1<<8)            	/**< \brief (SERCOM_I2CM_CTRLB) Smart Mode Enable */
	#define I2C_QCEN		(1<<9)            	/**< \brief (SERCOM_I2CM_CTRLB) Quick Command Enable */
	#define I2C_CMD(value)	(((value)&3)<<16)	/**< \brief (SERCOM_I2CM_CTRLB) Command */
	#define I2C_CMD_1		(1<<16)           	/**< \brief (SERCOM_I2CM_CTRLB) Command */
	#define I2C_CMD_2		(2<<16)           	/**< \brief (SERCOM_I2CM_CTRLB) Command */
	#define I2C_CMD_STOP	(3<<16)           	/**< \brief (SERCOM_I2CM_CTRLB) Command */
	#define I2C_ACKACT		(1<<18)           	/**< \brief (SERCOM_I2CM_CTRLB) Acknowledge Action */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	struct S_SPI
	{
		RW32    CTRLA;       		/**< \brief Offset: 0x00 (R/W 32) SPI Control A */
		RW32    CTRLB;       		/**< \brief Offset: 0x04 (R/W 32) SPI Control B */
		RO8     z__Reserved1[0x4];	
		RW8		BAUD;				/**< \brief Offset: 0x0C (R/W  8) SPI Baud Rate */
		RO8     z__Reserved2[0x7];	
		RW8		INTENCLR;			/**< \brief Offset: 0x14 (R/W  8) SPI Interrupt Enable Clear */
		RO8     z__Reserved3[0x1];	
		RW8		INTENSET;			/**< \brief Offset: 0x16 (R/W  8) SPI Interrupt Enable Set */
		RO8     z__Reserved4[0x1];	
		RW8		INTFLAG;			/**< \brief Offset: 0x18 (R/W  8) SPI Interrupt Flag Status and Clear */
		RO8     z__Reserved5[0x1];	
		RW16	STATUS;      		/**< \brief Offset: 0x1A (R/W 16) SPI Status */
		RO32	SYNCBUSY;    		/**< \brief Offset: 0x1C (R/  32) SPI Syncbusy */
		RO8		z__Reserved6[0x4];	
		RW32    ADDR;        		/**< \brief Offset: 0x24 (R/W 32) SPI Address */
		RW32    DATA;        		/**< \brief Offset: 0x28 (R/W 32) SPI Data */
		RO8     z__Reserved7[0x4];	
		RW8		DBGCTRL;			/**< \brief Offset: 0x30 (R/W  8) SPI Debug Control */
	};

	typedef S_SPI S_SPI0, S_SPI1, S_SPI2, S_SPI3, S_SPI4, S_SPI5;

	#define SPI_SWRST  			(1<<0)            /**< \brief (SERCOM_SPI_CTRLA) Software Reset */
	#define SPI_ENABLE 			(1<<1)            /**< \brief (SERCOM_SPI_CTRLA) Enable */
	#define SPI_RUNSTDBY		(1<<7)            /**< \brief (SERCOM_SPI_CTRLA) Run during Standby */
	#define SPI_IBON			(1<<8)            /**< \brief (SERCOM_SPI_CTRLA) Immediate Buffer Overflow Notification */

	#define SPI_DOPO(value) 	((value)&3 << 16)
	#define SPI_DIPO(value) 	((value)&3 << 20)

	#define SPI_FORM_SPI		(0<<24)
	#define SPI_FORM_SPI_ADDR	(2<<24)
	#define SPI_CPHA   			(1<<28)   	/**< \brief (SERCOM_SPI_CTRLA) Clock Phase */
	#define SPI_CPOL   			(1<<29)   	/**< \brief (SERCOM_SPI_CTRLA) Clock Polarity */
	#define SPI_DORD   			(1<<30)   	/**< \brief (SERCOM_SPI_CTRLA) Data Order */

	#define SPI_CHSIZE_8BIT		(0<<0)		/**< \brief (SERCOM_SPI_CTRLB) Character Size */
	#define SPI_CHSIZE_9BIT		(1<<0)
	#define SPI_PLOADEN			(1<<6)    	/**< \brief (SERCOM_SPI_CTRLB) Data Preload Enable */
	#define SPI_SSDE			(1<<9)    	/**< \brief (SERCOM_SPI_CTRLB) Slave Select Low Detect Enable */
	#define SPI_MSSEN			(1<<13)   	/**< \brief (SERCOM_SPI_CTRLB) Master Slave Select Enable */
	#define SPI_AMODE_MASK		(0<<14)     /**< \brief (SERCOM_SPI_CTRLB) Address Mode */
	#define SPI_AMODE_2_ADDR	(1<<14)
	#define SPI_AMODE_RANGE		(2<<14)
	#define SPI_RXEN			(1<<17)		/**< \brief (SERCOM_SPI_CTRLB) Receiver Enable */
	#define SPI_FIFOCLR_NONE	(0<<22)
	#define SPI_FIFOCLR_TXFIFO	(1<<22)
	#define SPI_FIFOCLR_RXFIFO	(2<<22)
	#define SPI_FIFOCLR_BOTH	(3<<22)

	#define SPI_DRE 			(1<<0)		/**< \brief (SERCOM_SPI_INTENCLR) Data Register Empty Interrupt Disable */
	#define SPI_TXC 			(1<<1)		/**< \brief (SERCOM_SPI_INTENCLR) Transmit Complete Interrupt Disable */
	#define SPI_RXC 			(1<<2)		/**< \brief (SERCOM_SPI_INTENCLR) Receive Complete Interrupt Disable */
	#define SPI_SSL 			(1<<3)		/**< \brief (SERCOM_SPI_INTENCLR) Slave Select Low Interrupt Disable */
	#define SPI_ERROR			(1<<7)		/**< \brief (SERCOM_SPI_INTENCLR) Combined Error Interrupt Disable */
	#define SPI_BUFOVF			(1<<2)		/**< \brief (SERCOM_SPI_STATUS) Buffer Overflow */
	#define SPI_CTRLB			(1<<2)		/**< \brief (SERCOM_SPI_SYNCBUSY) CTRLB Synchronization Busy */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	struct S_USART
	{
		RW32   	CTRLA;       			/**< \brief Offset: 0x00 (R/W 32) USART Control A */
		RW32   	CTRLB;       			/**< \brief Offset: 0x04 (R/W 32) USART Control B */
		RO8     z__Reserved1[0x4];
		RW16    BAUD;        			/**< \brief Offset: 0x0C (R/W 16) USART Baud Rate */
		RW8		RXPL;        			/**< \brief Offset: 0x0E (R/W  8) USART Receive Pulse Length */
		RO8     z__Reserved2[0x5];
		RW8		INTENCLR;				/**< \brief Offset: 0x14 (R/W  8) USART Interrupt Enable Clear */
		RO8     z__Reserved3[0x1];
		RW8		INTENSET;				/**< \brief Offset: 0x16 (R/W  8) USART Interrupt Enable Set */
		RO8     z__Reserved4[0x1];
		RW8		INTFLAG;				/**< \brief Offset: 0x18 (R/W  8) USART Interrupt Flag Status and Clear */
		RO8     z__Reserved5[0x1];
		RW16	STATUS;      			/**< \brief Offset: 0x1A (R/W 16) USART Status */
		RO32	SYNCBUSY;    			/**< \brief Offset: 0x1C (R/  32) USART Syncbusy */
		RO8     z__Reserved6[0x8];
		RW16	DATA;					/**< \brief Offset: 0x28 (R/W 16) USART Data */
		RO8     z__Reserved7[0x6];
		RW8		DBGCTRL;				/**< \brief Offset: 0x30 (R/W  8) USART Debug Control */
	};

	typedef S_USART S_USART0, S_USART1, S_USART2, S_USART3, S_USART4, S_USART5;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_TCC
	{
		RW32        CTRLA;       		/**< \brief Offset: 0x00 (R/W 32) Control A */
		RW8         CTRLBCLR;    		/**< \brief Offset: 0x04 (R/W  8) Control B Clear */
		RW8         CTRLBSET;    		/**< \brief Offset: 0x05 (R/W  8) Control B Set */
		RO8         z__Reserved1[0x2];
		RO32        SYNCBUSY;    		/**< \brief Offset: 0x08 (R/  32) Synchronization Busy */
		RW32       	FCTRLA;      		/**< \brief Offset: 0x0C (R/W 32) Recoverable Fault A Configuration */
		RW32       	FCTRLB;      		/**< \brief Offset: 0x10 (R/W 32) Recoverable Fault B Configuration */
		RW32        WEXCTRL;     		/**< \brief Offset: 0x14 (R/W 32) Waveform Extension Configuration */
		RW32        DRVCTRL;     		/**< \brief Offset: 0x18 (R/W 32) Driver Control */
		RO8         z__Reserved2[0x2];
		RW8         DBGCTRL;			/**< \brief Offset: 0x1E (R/W  8) Debug Control */
		RO8         z__Reserved3[0x1];
		RW32        EVCTRL;      		/**< \brief Offset: 0x20 (R/W 32) Event Control */
		RW32      	INTENCLR;    		/**< \brief Offset: 0x24 (R/W 32) Interrupt Enable Clear */
		RW32      	INTENSET;    		/**< \brief Offset: 0x28 (R/W 32) Interrupt Enable Set */
		RW32		INTFLAG;     		/**< \brief Offset: 0x2C (R/W 32) Interrupt Flag Status and Clear */
		RW32        STATUS;      		/**< \brief Offset: 0x30 (R/W 32) Status */
		RW32        COUNT;       		/**< \brief Offset: 0x34 (R/W 32) Count */
		RW16        PATT;        		/**< \brief Offset: 0x38 (R/W 16) Pattern */
		RO8         z__Reserved4[0x2];
		RW32        WAVE;        		/**< \brief Offset: 0x3C (R/W 32) Waveform Control */
		RW32        PER;         		/**< \brief Offset: 0x40 (R/W 32) Period */
		RW32        CC[4];       		/**< \brief Offset: 0x44 (R/W 32) Compare and Capture */
		RO8         z__Reserved5[0x10];	
		RW16        PATTB;				/**< \brief Offset: 0x64 (R/W 16) Pattern Buffer */
		RO8         z__Reserved6[0x2];
		RW32        WAVEB;       		/**< \brief Offset: 0x68 (R/W 32) Waveform Control Buffer */
		RW32		PERB;        		/**< \brief Offset: 0x6C (R/W 32) Period Buffer */
		RW32        CCB[4];      		/**< \brief Offset: 0x70 (R/W 32) Compare and Capture Buffer */
	};

	typedef S_TCC S_TCC0, S_TCC1, S_TCC2;

	#define TCC_SWRST				(1<<0)            /**< \brief (TCC_CTRLA) Software Reset */
	#define TCC_ENABLE				(1<<1)            /**< \brief (TCC_CTRLA) Enable */
	#define TCC_RESOLUTION_NONE   	(0 << 5)
	#define TCC_RESOLUTION_DITH4  	(1 << 5)
	#define TCC_RESOLUTION_DITH5  	(2 << 5)
	#define TCC_RESOLUTION_DITH6  	(3 << 5)
	#define TCC_PRESCALER_DIV1    	(0 << 8)
	#define TCC_PRESCALER_DIV2    	(1 << 8)
	#define TCC_PRESCALER_DIV4    	(2 << 8)
	#define TCC_PRESCALER_DIV8    	(3 << 8)
	#define TCC_PRESCALER_DIV16   	(4 << 8)
	#define TCC_PRESCALER_DIV64   	(5 << 8)
	#define TCC_PRESCALER_DIV256  	(6 << 8)
	#define TCC_PRESCALER_DIV1024 	(7 << 8)
	#define TCC_RUNSTDBY_Pos      	(1 << 11)           /**< \brief (TCC_CTRLA) Run in Standby */
	#define TCC_PRESCSYNC_GCLK    	(0 << 12)
	#define TCC_PRESCSYNC_PRESC   	(1 << 12)
	#define TCC_PRESCSYNC_RESYNC  	(2 << 12)
	#define TCC_ALOCK         		(1<<14)           /**< \brief (TCC_CTRLA) Auto Lock */
	#define TCC_CPTEN0        		(1<<24)           /**< \brief (TCC_CTRLA) Capture Channel 0 Enable */
	#define TCC_CPTEN1        		(1<<25)           /**< \brief (TCC_CTRLA) Capture Channel 1 Enable */
	#define TCC_CPTEN2        		(1<<26)           /**< \brief (TCC_CTRLA) Capture Channel 2 Enable */
	#define TCC_CPTEN3        		(1<<27)           /**< \brief (TCC_CTRLA) Capture Channel 3 Enable */
		
	#define TCC_DIR        		(1<<0)            /**< \brief (TCC_CTRLBCLR) Counter Direction */
	#define TCC_LUPD       		(1<<1)            /**< \brief (TCC_CTRLBCLR) Lock Update */
	#define TCC_ONESHOT    		(1<<2)            /**< \brief (TCC_CTRLBCLR) One-Shot */
	#define TCC_IDXCMD_DISABLE 		(0<<3)
	#define TCC_IDXCMD_SET     		(1<<3)
	#define TCC_IDXCMD_CLEAR   		(2<<3)
	#define TCC_IDXCMD_HOLD    		(3<<3)
	#define TCC_CMD_NONE       		(0<<5)
	#define TCC_CMD_RETRIGGER  		(1<<5)
	#define TCC_CMD_STOP       		(2<<5)
	#define TCC_CMD_UPDATE     		(3<<5)
	#define TCC_CMD_READSYNC   		(4<<5)

	#define TCC_OVF        (1<<0)            /**< \brief (TCC_INTENCLR) Overflow Interrupt Enable */
	#define TCC_TRG        (1<<1)            /**< \brief (TCC_INTENCLR) Retrigger Interrupt Enable */
	#define TCC_CNT        (1<<2)            /**< \brief (TCC_INTENCLR) Counter Interrupt Enable */
	#define TCC_ERR        (1<<3)            /**< \brief (TCC_INTENCLR) Error Interrupt Enable */
	#define TCC_UFS        (1<<10)           /**< \brief (TCC_INTENCLR) Non-Recoverable Update Fault Interrupt Enable */
	#define TCC_DFS        (1<<11)           /**< \brief (TCC_INTENCLR) Non-Recoverable Debug Fault Interrupt Enable */
	#define TCC_FAULTA     (1<<12)           /**< \brief (TCC_INTENCLR) Recoverable Fault A Interrupt Enable */
	#define TCC_FAULTB     (1<<13)           /**< \brief (TCC_INTENCLR) Recoverable Fault B Interrupt Enable */
	#define TCC_FAULT0     (1<<14)           /**< \brief (TCC_INTENCLR) Non-Recoverable Fault 0 Interrupt Enable */
	#define TCC_FAULT1     (1<<15)           /**< \brief (TCC_INTENCLR) Non-Recoverable Fault 1 Interrupt Enable */
	#define TCC_MC0        (1<<16)           /**< \brief (TCC_INTENCLR) Match or Capture Channel 0 Interrupt Enable */
	#define TCC_MC1        (1<<17)           /**< \brief (TCC_INTENCLR) Match or Capture Channel 1 Interrupt Enable */
	#define TCC_MC2        (1<<18)           /**< \brief (TCC_INTENCLR) Match or Capture Channel 2 Interrupt Enable */
	#define TCC_MC3        (1<<19)           /**< \brief (TCC_INTENCLR) Match or Capture Channel 3 Interrupt Enable */

	#define TCC_EVACT0_OFF       	(0x0)		/**< \brief (TCC_EVCTRL) Event action disabled */
	#define TCC_EVACT0_RETRIGGER 	(0x1)		/**< \brief (TCC_EVCTRL) Start, restart or re-trigger counter on event */
	#define TCC_EVACT0_COUNTEV   	(0x2)		/**< \brief (TCC_EVCTRL) Count on event */
	#define TCC_EVACT0_START     	(0x3)		/**< \brief (TCC_EVCTRL) Start counter on event */
	#define TCC_EVACT0_INC       	(0x4)		/**< \brief (TCC_EVCTRL) Increment counter on event */
	#define TCC_EVACT0_COUNT     	(0x5)		/**< \brief (TCC_EVCTRL) Count on active state of asynchronous event */
	#define TCC_EVACT0_FAULT     	(0x7)		/**< \brief (TCC_EVCTRL) Non-recoverable fault */
	#define TCC_EVACT1_OFF       	(0x0<<3)	/**< \brief (TCC_EVCTRL) Event action disabled */
	#define TCC_EVACT1_RETRIGGER 	(0x1<<3)	/**< \brief (TCC_EVCTRL) Re-trigger counter on event */
	#define TCC_EVACT1_DIR       	(0x2<<3)	/**< \brief (TCC_EVCTRL) Direction control */
	#define TCC_EVACT1_STOP      	(0x3<<3)	/**< \brief (TCC_EVCTRL) Stop counter on event */
	#define TCC_EVACT1_DEC       	(0x4<<3)	/**< \brief (TCC_EVCTRL) Decrement counter on event */
	#define TCC_EVACT1_PPW       	(0x5<<3)	/**< \brief (TCC_EVCTRL) Period capture value in CC0 register, pulse width capture value in CC1 register */
	#define TCC_EVACT1_PWP       	(0x6<<3)	/**< \brief (TCC_EVCTRL) Period capture value in CC1 register, pulse width capture value in CC0 register */
	#define TCC_EVACT1_FAULT     	(0x7<<3)	/**< \brief (TCC_EVCTRL) Non-recoverable fault */
	#define TCC_CNTSEL_START     	(0x0<<6)	/**< \brief (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts */
	#define TCC_CNTSEL_END       	(0x1<<6)	/**< \brief (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends */
	#define TCC_CNTSEL_BETWEEN   	(0x2<<6)	/**< \brief (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends, except for the first and last cycles */
	#define TCC_CNTSEL_BOUNDARY  	(0x3<<6)	/**< \brief (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts or a counter cycle ends */
	#define TCC_OVFEO        		(1<<8)    	/**< \brief (TCC_EVCTRL) Overflow/Underflow Output Event Enable */
	#define TCC_TRGEO        		(1<<9)    	/**< \brief (TCC_EVCTRL) Retrigger Output Event Enable */
	#define TCC_CNTEO        		(1<<10)   	/**< \brief (TCC_EVCTRL) Timer/counter Output Event Enable */
	#define TCC_TCINV0       		(1<<12)   	/**< \brief (TCC_EVCTRL) Inverted Event 0 Input Enable */
	#define TCC_TCINV1       		(1<<13)   	/**< \brief (TCC_EVCTRL) Inverted Event 1 Input Enable */
	#define TCC_TCEI0        		(1<<14)   	/**< \brief (TCC_EVCTRL) Timer/counter Event 0 Input Enable */
	#define TCC_TCEI1        		(1<<15)   	/**< \brief (TCC_EVCTRL) Timer/counter Event 1 Input Enable */
	#define TCC_MCEI0        		(1<<16)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 0 Event Input Enable */
	#define TCC_MCEI1        		(1<<17)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 1 Event Input Enable */
	#define TCC_MCEI2        		(1<<18)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 2 Event Input Enable */
	#define TCC_MCEI3        		(1<<19)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 3 Event Input Enable */
	#define TCC_MCEO0        		(1<<24)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 0 Event Output Enable */
	#define TCC_MCEO1        		(1<<25)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 1 Event Output Enable */
	#define TCC_MCEO2        		(1<<26)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 2 Event Output Enable */
	#define TCC_MCEO3        		(1<<27)   	/**< \brief (TCC_EVCTRL) Match or Capture Channel 3 Event Output Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_EIC
	{
		RW8   	CTRL;        /**< \brief Offset: 0x00 (R/W  8) Control */
		RO8   	STATUS;      /**< \brief Offset: 0x01 (R/   8) Status */
		RW8   	NMICTRL;     /**< \brief Offset: 0x02 (R/W  8) Non-Maskable Interrupt Control */
		RW8   	NMIFLAG;     /**< \brief Offset: 0x03 (R/W  8) Non-Maskable Interrupt Flag Status and Clear */
		RW32  	EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) Event Control */
		RW32  	INTENCLR;    /**< \brief Offset: 0x08 (R/W 32) Interrupt Enable Clear */
		RW32  	INTENSET;    /**< \brief Offset: 0x0C (R/W 32) Interrupt Enable Set */
		RW32  	INTFLAG;     /**< \brief Offset: 0x10 (R/W 32) Interrupt Flag Status and Clear */
		RW32  	WAKEUP;      /**< \brief Offset: 0x14 (R/W 32) Wake-Up Enable */
		RW32  	CONFIG[2];   /**< \brief Offset: 0x18 (R/W 32) Configuration n */
	};

	#define EIC_SWRST          (1<<0)            /**< \brief (EIC_CTRL) Software Reset */
	#define EIC_ENABLE         (1<<1)            /**< \brief (EIC_CTRL) Enable */

	#define EIC_SYNCBUSY		(1<<7)            /**< \brief (EIC_STATUS) Synchronization Busy */
	#define EIC_NMISENSE_NONE   (0 << 0)
	#define EIC_NMISENSE_RISE   (1 << 0)
	#define EIC_NMISENSE_FALL   (2 << 0)
	#define EIC_NMISENSE_BOTH   (3 << 0)
	#define EIC_NMISENSE_HIGH   (4 << 0)
	#define EIC_NMISENSE_LOW    (5 << 0)
	#define EIC_NMIFILTEN		(1<<3)            /**< \brief (EIC_NMICTRL) Non-Maskable Interrupt Filter Enable */

	#define EIC_EXTINT0    	(1<<0)							
	#define EIC_EXTINT1    	(1<<1)							
	#define EIC_EXTINT2    	(1<<2)							
	#define EIC_EXTINT3    	(1<<3)							
	#define EIC_EXTINT4    	(1<<4)							
	#define EIC_EXTINT5    	(1<<5)							
	#define EIC_EXTINT6    	(1<<6)							
	#define EIC_EXTINT7    	(1<<7)							
	#define EIC_EXTINT8    	(1<<8)							
	#define EIC_EXTINT9    	(1<<9)							
	#define EIC_EXTINT10    (1<<10)							
	#define EIC_EXTINT11    (1<<11)							
	#define EIC_EXTINT12    (1<<12)							
	#define EIC_EXTINT13    (1<<13)							
	#define EIC_EXTINT14    (1<<14)							
	#define EIC_EXTINT15    (1<<15)							

	#define EIC_SENSE_NONE  (0)			/**< \brief (EIC_CONFIG) No detection */
	#define EIC_SENSE_RISE  (1)			/**< \brief (EIC_CONFIG) Rising-edge detection */
	#define EIC_SENSE_FALL  (2)			/**< \brief (EIC_CONFIG) Falling-edge detection */
	#define EIC_SENSE_BOTH  (3)			/**< \brief (EIC_CONFIG) Both-edges detection */
	#define EIC_SENSE_HIGH  (4)			/**< \brief (EIC_CONFIG) High-level detection */
	#define EIC_SENSE_LOW   (5)			/**< \brief (EIC_CONFIG) Low-level detection */
	#define EIC_FILTEN		(1<<3)      /**< \brief (EIC_CONFIG) Filter 0 Enable */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_EVSYS	
	{
		RW8     CTRL;			/**< \brief Offset: 0x00 ( /W  8) Control */
		RO8		Reserved1[0x3];
		RW32    CHANNEL;     	/**< \brief Offset: 0x04 (R/W 32) Channel */
		RW16    USER;        	/**< \brief Offset: 0x08 (R/W 16) User Multiplexer */
		RO8     Reserved2[0x2];
		RO32   	CHSTATUS;    	/**< \brief Offset: 0x0C (R/  32) Channel Status */
		RW32   	INTENCLR;    	/**< \brief Offset: 0x10 (R/W 32) Interrupt Enable Clear */
		RW32   	INTENSET;    	/**< \brief Offset: 0x14 (R/W 32) Interrupt Enable Set */
		RW32	INTFLAG;     	/**< \brief Offset: 0x18 (R/W 32) Interrupt Flag Status and Clear */
	};

	#define EVSYS_CTRL_SWRST        	(1<<0)            		/**< \brief (EVSYS_CTRL) Software Reset */
	#define EVSYS_CTRL_GCLKREQ      	(1<<4)            		/**< \brief (EVSYS_CTRL) Generic Clock Requests */

	#define EVSYS_CHANNEL(value)		((value)&0xF)           /**< \brief (EVSYS_CHANNEL) Channel Selection */
	#define EVSYS_SWEVT					(1<<8)					/**< \brief (EVSYS_CHANNEL) Software Event */
	#define EVSYS_EVGEN(value)			(((value)&0x7F)<<16)    /**< \brief (EVSYS_CHANNEL) Event Generator Selection */
	#define EVSYS_PATH_SYNCHRONOUS		(0 << 24)
	#define EVSYS_PATH_RESYNCHRONIZED	(1 << 24)
	#define EVSYS_PATH_ASYNCHRONOUS		(2 << 24)
	#define EVSYS_EDGSEL_NO_EVT_OUTPUT	(0 << 26)
	#define EVSYS_EDGSEL_RISING_EDGE	(1 << 26)
	#define EVSYS_EDGSEL_FALLING_EDGE	(2 << 26)
	#define EVSYS_EDGSEL_BOTH_EDGES		(3 << 26)
	#define EVSYS_USER_CHANNEL(value)	((((value)+1)&0x1F)<<8)     /**< \brief (EVSYS_USER) Channel Event Selection */

	#define EVSYS_GEN_NONE			 (0<<16)
	#define EVSYS_GEN_RTC_CMP_0      (1<<16)
	#define EVSYS_GEN_RTC_CMP_1      (2<<16)
	#define EVSYS_GEN_RTC_OVF        (3<<16)
	#define EVSYS_GEN_RTC_PER_0      (4<<16)
	#define EVSYS_GEN_RTC_PER_1      (5<<16)
	#define EVSYS_GEN_RTC_PER_2      (6<<16)
	#define EVSYS_GEN_RTC_PER_3      (7<<16)
	#define EVSYS_GEN_RTC_PER_4      (8<<16)
	#define EVSYS_GEN_RTC_PER_5      (9<<16)
	#define EVSYS_GEN_RTC_PER_6      (10<<16)
	#define EVSYS_GEN_RTC_PER_7      (11<<16)
	#define EVSYS_GEN_EIC_EXTINT_0   (12<<16)
	#define EVSYS_GEN_EIC_EXTINT_1   (13<<16)
	#define EVSYS_GEN_EIC_EXTINT_2   (14<<16)
	#define EVSYS_GEN_EIC_EXTINT_3   (15<<16)
	#define EVSYS_GEN_EIC_EXTINT_4   (16<<16)
	#define EVSYS_GEN_EIC_EXTINT_5   (17<<16)
	#define EVSYS_GEN_EIC_EXTINT_6   (18<<16)
	#define EVSYS_GEN_EIC_EXTINT_7   (19<<16)
	#define EVSYS_GEN_EIC_EXTINT_8   (20<<16)
	#define EVSYS_GEN_EIC_EXTINT_9   (21<<16)
	#define EVSYS_GEN_EIC_EXTINT_10  (22<<16)
	#define EVSYS_GEN_EIC_EXTINT_11  (23<<16)
	#define EVSYS_GEN_EIC_EXTINT_12  (24<<16)
	#define EVSYS_GEN_EIC_EXTINT_13  (25<<16)
	#define EVSYS_GEN_EIC_EXTINT_14  (26<<16)
	#define EVSYS_GEN_EIC_EXTINT_15  (27<<16)
	#define EVSYS_GEN_EIC_EXTINT_16  (28<<16)
	#define EVSYS_GEN_EIC_EXTINT_17  (29<<16)
	#define EVSYS_GEN_DMAC_CH_0      (30<<16)
	#define EVSYS_GEN_DMAC_CH_1      (31<<16)
	#define EVSYS_GEN_DMAC_CH_2      (32<<16)
	#define EVSYS_GEN_DMAC_CH_3      (33<<16)
	#define EVSYS_GEN_TCC0_OVF       (34<<16)
	#define EVSYS_GEN_TCC0_TRG       (35<<16)
	#define EVSYS_GEN_TCC0_CNT       (36<<16)
	#define EVSYS_GEN_TCC0_MCX_0     (37<<16)
	#define EVSYS_GEN_TCC0_MCX_1     (38<<16)
	#define EVSYS_GEN_TCC0_MCX_2     (39<<16)
	#define EVSYS_GEN_TCC0_MCX_3     (40<<16)
	#define EVSYS_GEN_TCC1_OVF       (41<<16)
	#define EVSYS_GEN_TCC1_TRG       (42<<16)
	#define EVSYS_GEN_TCC1_CNT       (43<<16)
	#define EVSYS_GEN_TCC1_MCX_0     (44<<16)
	#define EVSYS_GEN_TCC1_MCX_1     (45<<16)
	#define EVSYS_GEN_TCC2_OVF       (46<<16)
	#define EVSYS_GEN_TCC2_TRG       (47<<16)
	#define EVSYS_GEN_TCC2_CNT       (48<<16)
	#define EVSYS_GEN_TCC2_MCX_0     (49<<16)
	#define EVSYS_GEN_TCC2_MCX_1     (50<<16)
	#define EVSYS_GEN_TC3_OVF        (51<<16)
	#define EVSYS_GEN_TC3_MCX_0      (52<<16)
	#define EVSYS_GEN_TC3_MCX_1      (53<<16)
	#define EVSYS_GEN_TC4_OVF        (54<<16)
	#define EVSYS_GEN_TC4_MCX_0      (55<<16)
	#define EVSYS_GEN_TC4_MCX_1      (56<<16)
	#define EVSYS_GEN_TC5_OVF        (57<<16)
	#define EVSYS_GEN_TC5_MCX_0      (58<<16)
	#define EVSYS_GEN_TC5_MCX_1      (59<<16)
	#define EVSYS_GEN_TC6_OVF        (60<<16)
	#define EVSYS_GEN_TC6_MCX_0      (61<<16)
	#define EVSYS_GEN_TC6_MCX_1      (62<<16)
	#define EVSYS_GEN_TC7_OVF        (63<<16)
	#define EVSYS_GEN_TC7_MCX_0      (64<<16)
	#define EVSYS_GEN_TC7_MCX_1      (65<<16)
	#define EVSYS_GEN_ADC_RESRDY     (66<<16)
	#define EVSYS_GEN_ADC_WINMON     (67<<16)
	#define EVSYS_GEN_AC_COMP_0      (68<<16)
	#define EVSYS_GEN_AC_COMP_1      (69<<16)
	#define EVSYS_GEN_AC_WIN_0       (70<<16)
	#define EVSYS_GEN_DAC_EMPTY      (71<<16)
									 
	#define EVSYS_USER_DMAC_CH_0     0
	#define EVSYS_USER_DMAC_CH_1     1
	#define EVSYS_USER_DMAC_CH_2     2
	#define EVSYS_USER_DMAC_CH_3     3
	#define EVSYS_USER_TCC0_EV_0     4
	#define EVSYS_USER_TCC0_EV_1     5
	#define EVSYS_USER_TCC0_MC_0     6
	#define EVSYS_USER_TCC0_MC_1     7
	#define EVSYS_USER_TCC0_MC_2     8
	#define EVSYS_USER_TCC0_MC_3     9
	#define EVSYS_USER_TCC1_EV_0     10
	#define EVSYS_USER_TCC1_EV_1     11
	#define EVSYS_USER_TCC1_MC_0     12
	#define EVSYS_USER_TCC1_MC_1     13
	#define EVSYS_USER_TCC2_EV_0     14
	#define EVSYS_USER_TCC2_EV_1     15
	#define EVSYS_USER_TCC2_MC_0     16
	#define EVSYS_USER_TCC2_MC_1     17
	#define EVSYS_USER_TC3_EVU       18
	#define EVSYS_USER_TC4_EVU       19
	#define EVSYS_USER_TC5_EVU       20
	#define EVSYS_USER_TC6_EVU       21
	#define EVSYS_USER_TC7_EVU       22
	#define EVSYS_USER_ADC_START     23
	#define EVSYS_USER_ADC_SYNC      24
	#define EVSYS_USER_AC_SOC_0      25
	#define EVSYS_USER_AC_SOC_1      26
	#define EVSYS_USER_DAC_START     27

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_TC
	{
		RW16		CTRLA;       		/**< \brief Offset: 0x00 (R/W 16) Control A */
		RW16        READREQ;     		/**< \brief Offset: 0x02 (R/W 16) Read Request */
		RW8       	CTRLBCLR;    		/**< \brief Offset: 0x04 (R/W  8) Control B Clear */
		RW8       	CTRLBSET;    		/**< \brief Offset: 0x05 (R/W  8) Control B Set */
		RW8         CTRLC;       		/**< \brief Offset: 0x06 (R/W  8) Control C */
		RO8         z__Reserved1[0x1];
		RW8         DBGCTRL;			/**< \brief Offset: 0x08 (R/W  8) Debug Control */
		RO8         z__Reserved2[0x1];
		RW16		EVCTRL;      		/**< \brief Offset: 0x0A (R/W 16) Event Control */
		RW8         INTENCLR;    		/**< \brief Offset: 0x0C (R/W  8) Interrupt Enable Clear */
		RW8         INTENSET;    		/**< \brief Offset: 0x0D (R/W  8) Interrupt Enable Set */
		RW8         INTFLAG;     		/**< \brief Offset: 0x0E (R/W  8) Interrupt Flag Status and Clear */
		RO8         STATUS;      		/**< \brief Offset: 0x0F (R/   8) Status */
		RW16		COUNT;       		/**< \brief Offset: 0x10 (R/W 16) COUNT16 Counter Value */
		RO8         z__Reserved3[0x6];
		RW16        CC[2];				/**< \brief Offset: 0x18 (R/W 16) COUNT16 Compare/Capture */
	};

	typedef S_TC S_TC3, S_TC4, S_TC5, S_TC6, S_TC7;

	#define TC_SWRST				(1<<0)            /**< \brief (TCC_CTRLA) Software Reset */
	#define TC_ENABLE				(1<<1)            /**< \brief (TCC_CTRLA) Enable */
	#define TC_RESOLUTION_NONE   	(0 << 5)
	#define TC_RESOLUTION_DITH4  	(1 << 5)
	#define TC_RESOLUTION_DITH5  	(2 << 5)
	#define TC_RESOLUTION_DITH6  	(3 << 5)
	#define TC_PRESCALER_DIV1    	(0 << 8)
	#define TC_PRESCALER_DIV2    	(1 << 8)
	#define TC_PRESCALER_DIV4    	(2 << 8)
	#define TC_PRESCALER_DIV8    	(3 << 8)
	#define TC_PRESCALER_DIV16   	(4 << 8)
	#define TC_PRESCALER_DIV64   	(5 << 8)
	#define TC_PRESCALER_DIV256  	(6 << 8)
	#define TC_PRESCALER_DIV1024 	(7 << 8)
	#define TC_RUNSTDBY      		(1 << 11)           /**< \brief (TCC_CTRLA) Run in Standby */
	#define TC_PRESCSYNC_GCLK    	(0 << 12)
	#define TC_PRESCSYNC_PRESC   	(1 << 12)
	#define TC_PRESCSYNC_RESYNC  	(2 << 12)

	#define TC_MODE_COUNT16       	(0 << 2)
	#define TC_MODE_COUNT8        	(1 << 2)
	#define TC_MODE_COUNT32       	(2 << 2)
	#define TC_WAVEGEN_NFRQ       	(0 << 5)
	#define TC_WAVEGEN_MFRQ       	(1 << 5)
	#define TC_WAVEGEN_NPWM       	(2 << 5)
	#define TC_WAVEGEN_MPWM       	(3 << 5)
	
	#define TC_RCONT         		(1<<14)           /**< \brief (TC_READREQ) Read Continuously */
	#define TC_RREQ		         	(1<<15)           /**< \brief (TC_READREQ) Read Request */

	#define TC_DIR        			(1<<0)            /**< \brief (TCC_CTRLBCLR) Counter Direction */
	#define TC_ONESHOT    			(1<<2)            /**< \brief (TCC_CTRLBCLR) One-Shot */
	#define TC_CMD_NONE       		(0<<6)
	#define TC_CMD_RETRIGGER  		(1<<6)
	#define TC_CMD_STOP       		(2<<6)

	#define TC_INVEN0         	(1<<0)    	/**< \brief (TC_CTRLC) Output Waveform 0 Invert Enable */
	#define TC_INVEN1         	(1<<1)    	/**< \brief (TC_CTRLC) Output Waveform 1 Invert Enable */
	#define TC_CPTEN0         	(1<<4)    	/**< \brief (TC_CTRLC) Capture Channel 0 Enable */
	#define TC_CPTEN1         	(1<<5)    	/**< \brief (TC_CTRLC) Capture Channel 1 Enable */

	#define TC_OVF          	(1<<0)    	/**< \brief (TC_INTFLAG) Overflow */
	#define TC_ERR          	(1<<1)    	/**< \brief (TC_INTFLAG) Error */
	#define TC_SYNCRDY      	(1<<3)    	/**< \brief (TC_INTFLAG) Synchronization Ready */
	#define TC_MC0          	(1<<4)    	/**< \brief (TC_INTFLAG) Match or Capture Channel 0 */
	#define TC_MC1          	(1<<5)    	/**< \brief (TC_INTFLAG) Match or Capture Channel 1 */

	#define TC_EVACT_OFF       	(0x0)		/**< \brief (TCC_EVCTRL) Event action disabled */
	#define TC_EVACT_RETRIGGER 	(0x1)		/**< \brief (TCC_EVCTRL) Start, restart or re-trigger counter on event */
	#define TC_EVACT_COUNT		(0x2)		/**< \brief (TCC_EVCTRL) Count on event */
	#define TC_EVACT_START     	(0x3)		/**< \brief (TCC_EVCTRL) Start counter on event */
	#define TC_EVACT_PPW       	(0x5)		/**< \brief (TCC_EVCTRL) Period capture value in CC0 register, pulse width capture value in CC1 register */
	#define TC_EVACT_PWP       	(0x6)		/**< \brief (TCC_EVCTRL) Period capture value in CC1 register, pulse width capture value in CC0 register */
	
	#define TC_TCINV         	(1<<4)    	/**< \brief (TC_EVCTRL) TC Inverted Event Input */
	#define TC_TCEI          	(1<<5)    	/**< \brief (TC_EVCTRL) TC Event Input */
	#define TC_OVFEO         	(1<<8)    	/**< \brief (TC_EVCTRL) Overflow/Underflow Event Output Enable */
	#define TC_MCEO0         	(1<<12)   	/**< \brief (TC_EVCTRL) Match or Capture Channel 0 Event Output Enable */
	#define TC_MCEO1         	(1<<13)   	/**< \brief (TC_EVCTRL) Match or Capture Channel 1 Event Output Enable */
	#define TC_MCEO          	(1<<12)   	/**< \brief (TC_EVCTRL) Match or Capture Channel x Event Output Enable */

	#define TC_STOP          	(1<<3)    	/**< \brief (TC_STATUS) Stop */
	#define TC_SLAVE         	(1<<4)    	/**< \brief (TC_STATUS) Slave */
	#define TC_SYNCBUSY      	(1<<7)    	/**< \brief (TC_STATUS) Synchronization Busy */

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct DMADESC;

	typedef volatile DMADESC *RWDMADESC;
	
	struct DMADESC
	{
		RW16			BTCTRL;      /**< \brief Offset: 0x00 (R/W 16) Block Transfer Control */
		RW16			BTCNT;       /**< \brief Offset: 0x02 (R/W 16) Block Transfer Count */
		RW_PTR			SRCADDR;     /**< \brief Offset: 0x04 (R/W 32) Block Transfer Source Address */
		RW_PTR			DSTADDR;     /**< \brief Offset: 0x08 (R/W 32) Block Transfer Destination Address */
		RWDMADESC		DESCADDR;    /**< \brief Offset: 0x0C (R/W 32) Next Descriptor Address */
	};


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_DMAC
	{
		RW16    		CTRL;        /**< \brief Offset: 0x00 (R/W 16) Control */
		RW16    		CRCCTRL;     /**< \brief Offset: 0x02 (R/W 16) CRC Control */
		RW32    		CRCDATAIN;   /**< \brief Offset: 0x04 (R/W 32) CRC Data Input */
		RW32    		CRCCHKSUM;   /**< \brief Offset: 0x08 (R/W 32) CRC Checksum */
		RW8     		CRCSTATUS;   /**< \brief Offset: 0x0C (R/W  8) CRC Status */
		RW8     		DBGCTRL;     /**< \brief Offset: 0x0D (R/W  8) Debug Control */
		RW8     		QOSCTRL;     /**< \brief Offset: 0x0E (R/W  8) QOS Control */

		RoReg8  		                  z__Reserved1[0x1];

		RW32    		SWTRIGCTRL;  /**< \brief Offset: 0x10 (R/W 32) Software Trigger Control */
		RW32    		PRICTRL0;    /**< \brief Offset: 0x14 (R/W 32) Priority Control 0 */

		RoReg8  		                  z__Reserved2[0x8];

		RW16    		INTPEND;     /**< \brief Offset: 0x20 (R/W 16) Interrupt Pending */

		RoReg8  		                  z__Reserved3[0x2];

		RO32    		INTSTATUS;   /**< \brief Offset: 0x24 (R/  32) Interrupt Status */
		RO32    		BUSYCH;      /**< \brief Offset: 0x28 (R/  32) Busy Channels */
		RO32    		PENDCH;      /**< \brief Offset: 0x2C (R/  32) Pending Channels */
		RO32    		ACTIVE;      /**< \brief Offset: 0x30 (R/  32) Active Channel and Levels */
		RWDMADESC		BASEADDR;    /**< \brief Offset: 0x34 (R/W 32) Descriptor Memory Section Base Address */
		RWDMADESC    	WRBADDR;     /**< \brief Offset: 0x38 (R/W 32) Write-Back Memory Section Base Address */

		RoReg8  		                  z__Reserved4[0x3];

		RW8     		CHID;        /**< \brief Offset: 0x3F (R/W  8) Channel ID */
		RW8     		CHCTRLA;     /**< \brief Offset: 0x40 (R/W  8) Channel Control A */

		RoReg8  		                  z__Reserved5[0x3];

		RW32    		CHCTRLB;     /**< \brief Offset: 0x44 (R/W 32) Channel Control B */

		RoReg8  		                  z__Reserved6[0x4];

		RW8  			CHINTENCLR;  /**< \brief Offset: 0x4C (R/W  8) Channel Interrupt Enable Clear */
		RW8  			CHINTENSET;  /**< \brief Offset: 0x4D (R/W  8) Channel Interrupt Enable Set */
		RW8     		CHINTFLAG;   /**< \brief Offset: 0x4E (R/W  8) Channel Interrupt Flag Status and Clear */
		RO8     		CHSTATUS;    /**< \brief Offset: 0x4F (R/   8) Channel Status */
	};

	#define DMAC_SWRST         			(1<<0)            /**< \brief (DMAC_CTRL) Software Reset */
	#define DMAC_DMAENABLE     			(1<<1)            /**< \brief (DMAC_CTRL) DMA Enable */
	#define DMAC_CRCENABLE     			(1<<2)            /**< \brief (DMAC_CTRL) CRC Enable */
	#define DMAC_LVLEN0        			(1<<8)            /**< \brief (DMAC_CTRL) Priority Level 0 Enable */
	#define DMAC_LVLEN1        			(1<<9)            /**< \brief (DMAC_CTRL) Priority Level 1 Enable */
	#define DMAC_LVLEN2        			(1<<10)           /**< \brief (DMAC_CTRL) Priority Level 2 Enable */
	#define DMAC_LVLEN3        			(1<<11)           /**< \brief (DMAC_CTRL) Priority Level 3 Enable */

	#define DMAC_CRCBEATSIZE_BYTE		(0 << 0)
	#define DMAC_CRCBEATSIZE_HWORD		(1 << 0)
	#define DMAC_CRCBEATSIZE_WORD		(2 << 0)
	#define DMAC_CRCPOLY_CRC16			(0 << 2)
	#define DMAC_CRCPOLY_CRC32			(1 << 2)
	#define DMAC_CRCSRC(value)			(((value)&0x3F) << 8)
	#define DMAC_CRCSRC_NOACT   		(0 << 8)
	#define DMAC_CRCSRC_IO      		(1 << 8)

	#define DMAC_CRCBUSY  				(1<<0)            /**< \brief (DMAC_CRCSTATUS) CRC Module Busy */
	#define DMAC_CRCZERO  				(1<<1)            /**< \brief (DMAC_CRCSTATUS) CRC Zero */

	#define DMAC_WRBQOS_DISABLE 		(0<<0)
	#define DMAC_WRBQOS_LOW     		(1<<0)
	#define DMAC_WRBQOS_MEDIUM  		(2<<0)
	#define DMAC_WRBQOS_HIGH    		(3<<0)
	#define DMAC_FQOS_DISABLE   		(0<<2)
	#define DMAC_FQOS_LOW       		(1<<2)
	#define DMAC_FQOS_MEDIUM    		(2<<2)
	#define DMAC_FQOS_HIGH      		(3<<2)
	#define DMAC_DQOS_DISABLE   		(0<<4)
	#define DMAC_DQOS_LOW       		(1<<4)
	#define DMAC_DQOS_MEDIUM    		(2<<4)
	#define DMAC_DQOS_HIGH      		(3<<4)

	#define DMCH_SWRST      			(1<<0)           /**< \brief (DMAC_CHCTRLA) Channel Software Reset */
	#define DMCH_ENABLE     			(1<<1)           /**< \brief (DMAC_CHCTRLA) Channel Enable */

	#define DMCH_EVACT_NOACT    		(0<<0)
	#define DMCH_EVACT_TRIG     		(1<<0)
	#define DMCH_EVACT_CTRIG    		(2<<0)
	#define DMCH_EVACT_CBLOCK   		(3<<0)
	#define DMCH_EVACT_SUSPEND  		(4<<0)
	#define DMCH_EVACT_RESUME   		(5<<0)
	#define DMCH_EVACT_SSKIP    		(6<<0)

	#define DMCH_EVIE       			(1<<3)            /**< \brief (DMAC_CHCTRLB) Channel Event Input Enable */
	#define DMCH_EVOE       			(1<<4)            /**< \brief (DMAC_CHCTRLB) Channel Event Output Enable */
	#define DMCH_LVL0       			(0<<5)
	#define DMCH_LVL1       			(1<<5)
	#define DMCH_LVL2       			(2<<5)
	#define DMCH_LVL3       			(3<<5)

	#define DMCH_TRIGSRC_DISABLE		(0x00 << 8)
	#define DMCH_TRIGSRC_SERCOM0_RX		(0x01 << 8)
	#define DMCH_TRIGSRC_SERCOM0_TX		(0x02 << 8)
	#define DMCH_TRIGSRC_SERCOM1_RX		(0x03 << 8)
	#define DMCH_TRIGSRC_SERCOM1_TX		(0x04 << 8)
	#define DMCH_TRIGSRC_SERCOM2_RX		(0x05 << 8)
	#define DMCH_TRIGSRC_SERCOM2_TX		(0x06 << 8)
	#define DMCH_TRIGSRC_SERCOM3_RX		(0x07 << 8)
	#define DMCH_TRIGSRC_SERCOM3_TX		(0x08 << 8)
	#define DMCH_TRIGSRC_SERCOM4_RX		(0x09 << 8)
	#define DMCH_TRIGSRC_SERCOM4_TX		(0x0A << 8)
	#define DMCH_TRIGSRC_SERCOM5_RX		(0x0B << 8)
	#define DMCH_TRIGSRC_SERCOM5_TX		(0x0C << 8)
	#define DMCH_TRIGSRC_TCC0_OVF   	(0x0D << 8)
	#define DMCH_TRIGSRC_TCC0_MC0   	(0x0E << 8)
	#define DMCH_TRIGSRC_TCC0_MC1   	(0x0F << 8)
	#define DMCH_TRIGSRC_TCC0_MC2   	(0x10 << 8)
	#define DMCH_TRIGSRC_TCC0_MC3   	(0x11 << 8)
	#define DMCH_TRIGSRC_TCC1_OVF   	(0x12 << 8)
	#define DMCH_TRIGSRC_TCC1_MC0   	(0x13 << 8)
	#define DMCH_TRIGSRC_TCC1_MC1   	(0x14 << 8)
	#define DMCH_TRIGSRC_TCC2_OVF   	(0x15 << 8)
	#define DMCH_TRIGSRC_TCC2_MC0   	(0x16 << 8)
	#define DMCH_TRIGSRC_TCC2_MC1		(0x17 << 8)
	#define DMCH_TRIGSRC_TC3_OVF 		(0x18 << 8)
	#define DMCH_TRIGSRC_TC3_MC0 		(0x19 << 8)
	#define DMCH_TRIGSRC_TC3_MC1 		(0x1A << 8)
	#define DMCH_TRIGSRC_TC4_OVF 		(0x1B << 8)
	#define DMCH_TRIGSRC_TC4_MC0 		(0x1C << 8)
	#define DMCH_TRIGSRC_TC4_MC1 		(0x1D << 8)
	#define DMCH_TRIGSRC_TC5_OVF 		(0x1E << 8)
	#define DMCH_TRIGSRC_TC5_MC0 		(0x1F << 8)
	#define DMCH_TRIGSRC_TC5_MC1 		(0x20 << 8)
	#define DMCH_TRIGSRC_TC6_OVF 		(0x21 << 8)
	#define DMCH_TRIGSRC_TC6_MC0		(0x22 << 8)
	#define DMCH_TRIGSRC_TC6_MC1		(0x23 << 8)
	#define DMCH_TRIGSRC_TC7_OVF		(0x24 << 8)
	#define DMCH_TRIGSRC_TC7_MC0		(0x25 << 8)
	#define DMCH_TRIGSRC_TC7_MC1		(0x26 << 8)
	#define DMCH_TRIGSRC_ADC_RESRDY		(0x27 << 8)
	#define DMCH_TRIGSRC_DAC_EMPTY		(0x28 << 8)
	#define DMCH_TRIGSRC_I2S_RX_0		(0x29 << 8)
	#define DMCH_TRIGSRC_I2S_RX_1		(0x2A << 8)
	#define DMCH_TRIGSRC_I2S_TX_0		(0x2B << 8)
	#define DMCH_TRIGSRC_I2S_TX_1		(0x2C << 8)
	#define DMCH_TRIGSRC_TCC3_OVF		(0x2D << 8)
	#define DMCH_TRIGSRC_TCC3_MC0		(0x2E << 8)
	#define DMCH_TRIGSRC_TCC3_MC1		(0x2F << 8)
	#define DMCH_TRIGSRC_TCC3_MC2		(0x30 << 8)
	#define DMCH_TRIGSRC_TCC3_MC3		(0x31 << 8)

	#define DMCH_TRIGACT_BLOCK  		(0<<22)
	#define DMCH_TRIGACT_BEAT   		(2<<22)
	#define DMCH_TRIGACT_TRANSACTION	(3<<22)
	#define DMCH_CMD_NOACT      		(0<<24)
	#define DMCH_CMD_SUSPEND    		(1<<24)
	#define DMCH_CMD_RESUME     		(2<<24)

	#define DMCH_TERR    				(1<<0)            /**< \brief (DMAC_CHINTENCLR) Channel Transfer Error Interrupt Enable */
	#define DMCH_TCMPL   				(1<<1)            /**< \brief (DMAC_CHINTENCLR) Channel Transfer Complete Interrupt Enable */
	#define DMCH_SUSP    				(1<<2)            /**< \brief (DMAC_CHINTENCLR) Channel Suspend Interrupt Enable */

	#define DMDSC_VALID       			(1<<0)            /**< \brief (DMAC_BTCTRL) Descriptor Valid */
	#define DMDSC_EVOSEL      			(1<<1)            /**< \brief (DMAC_BTCTRL) Event Output Selection */
	#define DMDSC_EVOSEL_DISABLE  		(0<<1)
	#define DMDSC_EVOSEL_BLOCK    		(1<<1)
	#define DMDSC_EVOSEL_BEAT     		(3<<1)
	#define DMDSC_BLOCKACT_NOACT  		(0<<3)
	#define DMDSC_BLOCKACT_INT    		(1<<3)
	#define DMDSC_BLOCKACT_SUSPEND		(2<<3)
	#define DMDSC_BLOCKACT_BOTH			(3<<3)
	#define DMDSC_BEATSIZE_BYTE   		(0<<8)
	#define DMDSC_BEATSIZE_HWORD  		(1<<8)
	#define DMDSC_BEATSIZE_WORD   		(2<<8)
	#define DMDSC_SRCINC      			(1<<10)           /**< \brief (DMAC_BTCTRL) Source Address Increment Enable */
	#define DMDSC_DSTINC      			(1<<11)           /**< \brief (DMAC_BTCTRL) Destination Address Increment Enable */
	#define DMDSC_STEPSEL     			(1<<12)           /**< \brief (DMAC_BTCTRL) Step Selection */
	#define DMDSC_STEPSEL_DST     		(0<<12)
	#define DMDSC_STEPSEL_SRC     		(1<<12)
	#define DMDSC_STEPSIZE_X1     		(0<<13)
	#define DMDSC_STEPSIZE_X2     		(1<<13)
	#define DMDSC_STEPSIZE_X4     		(2<<13)
	#define DMDSC_STEPSIZE_X8     		(3<<13)
	#define DMDSC_STEPSIZE_X16    		(4<<13)
	#define DMDSC_STEPSIZE_X32    		(5<<13)
	#define DMDSC_STEPSIZE_X64    		(6<<13)
	#define DMDSC_STEPSIZE_X128   		(7<<13)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_RTT
	{
		AT91_REG		MR;
		AT91_REG		AR;
		const AT91_REG	VR;
		const AT91_REG	SR;
	};


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	 
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_ADC
	{
	};


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_MATRIX
	{
		AT91_REG		MCFG[16];
		AT91_REG		SCFG[16];
		struct { AT91_REG A; AT91_REG B; } PRAS[18];
		AT91_REG		zreserve;
		AT91_REG		SYSIO;
		AT91_REG		zreserve1;
		AT91_REG		SMCNFCS;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_EFC
	{
		AT91_REG		FMR;
		AT91_REG		FCR;
		AT91_REG		FSR;
		AT91_REG		FRR;
	};

	typedef S_EFC S_EFC0, S_EFC1;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_SUPC
	{
		AT91_REG		CR;
		AT91_REG		SMMR;
		AT91_REG		MR;
		AT91_REG		WUMR;
		AT91_REG		WUIR;
		AT91_REG		SR;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	struct S_CMCC
	{
		AT91_REG		TYPE;
		AT91_REG		CFG;
		AT91_REG		CTRL;
		AT91_REG		SR;
		AT91_REG		z__rsrvd[4];
		AT91_REG		MAINT0;
		AT91_REG		MAINT1;
		AT91_REG		MCFG;
		AT91_REG		MEN;
		AT91_REG		MCTRL;
		AT91_REG		MSR;
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

};


namespace HW
{
	//namespace PID
	//{
	//	enum {	MKPID(SUPC, 0),		MKPID(RSTC, 1),		MKPID(RTC, 2),		MKPID(RTT, 3),		MKPID(WDT, 4),		MKPID(PMC, 5),		MKPID(EEFC0, 6),	MKPID(EEFC1, 7),
	//			MKPID(UART0, 8),	MKPID(UART1, 9),	MKPID(SMC, 10),		MKPID(PIOA, 11),	MKPID(PIOB, 12),	MKPID(PIOC, 13),	MKPID(USART0, 14),	MKPID(USART1, 15),
	//			MKPID(HSMCI, 18),	MKPID(TWI0, 19),	MKPID(TWI1, 20),	MKPID(SPI, 21),		MKPID(SSC, 22),		MKPID(TC0, 23),		MKPID(TC1, 24),		MKPID(TC2, 25),		MKPID(TC3, 26),		MKPID(TC4, 27), 
	//			MKPID(TC5, 28),		MKPID(ADC, 29),		MKPID(DACC, 30),	MKPID(PWM, 31),		MKPID(CRCCU, 32),	MKPID(ACC, 33),		MKPID(UDP, 34) };
	//};



	MK_PTR(GCLK,	0x40000C00);
	MK_PTR(PM,		0x40000400);
	MK_PTR(SYSCTRL,	0x40000800);
	MK_PTR(WDT,		0x40001000);

	MK_PTR(PORTA,	0x41004400);
	MK_PTR(PORTB,	0x41004480);

	MK_PTR(PIOA,	0x60000000);
	MK_PTR(PIOB,	0x60000080);

	MK_PTR(NVMCTRL,	0x41004000);
	MK_PTR(RTC,		0x40001400);

	MK_PTR(USART0,	0x42000800);
	MK_PTR(USART1,	0x42000C00);
	MK_PTR(USART2,	0x42001000);
	MK_PTR(USART3,	0x42001400);
	MK_PTR(USART4,	0x42001800);
	MK_PTR(USART5,	0x42001C00);

	MK_PTR(SPI0,	0x42000800);
	MK_PTR(SPI1,	0x42000C00);
	MK_PTR(SPI2,	0x42001000);
	MK_PTR(SPI3,	0x42001400);
	MK_PTR(SPI4,	0x42001800);
	MK_PTR(SPI5,	0x42001C00);

	MK_PTR(I2C0,	0x42000800);
	MK_PTR(I2C1,	0x42000C00);
	MK_PTR(I2C2,	0x42001000);
	MK_PTR(I2C3,	0x42001400);
	MK_PTR(I2C4,	0x42001800);
	MK_PTR(I2C5,	0x42001C00);

	MK_PTR(TCC0,	0x42002000);
	MK_PTR(TCC1,	0x42002400);
	MK_PTR(TCC2,	0x42002800);

	MK_PTR(EIC,		0x40001800);
	MK_PTR(EVSYS,	0x42000400);

	MK_PTR(TC3,		0x42002C00);
	MK_PTR(TC4,		0x42003000);
	MK_PTR(TC5,		0x42003400);
	MK_PTR(TC6,		0x42003800);
	MK_PTR(TC7,		0x42003C00);

	MK_PTR(DMAC, 	0x41004800);

	//MK_PTR(MATRIX,	0x400E0200);
	//MK_PTR(UART0,	0x400E0600);
	//MK_PTR(UART1,	0x400E0800);

	//MK_PTR(EFC1,	0x400E0C00);
	//MK_PTR(PIOC,	0x400E1200);

	//MK_PTR(RSTC,	0X400E1400);
	//MK_PTR(SUPC,	0X400E1410);
	//MK_PTR(RTT,		0X400E1430);

	//MK_PTR(SPI,		0X40008000);

	//MK_PTR(TC1,		0X40014000);

//	MK_PTR(PWM,		0X40094000);

	//MK_PTR(USART1,	0x40028000);

	//MK_PTR(ADC,		0X40038000);

	//MK_PTR(CMCC,	0X4007C000);

	//MK_PTR(TWI0,	0x40018000);
	//MK_PTR(TWI1,	0x4001C000);


//	MK_PTR(UDP,		0x400A4000);



	inline void ResetWDT() { WDT->Reset(); }

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	inline bool RamCheck(void *ptr)
	{
		//u32 v = (u32)ptr;

		return ((u32)ptr & ~0x20003FFF) == 0; //(v >= 0x20000000 && v < 0x20004000);

	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	inline bool RomCheck(void *ptr)
	{
		//u32 v = (u32)ptr;

		return ((u32)ptr & ~0x1FFFF) == 0; //(v >= 0x000000 && v < 0x20000);

	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

}; // namespace HW

extern T_HW::AT91_IHP VectorTableInt[16];
extern T_HW::AT91_IHP VectorTableExt[28];

extern T_HW::DMADESC DmaTable[12];

#undef MK_PTR
#undef MKPID



#endif // SAMD21_H__09_04_2019__08_14
