/***************************************************************************
 *  *
 *   * Project         _____    __   ____   _      _
 *    *                (  _  )  /__\ (_  _)_| |_  _| |_
 *     *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *      *                (_____)(__)(__)(__)  |_|    |_|
 *       *
 *        *
 *         * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *          *
 *           * Licensed under the Apache License, Version 2.0 (the "License");
 *            * you may not use this file except in compliance with the License.
 *             * You may obtain a copy of the License at
 *              *
 *               *     http://www.apache.org/licenses/LICENSE-2.0
 *                *
 *                 * Unless required by applicable law or agreed to in writing, software
 *                  * distributed under the License is distributed on an "AS IS" BASIS,
 *                   * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *                    * See the License for the specific language governing permissions and
 *                     * limitations under the License.
 *                      *
 *                       ***************************************************************************/

#include "PWServer.hpp"
#include "oatpp/network/Connection.hpp"

#include <thread>
#include <chrono>

namespace oatpp { namespace network { namespace server {
  
const v_int32 PWServer::STATUS_CREATED = 0;
const v_int32 PWServer::STATUS_RUNNING = 1;
const v_int32 PWServer::STATUS_STOPPING = 2;
const v_int32 PWServer::STATUS_DONE = 3;

PWServer::PWServer(const std::shared_ptr<ServerConnectionProvider>& connectionProvider,
               const std::shared_ptr<ConnectionHandler>& connectionHandler)
  : m_status(STATUS_CREATED)
  , m_connectionProvider(connectionProvider)
  , m_connectionHandler(connectionHandler)
{}

void PWServer::addConn(oatpp::data::v_io_handle handle){
  std::shared_ptr<const std::unordered_map<oatpp::String, oatpp::String>> params;

  if (getStatus() != STATUS_RUNNING) {
    setStatus(STATUS_CREATED, STATUS_RUNNING);
  }
    
  auto connection = Connection::createShared(handle);
    
  if (connection) {
    if(getStatus() == STATUS_RUNNING){
      m_connectionHandler->handleConnection(connection, params /* null params */);
    } else {
      OATPP_LOGD("PWServer", "Already stopped. Closing connection...");
    }
  }
}

void PWServer::run(){
}
  
void PWServer::stop(){
  setStatus(STATUS_STOPPING);
}

bool PWServer::setStatus(v_int32 expectedStatus, v_int32 newStatus){
  v_int32 expected = expectedStatus;
  return m_status.compare_exchange_weak(expected, newStatus);
}
  
void PWServer::setStatus(v_int32 status){
  m_status.store(status);
}
  
v_int32 PWServer::getStatus(){
  return m_status.load();
}


}}}
