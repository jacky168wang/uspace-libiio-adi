/**
 * \brief Contains init setting structure declarations for the _instance API
 *
 * The top level structure mykonosDevice_t mykDevice uses keyword
 * extern to allow the application layer main() to have visibility
 * to these settings.
 * 
 * All data structures required for operation have been initialized with values which reflect these settings:
 * 
 * Device Clock:
 * Cus 76.8Mhz
 * 
 * Profiles:
 * Cus RX 11MHz, Iqrate 23.04MSPS, DEC5
 * Cus TX 20/20MHz, Iqrate 92.16MSPS, DEC5
 * Cus ORX 75MHz, Iqrate 92.16MSPS, DEC5
 * 
 * 
 */

#include <stddef.h>
#include "t_mykonos.h"
#include "t_mykonos_gpio.h"

static int16_t txFirCoefs[] = {33,-13,-26,-45,-66,-86,-99,16904,-99,-86,-66,-45,-26,-13,33,0};

static mykonosFir_t txFir =
{
    6,              /* Filter gain in dB*/
    16,             /* Number of coefficients in the FIR filter*/
    &txFirCoefs[0]  /* A pointer to an array of filter coefficients*/
};

static int16_t rxFirCoefs[] = {0,1,0,-2,-5,-6,-2,9,23,29,12,-30,-79,-100,-53,67,211,281,181,-105,-464,-672,-510,80,882,1444,1279,181,-1539,-3040,-3247,-1356,2690,8011,13028,16070,16070,13028,8011,2690,-1356,-3247,-3040,-1539,181,1279,1444,882,80,-510,-672,-464,-105,181,281,211,67,-53,-100,-79,-30,12,29,23,9,-2,-6,-5,-2,0,1,0};

static mykonosFir_t rxFir =
{
    -6,             /* Filter gain in dB*/
    72,             /* Number of coefficients in the FIR filter*/
    &rxFirCoefs[0]  /* A pointer to an array of filter coefficients*/
};

static int16_t obsrxFirCoefs[] = {0,-21,18,39,-36,-87,81,157,-149,-269,260,432,-423,-672,668,1025,-1036,-1570,1650,2547,-2971,-5686,4361,18361,18361,4361,-5686,-2971,2547,1650,-1570,-1036,1025,668,-672,-423,432,260,-269,-149,157,81,-87,-36,39,18,-21,0};
static mykonosFir_t obsrxFir =
{
    0,              /* Filter gain in dB*/
    48,             /* Number of coefficients in the FIR filter*/
    &obsrxFirCoefs[0]/* A pointer to an array of filter coefficients*/
};

static mykonosJesd204bFramerConfig_t rxFramer =
{
    0,              /* JESD204B Configuration Bank ID -extension to Device ID (Valid 0..15)*/
    0,              /* JESD204B Configuration Device ID - link identification number. (Valid 0..255)*/
    0,              /* JESD204B Configuration starting Lane ID.  If more than one lane used, each lane will increment from the Lane0 ID. (Valid 0..31)*/
    2,              /* number of ADCs (0, 2, or 4) - 2 ADCs per receive chain*/
    32,             /* number of frames in a multiframe (default=32), F*K must be a multiple of 4. (F=2*M/numberOfLanes)*/
    1,              /* scrambling off if framerScramble= 0, if framerScramble>0 scramble is enabled.*/
    1,              /* 0=use internal SYSREF, 1= use external SYSREF*/
    0x03,           /* serializerLanesEnabled - bit per lane, [0] = Lane0 enabled, [1] = Lane1 enabled*/
    0x04,           /* serializerLaneCrossbar*/
    22,             /* serializerAmplitude - default 22 (valid (0-31)*/
    4,              /* preEmphasis - < default 4 (valid 0 - 7)*/
    0,              /* invertLanePolarity - default 0 ([0] will invert lane [0], bit1 will invert lane1)*/
    0,              /* lmfcOffset - LMFC_Offset offset value for deterministic latency setting*/
    0,              /* Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, 0 = not set*/
    0,              /* Flag for determining if auto channel select for the xbar should be set. Where, if > 0 = set, '0' = not set*/
    0,              /* Selects SYNCb input source. Where, 0 = use RXSYNCB for this framer, 1 = use OBSRX_SYNCB for this framer*/
    0,              /* Flag for determining if CMOS mode for RX Sync signal is used. Where, if > 0 = CMOS, '0' = LVDS*/
    0,              /* Selects framer bit repeat or oversampling mode for lane rate matching. Where, 0 = bitRepeat mode (changes effective lanerate), 1 = overSample (maintains same lane rate between ObsRx framer and Rx framer and oversamples the ADC samples)*/
    0               /* Flag for determining if API will calculate the appropriate settings for framer lane outputs to physical lanes. Where, if '0' = API will set automatic lane crossbar, '1' = set to manual mode and the value in serializerLaneCrossbar will be used*/
};

static mykonosJesd204bFramerConfig_t obsRxFramer =
{
    0,              /* JESD204B Configuration Bank ID -extension to Device ID (Valid 0..15)*/
    0,              /* JESD204B Configuration Device ID - link identification number. (Valid 0..255)*/
    0,              /* JESD204B Configuration starting Lane ID.  If more than one lane used, each lane will increment from the Lane0 ID. (Valid 0..31)*/
    2,              /* number of ADCs (0, 2, or 4) - 2 ADCs per receive chain*/
    32,             /* number of frames in a multiframe (default=32), F*K must be a multiple of 4. (F=2*M/numberOfLanes)*/
    1,              /* scrambling off if framerScramble= 0, if framerScramble>0 scramble is enabled.*/
    1,              /* 0=use internal SYSREF, 1= use external SYSREF*/
    0x0C,           /* serializerLanesEnabled - bit per lane, [0] = Lane0 enabled, [1] = Lane1 enabled*/
    0x40,           /* Lane crossbar to map framer lane outputs to physical lanes*/
    22,             /* serializerAmplitude - default 22 (valid (0-31)*/
    4,              /* preEmphasis - < default 4 (valid 0 - 7)*/
    0,              /* invertLanePolarity - default 0 ([0] will invert lane [0], bit1 will invert lane1)*/
    0,              /* lmfcOffset - LMFC_Offset offset value for deterministic latency setting*/
    0,              /* Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, 0 = not set*/
    0,              /* Flag for determining if auto channel select for the xbar should be set. Where, if > 0 = set, '0' = not set*/
    1,              /* Selects SYNCb input source. Where, 0 = use RXSYNCB for this framer, 1 = use OBSRX_SYNCB for this framer*/
    0,              /* Flag for determining if CMOS mode for RX Sync signal is used. Where, if > 0 = CMOS, '0' = LVDS*/
    1,              /* Selects framer bit repeat or oversampling mode for lane rate matching. Where, 0 = bitRepeat mode (changes effective lanerate), 1 = overSample (maintains same lane rate between ObsRx framer and Rx framer and oversamples the ADC samples)*/
    0               /* Flag for determining if API will calculate the appropriate settings for framer lane outputs to physical lanes. Where, if '0' = API will set automatic lane crossbar, '1' = set to manual mode and the value in serializerLaneCrossbar will be used*/
};

static mykonosJesd204bDeframerConfig_t deframer =
{
    0,              /* bankId extension to Device ID (Valid 0..15)*/
    0,              /* deviceId  link identification number. (Valid 0..255)*/
    0,              /* lane0Id Lane0 ID. (Valid 0..31)*/
    2,              /* M  number of DACss (0, 2, or 4) - 2 DACs per transmit chain */
    32,             /* K  #frames in a multiframe (default=32), F*K=multiple of 4. (F=2*M/numberOfLanes)*/
    1,              /* scramble  scrambling off if scramble= 0.*/
    1,              /* External SYSREF select. 0 = use internal SYSREF, 1 = external SYSREF*/
    0x0F,           /* Deserializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
    0xE4,           /* Lane crossbar to map physical lanes to deframer lane inputs [1:0] = Deframer Input 0 Lane section, [3:2] = Deframer Input 1 lane select, etc */
    1,              /* Equalizer setting. Applied to all deserializer lanes. Range is 0..4*/
    0,              /* PN inversion per each lane.  bit[0] = 1 Invert PN of Lane 0, bit[1] = Invert PN of Lane 1, etc).*/
    0,              /* LMFC_Offset offset value to adjust deterministic latency. Range is 0..31*/
    0,              /* Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, '0' = not set*/
    0,              /* Flag for determining if auto channel select for the xbar should be set. Where, if > 0 = set, '0' = not set*/
    0,              /* Flag for determining if CMOS mode for TX Sync signal is used. Where, if > 0 = CMOS, '0' = LVDS*/
    0               /* Flag for determining if API will calculate the appropriate settings for deframer lane in to physical lanes. Where, if '0' = API will set automatic lane crossbar, '1' = set to manual mode and the value in deserializerLaneCrossbar will be used*/
};

static mykonosRxGainControl_t rxGainControl =
{
    MGC,            /* Current Rx gain control mode setting*/
    255,            /* Rx1 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* Rx2 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* Max gain index for the currently loaded Rx1 Gain table*/
    195,            /* Min gain index for the currently loaded Rx1 Gain table*/
    255,            /* Max gain index for the currently loaded Rx2 Gain table*/
    195,            /* Min gain index for the currently loaded Rx2 Gain table*/
    0,              /* Stores Rx1 RSSI value read back from the Mykonos*/
    0               /* Stores Rx2 RSSI value read back from the Mykonos*/
};

static mykonosORxGainControl_t orxGainControl =
{
    MGC,            /* Current ORx gain control mode setting*/
    255,            /* ORx1 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* ORx2 Gain Index, can be used in different ways for manual and AGC gain control*/
    255,            /* Max gain index for the currently loaded ORx Gain table*/
    237             /* Min gain index for the currently loaded ORx Gain table*/
};

static mykonosSnifferGainControl_t snifferGainControl =
{
    MGC,            /* Current Sniffer gain control mode setting*/
    255,            /* Current Sniffer gain index. Can be used differently for MANUAL Gain control/AGC*/
    255,            /* Max gain index for the currently loaded Sniffer Gain table*/
    203             /* Min gain index for the currently loaded Sniffer Gain table*/
};

static mykonosPeakDetAgcCfg_t rxPeakAgc =
{
    0x00,	/* apdHighThresh: */
    0x00,	/* apdLowThresh */
    0x00,	/* hb2HighThresh */
    0x00,	/* hb2LowThresh */
    0x00,	/* hb2VeryLowThresh */
    0x00,	/* apdHighThreshExceededCnt */
    0x00,	/* apdLowThreshExceededCnt */
    0x00,	/* hb2HighThreshExceededCnt */
    0x00,	/* hb2LowThreshExceededCnt */
    0x00,	/* hb2VeryLowThreshExceededCnt */
    0x0,	/* apdHighGainStepAttack */
    0x0,	/* apdLowGainStepRecovery */
    0x0,	/* hb2HighGainStepAttack */
    0x0,	/* hb2LowGainStepRecovery */
    0x0,	/* hb2VeryLowGainStepRecovery */
    0x0,	/* apdFastAttack */
    0x0,	/* hb2FastAttack */
    0x0,	/* hb2OverloadDetectEnable */
    0x0,	/* hb2OverloadDurationCnt */
    0x0	/* hb2OverloadThreshCnt */
};

static mykonosPowerMeasAgcCfg_t rxPwrAgc =
{
    0x00,	/* pmdUpperHighThresh */
    0x00,	/* pmdUpperLowThresh */
    0x00,	/* pmdLowerHighThresh */
    0x00,	/* pmdLowerLowThresh */
    0x0,	/* pmdUpperHighGainStepAttack */
    0x0,	/* pmdUpperLowGainStepAttack */
    0x0,	/* pmdLowerHighGainStepRecovery */
    0x0,	/* pmdLowerLowGainStepRecovery */
    0x00,	/* pmdMeasDuration */
    0x00	/* pmdMeasConfig */
};

static mykonosAgcCfg_t rxAgcConfig =
{
    0,	/* agcRx1MaxGainIndex */
    0,	/* agcRx1MinGainIndex */
    0,	/* agcRx2MaxGainIndex */
    0,	/* agcRx2MinGainIndex: */
    0,	/* agcObsRxMaxGainIndex */
    0,	/* agcObsRxMinGainIndex */
    0,		/* agcObsRxSelect */
    0,		/* agcPeakThresholdMode */
    0,		/* agcLowThsPreventGainIncrease */
    0,	/* agcGainUpdateCounter */
    0,	/* agcSlowLoopSettlingDelay */
    0,	/* agcPeakWaitTime */
    0,	/* agcResetOnRxEnable */
    0,	/* agcEnableSyncPulseForGainCounter */
    &rxPeakAgc,
    &rxPwrAgc
};

static mykonosPeakDetAgcCfg_t obsRxPeakAgc =
{
    0x00,	/* apdHighThresh: */
    0x00,	/* apdLowThresh */
    0x00,	/* hb2HighThresh */
    0x00,	/* hb2LowThresh */
    0x00,	/* hb2VeryLowThresh */
    0x00,	/* apdHighThreshExceededCnt */
    0x00,	/* apdLowThreshExceededCnt */
    0x00,	/* hb2HighThreshExceededCnt */
    0x00,	/* hb2LowThreshExceededCnt */
    0x00,	/* hb2VeryLowThreshExceededCnt */
    0x0,	/* apdHighGainStepAttack */
    0x0,	/* apdLowGainStepRecovery */
    0x0,	/* hb2HighGainStepAttack */
    0x0,	/* hb2LowGainStepRecovery */
    0x0,	/* hb2VeryLowGainStepRecovery */
    0x0,	/* apdFastAttack */
    0x0,	/* hb2FastAttack */
    0x0,	/* hb2OverloadDetectEnable */
    0x0,	/* hb2OverloadDurationCnt */
    0x0		/* hb2OverloadThreshCnt */
};

static mykonosPowerMeasAgcCfg_t obsRxPwrAgc =
{
    0x00,	/* pmdUpperHighThresh */
    0x00,	/* pmdUpperLowThresh */
    0x00,	/* pmdLowerHighThresh */
    0x00,	/* pmdLowerLowThresh */
    0x0,	/* pmdUpperHighGainStepAttack */
    0x0,	/* pmdUpperLowGainStepAttack */
    0x0,	/* pmdLowerHighGainStepRecovery */
    0x0,	/* pmdLowerLowGainStepRecovery */
    0x00,	/* pmdMeasDuration */
    0x00	/* pmdMeasConfig */
};

static mykonosAgcCfg_t obsRxAgcConfig =
{
    0,	/* agcRx1MaxGainIndex */
    0,	/* agcRx1MinGainIndex */
    0,	/* agcRx2MaxGainIndex */
    0,	/* agcRx2MinGainIndex: */
    0,	/* agcObsRxMaxGainIndex */
    0,	/* agcObsRxMinGainIndex */
    0,		/* agcObsRxSelect */
    0,		/* agcPeakThresholdMode */
    0,		/* agcLowThsPreventGainIncrease */
    0,	/* agcGainUpdateCounter */
    0,		/* agcSlowLoopSettlingDelay */
    0,		/* agcPeakWaitTime */
    0,		/* agcResetOnRxEnable */
    0,		/* agcEnableSyncPulseForGainCounter */
    &obsRxPeakAgc,
    &obsRxPwrAgc
};

static uint16_t rxAdcCustom[]={800,477,201,98,1280,112,1505,53,996,16,614,40,48,30,17,195};

static mykonosRxProfile_t rxProfile =
{/* RX 11MHz, Iqrate 23.04MSPS, DEC5 */
    1,              /* The divider used to generate the ADC clock*/
    &rxFir,         /* Pointer to Rx FIR filter structure*/
    4,              /* Rx FIR decimation (1,2,4)*/
    5,              /* Decimation of Dec5 or Dec4 filter (5,4)*/
    1,              /* If set, and DEC5 filter used, will use a higher rejection DEC5 FIR filter (1=Enabled, 0=Disabled)*/
    2,              /* RX Half band 1 decimation (1 or 2)*/
    23040,          /* Rx IQ data rate in kHz*/
    11520000,       /* The Rx RF passband bandwidth for the profile*/
    20000,          /* Rx BBF 3dB corner in kHz*/
    &rxAdcCustom[0] /* pointer to custom ADC profile*/
};
static uint16_t orxAdcCustom[]={713,515,201,98,1280,491,1591,279,977,78,593,28,48,29,17,192};

static mykonosRxProfile_t orxProfile =
{/* ORX 75MHz, Iqrate 92.16MSPS, DEC5 */
    1,              /* The divider used to generate the ADC clock*/
    &obsrxFir,      /* Pointer to Rx FIR filter structure or NULL*/
    2,              /* Rx FIR decimation (1,2,4)*/
    5,              /* Decimation of Dec5 or Dec4 filter (5,4)*/
    0,              /* If set, and DEC5 filter used, will use a higher rejection DEC5 FIR filter (1=Enabled, 0=Disabled)*/
    1,              /* RX Half band 1 decimation (1 or 2)*/
    92160,          /* Rx IQ data rate in kHz*/
    75000000,       /* The Rx RF passband bandwidth for the profile*/
    37500,          /* Rx BBF 3dB corner in kHz*/
    &orxAdcCustom[0] /* pointer to custom ADC profile*/
};



static mykonosTxProfile_t txProfile =
{ /* TX 20/20MHz, Iqrate 92.16MSPS, DEC5 */
    DACDIV_2p5,     /* The divider used to generate the DAC clock*/
    &txFir,         /* Pointer to Tx FIR filter structure*/
    1,              /* The Tx digital FIR filter interpolation (1,2,4)*/
    2,              /* Tx Halfband1 filter interpolation (1,2)*/
    2,              /* Tx Halfband2 filter interpolation (1,2)*/
    1,              /* TxInputHbInterpolation (1,2)*/
    92160,          /* Tx IQ data rate in kHz*/
    20000000,       /* Primary Signal BW*/
    20000000,       /* The Tx RF passband bandwidth for the profile*/
    92000,          /* The DAC filter 3dB corner in kHz*/
    20000,          /* Tx BBF 3dB corner in kHz*/
    0               /* Enable DPD, only valid for AD9373*/
};

static mykonosDigClocks_t mykonosClocks =
{
    76800,          /* CLKPLL and device reference clock frequency in kHz*/
    11059200,       /* CLKPLL VCO frequency in kHz*/
    VCODIV_3,       /* CLKPLL VCO divider*/
    4               /* CLKPLL high speed clock divider*/
};

static mykonosRxSettings_t  rxSettings =
{
    &rxProfile,     /* Rx datapath profile, 3dB corner frequencies, and digital filter enables*/
    &rxFramer,      /* Rx JESD204b framer configuration structure*/
    &rxGainControl, /* Rx Gain control settings structure*/
    &rxAgcConfig,   /* Rx AGC control settings structure*/
    1,              /* The desired Rx Channels to enable during initialization*/
    0,              /* Internal LO = 0, external LO*2 = 1*/
    2550000000U,    /* Rx PLL LO Frequency (internal or external LO)*/
    0               /* Flag to choose if complex baseband or real IF data are selected for Rx and ObsRx paths. Where, if > 0 = real IF data, '0' = zero IF (IQ) data*/
};

static mykonosDpdConfig_t dpdConfig =
{
    5,              /* 1/2^(damping + 8) fraction of power `forgotten' per sample (default: `1/8192' = 5, valid 0 to 15), 0 = infinite damping*/
    1,              /* number of weights to use for int8_cpx weights weights member of this structure (default = 1)*/
    2,              /* DPD model version: one of four different generalized polynomial models: 0 = same as R0 silicon, 1-3 are new and the best one depends on the PA (default: 2)*/
    1,              /* 1 = Update saved model whenever peak Tx digital RMS is within 1dB of historical peak Tx RMS*/
    20,             /* Determines how much weight the loaded prior model has on DPD modeling (Valid 0 - 32, default 20)*/
    0,              /* Default off = 0, 1=enables automatic outlier removal during DPD modeling */
    512,            /* Number of samples to capture (default: 512, valid 64-32768)*/
    4096,           /* threshold for sample in AM-AM plot outside of 1:1 line to be thrown out. (default: 50% = 8192/2, valid 8192 to 1)*/
    0,              /* 16th of an ORx sample (16=1sample), (default 0, valid -64 to 64)*/
    255,            /* Default 255 (-30dBFs=(20Log10(value/8192)), (valid range  1 to 8191)*/
    {{64,0},{0,0},{0,0}}/* DPD model error weighting (real/imag valid from -128 to 127)*/
};

static mykonosClgcConfig_t clgcConfig =
{
    -2000,          /* (value = 100 * dB (valid range -32768 to 32767) - total gain and attenuation from Mykonos Tx1 output to ORx1 input in (dB * 100)*/
    -2000,          /* (value = 100 * dB (valid range -32768 to 32767) - total gain and attenuation from Mykonos Tx2 output to ORx2 input in (dB * 100)*/
    0,              /* (valid range 0 - 40dB), no default, depends on PA, Protects PA by making sure Tx1Atten is not reduced below the limit*/
    0,              /* (valid range 0 - 40dB), no default, depends on PA, Protects PA by making sure Tx2Atten is not reduced below the limit*/
    75,             /* valid range 1-100, default 75*/
    75,             /* valid range 1-100, default 45*/
    0,              /* 0= allow CLGC to run, but Tx1Atten will not be updated. User can still read back power measurements.  1=CLGC runs, and Tx1Atten automatically updated*/
    0,              /* 0= allow CLGC to run, but Tx2Atten will not be updated. User can still read back power measurements.  1=CLGC runs, and Tx2Atten automatically updated*/
    0,              /* 16th of an ORx sample (16=1sample), (default 0, valid -64 to 64)*/
    255,            /* Default 255 (-30dBFs=(20Log10(value/8192)), (valid range  1 to 8191)*/
    600,            /* Threshold for Tx1 in order to stop tracking, value = 100 * dB, default 6db then value = 600*/
    600,            /* Threshold for Tx2 in order to stop tracking, value = 100 * dB, default 6db then value = 600*/
    0,              /* Threshold feature enable for Tx1, 0 = disable, 1 = enable, default = 0*/
    0               /* Threshold feature enable for Tx1, 0 = disable, 1 = enable, default = 0*/
};

static mykonosVswrConfig_t vswrConfig =
{
    0,              /* 16th of an ORx sample (16=1sample), (default 0, valid -64 to 64)*/
    255,            /* Default 255 (-30dBFs=(20Log10(value/8192)), (valid range  1 to 8191)*/
    0,              /* 3p3V GPIO pin to use to control VSWR switch for Tx1 (valid 0-11) (output from Mykonos)*/
    1,              /* 3p3V GPIO pin to use to control VSWR switch for Tx2 (valid 0-11) (output from Mykonos)*/
    0,              /* 3p3v GPIO pin polarity for forward path of Tx1, opposite used for reflection path (1 = high level, 0 = low level)*/
    0,              /* 3p3v GPIO pin polarity for forward path of Tx2, opposite used for reflection path (1 = high level, 0 = low level)*/
    50,             /* Delay for Tx1 after flipping the VSWR switch until measurement is made. In us resolution*/
    50              /* Delay for Tx2 after flipping the VSWR switch until measurement is made. In us resolution*/
};

static mykonosTxSettings_t txSettings =
{
    &txProfile,     /* Tx datapath profile, 3dB corner frequencies, and digital filter enables*/
    &deframer,      /* Mykonos JESD204b deframer config for the Tx data path*/
    TX1,            /* The desired Tx channels to enable during initialization*/
    0,              /* Internal LO=0, external LO*2 if =1*/
    2500000000U,    /* Tx PLL LO frequency (internal or external LO)*/
    TXATTEN_0P05_DB,/* Initial and current Tx1 Attenuation*/
    10000,          /* Initial and current Tx1 Attenuation mdB*/
    10000,          /* Initial and current Tx2 Attenuation mdB*/
    NULL,           /* DPD,CLGC,VSWR settings. Only valid for AD9373 device, set pointer to NULL otherwise*/
    NULL,           /* CLGC Config Structure. Only valid for AD9373 device, set pointer to NULL otherwise*/
    NULL            /* VSWR Config Structure. Only valid for AD9373 device, set pointer to NULL otherwise*/
};

static uint16_t lpbkAdcCustom[]={800,477,201,98,1280,112,1505,53,996,16,614,40,48,30,17,195};

static mykonosObsRxSettings_t obsRxSettings =
{
    &orxProfile,    /* ORx datapath profile, 3dB corner frequencies, and digital filter enables*/
    &orxGainControl,/* ObsRx gain control settings structure*/
    &obsRxAgcConfig,/* ORx AGC control settings structure*/
    NULL,/* Sniffer datapath profile, 3dB corner frequencies, and digital filter enables*/
    NULL,/* SnRx gain control settings structure*/
    &obsRxFramer,   /* ObsRx JESD204b framer configuration structure */
    (MYK_ORX1_ORX2 | MYK_OBS_RXOFF),/* obsRxChannel */
    OBSLO_TX_PLL,   /* (obsRxLoSource) The Obs Rx mixer can use the Tx Synth(TX_PLL) or Sniffer Synth (SNIFFER_PLL) */
    2600000000U,    /* SnRx PLL LO frequency in Hz */
    0,              /* Flag to choose if complex baseband or real IF data are selected for Rx and ObsRx paths. Where if > 0 = real IF data, '0' = complex data*/
    &lpbkAdcCustom[0],/* Custom Loopback ADC profile to set the bandwidth of the ADC response*/
    OBS_RXOFF       /* Default ObsRx channel to enter when radioOn called */
};

static mykonosArmGpioConfig_t armGpio =
{
    0,	// useRx2EnablePin; /*!< 0= RX1_ENABLE controls RX1 and RX2, 1 = separate RX1_ENABLE/RX2_ENABLE pins */
    0,	// useTx2EnablePin; /*!< 0= TX1_ENABLE controls TX1 and TX2, 1 = separate TX1_ENABLE/TX2_ENABLE pins */
    0,	// txRxPinMode;     /*!< 0= ARM command mode, 1 = Pin mode to power up Tx/Rx chains */
    0,	// orxPinMode;      /*!< 0= ARM command mode, 1 = Pin mode to power up ObsRx receiver*/

    /*Mykonos ARM input GPIO pins -- Only valid if orxPinMode = 1 */
    0,	// orxTriggerPin; /*!< Select desired GPIO pin (valid 4-15) */
    0,	// orxMode2Pin;   /*!< Select desired GPIO pin (valid 0-18) */
    0,	// orxMode1Pin;   /*!< Select desired GPIO pin (valid 0-18) */
    0,	// orxMode0Pin;   /*!< Select desired GPIO pin (valid 0-18) */

    /* Mykonos ARM output GPIO pins  --  always available, even when pin mode not enabled*/
    0,	// rx1EnableAck;  /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,	// rx2EnableAck;  /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,	// tx1EnableAck;  /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,	// tx2EnableAck;  /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,	// orx1EnableAck;  /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,	// orx2EnableAck;  /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0,	// srxEnableAck;  /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    0 	// txObsSelect;   /*!< Select desired GPIO pin (0-15), [4] = Output Enable */
    /* When 2Tx are used with only 1 ORx input, this GPIO tells the BBIC which Tx channel is   */
    /* active for calibrations, so BBIC can route correct RF Tx path into the single ORx input*/
};

static mykonosGpio3v3_t gpio3v3 =
{
    0,				/*!< Oe per pin, 1=output, 0 = input */
    GPIO3V3_BITBANG_MODE,	/*!< Mode for GPIO3V3[3:0] */
    GPIO3V3_BITBANG_MODE,	/*!< Mode for GPIO3V3[7:4] */
    GPIO3V3_BITBANG_MODE,	/*!< Mode for GPIO3V3[11:8] */
};

static mykonosGpioLowVoltage_t gpio =
{
    0,/* Oe per pin, 1=output, 0 = input */
    GPIO_MONITOR_MODE,/* Mode for GPIO[3:0] */
    GPIO_MONITOR_MODE,/* Mode for GPIO[7:4] */
    GPIO_MONITOR_MODE,/* Mode for GPIO[11:8] */
    GPIO_MONITOR_MODE,/* Mode for GPIO[15:12] */
    GPIO_MONITOR_MODE,/* Mode for GPIO[18:16] */
};

static mykonosAuxIo_t mykonosAuxIo =
{
    0,	//auxDacEnableMask uint16_t
    {0,0,0,0,0,0,0,0,0,0},	 //AuxDacValue uint16[10]
    {0,0,0,0,0,0,0,0,0,0},	 //AuxDacSlope uint8[10]
    {0,0,0,0,0,0,0,0,0,0},	 //AuxDacVref uint8[10]
    &gpio3v3,	//pointer to gpio3v3 struct
    &gpio,	//pointer to gpio1v8 struct
    &armGpio
};

static spiSettings_t mykSpiSettings =
{
    1, /* chip select index - valid 1~8 */
    0, /* the level of the write bit of a SPI write instruction word, value is inverted for SPI read operation */
    1, /* 1 = 16-bit instruction word, 0 = 8-bit instruction word */
    1, /* 1 = MSBFirst, 0 = LSBFirst */
    0, /* clock phase, sets which clock edge the data updates (valid 0 or 1) */
    0, /* clock polarity 0 = clock starts low, 1 = clock starts high */
    0, /* Not implemented in ADIs platform layer. SW feature to improve SPI throughput */
    1, /* Not implemented in ADIs platform layer. For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr=addr-1 */
    1  /* 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
};

static mykonosTempSensorConfig_t tempSensor =
{
    7,              /* 3-bit value that controls the AuxADC decimation factor when used for temp sensor calculations; AuxADC_decimation = 256 * 2^tempDecimation*/
    67,             /* 8-bit offset that gets added to temp sensor code internally*/
    1,              /* this bit overrides the factory-calibrated fuse offset and uses the value stored in the offset member*/
    15,             /* 4-bit code with a resolution of 1°C/LSB, each time a temperature measurement is performed, the device compares the current temperature against the previous value.*/
};

static mykonosTempSensorStatus_t tempStatus =
{
    0,              /* 16-bit signed temperature value (in deg C) that is read back*/
    0,              /* If the absolute value of the difference is greater than the value in temperature configuration tempWindow, the windowExceeded flag is set.*/
    0,              /* when windowExceeded member gets set, this bit is set to 1 if current value is greater than previous value, else reset*/
    0,              /* when the reading is complete and a valid temperature value stored in tempCode.*/
};

/*static*/ mykonosDevice_t mykDevice =
{
    &mykSpiSettings,    /* SPI settings data structure pointer */
    &rxSettings,        /* Rx settings data structure pointer */
    &txSettings,        /* Tx settings data structure pointer */
    &obsRxSettings,     /* ObsRx settings data structure pointer */
    &mykonosAuxIo,          /* Auxiliary IO settings data structure pointer */
    &mykonosClocks,     /* Holds settings for CLKPLL and reference clock */
    0                   /* Mykonos initialize function uses this as an output to remember which profile data structure pointers are valid */
};