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
#include "bluetooth/bt_app.h"
//#include "bluetooth/bt_dev.h"
// NOTE: bt_gatt / blueio_blesrvc APIs used by older firmware revisions are not
// available in the current IOsonata BLE headers in this project environment.
// This build fix removes custom GATT service definitions and keeps the app as
// an advertiser/peripheral using BtAppInit/BtAppRun and manufacturer data.
//#include "bluetooth/bt_gatt.h"
//#include "bluetooth/blueio_blesrvc.h"

static void HardwareInit()
{
	// Platform-specific setup is handled by IOsonata startup/BtAppInit in this config.
}

int main()
{
	HardwareInit();

	BtAppInit(nullptr);//, true);

	BtAppRun();

	return 0;
}
