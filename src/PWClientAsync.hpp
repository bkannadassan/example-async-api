#ifndef PWClientAsync_hpp
#define PWClientAsync_hpp

#include "./PWClient.hpp"

#include "oatpp/core/async/Executor.hpp"

class PWClientAsync {
private:
  typedef oatpp::web::protocol::http::incoming::Response Response;
private:
  constexpr static const char* TAG = "PWClientAsync";
private:
  
  class SendDtoCoroutine : public oatpp::async::Coroutine<SendDtoCoroutine> {
  private:
    std::shared_ptr<PWClient> m_client;
    oatpp::String m_message;
    v_int32 m_code;
  public:
    
    SendDtoCoroutine(const std::shared_ptr<PWClient> client,
                    const oatpp::String& message,
                    v_int32 code)
      : m_client(client)
      , m_message(message)
      , m_code(code)
    {}
    
    /**
     * Create DTO and send it via doPostWithDtoAsync method
     */
    Action act() override {
      printf("\n I have Sent Async Out \n");
      auto dto = MyRequestDto::createShared();
      dto->message = m_message;
      dto->code = m_code;
      return m_client->doPostWithDtoAsync(dto).callbackTo(&SendDtoCoroutine::onResponse);
    }
    
    /**
     * Receive response and read body asynchronously
     */
    Action onResponse(const std::shared_ptr<Response>& response) {
      printf("\n I have Received the Async Response Back \n");
      return response->readBodyToStringAsync().callbackTo(&SendDtoCoroutine::onBody);
    }
    
    /**
     * Print read body from the stream and finish
     */
    Action onBody(const oatpp::String& body) {
      OATPP_LOGD(TAG, "[doPostWithDtoAsync] data='%s'", body->c_str());
      return finish();
    }
    
  };
  
  class SendCoroutine : public oatpp::async::Coroutine<SendCoroutine> {
  private:
    std::shared_ptr<PWClient> m_client;
  public:
    
    SendCoroutine(const std::shared_ptr<PWClient> client) : m_client(client) {}

    Action act() override {
      return m_client->doPostAsync("<POST-DATA-HERE>").callbackTo(&SendDtoCoroutine::onResponse);
    }
    
    Action onResponse(const std::shared_ptr<Response>& response) {
      return response->readBodyToStringAsync().callbackTo(&SendDtoCoroutine::onBody);
    }
    
    Action onBody(const oatpp::String& body) {
      OATPP_LOGD(TAG, "[SendCoroutine. doPostAsync] data='%s'", body->c_str());
      return finish();
    }
    
  };
  
public:
  
  void static runExample(const std::shared_ptr<PWClient>& client);
  
};

#endif /* PWClientAsync_hpp */
