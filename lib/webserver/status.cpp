// Embedded web-server library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "status.h"

#include <base/basicmacros.h>
#include <base/string_helpers.h>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>


namespace webserver {

  WebserverStatus::WebserverStatus() :
    http_served_(webserver::HTTP_CODES_COUNT, 0), 
    sustained_rate_position_(0), 
    attained_rate_position_(0),
    latency_position_(0),
    sustained_rate_1_(0), 
    sustained_rate_5_(0),
    sustained_rate_15_(0),
    max_attained_rate_(0),
    attained_rate_1_(0),
    attained_rate_5_(0),
    attained_rate_15_(0),
    latency_1_(0),
    latency_5_(0),
    latency_15_(0),
    total_served_requests_(0) 
  {
    traffic_in_ = 0;
    traffic_out_ = 0;
    outgoing_rate_ = 0;
    fast_latency_sum_ = 0;
    fast_latency_count_ = 0;
    max_latency_ = 0;
  }


  WebserverStatus::~WebserverStatus() { 
  }


  void WebserverStatus::IncomingRequest(const size_t length) {
    traffic_in_ += length;
  }


  void WebserverStatus::ServedRequest(const OutgoingHttpMessage::sptr& message) {
    http_served_[message->GetResponseCode()]++;
    traffic_out_ += message->GetLength();

    clocks::HiResTimer* t = message->GetTimer();
    if (t) {
      t->Mark();
      double nanoDifference = static_cast<double>(t->GetDifferenceNanoseconds());
      unsigned int latency = static_cast<unsigned int>(t->GetDifferenceSeconds()) * 1000000 +
        static_cast<unsigned int>(::round(nanoDifference / 1000));
      fast_latency_sum_.fetch_and_add(latency);
      ++fast_latency_count_;
      ++outgoing_rate_;

      if (latency > max_latency_ && max_attained_rate_ != 0) {
        max_latency_ = latency;
      }
    }
  }


  uint64_t WebserverStatus::TotalServedRequests() const {
    return total_served_requests_;
  }


  const WebserverStatus::HttpResponseTable& WebserverStatus::GetHttpServed() const {
    return http_served_;
  }


  uint64_t WebserverStatus::GetTrafficIn() const {
    return traffic_in_;
  }


  uint64_t WebserverStatus::GetTrafficOut() const {
    return traffic_out_;
  }


  unsigned int WebserverStatus::SustainedRate1() const {
    return sustained_rate_1_;
  }


  unsigned int WebserverStatus::SustainedRate5() const {
    return sustained_rate_5_;
  }


  unsigned int WebserverStatus::SustainedRate15() const {
    return sustained_rate_15_;
  }


  unsigned int WebserverStatus::AttainedRate1() const {
    return attained_rate_1_;
  }


  unsigned int WebserverStatus::AttainedRate5() const {
    return attained_rate_5_;
  }


  unsigned int WebserverStatus::AttainedRate15() const {
    return attained_rate_15_;
  }


  unsigned int WebserverStatus::MaxAttainedRate() const {
    return max_attained_rate_;
  }


  unsigned int WebserverStatus::MaxLatency() const {
    return max_latency_;
  }


  unsigned int WebserverStatus::Latency1() const {
    return latency_1_ > 0 ? latency_1_ : 0;
  }


  unsigned int WebserverStatus::Latency5() const {
    return latency_5_ > 0 ? latency_5_ : 0;
  }


  unsigned int WebserverStatus::Latency15() const {
    return latency_15_ > 0 ? latency_15_ : 0;
  }


  void WebserverStatus::Run() {
    while (!ShouldStop()) {
      const unsigned int rate = outgoing_rate_;
      outgoing_rate_ -= rate;
      max_attained_rate_ = max_attained_rate_ < rate ? rate : max_attained_rate_;
      CalculateSustainedRate_(rate);
      CalculateAttainedRate_(rate);
      CalculateLatency_();
      CalculateRequests_();
      Wait(1000);
    }
  }


  void WebserverStatus::CalculateSustainedRate_(const unsigned int rate) {
    size_t size_1 = 60;
    size_t size_5 = 300;
    size_t size_15 = 900;

    if (sustained_rate_1_history_.size() < 60) {
      sustained_rate_1_history_.push_back(rate);
      size_1 = sustained_rate_1_history_.size();
    }
    else {
      sustained_rate_1_history_[sustained_rate_position_ % 60] = rate;
    }

    if (sustained_rate_5_history_.size() < 300) {
      sustained_rate_5_history_.push_back(rate);
      size_5 = sustained_rate_5_history_.size();
    }
    else {
      sustained_rate_5_history_[sustained_rate_position_ % 300] = rate;
    }

    if (sustained_rate_15_history_.size() < 900) {
      sustained_rate_15_history_.push_back(rate);
      size_15 = sustained_rate_15_history_.size();
    }
    else {
      sustained_rate_15_history_[sustained_rate_position_] = rate;
    }

    if (++sustained_rate_position_ >= 900) {
      sustained_rate_position_ = 0;
    }

    const float acc_rate_1 = static_cast<float>(std::accumulate(sustained_rate_1_history_.begin(), sustained_rate_1_history_.end(), 0));
    const float acc_rate_5 = static_cast<float>(std::accumulate(sustained_rate_5_history_.begin(), sustained_rate_5_history_.end(), 0));
    const float acc_rate_15 = static_cast<float>(std::accumulate(sustained_rate_15_history_.begin(), sustained_rate_15_history_.end(), 0));

    sustained_rate_1_ = static_cast<unsigned int>(acc_rate_1 / static_cast<float>(size_1));
    sustained_rate_5_ = static_cast<unsigned int>(acc_rate_5 / static_cast<float>(size_5));
    sustained_rate_15_ = static_cast<unsigned int>(acc_rate_15 / static_cast<float>(size_15));
  }


  void WebserverStatus::CalculateAttainedRate_(const unsigned int rate) {
    if (rate == 0) {
      return;
    }

    size_t size_1 = 60;
    size_t size_5 = 300;
    size_t size_15 = 900;

    if (attained_rate_1_history_.size() < 60) {
      attained_rate_1_history_.push_back(rate);
      size_1 = attained_rate_1_history_.size();
    }
    else {
      attained_rate_1_history_[attained_rate_position_ % 60] = rate;
    }

    if (attained_rate_5_history_.size() < 300) {
      attained_rate_5_history_.push_back(rate);
      size_5 = attained_rate_5_history_.size();
    }
    else {
      attained_rate_5_history_[attained_rate_position_ % 300] = rate;
    }

    if (attained_rate_15_history_.size() < 900) {
      attained_rate_15_history_.push_back(rate);
      size_15 = attained_rate_15_history_.size();
    }
    else {
      attained_rate_15_history_[attained_rate_position_] = rate;
    }

    if (++attained_rate_position_ >= 900) {
      attained_rate_position_ = 0;
    }

    const float acc_rate_1 = static_cast<float>(std::accumulate(attained_rate_1_history_.begin(), attained_rate_1_history_.end(), 0));
    const float acc_rate_5 = static_cast<float>(std::accumulate(attained_rate_5_history_.begin(), attained_rate_5_history_.end(), 0));
    const float acc_rate_15 = static_cast<float>(std::accumulate(attained_rate_15_history_.begin(), attained_rate_15_history_.end(), 0));

    attained_rate_1_ = static_cast<unsigned int>(acc_rate_1 / static_cast<float>(size_1));
    attained_rate_5_ = static_cast<unsigned int>(acc_rate_5 / static_cast<float>(size_15));
    attained_rate_15_ = static_cast<unsigned int>(acc_rate_15 / static_cast<float>(size_15));
  }


  void WebserverStatus::CalculateLatency_() {
    if (fast_latency_sum_ != 0 && fast_latency_count_ != 0) {
      float average = static_cast<float>(fast_latency_sum_) / static_cast<float>(fast_latency_count_);
      fast_latency_sum_ = 0;
      fast_latency_count_ = 0;

      if (average > 0) {
        size_t size_1 = 60;
        size_t size_5 = 300;
        size_t size_15 = 900;

        if (latency_1_history_.size() < 60) {
          latency_1_history_.push_back(average);
          size_1 = latency_1_history_.size();
        }
        else {
          latency_1_history_[latency_position_ % 60] = average;
        }

        if (latency_5_history_.size() < 300) {
          latency_5_history_.push_back(average);
          size_5 = latency_5_history_.size();
        }
        else {
          latency_5_history_[latency_position_ % 300] = average;
        }

        if (latency_15_history_.size() < 900) {
          latency_15_history_.push_back(average);
          size_15 = latency_15_history_.size();
        }
        else {
          latency_15_history_[latency_position_] = average;
        }

        if (++latency_position_ >= 900) {
          latency_position_ = 0;
        }

        latency_1_ = static_cast<unsigned int>(std::accumulate(latency_1_history_.begin(), latency_1_history_.end(), 0.0) / static_cast<float>(size_1));
        latency_5_ = static_cast<unsigned int>(std::accumulate(latency_5_history_.begin(), latency_5_history_.end(), 0.0) / static_cast<float>(size_5));
        latency_15_ = static_cast<unsigned int>(std::accumulate(latency_15_history_.begin(), latency_15_history_.end(), 0.0) / static_cast<float>(size_15));
      }
    }
  }


  void WebserverStatus::CalculateRequests_() {
    total_served_requests_ = 0;
    for (HttpResponseTable::const_iterator i = http_served_.begin(); i != http_served_.end(); ++i) {
      total_served_requests_ += *i;
    }
  }

} // namespace webserver
