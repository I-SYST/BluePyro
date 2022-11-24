/**-------------------------------------------------------------------------
@example	bue_pyro.h

@brief	Board specific definitions

This file contains all I/O definitions for a specific board for the
application firmware.  This files should be located in each project and
modified to suit the need for the application use case.

@author	Hoang Nguyen Hoan
@date	Dec. 16, 2019

@license

Copyright (c) 2019, I-SYST inc., all rights reserved

Permission to use, copy, modify, and distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies, and none of the
names : I-SYST or its contributors may be used to endorse or
promote products derived from this software without specific prior written
permission.

For info or contributing contact : hnhoan at i-syst dot com

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------------*/

#ifndef __BLUEPYRO_H__
#define __BLUEPYRO_H__

#include <stdint.h>

#define BLUEPYRO_ADV_NAME_MAXLEN		8

#pragma pack(push, 1)

typedef struct __BluePyro_AdvData {
	uint8_t Type;			//!< Advertisement data type BLEADV_MANDATA_TYPE_APP
	uint8_t MotionDet;		//!< Motion detection event : 1 - Motion detected, 0 - No motion
	PyroCfgReg_t PyroCfg;
	uint8_t Name[BLUEPYRO_ADV_NAME_MAXLEN];		//!< Location name
} BluePyroAdvData_t;
//BLUEPYRO_ADVDATA;
#pragma pack(pop)

// UUID : a0080000-01fb-4109-8a0a-1816898a6725
#define BLUEPYRO_UUID_BASE			{ 0x25, 0x67, 0x8a, 0x89, 0x16, 0x18, 0x0a, 0x8a, \
									  0x09, 0x41, 0xfb, 0x01, 0x00, 0x00, 0x08, 0xa0 }

#define BLEPYRO_UUID_SERVICE		0xFFE0
#define BLEPYRO_UUID_CFG_CHAR		0xFFE1
#define BLEPYRO_UUID_DFU_CHAR		0xFFE2


#endif // __BLUEPYRO_H__

