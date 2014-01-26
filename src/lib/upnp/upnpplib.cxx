/*
 * Copyright (C) 2003-2014 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include "upnpplib.hxx"
#include "Callback.hxx"
#include "Domain.hxx"
#include "Init.hxx"
#include "Log.hxx"

#include <upnp/upnptools.h>

LibUPnP::LibUPnP()
{
	if (!UpnpGlobalInit(init_error))
		return;

	auto code = UpnpRegisterClient(o_callback, nullptr, &m_clh);
	if (code != UPNP_E_SUCCESS) {
		UpnpGlobalFinish();
		init_error.Format(upnp_domain, code,
				  "UpnpRegisterClient() failed: %s",
				  UpnpGetErrorMessage(code));
		return;
	}
}

int
LibUPnP::o_callback(Upnp_EventType et, void* evp, void* cookie)
{
	if (cookie == nullptr)
		/* this is the cookie passed to UpnpRegisterClient();
		   but can this ever happen?  Will libupnp ever invoke
		   the registered callback without that cookie? */
		return UPNP_E_SUCCESS;

	UpnpCallback &callback = UpnpCallback::FromUpnpCookie(cookie);
	return callback.Invoke(et, evp);
}

LibUPnP::~LibUPnP()
{
	UpnpGlobalFinish();
}
