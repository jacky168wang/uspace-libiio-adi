/**
 * \file talise_config.c
 * \brief Contains Talise configuration settings for the Talise API
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 * The top level structure taliseDevice_t talDevice uses keyword
 * extern to allow the application layer main() to have visibility
 * to these settings.
 *
 * This file may not be fully complete for the end user application and 
 * may need to updated for AGC, GPIO, and DAC full scale settings. 
 * To create a full initialisation routine, the user should also refer to the 
 * Iron Python initialisation routine generated by the GUI, and also the Talise User Guide.
 *
 */

#include "talise_types.h"
#include "talise_config.h"
#include "talise_error.h"
#ifdef ADI_ZYNQ_PLATFORM
#include "zynq_platform.h"
#endif

int16_t txFirCoefs[20] = {-39, 24, -62, 112, -175, 298, -574, 1284, -2814, 20281, -2814, 1284, -574, 298, -175, 112, -62, 24, -39, 0};

int16_t rxFirCoefs[72] = {0, 0, 2, 0, -7, -1, 15, 4, -31, -10, 57, 23, -99, -47, 161, 86, -247, -146, 372, 238, -542, -381, 762, 583, -1064, -883, 1470, 1331, -2057, -2059, 2975, 3403, -4748, -6898, 9698, 30806, 30806, 9698, -6898, -4748, 3403, 2975, -2059, -2057, 1331, 1470, -883, -1064, 583, 762, -381, -542, 238, 372, -146, -247, 86, 161, -47, -99, 23, 57, -10, -31, 4, 15, -1, -7, 0, 2, 0, 0};

int16_t obsrxFirCoefs[24] = {-15, 9, -13, 19, -28, 38, -73, 110, -195, 409, -1006, 17711, -1006, 409, -195, 110, -73, 38, -28, 19, -13, 9, -15, 0};

#ifdef ADI_ZYNQ_PLATFORM /** < Insert Customer Platform HAL State Container here>*/
/*
 * Platform Layer SPI settings - this structure is specific to ADI's platform layer code.
 * User should replace with their own structure or settings for their hardware
 */
zynqSpiSettings_t spiDev1 =
{
	.chipSelectIndex = 1,
	.writeBitPolarity = 0,
	.longInstructionWord = 1,
	.CPHA = 0,
	.CPOL = 0,
	.mode = 0,
	.spiClkFreq_Hz = 25000000
};

/*
 * Platform Layer settings - this structure is specific to ADI's platform layer code.
 * User should replace with their own structure or settings for their hardware
 * The structure is held in taliseDevice_t below as a void pointer, allowing
 * the customer to pass any information for their specific hardware down to the
 * hardware layer code.
 */
zynqAdiDev_t talDevHalInfo =
{
	.devIndex = 1,
	.spiSettings = &spiDev1,
	.spiErrCode = 0,
	.timerErrCode = 0,
	.gpioErrCode = 0,
	.logLevel = ADIHAL_LOG_ALL
};
#endif
/**
 *  TalDevice a structure used by the Talise API to hold the platform hardware
 *  structure information, as well as an internal Talise API state container
 *  (devStateInfo) of runtime information used by the API.
 **/
taliseDevice_t talDevice =
{
#ifdef ADI_ZYNQ_PLATFORM
    /* Void pointer of users platform HAL settings to pass to HAL layer calls
     * Talise API does not use the devHalInfo member */
	.devHalInfo = &talDevHalInfo,
#else
	.devHalInfo = NULL,     /*/** < Insert Customer Platform HAL State Container here>*/
#endif
	/* devStateInfo is maintained internal to the Talise API, just create the memory */
	.devStateInfo = {0}

};

taliseInit_t talInit =
{
	/* SPI settings */
    .spiSettings =
    {
		.MSBFirst            = 1,  /* 1 = MSBFirst, 0 = LSBFirst */
		.enSpiStreaming      = 0,  /* Not implemented in ADIs platform layer. SW feature to improve SPI throughput */
		.autoIncAddrUp       = 1,  /* Not implemented in ADIs platform layer. For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr=addr-1 */
		.fourWireMode        = 1,  /* 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
		.cmosPadDrvStrength  = TAL_CMOSPAD_DRV_2X /* Drive strength of CMOS pads when used as outputs (SDIO, SDO, GP_INTERRUPT, GPIO 1, GPIO 0) */
	},
	
    /* Rx settings */
    .rx = 
    {
        .rxProfile =
        {
            .rxFir = 
            {
                .gain_dB = -6,                /* filter gain */
                .numFirCoefs = 72,            /* number of coefficients in the FIR filter */
                .coefs = &rxFirCoefs[0]
            },
            .rxFirDecimation = 2,            /* Rx FIR decimation (1,2,4) */
            .rxDec5Decimation = 4,            /* Decimation of Dec5 or Dec4 filter (5,4) */
            .rhb1Decimation = 2,            /* RX Half band 1 decimation (1 or 2) */
            .rxOutputRate_kHz = 122880,            /* Rx IQ data rate in kHz */
            .rfBandwidth_Hz = 100000000,    /* The Rx RF passband bandwidth for the profile */
            .rxBbf3dBCorner_kHz = 100000,    /* Rx BBF 3dB corner in kHz */
            .rxAdcProfile = {265, 146, 181, 90, 1280, 366, 1257, 27, 1258, 17, 718, 39, 48, 46, 27, 161, 0, 0, 0, 0, 40, 0, 7, 6, 42, 0, 7, 6, 42, 0, 25, 27, 0, 0, 25, 27, 0, 0, 165, 44, 31, 905},            /* pointer to custom ADC profile */
            .rxDdcMode = TAL_RXDDC_BYPASS,   /* Rx DDC mode */
            .rxNcoShifterCfg =
            {
                .bandAInputBandWidth_kHz = 0,
                .bandAInputCenterFreq_kHz = 0,
                .bandANco1Freq_kHz = 0,
                .bandANco2Freq_kHz = 0,
                .bandBInputBandWidth_kHz = 0,
                .bandBInputCenterFreq_kHz = 0,
                .bandBNco1Freq_kHz = 0,
                .bandBNco2Freq_kHz = 0
            }
        },
        .framerSel = TAL_FRAMER_A,            /* Rx JESD204b framer configuration */
        .rxGainCtrl = 
        {
            .gainMode = TAL_MGC,            /* taliserxGainMode_t gainMode */
            .rx1GainIndex = 255,            /* uint8_t rx1GainIndex */
            .rx2GainIndex = 255,            /* uint8_t rx2GainIndex */
            .rx1MaxGainIndex = 255,            /* uint8_t rx1MaxGainIndex */
            .rx1MinGainIndex = 195,            /* uint8_t rx1MinGainIndex */
            .rx2MaxGainIndex = 255,            /* uint8_t rx2MaxGainIndex */
            .rx2MinGainIndex = 195            /* uint8_t rx2MinGainIndex */
        },
        .rxChannels = TAL_RX1RX2,                /* The desired Rx Channels to enable during initialization */
    },


    /* Tx settings */
    .tx = 
    {
        .txProfile =
        {
            .dacDiv = 1,                        /* The divider used to generate the DAC clock */
            .txFir = 
            {
                .gain_dB = 6,                        /* filter gain */
                .numFirCoefs = 20,                    /* number of coefficients in the FIR filter */
                .coefs = &txFirCoefs[0]
            },
            .txFirInterpolation = 1,                    /* The Tx digital FIR filter interpolation (1,2,4) */
            .thb1Interpolation = 2,                    /* Tx Halfband1 filter interpolation (1,2) */
            .thb2Interpolation = 2,                    /* Tx Halfband2 filter interpolation (1,2)*/
            .thb3Interpolation = 1,                    /* Tx Halfband3 filter interpolation (1,2)*/
            .txInt5Interpolation = 1,                    /* Tx Int5 filter interpolation (1,5) */
            .txInputRate_kHz = 491520,                    /* Primary Signal BW */
            .primarySigBandwidth_Hz = 200000000,    /* The Rx RF passband bandwidth for the profile */
            .rfBandwidth_Hz = 450000000,            /* The Tx RF passband bandwidth for the profile */
            .txDac3dBCorner_kHz = 450000,                /* The DAC filter 3dB corner in kHz */
            .txBbf3dBCorner_kHz = 225000,                /* The BBF 3dB corner in kHz */
            .loopBackAdcProfile = {150, 178, 181, 90, 1280, 1338, 1699, 493, 1386, 162, 934, 4, 48, 46, 35, 201, 0, 0, 0, 0, 50, 0, 0, 6, 24, 0, 0, 6, 24, 0, 25, 27, 0, 0, 25, 27, 0, 0, 165, 44, 15, 905}
        },
        .deframerSel = TAL_DEFRAMER_A,                    /* Talise JESD204b deframer config for the Tx data path */
        .txChannels = TAL_TX1TX2,                            /* The desired Tx channels to enable during initialization */
        .txAttenStepSize = TAL_TXATTEN_0P05_DB,            /* Tx Attenuation step size */
        .tx1Atten_mdB = 0,                            /* Initial Tx1 Attenuation */
        .tx2Atten_mdB = 0,                            /* Initial Tx2 Attenuation */
        .disTxDataIfPllUnlock = TAL_TXDIS_TX_RAMP_DOWN_TO_ZERO    /* Options to disable the transmit data when the RFPLL unlocks. */
    },


    /* ObsRx settings */
    .obsRx = 
    {
        .orxProfile =
        {
            .rxFir = 
            {
                .gain_dB = 6,                /* filter gain */
                .numFirCoefs = 24,            /* number of coefficients in the FIR filter */
                .coefs = &obsrxFirCoefs[0]
            },
            .rxFirDecimation = 1,            /* Rx FIR decimation (1,2,4) */
            .rxDec5Decimation = 4,            /* Decimation of Dec5 or Dec4 filter (5,4) */
            .rhb1Decimation = 1,            /* RX Half band 1 decimation (1 or 2) */
            .orxOutputRate_kHz = 491520,            /* Rx IQ data rate in kHz */
            .rfBandwidth_Hz = 450000000,    /* The Rx RF passband bandwidth for the profile */
            .rxBbf3dBCorner_kHz = 225000,    /* Rx BBF 3dB corner in kHz */
            .orxLowPassAdcProfile = {155, 163, 181, 90, 1280, 1254, 1572, 340, 1431, 142, 973, 8, 48, 47, 36, 205, 0, 0, 0, 0, 51, 0, 0, 6, 24, 0, 0, 6, 24, 0, 25, 27, 0, 0, 25, 27, 0, 0, 165, 44, 15, 905},
            .orxBandPassAdcProfile = {124, 131, 154, 90, 1280, 2779, 1986, 0, 1037, 839, 988, 109, 11, 15, 28, 179, 0, 0, 0, 0, 45, 0, 0, 0, 24, 0, 0, 0, 24, 0, 25, 27, 0, 0, 25, 27, 0, 0, 165, 44, 15, 905},
            .orxDdcMode = TAL_ORXDDC_DISABLED,   /* ORx DDC mode */
            .orxMergeFilter  = {-167, 419, -208, -498, 968, -320, -1273, 2154, -402, -4155, 9170, 21413}
        },
        .orxGainCtrl = 
        {
            .gainMode = TAL_MGC,
            .orx1GainIndex = 255,
            .orx2GainIndex = 255,
            .orx1MaxGainIndex = 255,
            .orx1MinGainIndex = 195,
            .orx2MaxGainIndex = 255,
            .orx2MinGainIndex = 195
        },
        .framerSel = TAL_FRAMER_B,                /* ObsRx JESD204b framer configuration */
        .obsRxChannelsEnable = TAL_ORX1ORX2,        /* The desired ObsRx Channels to enable during initialization */
        .obsRxLoSource = TAL_OBSLO_RF_PLL                /* The ORx mixers can use the TX_PLL */
    },

    /* Digital Clock Settings */
    .clocks = 
    {
        .deviceClock_kHz = 122880,            /* CLKPLL and device reference clock frequency in kHz */
        .clkPllVcoFreq_kHz = 9830400,        /* CLKPLL VCO frequency in kHz */
        .clkPllHsDiv = TAL_HSDIV_2P5,            /* CLKPLL high speed clock divider */
        .rfPllUseExternalLo = 0,                /* 1= Use external LO for RF PLL, 0 = use internal LO generation for RF PLL */
        .rfPllPhaseSyncMode = TAL_RFPLLMCS_NOSYNC                /* RFPLL MCS (Phase sync) mode */
    },

    /* JESD204B settings */
    .jesd204Settings = 
    {
        /* Framer A settings */
        .framerA = 
        {
            .bankId = 1,                    /* JESD204B Configuration Bank ID -extension to Device ID (Valid 0..15) */
            .deviceId = 0,                    /* JESD204B Configuration Device ID - link identification number. (Valid 0..255) */
            .lane0Id = 0,                    /* JESD204B Configuration starting Lane ID.  If more than one lane used, each lane will increment from the Lane0 ID. (Valid 0..31) */
            .M = 4,                            /* number of ADCs (0, 2, or 4) - 2 ADCs per receive chain */
            .K = 32,                        /* number of frames in a multiframe (default=32), F*K must be a multiple of 4. (F=2*M/numberOfLanes) */
            .F = 8,                            /* F (number of bytes per frame) */
            .Np = 16,                            /* Np (converter sample resolution) */
            .scramble = 1,                    /* scrambling off if framerScramble= 0, if framerScramble>0 scramble is enabled. */
            .externalSysref = 1,            /* 0=use internal SYSREF, 1= use external SYSREF */
            .serializerLanesEnabled = 0x02,    /* serializerLanesEnabled - bit per lane, [0] = Lane0 enabled, [1] = Lane1 enabled */
            .serializerLaneCrossbar = 0xE4,    /* serializerLaneCrossbar */
            .lmfcOffset = 31,                /* lmfcOffset - LMFC offset value for deterministic latency setting */
            .newSysrefOnRelink = 0,            /* newSysrefOnRelink */
            .syncbInSelect = 0,                /* syncbInSelect; */
            .overSample = 0,                    /* 1=overSample, 0=bitRepeat */
            .syncbInLvdsMode = 1,
            .syncbInLvdsPnInvert = 0,
            .enableManualLaneXbar = 0 /* 0=auto, 1=manual */
        },
        /* Framer B settings */
        .framerB = 
        {
            .bankId = 0,                    /* JESD204B Configuration Bank ID -extension to Device ID (Valid 0..15) */
            .deviceId = 0,                    /* JESD204B Configuration Device ID - link identification number. (Valid 0..255) */
            .lane0Id = 0,                    /* JESD204B Configuration starting Lane ID.  If more than one lane used, each lane will increment from the Lane0 ID. (Valid 0..31) */
            .M = 4,                            /* number of ADCs (0, 2, or 4) - 2 ADCs per receive chain */
            .K = 32,                        /* number of frames in a multiframe (default=32), F*K must be a multiple of 4. (F=2*M/numberOfLanes) */
            .F = 8,                            /* F (number of bytes per frame) */
            .Np = 16,                            /* Np (converter sample resolution) */
            .scramble = 1,                    /* scrambling off if framerScramble= 0, if framerScramble>0 scramble is enabled. */
            .externalSysref = 1,            /* 0=use internal SYSREF, 1= use external SYSREF */
            .serializerLanesEnabled = 0x01,    /* serializerLanesEnabled - bit per lane, [0] = Lane0 enabled, [1] = Lane1 enabled */
            .serializerLaneCrossbar = 0xE4,    /* serializerLaneCrossbar */
            .lmfcOffset = 31,                /* lmfcOffset - LMFC offset value for deterministic latency setting */
            .newSysrefOnRelink = 0,            /* newSysrefOnRelink */
            .syncbInSelect = 1,                /* syncbInSelect; */
            .overSample = 0,                    /* 1=overSample, 0=bitRepeat */
            .syncbInLvdsMode = 1,
            .syncbInLvdsPnInvert = 0,
            .enableManualLaneXbar = 0 /* 0=auto, 1=manual */
        },
        /* Deframer A settings */
        .deframerA = 
        {
            .bankId = 0,                    /* bankId extension to Device ID (Valid 0..15) */
            .deviceId = 0,                    /* deviceId  link identification number. (Valid 0..255) */
            .lane0Id = 0,                    /* lane0Id Lane0 ID. (Valid 0..31) */
            .M = 4,                            /* M  number of DACss (0, 2, or 4) - 2 DACs per transmit chain */
            .K = 32,                        /* K  #frames in a multiframe (default=32), F*K=multiple of 4. (F=2*M/numberOfLanes) */
            .scramble = 1,                    /* scramble  scrambling off if scramble= 0 */
            .externalSysref = 1,            /* externalSysref  0= use internal SYSREF, 1= external SYSREF */
            .deserializerLanesEnabled = 0x01,    /* deserializerLanesEnabled  bit per lane, [0] = Lane0 enabled */
            .deserializerLaneCrossbar = 0xE4,    /* deserializerLaneCrossbar */
            .lmfcOffset = 17,                /* lmfcOffset	 LMFC offset value to adjust deterministic latency */
            .newSysrefOnRelink = 0,            /* newSysrefOnRelink */
            .syncbOutSelect = 0,                /* SYNCBOUT0/1 select */
            .Np = 16,                /* Np (converter sample resolution) */
            .syncbOutLvdsMode = 1,
            .syncbOutLvdsPnInvert = 0,
            .syncbOutCmosSlewRate = 0,
            .syncbOutCmosDriveLevel = 0,
            .enableManualLaneXbar = 0 /* 0=auto, 1=manual */
        },
        /* Deframer B settings */
        .deframerB = 
        {
            .bankId = 0,                    /* bankId extension to Device ID (Valid 0..15) */
            .deviceId = 0,                    /* deviceId  link identification number. (Valid 0..255) */
            .lane0Id = 0,                    /* lane0Id Lane0 ID. (Valid 0..31) */
            .M = 0,                            /* M  number of DACss (0, 2, or 4) - 2 DACs per transmit chain */
            .K = 32,                        /* K  #frames in a multiframe (default=32), F*K=multiple of 4. (F=2*M/numberOfLanes) */
            .scramble = 1,                    /* scramble  scrambling off if scramble= 0 */
            .externalSysref = 1,            /* externalSysref  0= use internal SYSREF, 1= external SYSREF */
            .deserializerLanesEnabled = 0x00,    /* deserializerLanesEnabled  bit per lane, [0] = Lane0 enabled */
            .deserializerLaneCrossbar = 0xE4,    /* deserializerLaneCrossbar */
            .lmfcOffset = 0,                /* lmfcOffset	 LMFC offset value to adjust deterministic latency */
            .newSysrefOnRelink = 0,            /* newSysrefOnRelink */
            .syncbOutSelect = 1,                /* SYNCBOUT0/1 select */
            .Np = 16,                /* Np (converter sample resolution) */
            .syncbOutLvdsMode = 1,
            .syncbOutLvdsPnInvert = 0,
            .syncbOutCmosSlewRate = 0,
            .syncbOutCmosDriveLevel = 0,
            .enableManualLaneXbar = 0 /* 0=auto, 1=manual */
        },
        .serAmplitude = 15,                    /* Serializer amplitude setting. Default = 15. Range is 0..15 */
        .serPreEmphasis = 1,                /* Serializer pre-emphasis setting. Default = 1 Range is 0..4 */
        .serInvertLanePolarity = 0,            /* Serializer Lane PN inversion select. Default = 0. Where, bit[0] = 1 will invert lane [0], bit[1] = 1 will invert lane 1, etc. */
        .desInvertLanePolarity = 0,            /* Deserializer Lane PN inversion select.  bit[0] = 1 Invert PN of Lane 0, bit[1] = Invert PN of Lane 1, etc */
        .desEqSetting = 1,                    /* Deserializer Equalizer setting. Applied to all deserializer lanes. Range is 0..4 */
        .sysrefLvdsMode = 1,                /* Use LVDS inputs on Talise for SYSREF */
        .sysrefLvdsPnInvert = 0              /*0= Do not PN invert SYSREF */
    }
};
