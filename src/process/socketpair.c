/*
 * Copyright 2019-2026 lixungeng
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _WIN32_WINNT 0x0600
#include <Winsock2.h>
#include <Windows.h>
#include <stdint.h>
#include "unistd_uw.h"
#define socklen_t int

int socketpair(int family, int type, int protocol, SOCKET fd[2])
{
	/* This code is originally from Tor.  Used with permission. */

	/* This socketpair does not work when localhost is down. So
	 * it's really not the same thing at all. But it's close enough
	 * for now, and really, when localhost is down sometimes, we
	 * have other problems too.
	 */
#ifdef _WIN32
#define ERR(e) WSA##e
#else
#define ERR(e) e
#endif
	SOCKET listener = -1;
	SOCKET connector = -1;
	SOCKET acceptor = -1;
	struct sockaddr_in listen_addr;
	struct sockaddr_in connect_addr;
	socklen_t size;
	int saved_errno = -1;

	if (protocol || family != AF_INET) {
		WSASetLastError(ERR(EAFNOSUPPORT));
		return -1;
	}

	if (!fd) {
		WSASetLastError(ERR(EINVAL));
		return -1;
	}

	listener = socket(AF_INET, type, 0);
	if (listener < 0)
		return -1;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	listen_addr.sin_port = 0;	/* kernel chooses port.	 */
	if (bind(listener, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) == -1)
		goto tidy_up_and_fail;
	if (protocol == SOCK_STREAM && listen(listener, 1) == -1)
		goto tidy_up_and_fail;

	connector = socket(AF_INET, type, 0);
	if (connector < 0)
		goto tidy_up_and_fail;

	memset(&connect_addr, 0, sizeof(connect_addr));

	/* We want to find out the port number to connect to.  */
	size = sizeof(connect_addr);
	if (getsockname(listener, (struct sockaddr*)&connect_addr, &size) == -1)
		goto tidy_up_and_fail;
	if (size != sizeof(connect_addr))
		goto abort_tidy_up_and_fail;
	if (connect(connector, (struct sockaddr*)&connect_addr,
		sizeof(connect_addr)) == -1)
		goto tidy_up_and_fail;

	size = sizeof(listen_addr);
	if (protocol == SOCK_STREAM)
		acceptor = accept(listener, (struct sockaddr*)&listen_addr, &size);
	else
	{
		acceptor = listener;
		listener = INVALID_SOCKET;
		getsockname(connector, (struct sockaddr*)&listen_addr, &size);
		//memcpy(&listen_addr, &connect_addr, sizeof(connect_addr));
		//size = sizeof(connect_addr);
	}
	if (acceptor < 0)
		goto tidy_up_and_fail;
	if (size != sizeof(listen_addr))
		goto abort_tidy_up_and_fail;
	/* Now check we are talking to ourself by matching port and host on the
	   two sockets.	 */
	if (getsockname(connector, (struct sockaddr*)&connect_addr, &size) == -1)
		goto tidy_up_and_fail;
	if (size != sizeof(connect_addr)
		|| listen_addr.sin_family != connect_addr.sin_family
		|| listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
		|| listen_addr.sin_port != connect_addr.sin_port)
		goto abort_tidy_up_and_fail;
	closesocket(listener);
	fd[0] = connector;
	fd[1] = acceptor;

	return 0;

abort_tidy_up_and_fail:
	saved_errno = ERR(ECONNABORTED);
tidy_up_and_fail:
	if (saved_errno < 0)
		saved_errno = WSAGetLastError();
	if (listener != -1)
		closesocket(listener);
	if (connector != -1)
		closesocket(connector);
	if (acceptor != -1)
		closesocket(acceptor);

	WSASetLastError(saved_errno);
	return -1;
#undef ERR
}

