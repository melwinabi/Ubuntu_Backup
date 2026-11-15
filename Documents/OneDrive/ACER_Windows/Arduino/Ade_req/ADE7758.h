

#ifndef ADE7758_H
#define ADE7758_H

#define AFECS 10             // Chip Select ADE7758 - It is the pin to which the AFE is connected (Pin 5 as CS in our case)
#define WRITE 0x80          // WRITE bit BT7 to write to registers
#define CLKIN 10000000      // ADE7758 frec, 10.000000MHz
#define PERIODO 50			// Actually it is frequency, it is used to calculate the amount of Cycles that it accumulates for energy.
#define PHASE_A	1
#define PHASE_B	2
#define PHASE_C	3


#define AWATTHR		0x01 //---------16
#define BWATTHR		0x02 //---------16
#define CWATTHR		0x03 //---------16
 
#define AVARHR		0x04 //---------16
#define BVARHR		0x05 //---------16
#define CVARHR		0x06 //---------16
 
#define AVAHR		0x07 //---------16
#define BVAHR		0x08 //---------16
#define CVAHR		0x09 //---------16
 
#define AIRMS		0x0A //---------24
#define BIRMS		0x0B //---------24
#define CIRMS		0x0C //---------24
 
#define AVRMS		0x0D //---------24
#define BVRMS		0x0E //---------24
#define CVRMS		0x0F //---------24
 
#define FREQ		0x10 //---------12
#define TEMP		0x11 //---------8
#define WFORM		0x12 //---------24
#define OPMODE		0x13 //---------8
#define MMODE		0x14 //---------8
#define WAVMODE		0x15 //---------8
#define COMPMODE	0x16 //---------8
#define LCYCMODE	0x17 //---------8
#define MASK		0x18 //---------24
#define STATUS		0x19 //---------24
#define RSTATUS		0x1A //---------24
#define ZXTOUT		0x1B //---------16
#define LINECYC		0x1C //---------16
#define SAGCYC		0x1D //---------8
#define SAGLVL		0x1E //---------8
#define VPINTLVL	0x1F //---------8
#define IPINTLVL	0x20 //---------8
#define VPEAK		0x21 //---------8
#define IPEAK		0x22 //---------8
#define GAIN		0x23 //---------8
#define AVRMSGAIN	0x24 //---------12
#define BVRMSGAIN	0x25 //---------12
#define CVRMSGAIN	0x26 //---------12
#define AIGAIN		0x27 //---------12
#define BIGAIN		0x28 //---------12
#define CIGAIN		0x29 //---------12
#define AWG			0x2A //---------12
#define BWG			0x2B //---------12
#define CWG			0x2C //---------12
#define AVARG		0x2D //---------12
#define BVARG		0x2E //---------12
#define CVARG		0x2F //---------12
#define AVAG		0x30 //---------12
#define	BVAG		0X31 //---------12
#define CVAG		0x32 //---------12
#define AVRMSOS		0x33 //---------12
#define BVRMSOS		0X34 //---------12
#define CVRMSOS		0X35 //---------12
#define AIRMSOS		0X36 //---------12
#define BIRMSOS		0X37 //---------12
#define CIRMSOS		0X38 //---------12
#define AWATTOS		0X39 //---------12
#define BWATTOS		0X3A //---------12
#define CWATTOS		0X3B //---------12
#define AVAROS		0X3C //---------12
#define BVAROS		0X3D //---------12
#define CVAROS		0X3E //---------12
#define APHCAL		0X3F //---------7
#define BPHCAL		0X40 //---------7
#define CPHCAL		0X41 //---------7
#define WDIV		0X42 //---------8
#define VARDIV		0X43 //---------8
#define VADIV		0X44 //---------8
#define APCFNUM		0X45 //---------16
#define APCFDEN		0X46 //---------12
#define VARCFNUM	0X47 //---------16
#define VARCFDEN	0X48 //---------12
 
#define CHKSUM		0X7E //---------8
#define VERSION		0x7f //---------8



#define	DISHPF	0x01
#define	DISLPF	0x02
#define	DISCF	0x04
#define	SWRST	0x40

#define	FREQSEL0	0x01
#define	FREQSEL1	0x02




#define	LWATT	0x01
#define	LVAR	0x02
#define	LVA		0x04
#define	ZXSEL_A	0x08
#define	ZXSEL_B	0x10
#define	ZXSEL_C	0x20
#define	RSTREAD	0x40
#define	FREQSEL	0x80




//             Bit Mask // Bit Location / Description			
#define AEHF      0x0001 // bit 0 - Indicates that an interrupt was caused by a change in Bit 14 among any one of the three WATTHR registers, that is, the WATTHR register is half full.
#define REHF      0x0002 // bit 1 - Indicates that an interrupt was caused by a change in Bit 14 among any one of the three VARHR registers, that is, the VARHR register is half full.
#define VAEHF     0x0004 // bit 2 - Indicates that an interrupt was caused by a 0 to 1 transition in Bit 15 among any one of the three VAHR registers, that is, the VAHR register is half full.
#define SAGA      0x0008 // bit 3 - Indicates that an interrupt was caused by a SAG on the line voltage of the Phase A.
#define SAGB      0x0010 // bit 4 - Indicates that an interrupt was caused by a SAG on the line voltage of the Phase B.
#define SAGC	  0x0020 // bit 5 - Indicates that an interrupt was caused by a SAG on the line voltage of the Phase C.
#define ZXTOA     0x0040 // bit 6 - Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase A.
#define ZXTOB     0x0080 // bit 7 - Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase B.
#define ZXTOC     0x0100 // bit 8 - Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase C
#define ZXA       0x0200 // bit 9 - Indicates a detection of a rising edge zero crossing in the voltage channel of Phase A.
#define ZXB       0x0400 // bit 10 - Indicates a detection of a rising edge zero crossing in the voltage channel of Phase B
#define ZXC       0x0800 // bit 11 - Indicates a detection of a rising edge zero crossing in the voltage channel of Phase C
#define LENERGY   0x1000 // bit 12 - In line energy accumulation, indicates the end of an integration over an integer number of half- line cycles (LINECYC). See the Calibration section.
#define RESET     0x2000 // bit 13 - Indicates that the 5 V power supply is below 4 V. Enables a software reset of the ADE7758 and sets the registers back to their default values. This bit in the STATUS or RSTATUS register is logic high for only one clock cycle after a reset event.
#define PKV       0x4000 // bit 14 - Indicates that an interrupt was caused when the selected voltage input is above the value in the VPINTLVL register.
#define PKI       0x8000 // bit 15 - Indicates that an interrupt was caused when the selected current input is above the value in the IPINTLVL register.
#define WFSM    0x010000 // bit 16 - Indicates that new data is present in the waveform register.
#define REVPAP  0x020000 // bit 17 - Indicates that an interrupt was caused by a sign change in the watt calculation among any one of the phases specified by the TERMSEL bits in the COMPMODE register.
#define REVPRP  0x040000 // bit 18 - Indicates that an interrupt was caused by a sign change in the VAR calculation among any one of the phases specified by the TERMSEL bits in the COMPMODE register.
#define SEQERR  0x080000 // bit 19 - Indicates that an interrupt was caused by a zero crossing from Phase A followed not by the zero crossing of Phase C but by that of Phase B.



						 
//constants
#define GAIN_1    0x00
#define GAIN_2    0x01
#define GAIN_4    0x02
#define INTEGRATOR_ON  1
#define INTEGRATOR_OFF 0
#define FULLSCALESELECT_0_5V    0x00
#define FULLSCALESELECT_0_25V   0x01
#define FULLSCALESELECT_0_125V  0x02



class ADE7758 
{
	//----------------------------------------------------------------------------      
	// Public Methods    functions
	//----------------------------------------------------------------------------      

	public:

		  ADE7758();
		  bool Init(void);
		  void setSPI(void);
		  void closeSPI(void);
	//----------------------------------------------------------------------------      
	// Modes and configurations
	//----------------------------------------------------------------------------      
	void setOpMode(char m);
	int	getOpMode();
	void setMMode(char m);
	int getMMode();
	void setWavMode(char m);
	int getWavMode();
	void setCompMode(char m);
	int getCompMode();
	void setLcycMode(char m);
	int getLcycMode();
	void gainSetup(char integrator, char scale, char PGA2, char PGA1);
	void setupDivs(unsigned char Watt_div,unsigned char VAR_div,unsigned char VA_div);
	long getMaskInterrupts(void);
	void setMaskInterrupts(unsigned long m);
	unsigned long getStatus(void);
	unsigned long resetStatus(void);
	long getAIRMS(void);
	long getBIRMS(void);
	long getCIRMS(void);
	long getAVRMS(void);
	long getBVRMS(void);
	long getCVRMS(void);
	long avrms();
	long bvrms();
	long cvrms();
	long airms();
	long birms();
	long cirms();
	int getFreq(void);
	void setLineCyc(int d);
	int getACurrentOffset();
	int getBCurrentOffset();
	int getCCurrentOffset();
	void setACurrentOffset(int o);
	void setBCurrentOffset(int o);
	void setCCurrentOffset(int o);
	int getAVoltageOffset();
	int getBVoltageOffset();
	int getCVoltageOffset();
	void setAVoltageOffset(int o);
	void setBVoltageOffset(int o);
	void setCVoltageOffset(int o);
	void setAWattOffset(int32_t o);
	int getAWattOffset();
	void setBWattOffset(int32_t o);
	void setCWattOffset(int32_t o);
	void setAVAROffset(int32_t o);
	void setBVAROffset(int32_t o);
	void setCVAROffset(int32_t o);
	void setZeroCrossingTimeout(int d);
	int getZeroCrossingTimeout();
	char setPotLine(char Phase, int Ciclos);
	int getWatt(char Phase);
	int getVar(char Phase);
	int getVa(char Phase);
	unsigned char getVersion(void);
	void setAPCFDEN(int d);
	int getAPCFDEN();
	void setAPCFNUM(int d);
	int getAPCFNUM();
	void setVARCFDEN(int d);
	int getVARCFDEN();
	void setVARCFNUM(int d);
	int getVARCFNUM();
	void setAVRMSGAIN(int d);
	int getAVRMSGAIN();
	void setBVRMSGAIN(int d);
	int getBVRMSGAIN();
	void setCVRMSGAIN(int d);
	int getCVRMSGAIN();
	void setAWG(int d);
	int getAWG();
	void setBWG(int d);
	int getBWG();
	void setCWG(int d);
	int getCWG();
	void setAVARG(int d);
	int getAVARG();
	void setBVARG(int d);
	int getBVARG();
	void setCVARG(int d);
	int getCVARG();
	void setAVAG(int d);
	int getAVAG();
	void setBVAG(int d);
	int getBVAG();
	void setCVAG(int d);
	int getCVAG();
	char getTemperature();
	   //private methods
	private:
		unsigned char read8(char reg);
		unsigned int read16(char reg);
		unsigned long read24(char reg);
		void write24(char reg, unsigned long data);
		void write16(char reg, unsigned int data);
		void write8(char reg, unsigned char data);
		void enableChip(void);  
		void disableChip(void);
};

#endif




