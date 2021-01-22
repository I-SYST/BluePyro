/**-------------------------------------------------------------------------
@file	eink_vb3300.h

@brief	Implementation of E-Ink VB3300 display driver


@author	Hoang Nguyen Hoan
@date	Apr. 5, 2020

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

#ifndef __EINK_VB3300_H__
#define __EINK_VB3300_H__

#include "display/eink_intrf.h"
#include "device.h"

// Panel setting register
#define VB3300_PSR_REG				0

#define VB3300_PSR_RST_N					(1<<0)
#define VB3300_PSR_SHD_N					(1<<1)
#define VB3300_PSR_SHL						(1<<2)
#define VB3300_PSR_UD						(1<<3)
#define VB3300_PSR_BWR						(1<<4)
#define VB3300_PSR_REG_EN					(1<<5)
#define VB3300_PSR_RES_MASK					(3<<6)
#define VB3300_PSR_RES_600X448				(0<<6)
#define VB3300_PSR_RES_640X480				(1<<6)
#define VB3300_PSR_RES_720X540				(2<<6)
#define VB3300_PSR_RES_800X600				(3<<6)

// Pwoer setting register
#define VB3300_PWR_REG				1

// Power off command
#define VB3300_POF_REG				2

// Power off sequence setting register
#define VB3300_PFS_REG				3

// Power On Command
#define VB3300_PON_REG				4

// Power On Measure Command
#define VB3300_PMES_REG				5

// Booster Soft Start Command
#define VB3300_BTST_REG				6

// Deep Sleep
#define VB3300_DSLP_REG				7

// Data start transmission 1 register
#define VB3300_DTM1_REG				0x10

// Data Stop Command
#define VB3300_DSP_REG				0x11

// Display Refresh Command
#define VB3300_DRF_REG				0x12

// Data start transmission 2 register
#define VB3300_DTM2_REG				0x13

// Partial data start transmission 1 register
#define VB3300_PDTM1_REG			0x14

// Partial data start transmission 2 register
#define VB3300_PDTM2_REG			0x15

// Partial display refresh command
#define VB3300_PDRF_REG				0x16

// Set VCOM/Reserve state
#define VB3300_SET_STG_REG			0x26

// OSC Control register
#define VB3300_OSC_REG				0x30

// Temperature Sensor Command
#define VB3300_TSC_REG				0x40

// Temperature Sensor Calibration Register
#define VB3300_TSE_REG				0x41

// Temperature Sensor Write Register
#define VB3300_TSW_REG				0x42

// Temperature Sensor Read Register
#define VB3300_TSR_REG				0x43

// VCOM and DATA interval setting register
#define VB3300_CDI_REG				0x50

// Low power protection register
#define VB3300_LPD_REG				0x51

// TCON setting
#define VB3300_TCON_REG				0x60

// Resolution setting
#define VB3300_TRES_REG				0x61

// Source & Gate start setting
#define VB3300_TSGS_REG				0x62

// Revision register
#define VB3300_REV_REG				0x70

// Status register
#define VB3300_FLG_REG				0x71

// Auto measure VCOM register
#define VB3300_AMV_REG				0x80

// VCOM value register
#define VB3300_VCOM_REG				0x81

// VCOM DC Setting Register
#define VB3300_VDCS_REG				0x82

#ifdef __cplusplus

class EInkVB3300 : public Device  {
public:
	bool Init(DeviceIntrf *const pIntrf);

	/**
	 * @brief	Power on or wake up device
	 *
	 * @return	true - If success
	 */
	virtual bool Enable() { return true; }

	/**
	 * @brief	Put device in power down or power saving sleep mode
	 */
	virtual void Disable() {}

	/**
	 * @brief	Reset device to it initial default state
	 */
	virtual void Reset();

private:
};
#endif

#endif // __EINK_VB3300_H__
