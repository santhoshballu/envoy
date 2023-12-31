#pragma once

#include "envoy/extensions/filters/http/geoip/v3/geoip.pb.h"
#include "envoy/extensions/filters/http/geoip/v3/geoip.pb.validate.h"

#include "source/extensions/filters/http/common/factory_base.h"
#include "source/extensions/filters/http/geoip/geoip_provider_config.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Geoip {

/**
 * Config registration for the geoip filter. @see NamedHttpFilterConfigFactory.
 */
class GeoipFilterFactory
    : public Common::FactoryBase<envoy::extensions::filters::http::geoip::v3::Geoip> {
public:
  GeoipFilterFactory() : FactoryBase("envoy.filters.http.geoip") {}

  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::http::geoip::v3::Geoip& proto_config,
      const std::string& stats_prefix, Server::Configuration::FactoryContext& context) override;

private:
  GeoipProviderFactoryContextPtr provider_context_;
};

} // namespace Geoip
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
