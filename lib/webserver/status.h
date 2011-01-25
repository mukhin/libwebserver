// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef WEBSERVER_STATUS_H__
#define WEBSERVER_STATUS_H__

#include "httptypes.h"
#include "outgoinghttpmessage.h"

#include <base/hash.tbb.h>
#include <base/prototype.h>
#include <inttypes.h>
#include <map>
#include <string>
#include <tbb/atomic.h>
#include <tbb/concurrent_vector.h>
#include <threads/singleton.h>
#include <threads/thread.h>


namespace webserver {

  class WebserverStatus : public threads::Worker {
  public:
    typedef tbb::concurrent_vector<uint64_t> HttpResponseTable;

    WebserverStatus();
    ~WebserverStatus();

    void IncomingRequest(const size_t length);
    void ServedRequest(const OutgoingHttpMessage::sptr& message);
    uint64_t TotalServedRequests() const;
    const HttpResponseTable& GetHttpServed() const;

    uint64_t GetTrafficIn() const;
    uint64_t GetTrafficOut() const;

    unsigned int SustainedRate1() const;
    unsigned int SustainedRate5() const;
    unsigned int SustainedRate15() const;

    unsigned int MaxAttainedRate() const;
    unsigned int AttainedRate1() const;
    unsigned int AttainedRate5() const;
    unsigned int AttainedRate15() const;

    unsigned int MaxLatency() const;
    unsigned int Latency1() const;
    unsigned int Latency5() const;
    unsigned int Latency15() const;

    void Run();

  private:
    void CalculateSustainedRate_(const unsigned int rate);
    void CalculateAttainedRate_(const unsigned int rate);
    void CalculateLatency_();
    void CalculateRequests_();

    HttpResponseTable http_served_;
    tbb::atomic<uint64_t> traffic_in_;
    tbb::atomic<uint64_t> traffic_out_;
    tbb::atomic<unsigned int> outgoing_rate_;
    unsigned int sustained_rate_position_;
    unsigned int attained_rate_position_;
    unsigned int latency_position_;
    tbb::atomic<unsigned int> max_latency_;

    unsigned int sustained_rate_1_;
    unsigned int sustained_rate_5_;
    unsigned int sustained_rate_15_;

    unsigned int max_attained_rate_;
    unsigned int attained_rate_1_;
    unsigned int attained_rate_5_;
    unsigned int attained_rate_15_;

    tbb::atomic<unsigned int> fast_latency_sum_;
    tbb::atomic<unsigned int> fast_latency_count_;
    unsigned int latency_1_;
    unsigned int latency_5_;
    unsigned int latency_15_;

    uint64_t total_served_requests_;

    tbb::concurrent_vector<unsigned int> sustained_rate_1_history_;
    tbb::concurrent_vector<unsigned int> sustained_rate_5_history_;
    tbb::concurrent_vector<unsigned int> sustained_rate_15_history_;

    tbb::concurrent_vector<unsigned int> attained_rate_1_history_;
    tbb::concurrent_vector<unsigned int> attained_rate_5_history_;
    tbb::concurrent_vector<unsigned int> attained_rate_15_history_;

    tbb::concurrent_vector<float> latency_1_history_;
    tbb::concurrent_vector<float> latency_5_history_;
    tbb::concurrent_vector<float> latency_15_history_;
  };

  typedef threads::Singleton<WebserverStatus> Status;


} // namespace webserver

#endif // WEBSERVER_STATUS_H__
