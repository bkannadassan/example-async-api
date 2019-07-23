//
//  main.cpp
//  web-starter-project
//
//  Created by Leonid on 2/12/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#include "./controller/MyController.hpp"
#include "./AppComponent.hpp"
#include "./PWTCPConnectionProvider.hpp"

#include "oatpp/network/server/Server.hpp"

#include <iostream>

const v_int32 STATUS_CREATED = 0;
const v_int32 STATUS_RUNNING = 1;
const v_int32 STATUS_STOPPING = 2;
const v_int32 STATUS_DONE = 3;

std::shared_ptr<oatpp::network::server::PWTCPConnectionProvider> connectionProvider;
std::shared_ptr<oatpp::web::server::HttpConnectionHandler> connectionHandler;
std::shared_ptr<oatpp::web::server::AsyncHttpConnectionHandler> aconnectionHandler;

/**
 *  run() method.
 *  1) set Environment components.
 *  2) add ApiController's endpoints to router
 *  3) run server
 */
class Handler : public oatpp::web::server::HttpRequestHandler {
public:

  /**
   * Handle incoming request and return outgoing response.
   */
  std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
    return ResponseFactory::createResponse(Status::CODE_200, "Hello World!");
  }
}; 
void run() {
/*
  auto router = oatpp::web::server::HttpRouter::createShared();

  connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

  connectionProvider = oatpp::network::server::PWTCPConnectionProvider::createShared(11000);

  static oatpp::network::server::Server server(connectionProvider, connectionHandler);

  OATPP_LOGD("Server", "Running on port %s...", connectionProvider->getProperty("port").toString()->c_str());
  OATPP_LOGD("Server", "Running on port %s...", connectionProvider->getProperty("host").toString()->c_str());
*/
  static AppComponent components; // Create scope Environment components

  /* create ApiControllers and add endpoints to router */

  auto router = components.httpRouter.getObject();

  auto myController = MyController::createShared();
  myController->addEndpointsToRouter(router);

  /* create server */

  static oatpp::network::server::Server server(components.serverConnectionProvider.getObject(),
                                        components.serverConnectionHandler.getObject());

  server.setStatus(::STATUS_CREATED, ::STATUS_RUNNING);

  OATPP_LOGD("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());
  OATPP_LOGD("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("host").toString()->c_str());
 
  //server.run();
}

void run1()
{
  run();
  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";
  
}
