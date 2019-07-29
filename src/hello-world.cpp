/*
  This example program provides a trivial server program that listens for TCP
  connections on port 9995.  When they arrive, it writes a short message to
  each client connection, and closes each connection once it is flushed.

  Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
*/


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
/**********************************OATPP Implementation****************************************/
#include "./controller/MyController.hpp"
#include "./PWServer.hpp"
#include "./AppComponent.hpp"
#include "./PWTCPConnectionProvider.hpp"
#include "oatpp/network/server/Server.hpp"
#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/Types.hpp"
/**********************************OATPP Implementation****************************************/
#ifndef _WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif
using namespace oatpp::network::server;
oatpp::network::server::PWServer *pwserver;

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>


struct event_base *base;
struct evconnlistener *listener;
static struct event *signal_event;

struct sockaddr_in sin;
static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 9995;
static const int PORT1 = 8000;

void listener_cb(struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

/**********************************OATPP Implementation****************************************/
int oatpp_data_processing_threads = 1; 
int oatpp_io_threads = 1;
int oatpp_timer_threads = 1;
std::string oatpp_ip("0.0.0.0");
int oatpp_port = 11000;

static void oatpp_start_server()
{
  static AppComponent components;

  static auto router = components.httpRouter.getObject();

  static auto myController = MyController::createShared();

  myController->addEndpointsToRouter(router);

  pwserver = new PWServer(components.serverConnectionProvider.getObject(),
                          components.serverConnectionHandler.getObject());

  pwserver->setStatus(Server::STATUS_CREATED, Server::STATUS_RUNNING);

  OATPP_LOGD("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());
  OATPP_LOGD("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("host").toString()->c_str());
}
/**********************************OATPP Implementation****************************************/

int
main(int argc, char **argv)
{
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}

        {
	  memset(&sin, 0, sizeof(sin));
	  sin.sin_family = AF_INET;
	  sin.sin_port = htons(PORT);

	  listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));
        }

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

/**********************************OATPP Implementation****************************************/
        oatpp::base::Environment::init();
        oatpp_start_server();
/**********************************OATPP Implementation****************************************/
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);
/**********************************OATPP Implementation****************************************/
        oatpp::base::Environment::destroy();
/**********************************OATPP Implementation****************************************/

	printf("done\n");
	return 0;
}

void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (event_base *)user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_disable(bev, EV_READ);

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
}

static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}

static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (event_base *)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}
