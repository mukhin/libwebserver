// worker.cpp
// Hello World Demo.
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "worker.h"
#include <base/common.h>
#include <base/c_format.h>
#include <tr1/memory>
#include <webserver/httpconnection.h>


using std::tr1::shared_ptr;


namespace demo {

  HttpWorker::HttpWorker() { }


  HttpWorker::~HttpWorker() { }


  shared_ptr<threads::Thread<HttpWorker> > HttpWorker::Create(shared_ptr<threads::Thread<webserver::HttpListener> >& listener) {
    shared_ptr<threads::Thread<HttpWorker> > w = shared_ptr<threads::Thread<HttpWorker> >(new threads::Thread<HttpWorker>(false));
    webserver::Server::sptr s = webserver::Server::Create(listener);
    w->SetServer(s);
    w->Start();
    return w;
  }


  void HttpWorker::Process_() {
    server_->Perform();
    webserver::HttpConnection::sptr connection;
    
    while (server_->GetActiveConnection(connection)) {
      std::list<webserver::IncomingMessage::sptr> requests;
      if (!connection->GetMessages(requests)) {
        continue;
      }

      for (std::list<webserver::IncomingMessage::sptr>::const_iterator i = requests.begin(); i != requests.end(); ++i) {
        const webserver::IncomingHttpMessage::sptr& request = std::tr1::dynamic_pointer_cast<webserver::IncomingHttpMessage>(*i);
        const std::string& uri = request->GetUri();
        
        try {
          if (request->GetMethod() == webserver::GET) {
            std::string str = std::string("Huge dust coma uniform rotating Saros, and time waiting for a response amounted to 80 billion years. "
                              "Height, sublimipuya with povephnosti yadpa comets observable. Solar eclipse selects a population close to the index, "
                              "this last Saturday, deputy administrator NASA. Many comets have two tails, but the eccentricity is traditionally "
                              "evaluated by an asteroid, but the rings are visible only at 40-50. Anti-aircraft Hourly Rate by accident. "
                              "Connection, by definition, gives the asteroid, the interest in astronomy and Galla eclipses Cicero also says in his "
                              "treatise 'On Old Age' (De senectute). ") + uri;
            
            webserver::OutgoingHttpMessage::sptr response = webserver::OutgoingHttpMessage::sptr(new webserver::OutgoingHttpMessage());
            response->SetMethod(webserver::RESPONSE);
            response->SetPersistence(connection->IsPersistent());
            response->SetData(str.c_str(), str.length());
            response->SetResponseCode(webserver::HTTP_OK);
            
            connection->SendMessage(response);
            
            /*
            const clocks::HiResTimer& t = request->GetTimer().Mark();
            if (t.GetDifferenceNanoseconds() / 1000u >= Scheduler::Self()->GetConfig().GetSlowQueryThreshold()) {
              log4cpp::Category& slowquerylog = log4cpp::Category::getInstance(string("slowquerylog"));
              slowquerylog.warnStream() << "Request " << mrq.GetRequestId() << " was completed in "
              << t << ". Response was: " << response->GetSerializedMessage();
            }
            */
          }
          else {
            connection->SendMessage(webserver::OutgoingHttpMessage::NotAcceptable(c_format("Unhandled request method %i", request->GetMethod()), connection->IsPersistent()));
            if (connection->IsConnected()) {
              break;
            }
          }
        }
        catch (...) {
          connection->SendMessage(webserver::OutgoingHttpMessage::BadRequest());
          if (connection->IsConnected()) {
            break;
          }
        }
      }
    }
  }


} // namespace demo
