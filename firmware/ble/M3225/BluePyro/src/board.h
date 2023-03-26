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

#include "blystnano_boards.h"

#define LED1_PORT			BLUEPYRO_CR24S_M3225_LED1_PORT
#define LED1_PIN			BLUEPYRO_CR24S_M3225_LED1_PIN
#define LED1_PINOP			BLUEPYRO_CR24S_M3225_LED1_PINOP

#define LED2_B_PORT			BLUEPYRO_CR24S_M3225_LED2B_PORT
#define LED2_B_PIN			BLUEPYRO_CR24S_M3225_LED2B_PIN
#define LED2_B_PINOP		0

#define LED2_G_PORT			BLUEPYRO_CR24S_M3225_LED2G_PORT
#define LED2_G_PIN			BLUEPYRO_CR24S_M3225_LED2G_PIN
#define LED2_G_PINOP		0

#define LED2_R_PORT			BLUEPYRO_CR24S_M3225_LED2R_PORT
#define LED2_R_PIN			BLUEPYRO_CR24S_M3225_LED2R_PIN
#define LED2_R_PINOP		0

#define UART_RX_PORT		BLUEPYRO_CR24S_M3225_UART_RX_PORT
#define UART_RX_PIN			BLUEPYRO_CR24S_M3225_UART_RX_PIN
#define UART_RX_PINOP		1
#define UART_TX_PORT		BLUEPYRO_CR24S_M3225_UART_TX_PORT
#define UART_TX_PIN			BLUEPYRO_CR24S_M3225_UART_TX_PIN
#define UART_TX_PINOP		1

#define IN_PORT				BLUEPYRO_CR24S_M3225_IN_PORT
#define IN_PIN				BLUEPYRO_CR24S_M3225_IN_PIN
#define IN_PINOP			0

#define OUT_PORT			BLUEPYRO_CR24S_M3225_OUT_PORT
#define OUT_PIN				BLUEPYRO_CR24S_M3225_OUT_PIN
#define OUT_PINOP			0

#define PYD_DL_PORT			BLUEPYRO_M3225_DL_PORT
#define PYD_DL_PIN			BLUEPYRO_M3225_DL_PIN
#define PYD_DL_PINOP		0

#define PYD_INT_NO			0

#define PYD_SI_PORT			BLUEPYRO_M3225_SI_PORT
#define PYD_SI_PIN			BLUEPYRO_M3225_SI_PIN
#define PYD_SI_PINOP		0

//#define PYD_ID_PORT		0
//#define PYD_ID_PIN		13
//#define PYD_ID_PINOP	0


#endif // __BOARD_H__

