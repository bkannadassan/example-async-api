//
//  MyController.hpp
//  web-starter-project
//
//  Created by Leonid on 2/12/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef MyController_hpp
#define MyController_hpp

#include "dto/MyDTOs.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include <execinfo.h>


/**
 *  EXAMPLE ApiController
 *  Basic examples of howto create ENDPOINTs
 *  More details on oatpp.io
 */
class MyController : public oatpp::web::server::api::ApiController {
protected:
  MyController(const std::shared_ptr<ObjectMapper>& objectMapper)
  : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:
  /**
   *  Inject @objectMapper component here as default parameter
   *  Do not return bare Controllable* object! use shared_ptr!
   */
  static std::shared_ptr<MyController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
                                                                    objectMapper)){
    return std::shared_ptr<MyController>(new MyController(objectMapper));
  }
  
  /**
   *  Begin ENDPOINTs generation ('ApiController' codegen)
   */
  static void show_stackframe() {
    printf("\n Bactrace Tracing \n\n");
    void *trace[16];
    char **messages = (char **)NULL;
    int i, trace_size = 0;

    trace_size = backtrace(trace, 16);
    messages = backtrace_symbols(trace, trace_size);
    printf("[bt] Execution path:\n");
    for (i=0; i<trace_size; ++i)
	  printf("[bt] %s\n", messages[i]);
    return;
  }

#include OATPP_CODEGEN_BEGIN(ApiController)
  
  /**
   *  Hello World endpoint Coroutine mapped to the "/" (root)
   */
  ENDPOINT_ASYNC("GET", "/", Root) {
    
    ENDPOINT_ASYNC_INIT(Root)
    
    /**
     *  Coroutine entrypoint act()
     *  returns Action (what to do next)
     */
    Action act() override {
      show_stackframe();
      auto dto = HelloDto::createShared();
      dto->message = "Hello Async!";
      dto->server = Header::Value::SERVER;
      dto->userAgent = request->getHeader(Header::USER_AGENT);
      return _return(controller->createDtoResponse(Status::CODE_200, dto));
    }
    
  };
  
  ENDPOINT_ASYNC("POST", "/", PostEchoStringBody) {
    
    ENDPOINT_ASYNC_INIT(PostEchoStringBody)
    
    Action act() override {
      /* return Action to start child coroutine to read body */
      MyController::show_stackframe();
      return request->readBodyToStringAsync().callbackTo(&PostEchoStringBody::returnResponse);
    }
    
    Action returnResponse(const oatpp::String& body){
      /* return Action to return created OutgoingResponse */
      MyController::show_stackframe();
      return _return(controller->createResponse(Status::CODE_200, body));
    }
    
  };

  /**
   *  Echo body endpoint Coroutine. Mapped to "/body/string".
   *  Returns body received in the request
   */
  ENDPOINT_ASYNC("GET", "/body/string", EchoStringBody) {
    
    ENDPOINT_ASYNC_INIT(EchoStringBody)
    
    Action act() override {
      /* return Action to start child coroutine to read body */
      MyController::show_stackframe();
      return request->readBodyToStringAsync().callbackTo(&EchoStringBody::returnResponse);
    }
    
    Action returnResponse(const oatpp::String& body){
      /* return Action to return created OutgoingResponse */
      MyController::show_stackframe();
      return _return(controller->createResponse(Status::CODE_200, body));
    }
    
  };
  
  /**
   *  Echo body endpoint Coroutine. Mapped to "/body/dto".
   *  Deserialize DTO reveived, and return same DTO
   *  Returns body as MessageDto received in the request
   */
  ENDPOINT_ASYNC("GET", "/body/dto", EchoDtoBody) {
    
    ENDPOINT_ASYNC_INIT(EchoDtoBody)
    
    Action act() override {
      return request->readBodyToDtoAsync<MessageDto>(controller->getDefaultObjectMapper()).callbackTo(&EchoDtoBody::returnResponse);
    }
    
    Action returnResponse(const MessageDto::ObjectWrapper& body){
      return _return(controller->createDtoResponse(Status::CODE_200, body));
    }
    
  };
  
  ENDPOINT_ASYNC("POST", "demo/api_async/json", PostJSONAsync) {

    ENDPOINT_ASYNC_INIT(PostJSONAsync)

    Action act() override {
      return request->readBodyToDtoAsync<MessageDto>(controller->getDefaultObjectMapper())
                   .callbackTo(&PostJSONAsync::onBodyObtained);
    }

    Action onBodyObtained(const MessageDto::ObjectWrapper& dto) {
      return _return(controller->createResponse(Status::CODE_200, "dtoMessage: " + dto->message));
    }
  };

  /**
   *  Finish ENDPOINTs generation ('ApiController' codegen)
   */
#include OATPP_CODEGEN_END(ApiController)
  
};

#endif /* MyController_hpp */
