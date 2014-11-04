/*
  Copyright (c) 2014 MediaTek Inc.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
   See the GNU Lesser General Public License for more details.
*/

#include "LWiFiClient.h"
#include <vmconn.h>
#include <vmtcp.h>

LWiFiClient::LWiFiClient():
    LTcpClient()
{
  m_apn = VM_TCP_APN_WIFI;
}

LWiFiClient::LWiFiClient(const LTcpClient &rhs):
    LTcpClient(rhs)
{
  m_apn = VM_TCP_APN_WIFI;
}

LWiFiClient::LWiFiClient(VMINT handle):
    LTcpClient(handle)
{
  m_apn = VM_TCP_APN_WIFI;
}

LWiFiClient::LWiFiClient(VMINT handle, VMINT serverHandle):
    LTcpClient(handle, serverHandle)
{
  m_apn = VM_TCP_APN_WIFI;
}
