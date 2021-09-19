/**-------------------------------------------------------------------------
@file	board.h

@brief	Board specific definitions

This file contains all I/O definitions for a specific board for the
application firmware.  This files should be located in each project and
modified to suit the need for the application use case.

@author	Hoang Nguyen Hoan
@date	Nov. 16, 2016

@license

Copyright (c) 2016, I-SYST inc., all rights reserved

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

#ifndef __BOARD_H__
#define __BOARD_H__

#include "blueio_board.h"

#define LED1_PORT			0
#define LED1_PIN			4
#define LED1_PINOP			0

#define LED2_B_PORT			1
#define LED2_B_PIN			4
#define LED2_B_PINOP		0

#define LED2_G_PORT			1
#define LED2_G_PIN			1
#define LED2_G_PINOP		0

#define LED2_R_PORT			1
#define LED2_R_PIN			5
#define LED2_R_PINOP		0

#define UART_RX_PORT		0
#define UART_RX_PIN			13
#define UART_RX_PINOP		1
#define UART_TX_PORT		1
#define UART_TX_PIN			8
#define UART_TX_PINOP		1

#define IN_PORT				0
#define IN_PIN				23
#define IN_PINOP			0

#define OUT_PORT			0
#define OUT_PIN				15
#define OUT_PINOP			0

#define PYD_DL_PORT			0
#define PYD_DL_PIN			16
#define PYD_DL_PINOP		0

#define PYD_INT_NO			0

#define PYD_SI_PORT			0
#define PYD_SI_PIN			17//5
#define PYD_SI_PINOP		0

#define PYD_ID_PORT			0
#define PYD_ID_PIN			13
#define PYD_ID_PINOP		0

#endif // __BOARD_H__

