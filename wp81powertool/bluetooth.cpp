/*
* 0x411180 IOCTL_BTH_QUERY_RADIO_STATE		in 0 byte, out 4 bytes "disabled"= < 3 else "enabled"
* 0x411184 IOCTL_BTH_SET_RADIO_STATE		in 4 bytes state0=0x2304 else 0x0002, out 0 byte
* https://stackoverflow.com/questions/75547578/how-to-enable-and-disable-bluetooth-using-winform-in-windows-10-using-c
*/

#include "stdafx.h"

BOOL ChangeRadioState(BOOL TurnOn)
{
	return TRUE;
}