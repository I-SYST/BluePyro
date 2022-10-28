/**-------------------------------------------------------------------------
@file	main.cpp

@brief	BluePyro demo firmware


@author	Nguyen Hoan Hoang
@date	July 25, 2020

@license

MIT License

Copyright (c) 2020 I-SYST inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

----------------------------------------------------------------------------*/
#include <string.h>
#include <math.h>
#include <atomic>
#include <stdio.h>

#include "app_util_platform.h"
#include "app_scheduler.h"
#include "nrf_sdh.h"
#include "fds.h"
//#include "nrf_power.h"

#include "istddef.h"
#include "idelay.h"
#include "bluetooth/ble_app.h"
#include "ble_app_nrf5.h"
#include "bluetooth/ble_srvc.h"
#include "bluetooth/blueio_blesrvc.h"
#include "blueio_board.h"
#include "coredev/uart.h"
#include "custom_board.h"
#include "coredev/iopincfg.h"
#include "iopinctrl.h"
#include "excelitas_serlink.h"
#include "interrupt.h"
#include "coredev/timer.h"
#include "miscdev/led.h"

#include "blue_pyro.h"
#include "board.h"

void PydIntHandler(int IntNo, void *pCtx);

#define BLUEPYRO_VERS		0

#define LED_COUNT_MAX		10
#define LED_STEP			15
#define LED_COLOR			8

#define FDS_PYROCFG_FILE     (0xF010)
#define FDS_PYROCFG_REC_KEY  (0x7010)

#define DEVICE_NAME                     "BluePyro"                          /**< Name of device. Will be included in the advertising data. */

#define MANUFACTURER_NAME               "I-SYST inc."                       /**< Manufacturer. Will be passed to Device Information Service. */

#define MODEL_NAME                      "M3225"                        /**< Model number. Will be passed to Device Information Service. */

#define MANUFACTURER_ID                 ISYST_BLUETOOTH_ID                  /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                   ISYST_BLUETOOTH_ID                  /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */

#define APP_ADV_INTERVAL                40//MSEC_TO_UNITS(40, UNIT_0_625_MS)	/**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

#ifdef RAW_DATA
#define APP_ADV_TIMEOUT					0	// Never timeout
#else
#define APP_ADV_TIMEOUT					2500//MSEC_TO_UNITS(2500, UNIT_10_MS)		/**< The advertising timeout (in units of 10ms seconds). */
#endif

#define MIN_CONN_INTERVAL               8//MSEC_TO_UNITS(8, UNIT_1_25_MS)     /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               40//MSEC_TO_UNITS(40, UNIT_1_25_MS)     /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */

#define BLE_UART_UUID_BASE			BLUEIO_UUID_BASE

#define BLE_UART_UUID_SERVICE		BLUEIO_UUID_UART_SERVICE		//!< BlueIO default service
#define BLE_UART_UUID_TX_CHAR		BLUEIO_UUID_UART_TX_CHAR		//!< Data characteristic
#define BLE_UART_UUID_RX_CHAR		BLUEIO_UUID_UART_RX_CHAR		//!< Command control characteristic

#pragma pack(push, 1)

typedef struct {
	uint8_t Name[BLUEPYRO_ADV_NAME_MAXLEN];
	PyroCfg_reg_t PyroCfg;
} APP_DATA;

#pragma pack(pop)

void BluePyroCfgWrSrvcCallback(BleSrvc_t *pBlueIOSvc, uint8_t *pData, int Offset, int Len);
void BluePyroDfuWrSrvcCallback(BleSrvc_t *pBlueIOSvc, uint8_t *pData, int Offset, int Len);

__attribute__ ((section(".Version"), used))
const AppInfo_t g_AppInfo = {
	DEVICE_NAME, {BLUEPYRO_VERS, 0, BUILDN},
	{'I', '-', 'S', 'Y', 'S', 'T', '-', 'B', 'L', 'U', 'E', 'P', 'Y', 'R', 'O',  25},
};

BLUEPYRO_ADVDATA g_AdvData = {
	BLEADV_MANDATA_TYPE_APP,
	0,
	{ 0 },
};

bool g_bAdvertising = false;

static const ble_uuid_t  s_AdvUuids[] = {
	{BLEPYRO_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN}
};

static const char s_CfgCharDescString[] = {
	"BluePyro Config characteristic",
};

static const char s_DfuCharDescString[] = {
	"BluePyro Enter DFU characteristic",
};

alignas(4) APP_DATA g_AppData = {
	.Name = {0,},
	.PyroCfg = {{
		.PulseMode = 1,
		.Reserved1 = 0,
		.HPFCutOff = 1,
		.Reserved2 = 2,
		.SignalSource = 0,
#ifdef RAW_DATA
		.OpMode = EXCELITAS_PYD2592_OPMODE_INT_READOUT,
#else
		.OpMode = EXCELITAS_PYD2592_OPMODE_WAKEUP,
#endif
		.WindowTime = 1,
		.PulseCounter = 1,
		.BlindTime = 0,
		.Threshold = 30
	}}
};

volatile PYD2592_DATA g_PydData;

alignas(4) static fds_record_t const g_AppDataRecord =
{
    .file_id           = FDS_PYROCFG_FILE,
    .key               = FDS_PYROCFG_REC_KEY,
    .data = {
		.p_data = &g_AppData,
    /* The length of a record is always expressed in 4-byte units (words). */
		.length_words = (sizeof(APP_DATA) + 3) >> 2,
    }
};

#ifdef RAW_DATA

static const char s_RxCharDescString[] = {
	"UART Rx characteristic",
};

static const char s_TxCharDescString[] = {
	"UART Tx characteristic",
};

void UartTxSrvcCallback(BLESRVC *pBlueIOSvc, uint8_t *pData, int Offset, int Len);

/// Characteristic definitions
BleSrvcChar_t g_UartChars[] = {
	{
		// Read characteristic
		.Uuid = BLE_UART_UUID_RX_CHAR,
		.MaxDataLen = 64,
		.Property =
		BLESVC_CHAR_PROP_READ | BLESVC_CHAR_PROP_NOTIFY | BLESVC_CHAR_PROP_VARLEN,
		.pDesc = s_RxCharDescString,		// char UTF-8 description string
		.WrCB = NULL,						// Callback for write char, set to NULL for read char
		.SetNotifCB = NULL,					// Callback on set notification
		.TxCompleteCB = NULL,				// Tx completed callback
		.pDefValue = NULL,					// pointer to char default values
		.ValueLen = 0,						// Default value length in bytes
	},
	{
		// Write characteristic
		.Uuid = BLE_UART_UUID_TX_CHAR,		// char UUID
		.MaxDataLen = 64,			// char max data length
		.Property = BLESVC_CHAR_PROP_WRITEWORESP | BLESVC_CHAR_PROP_VARLEN,// char properties define by BLUEIOSVC_CHAR_PROP_...
		.pDesc = s_TxCharDescString,		// char UTF-8 description string
		.WrCB = UartTxSrvcCallback,			// Callback for write char, set to NULL for read char
		.SetNotifCB = NULL,					// Callback on set notification
		.TxCompleteCB = NULL,				// Tx completed callback
		.pDefValue = NULL,					// pointer to char default values
		.ValueLen = 0						// Default value length in bytes
	},
};

static const int s_BleUartNbChar = sizeof(g_UartChars) / sizeof(BLESRVC_CHAR);

/// Service definition
const BleSrvcCfg_t s_UartSrvcCfg = {
	.SecType = BLESRVC_SECTYPE_NONE,		// Secure or Open service/char
	.UuidBase = {BLE_UART_UUID_BASE,},		// Base UUID
	1,
	.UuidSvc = BLE_UART_UUID_SERVICE,		// Service UUID
	.NbChar = s_BleUartNbChar,				// Total number of characteristics for the service
	.pCharArray = g_UartChars,				// Pointer a an array of characteristic
	.pLongWrBuff = NULL,				// pointer to user long write buffer
	.LongWrBuffSize = 0,	// long write buffer size
};

BleSrvc_t g_UartBleSrvc;
#endif

/// Characteristic definitions
BleSrvcChar_t g_BluePyroChars[] = {
	{
		// Read/write config characteristic
		.Uuid = BLEPYRO_UUID_CFG_CHAR,
		.MaxDataLen = 20,
		.Property = BLESRVC_CHAR_PROP_WRITE | BLESRVC_CHAR_PROP_READ | BLESRVC_CHAR_PROP_VARLEN,
		.pDesc = s_CfgCharDescString,		// char UTF-8 description string
		.WrCB = BluePyroCfgWrSrvcCallback,
		.SetNotifCB = NULL,					// Callback on set notification
		.TxCompleteCB = NULL,				// Tx completed callback
		.pDefValue = NULL,					// pointer to char default values
		.ValueLen = 0,						// Default value length in bytes
	},
	{
		// Read/write config characteristic
		.Uuid = BLEPYRO_UUID_DFU_CHAR,
		.MaxDataLen = 4,
		.Property = BLESRVC_CHAR_PROP_WRITE | BLESRVC_CHAR_PROP_VARLEN,
		.pDesc = s_DfuCharDescString,		// char UTF-8 description string
		.WrCB = BluePyroDfuWrSrvcCallback,
		.SetNotifCB = NULL,					// Callback on set notification
		.TxCompleteCB = NULL,				// Tx completed callback
		.pDefValue = NULL,					// pointer to char default values
		.ValueLen = 0,						// Default value length in bytes
	},
};

static const int s_BluePyroNbChar = sizeof(g_BluePyroChars) / sizeof(BleSrvcChar_t);

uint8_t g_LWrBuffer[512];

/// Service definition
const BleSrvcCfg_t s_BluePyroSrvcCfg = {
	.SecType = BLESRVC_SECTYPE_NONE,		// Secure or Open service/char
	.bCustom = true,
	.UuidBase = BLUEPYRO_UUID_BASE,			// Base UUID
	//.NbUuidBase = 4,
	.UuidSvc = BLEPYRO_UUID_SERVICE,		// Service UUID
	.NbChar = s_BluePyroNbChar,				// Total number of characteristics for the service
	.pCharArray = g_BluePyroChars,			// Pointer a an array of characteristic
	.pLongWrBuff = NULL,					// pointer to user long write buffer
	.LongWrBuffSize = 0,					// long write buffer size
};

BleSrvc_t g_BluePyroSrvc;

static char s_FirmVers[16] = "00.00.200101";

const BleAppDevInfo_t s_BluePyroDevDesc = {
	MODEL_NAME,       		// Model name
	MANUFACTURER_NAME,		// Manufacturer name
	"123",					// Serial number string
	s_FirmVers,					// Firmware version string
	"0.0",					// Hardware version string
};

const BleAppCfg_t s_BleAppCfg = {
	.Role = BLEAPP_ROLE_PERIPHERAL,
	//.ClkCfg = { NRF_CLOCK_LF_SRC_XTAL, 0, 0, NRF_CLOCK_LF_ACCURACY_20_PPM},
	.CentLinkCount = 0, 				// Number of central link
	.PeriLinkCount = 1, 				// Number of peripheral link
	//.AppMode = BLEAPP_MODE_APPSCHED,	// Use scheduler
	.pDevName = DEVICE_NAME,			// Device name
	.VendorID = ISYST_BLUETOOTH_ID,		// PnP Bluetooth/USB vendor id
	.ProductId = 1,						// PnP Product ID
	.ProductVer = 0,					// Pnp prod version
	//.bEnDevInfoService = true,			// Enable device information service (DIS)
	.pDevDesc = &s_BluePyroDevDesc,
	.pAdvManData = (uint8_t*)&g_AdvData,			// Manufacture specific data to advertise
	.AdvManDataLen = sizeof(BLUEPYRO_ADVDATA),	// Length of manufacture specific data
	.pSrManData = NULL,
	.SrManDataLen = 0,
	.SecType = BLEAPP_SECTYPE_NONE,//BLEAPP_SECTYPE_STATICKEY_MITM,//BLEAPP_SECTYPE_NONE,    // Secure connection type
	.SecExchg = BLEAPP_SECEXCHG_NONE,	// Security key exchange
	.pAdvUuid = NULL,      			// Service uuids to advertise
	//.NbAdvUuid = 0, 					// Total number of uuids
	.AdvInterval = APP_ADV_INTERVAL,	// Advertising interval in msec
	.AdvTimeout = APP_ADV_TIMEOUT,		// Advertising timeout in sec
	.AdvSlowInterval = 0,				// Slow advertising interval, if > 0, fallback to
										// slow interval on adv timeout and advertise until connected
	.ConnIntervalMin = MIN_CONN_INTERVAL,
	.ConnIntervalMax = MAX_CONN_INTERVAL,
	.ConnLedPort = BLUEIO_CONNECT_LED_PORT,// Led port nuber
	.ConnLedPin = BLUEIO_CONNECT_LED_PIN,// Led pin number
	.TxPower = 0,						// Tx power
	.SDEvtHandler = NULL				// RTOS Softdevice handler
};

static const IOPinCfg_t s_IOPins[] = {
	{LED1_PORT, LED1_PIN, LED1_PINOP, IOPINDIR_OUTPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},	// LED 1
	{LED2_B_PORT, LED2_B_PIN, LED2_B_PINOP, IOPINDIR_OUTPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},	// LED 2 Blue
	{LED2_G_PORT, LED2_G_PIN, LED2_G_PINOP, IOPINDIR_OUTPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},	// LED 2 Green
	{LED2_R_PORT, LED2_R_PIN, LED2_R_PINOP, IOPINDIR_OUTPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},	// LED 2 Red
	//{PYD_ID_PORT, PYD_ID_PIN, PYD_ID_PINOP, IOPINDIR_INPUT, IOPINRES_PULLUP, IOPINTYPE_NORMAL},	// In
	{IN_PORT, IN_PIN, IN_PINOP, IOPINDIR_INPUT, IOPINRES_PULLDOWN, IOPINTYPE_NORMAL},	// In
	{OUT_PORT, OUT_PIN, OUT_PINOP, IOPINDIR_OUTPUT, IOPINRES_PULLDOWN, IOPINTYPE_NORMAL},	// Out
	//{PYD_DL_PORT, PYD_DL_PIN, PYD_DL_PINOP, IOPINDIR_OUTPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},	// PYD
};

static const int s_NbIOPins = sizeof(s_IOPins) / sizeof(IOPINCFG);

int nRFUartEvthandler(UARTDEV *pDev, UART_EVT EvtId, uint8_t *pBuffer, int BufferLen);

#define UARTFIFOSIZE			CFIFO_MEMSIZE(256)

static uint8_t s_UartRxFifo[UARTFIFOSIZE];
static uint8_t s_UartTxFifo[UARTFIFOSIZE];

/// UART pins definitions
static IOPinCfg_t s_UartPins[] = {
	{UART_RX_PORT, UART_RX_PIN, UART_RX_PINOP, IOPINDIR_INPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},		// RX
	{UART_TX_PORT, UART_TX_PIN, UART_TX_PINOP, IOPINDIR_OUTPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},		// TX
	//{UART_CTS_PORT, UART_CTS_PIN, UART_CTS_PINOP, IOPINDIR_INPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},	// CTS
	//{UART_RTS_PORT, UART_RTS_PIN, UART_RTS_PINOP, IOPINDIR_OUTPUT, IOPINRES_NONE, IOPINTYPE_NORMAL},	// RTS
};

/// UART configuration
static const UARTCfg_t s_UartCfg = {
	.DevNo = 0,							// Device number zero based
	.pIOPinMap = s_UartPins,				// UART assigned pins
	.NbIOPins = sizeof(s_UartPins) / sizeof(IOPINCFG),	// Total number of UART pins used
	.Rate = 115200,						// Baudrate
	.DataBits = 8,						// Data bits
	.Parity = UART_PARITY_NONE,			// Parity
	.StopBits = 1,						// Stop bit
	.FlowControl = UART_FLWCTRL_NONE,	// Flow control
	.bIntMode = true,					// Interrupt mode
	.IntPrio = APP_IRQ_PRIORITY_LOW,	// Interrupt priority
	.EvtCallback = nRFUartEvthandler,	// UART event handler
	.bFifoBlocking = true,				// Blocking FIFO
	.RxMemSize = UARTFIFOSIZE,
	.pRxMem = s_UartRxFifo,
	.TxMemSize = UARTFIFOSIZE,
	.pTxMem = s_UartTxFifo,
};

/// UART object instance
UART g_Uart;

void TimerHandler(TIMER * const pTimer, uint32_t Evt);

const static TimerCfg_t s_TimerCfg = {
    .DevNo = 2,
	.ClkSrc = TIMER_CLKSRC_DEFAULT,
	.Freq = 0,			// 0 => Default highest frequency
	.IntPrio = APP_IRQ_PRIORITY_HIGH,
	.EvtHandler = TimerHandler
};

Timer g_Timer;

static const PwmCfg_t s_PwmCfg = {
	.DevNo = 0,
	.Freq = 50,
	.Mode = PWM_MODE_EDGE,
	.bIntEn = false,
	.IntPrio = 6,
	.pEvtHandler = NULL
};

static const PwmChanCfg_t s_PwmChanCfg[] = {
	{
		.Chan = 0,
		.Pol = PWM_POL_LOW,
		.Port = LED2_R_PORT,
		.Pin = LED2_R_PIN,
	},
	{
		.Chan = 1,
		.Pol = PWM_POL_LOW,
		.Port = LED2_G_PORT,
		.Pin = LED2_G_PIN,
	},
	{
		.Chan = 2,
		.Pol = PWM_POL_LOW,
		.Port = LED2_B_PORT,
		.Pin = LED2_B_PIN,
	},
};

const int s_NbPwmChan = sizeof(s_PwmChanCfg) / sizeof(PwmChanCfg_t);

Pwm g_Pwm;

LedPwm g_Led2;
int g_LedLevel = 0;

alignas(4) DirectLinkDev_t g_DLink;
alignas(4) SerialInDev_t g_SerialIn;
volatile bool g_FdsInitialized = false;
uint32_t g_Timestamp = 0;
uint32_t g_PacketCnt = 0;
std::atomic<bool> g_bAdv(false);

void TimerHandler(TIMER * const pTimer, uint32_t Evt)
{
	g_Timestamp = TimerGetMilisecond(pTimer);
	if (Evt == TIMER_EVT_TRIGGER(0))
	{
		g_LedLevel--;
		if (g_LedLevel > 0)
		{
			g_Timer.EnableTimerTrigger(0, 250UL, TIMER_TRIG_TYPE_SINGLE);
		}
		else
		{
			g_LedLevel = 0;
		}
		g_Led2.Level(((uint32_t)g_LedLevel  * LED_STEP) << LED_COLOR);
	}
}

static void fds_evt_handler(fds_evt_t const * p_evt)
{
    switch (p_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_evt->result == NRF_SUCCESS)
            {
            	g_FdsInitialized = true;
            }
            break;

        case FDS_EVT_WRITE:
        {
            if (p_evt->result == NRF_SUCCESS)
            {
            }
        } break;

        case FDS_EVT_DEL_RECORD:
        {
            if (p_evt->result == NRF_SUCCESS)
            {
            }
        } break;

        default:
            break;
    }
}

void UpdateRecord()
{
    fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};

    uint32_t rc = fds_record_find(FDS_PYROCFG_FILE, FDS_PYROCFG_REC_KEY, &desc, &tok);

    if (rc == NRF_SUCCESS)
    {
    	rc = fds_record_update(&desc, &g_AppDataRecord);
    	APP_ERROR_CHECK(rc);
    }
}

void PyroCfgUpdateSched(void * p_event_data, uint16_t event_size)
{
	memcpy(g_AdvData.Name, g_AppData.Name, BLUEPYRO_ADV_NAME_MAXLEN);

	SerialIn(&g_SerialIn, g_AppData.PyroCfg.Val, 25);
	UpdateRecord();
	msDelay(1);

	g_AdvData.PyroCfg = g_AppData.PyroCfg;
}

void BluePyroCfgWrSrvcCallback(BleSrvc_t *pBlueIOSvc, uint8_t *pData, int Offset, int Len)
{
	int len = min((size_t)Len, sizeof(APP_DATA));

	if (Len >= sizeof(APP_DATA))
	{
		uint32_t d = g_AppData.PyroCfg.Val & 0b111111010;	// Clear all bits we want to set

		memcpy(&g_AppData, pData, sizeof(APP_DATA));

		//printf("%x, %x\r\n", g_AppData.PyroCfg.Val, d);
		g_AppData.PyroCfg.Val |= d;
		g_AppData.PyroCfg.OpMode = 2;
		g_AppData.PyroCfg.Reserved1 = 0;
		g_AppData.PyroCfg.Reserved2 = 2;

		app_sched_event_put(NULL, 0, PyroCfgUpdateSched);
	}
}

void BluePyroDfuWrSrvcCallback(BleSrvc_t *pBlueIOSvc, uint8_t *pData, int Offset, int Len)
{
	if (*pData == 0xFF)
	{
		BleAppEnterDfu();
	}
}

void AdvChedHandler(void * p_event_data, uint16_t event_size)
{
	if (isConnected() == false)
	{
		if (BleAppAdvManDataSet((uint8_t*)&g_AdvData, sizeof(g_AdvData), NULL, 0) == false)
		{
			g_bAdv = true;
			BleAppAdvStart();//BLEAPP_ADVMODE_FAST);
		}
	}
}

void BleAppAdvTimeoutHandler()
{
	g_AdvData.MotionDet--;

	if (g_AdvData.MotionDet == 0xff)
	{
		g_AdvData.MotionDet = 0;
		g_bAdv = false;
		//sd_power_system_off();
	}
	else
	{
		app_sched_event_put(NULL, 0, AdvChedHandler);
	}

	if (g_AdvData.MotionDet == 0)
	{
		g_Led2.Level(0);
		g_Pwm.Disable();
	}
}

void UartTxSrvcCallback(BleSrvc_t *pBlueIOSvc, uint8_t *pData, int Offset, int Len)
{
	g_Uart.Tx(pData, Len);
}

void BlePeriphEvtUserHandler(ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    	case BLE_GAP_EVT_DISCONNECTED:
    		//BleAppAdvStart(BLEAPP_ADVMODE_FAST);
    		break;
    }

#ifdef RAW_DATA
    BleSrvcEvtHandler(&g_UartBleSrvc, p_ble_evt);
#endif

    BleSrvcEvtHandler(&g_BluePyroSrvc, (uint32_t)p_ble_evt);
}

void BleAppInitUserServices()
{
    uint32_t err_code;

#ifdef RAW_DATA
    err_code = BleSrvcInit(&g_UartBleSrvc, &s_UartSrvcCfg);
    APP_ERROR_CHECK(err_code);
#endif

    err_code = BleSrvcInit(&g_BluePyroSrvc, &s_BluePyroSrvcCfg);
    APP_ERROR_CHECK(err_code);

    BleSrvcCharSetValue(&g_BluePyroSrvc, 0, (uint8_t*)&g_AppData, sizeof(APP_DATA));
}

void BleAppInitUserData()
{
    /* Register first to receive an event when initialization is complete. */
    (void) fds_register(fds_evt_handler);
    uint32_t rc = fds_init();
    APP_ERROR_CHECK(rc);

    while (g_FdsInitialized != true)
    {
        nrf_sdh_evts_poll();                    /* let the handlers run first, incase the EVENT occured before creating this task */
    }

    fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};

    rc = fds_record_find(FDS_PYROCFG_FILE, FDS_PYROCFG_REC_KEY, &desc, &tok);

    if (rc == NRF_SUCCESS)
    {
    	do {
			/* A config file is in flash. Let's update it. */
			fds_flash_record_t config = {0};

			/* Open the record and read its contents. */
			rc = fds_record_open(&desc, &config);
			APP_ERROR_CHECK(rc);

			memcpy(&g_AppData, config.p_data, sizeof(APP_DATA));
			memcpy(g_AdvData.Name, g_AppData.Name, BLUEPYRO_ADV_NAME_MAXLEN);

			/* Close the record when done reading. */
			rc = fds_record_close(&desc);
			APP_ERROR_CHECK(rc);

			rc = fds_record_find(FDS_PYROCFG_FILE, FDS_PYROCFG_REC_KEY, &desc, &tok);
    	} while (rc == NRF_SUCCESS);

    	IOPinSetDir(PYD_DL_PORT, PYD_DL_PIN, IOPINDIR_INPUT);

    	SerialIn(&g_SerialIn, g_AppData.PyroCfg.Val, 25);
    }
    else
    {
        rc = fds_record_write(&desc, &g_AppDataRecord);
        APP_ERROR_CHECK(rc);
    }

    g_bAdvertising = true;
	g_AdvData.PyroCfg = g_AppData.PyroCfg;
}

void PrintDataChedHandler(void * p_event_data, uint16_t event_size)
{
	PYD2592_DATA data;
	char buff[512];

	uint32_t state = DisableInterrupt();
	memcpy(&data, (void*)&g_PydData, sizeof(PYD2592_DATA));
	EnableInterrupt(state);

	sprintf(buff, "%u:%u : Reg:%x, Counts:%d, ", g_PacketCnt, data.Timestamp, data.CfgReg, data.AdcCount);
	//g_Uart.printf(buff);

	if (data.OutRange)
	{
		//g_Uart.printf("Normal\r\n");
		strcat(buff, "Normal\r\n");
	}
	else
	{
		if (abs(data.AdcCount) <= 511)
		{
//			g_Uart.printf("PIR low\r\n");
			strcat(buff, "PIR low\r\n");
		}
		else if (abs(data.AdcCount) >= ((1<<14) - 511))
		{
			//g_Uart.printf("PIR sat\r\n");
			strcat(buff, "PIR sat\r\n");
		}
		else
		{
			//g_Uart.printf("Buffer overflow\r\n");
			strcat(buff, "Buffer overflow\r\n");
		}
	}

	g_Uart.printf(buff);

#ifdef RAW_DATA
	BleSrvcCharNotify(&g_UartBleSrvc, 0, (uint8_t*)buff, strlen(buff) + 1);
#endif

	g_PacketCnt++;
}

#if 0
void PyDetectChedHandler(void * p_event_data, uint16_t event_size)
{
	if (isConnected() == false)
	{
		if (BleAppAdvManDataSet((uint8_t*)&g_AdvData, sizeof(g_AdvData), NULL, 0) == false)
		{
			g_bAdv = true;
			BleAppAdvStart(BLEAPP_ADVMODE_FAST);
		}
	}
}
#endif

void PydIntHandler(int IntNo, void *pCtx)
{
	uint64_t d = 0;

//	IOPinClear(LED2_G_PORT, LED2_G_PIN);
	if (IntNo == PYD_INT_NO)
	{
		IOPinDisableInterrupt(PYD_INT_NO);
		IOPinSet(OUT_PORT, OUT_PIN);
#ifndef RAW_DATA
		//BleAppAdvStop();
		g_AdvData.MotionDet = 1;

		app_sched_event_put(NULL, 0, AdvChedHandler);
#if 0
		if (isConnected() == false && g_bAdv == false)
		{
			if (BleAppAdvManDataSet((uint8_t*)&g_AdvData, sizeof(g_AdvData), NULL, 0) == false)
			{
				BleAppAdvStart(BLEAPP_ADVMODE_FAST);
			}
		}
#endif
		g_Pwm.Enable();
		g_LedLevel = 1;//LED_COUNT_MAX;
		g_Led2.Level((g_LedLevel * LED_STEP) << LED_COLOR);
		IOPinSet(PYD_DL_PORT, PYD_DL_PIN);
		IOPinSetDir(PYD_DL_PORT, PYD_DL_PIN, IOPINDIR_OUTPUT);
		usDelay(1);
		IOPinClear(PYD_DL_PORT, PYD_DL_PIN);
		usDelay(20);
		IOPinSetDir(PYD_DL_PORT, PYD_DL_PIN, IOPINDIR_INPUT);
		IOPinClear(OUT_PORT, OUT_PIN);
#else
		static int dcnt = 10;

		g_PydData.Timestamp = g_Timer.mSecond();

		int c = DirectLinkRead(&g_DLink, 40, (uint8_t*)&g_PydData.Val);

		if (c > 0)
		{
			dcnt--;

			if (dcnt < 0)
			{
				dcnt = 10;
				app_sched_event_put(NULL, 0, PrintDataChedHandler);
			}
		}


#endif
		IOPinEnableInterrupt(PYD_INT_NO, APP_IRQ_PRIORITY_MID, PYD_DL_PORT, PYD_DL_PIN, IOPINSENSE_HIGH_TRANSITION, PydIntHandler, NULL);
		g_Timer.EnableTimerTrigger(0, 250UL, TIMER_TRIG_TYPE_SINGLE);
	}
}

void UartRxChedHandler(void * p_event_data, uint16_t event_size)
{
	uint8_t buff[128];
	uint8_t *p = (uint8_t*)p_event_data;

	switch (*p)
	{
		case '0': // Turn everything off (led off, adv off)
			IOPinSet(LED1_PORT, LED1_PIN);
			IOPinSet(LED2_R_PORT, LED2_R_PIN);
			IOPinSet(LED2_G_PORT, LED2_G_PIN);
			IOPinSet(LED2_B_PORT, LED2_B_PIN);
			//if (g_bAdvertising == true)
			{
				BleAppAdvStop();
				g_bAdvertising = false;
			}
			break;
		case '1': // Blue LED on
			IOPinClear(LED1_PORT, LED1_PIN);
			break;
		case '2':
			IOPinClear(LED2_R_PORT, LED2_R_PIN);
			break;
		case '3':
			IOPinClear(LED2_G_PORT, LED2_G_PIN);
			break;
		case '4':
			IOPinClear(LED2_B_PORT, LED2_B_PIN);
			break;
		case '5': // Adv on
			//if (g_bAdvertising == false)
			{
				g_bAdvertising = true;
				BleAppAdvStart();//BLEAPP_ADVMODE_FAST);
			}
			break;
	}
	g_Uart.printf("uart %c\n", *p);
}

int nRFUartEvthandler(UARTDEV *pDev, UART_EVT EvtId, uint8_t *pBuffer, int BufferLen)
{
	int cnt = 0;
	uint8_t buff[20];

	switch (EvtId)
	{
		case UART_EVT_RXTIMEOUT:
		case UART_EVT_RXDATA:
			cnt = g_Uart.Rx(buff, 1);
			if (cnt > 0)
			{
				app_sched_event_put(buff, 1, UartRxChedHandler);
			}
			break;
		case UART_EVT_TXREADY:
			break;
		case UART_EVT_LINESTATE:
			break;
	}

	return cnt;
}

void HardwareInit()
{
	strcpy((char*)g_AdvData.Name, MODEL_NAME);
	memcpy(g_AppData.Name, g_AdvData.Name, strlen((char*)g_AdvData.Name));

	IOPinCfg(s_IOPins, s_NbIOPins);

	IOPinSet(LED1_PORT, LED1_PIN);
	IOPinSet(LED2_B_PORT, LED2_B_PIN);
	IOPinSet(LED2_G_PORT, LED2_G_PIN);
	IOPinSet(LED2_R_PORT, LED2_R_PIN);

	g_Pwm.Init(s_PwmCfg);
	g_Led2.Init(&g_Pwm, (PwmChanCfg_t*)s_PwmChanCfg, s_NbPwmChan);

	g_Timer.Init(s_TimerCfg);

#ifdef RAW_DATA

	g_Uart.Init(s_UartCfg);
	g_Uart.printf("BluePyro started\r\n");
#endif

	DirectLinkInit(&g_DLink, PYD_DL_PORT, PYD_DL_PIN, PYD_DL_PINOP);

	SerialInInit(&g_SerialIn, PYD_SI_PORT, PYD_SI_PIN, PYD_SI_PINOP);

	IOPinSetDir(PYD_DL_PORT, PYD_DL_PIN, IOPINDIR_INPUT);

	SerialIn(&g_SerialIn, g_AppData.PyroCfg.Val, 25);


#if 0	// For testing
	uint32_t m = 0x1a55aa5;//0x1000000;
	while(m)
	{

	SerialIn(&g_SerialIn, m, 25);

	msDelay(10);

	uint64_t d = 0;
	int c = DirectLinkRead(&g_DLink, 40, (uint8_t*)&d);
	uint32_t x = d & 0x1FFFFFFUL;

	if (m != x)
	{
		printf("Failed\n");
	}
	printf("%x %x\n", m, x);
	m >>= 1;
	}
#endif

	msDelay(10);

	IOPinEnableInterrupt(PYD_INT_NO, APP_IRQ_PRIORITY_MID, PYD_DL_PORT, PYD_DL_PIN, IOPINSENSE_HIGH_TRANSITION, PydIntHandler, NULL);

	msDelay(10);
}

//
// Print a greeting message on standard output and exit.
//
// On embedded platforms this might require semi-hosting or similar.
//
// For example, for toolchains derived from GNU Tools for Embedded,
// to enable semi-hosting, the following was added to the linker:
//
// --specs=rdimon.specs -Wl,--start-group -lgcc -lc -lm -lrdimon -Wl,--end-group
//
// Adjust it for other toolchains.
//

int main()
{
	HardwareInit();

    BleAppInit((const BleAppCfg_t *)&s_BleAppCfg);//, true);

	BleAppRun();

    return 0;
}

