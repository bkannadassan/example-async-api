/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
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
 *
 ***************************************************************************/

#include "./PWTCPConnectionProvider.hpp"
#include "./PWServer.hpp"
#include "./AppComponent.hpp"
#include "oatpp/network/server/Server.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "oatpp/network/Connection.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

#include <signal.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/http.h>
#include <event2/util.h>
#include <event2/event.h>
#include <fcntl.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>


extern struct event_base *base;
extern struct sockaddr_in sin;
extern struct evconnlistener *listener;
extern oatpp::network::server::PWServer *server;

namespace oatpp { namespace network { namespace server {
PWTCPConnectionProvider::PWTCPConnectionProvider(const oatpp::String& host, v_word16 port)
  : m_host(host)
  , m_port(port)
{
  m_serverHandle = instantiateServer();
  setProperty(PROPERTY_HOST, m_host);
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(port));
}

PWTCPConnectionProvider::PWTCPConnectionProvider(v_word16 port)
  : m_port(port)
  , m_closed(false)
{
  m_serverHandle = instantiateServer();
  setProperty(PROPERTY_HOST, "localhost");
  setProperty(PROPERTY_PORT, oatpp::utils::conversion::int32ToStr(port));
}

PWTCPConnectionProvider::~PWTCPConnectionProvider() {
  close();
}

void PWTCPConnectionProvider::close() {
  if(!m_closed) {
    m_closed = true;
    ::close(m_serverHandle);
  }
}

void
pw_accept_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data) {
    ::server->addConn(fd);
}
  
static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (event_base *)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}

static struct event *signal_event;
oatpp::data::v_io_handle PWTCPConnectionProvider::instantiateServer()
{
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(m_port);

  listener = evconnlistener_new_bind(::base, pw_accept_cb, (void *)::base,
      LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
      (struct sockaddr*)&::sin,
      sizeof(::sin));
  if (!listener) {
	  fprintf(stderr, "Could not create a listener!\n");
	  return 1;
  }

  signal_event=NULL;

  signal_event = evsignal_new(::base, SIGINT, signal_cb, (void *)::base);

  if (!signal_event || event_add(signal_event, NULL)<0) {
	  fprintf(stderr, "Could not create/add a signal event!\n");
	  return 1;
  }


  return evconnlistener_get_fd(listener);
}
  
std::shared_ptr<oatpp::data::stream::IOStream> PWTCPConnectionProvider::getConnection(){
  return NULL;
}

}}}
