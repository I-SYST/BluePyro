/**-------------------------------------------------------------------------
@file	base64.h

@brief	Base64 encode/decode.

This is the base64 binary to ASCII encoder/decoder

@author Hoang Nguyen Hoan
@date	Nov. 3, 2012

@license

Copyright (c) 2012, I-SYST inc., all rights reserved

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

#ifndef __BASE64_H__
#define __BASE64_H__

/** @addtogroup Utilities
  * @{
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Encode binary to base64 ASCII format.
 *
 * @param 	pSrc 	: Pointer to source binary data
 * @param	SrcLen	: Source data length in bytes
 * @param	pDest	: Pointer to ASCII destination buffer
 * @param	DstLen	: Destination buffer length in bytes
 *
 * @return	Number of bytes encoded
 */
int Base64Encode(uint8_t *pSrc, int SrcLen, char *pDest, int DstLen);

/** @} End of group Utilities */

#ifdef __cplusplus
}
#endif


#endif // __BASE64_H__
