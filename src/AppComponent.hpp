//
//  AppComponent.hpp
//  oatpp-web-starter
//
//  Created by Leonid on 3/2/18.
//  Copyright © 2018 lganzzzo. All rights reserved.
//

#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "./PWTCPConnectionProvider.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/component.hpp"

extern std::shared_ptr<oatpp::network::server::PWTCPConnectionProvider> connectionProvider;
extern std::shared_ptr<oatpp::web::server::AsyncHttpConnectionHandler> aconnectionHandler;

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
public:

  /**
   * Create Async Executor
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
    return std::make_shared<oatpp::async::Executor>(
      1 /* Data-Processing threads */,
      1 /* I/O threads */,
      1 /* Timer threads */
    );
  }());

  /**
   *  Create ConnectionProvider component which listens on the port
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
    /* non_blocking connections should be used with AsyncHttpConnectionHandler for AsyncIO */
     connectionProvider = oatpp::network::server::PWTCPConnectionProvider::ccreateShared("10.62.18.29", 11000);
     return connectionProvider;
    //return oatpp::network::server::PWTCPConnectionProvider::createShared(11000);
  }());
  
  /**
   *  Create Router component
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());
  
  /**
   *  Create ConnectionHandler component which uses Router component to route requests
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, serverConnectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor); // get Async executor component
    aconnectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
    return aconnectionHandler;
  }());
  
  /**
   *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
    auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
    auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
    deserializerConfig->allowUnknownFields = false;
    auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig, deserializerConfig);
    return objectMapper;
  }());

};

#endif /* AppComponent_hpp */
