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

#ifndef network_server_PWServer_hpp
#define network_server_PWServer_hpp

#include <oatpp/network/server/ConnectionHandler.hpp>

#include <oatpp/network/ConnectionProvider.hpp>

#include "oatpp/core/Types.hpp"

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <atomic>

namespace oatpp { namespace network { namespace server {

/**
 * PWServer calls &id:oatpp::network::ConnectionProvider::getConnection; in the loop and passes obtained Connection
 * to &id:oatpp::network::server::ConnectionHandler;.
 */
class PWServer : public base::Countable {
private:
  std::atomic<v_int32> m_status;
  
  oatpp::String m_port;
  
  std::shared_ptr<ServerConnectionProvider> m_connectionProvider;
  std::shared_ptr<ConnectionHandler> m_connectionHandler;
  
public:

  void addConn(oatpp::data::v_io_handle);
  bool setStatus(v_int32 expectedStatus, v_int32 newStatus);
  void setStatus(v_int32 status);
  /**
   * Constructor.
   * @param connectionProvider - &id:oatpp::network::ConnectionProvider;.
   * @param connectionHandler - &id:oatpp::network::server::ConnectionHandler;.
   */
  PWServer(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
         const std::shared_ptr<ConnectionHandler>& connectionHandler);
  
public:

  /**
   * Status constant.
   */
  static const v_int32 STATUS_CREATED;

  /**
   * Status constant.
   */
  static const v_int32 STATUS_RUNNING;

  /**
   * Status constant.
   */
  static const v_int32 STATUS_STOPPING;

  /**
   * Status constant.
   */
  static const v_int32 STATUS_DONE;

  /**
   * Create shared PWServer.
   * @param connectionProvider - &id:oatpp::network::ConnectionProvider;.
   * @param connectionHandler - &id:oatpp::network::server::ConnectionHandler;.
   * @return - `std::shared_ptr` to PWServer.
   */
  static std::shared_ptr<PWServer> createShared(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
                                              const std::shared_ptr<ConnectionHandler>& connectionHandler){
    return std::make_shared<PWServer>(connectionProvider, connectionHandler);
  }

  /**
   * Call &id:oatpp::network::ConnectionProvider::getConnection; in the loop and passes obtained Connection
   * to &id:oatpp::network::server::ConnectionHandler;.
   */
  void run();

  /**
   * Break server loop.
   * Note: thread can still be blocked on the &l:PWServer::run (); call as it may be waiting for ConnectionProvider to provide connection.
   */
  void stop();

  /**
   * Get server status.
   * @return - one of:<br>
   * <ul>
   *   <li>&l:PWServer::STATUS_CREATED;</li>
   *   <li>&l:PWServer::STATUS_RUNNING;</li>
   *   <li>&l:PWServer::STATUS_STOPPING;</li>
   *   <li>&l:PWServer::STATUS_DONE;</li>
   * </ul>
   */
  v_int32 getStatus();
  
};

  
}}}

#endif /* network_server_PWServer_hpp */
