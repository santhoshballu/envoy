#pragma once

#include <atomic>
#include <cstdint>
#include <list>
#include <memory>

#include "envoy/common/time.h"
#include "envoy/event/deferred_deletable.h"
#include "envoy/event/dispatcher.h"
#include "envoy/network/filter.h"
#include "envoy/network/listen_socket.h"
#include "envoy/network/listener.h"

#include "source/common/common/linked_object.h"
#include "source/common/network/listener_filter_buffer_impl.h"
#include "source/server/active_listener_base.h"

namespace Envoy {
namespace Server {

class ActiveStreamListenerBase;

/**
 * Wrapper for an active accepted socket owned by the active tcp listener.
 */
class ActiveTcpSocket : public Network::ListenerFilterManager,
                        public Network::ListenerFilterCallbacks,
                        public LinkedObject<ActiveTcpSocket>,
                        public Event::DeferredDeletable,
                        Logger::Loggable<Logger::Id::conn_handler> {
public:
  ActiveTcpSocket(ActiveStreamListenerBase& listener, Network::ConnectionSocketPtr&& socket,
                  bool hand_off_restored_destination_connections);
  ~ActiveTcpSocket() override;

  void onTimeout();
  void startTimer();
  void unlink();
  void newConnection();

  class GenericListenerFilter : public Network::ListenerFilter {
  public:
    GenericListenerFilter(const Network::ListenerFilterMatcherSharedPtr& matcher,
                          Network::ListenerFilterPtr listener_filter)
        : listener_filter_(std::move(listener_filter)), matcher_(std::move(matcher)) {}
    Network::FilterStatus onAccept(ListenerFilterCallbacks& cb) override {
      if (isDisabled(cb)) {
        return Network::FilterStatus::Continue;
      }
      return listener_filter_->onAccept(cb);
    }

    Network::FilterStatus onData(Network::ListenerFilterBuffer& buffer) override {
      return listener_filter_->onData(buffer);
    }

    size_t maxReadBytes() const override { return listener_filter_->maxReadBytes(); }

    /**
     * Check if this filter filter should be disabled on the incoming socket.
     * @param cb the callbacks the filter instance can use to communicate with the filter chain.
     **/
    bool isDisabled(ListenerFilterCallbacks& cb) {
      if (matcher_ == nullptr) {
        return false;
      } else {
        return matcher_->matches(cb);
      }
    }

  private:
    const Network::ListenerFilterPtr listener_filter_;
    const Network::ListenerFilterMatcherSharedPtr matcher_;
  };
  using ListenerFilterWrapperPtr = std::unique_ptr<GenericListenerFilter>;

  // Network::ListenerFilterManager
  void addAcceptFilter(const Network::ListenerFilterMatcherSharedPtr& listener_filter_matcher,
                       Network::ListenerFilterPtr&& filter) override {
    accept_filters_.emplace_back(
        std::make_unique<GenericListenerFilter>(listener_filter_matcher, std::move(filter)));
  }

  // Network::ListenerFilterCallbacks
  Network::ConnectionSocket& socket() override { return *socket_.get(); }

  void startFilterChain() { continueFilterChain(true); }

  void setDynamicMetadata(const std::string& name, const ProtobufWkt::Struct& value) override;
  envoy::config::core::v3::Metadata& dynamicMetadata() override {
    return stream_info_->dynamicMetadata();
  };
  const envoy::config::core::v3::Metadata& dynamicMetadata() const override {
    return stream_info_->dynamicMetadata();
  };
  StreamInfo::FilterState& filterState() override { return *stream_info_->filterState().get(); }
  StreamInfo::StreamInfo* streamInfo() const { return stream_info_.get(); }
  bool connected() const { return connected_; }
  bool isEndFilterIteration() const { return iter_ == accept_filters_.end(); }

private:
  /**
   * If a filter returned `FilterStatus::ContinueIteration`, `continueFilterChain(true)`
   * should be called to continue the filter chain iteration. Or `continueFilterChain(false)`
   * should be called if the filter returned `FilterStatus::StopIteration` and closed
   * the socket.
   * @param success boolean telling whether the filter execution was successful or not.
   */
  void continueFilterChain(bool success);

  void createListenerFilterBuffer();

  // The owner of this ActiveTcpSocket.
  ActiveStreamListenerBase& listener_;
  Network::ConnectionSocketPtr socket_;
  const bool hand_off_restored_destination_connections_;
  std::list<ListenerFilterWrapperPtr> accept_filters_;
  std::list<ListenerFilterWrapperPtr>::iterator iter_;
  Event::TimerPtr timer_;
  std::unique_ptr<StreamInfo::StreamInfo> stream_info_;
  bool connected_{false};

  Network::ListenerFilterBufferImplPtr listener_filter_buffer_;
};

} // namespace Server
} // namespace Envoy