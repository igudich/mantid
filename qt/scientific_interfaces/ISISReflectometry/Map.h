#ifndef MANTID_ISISREFLECTOMETRY_MAP_H
#define MANTID_ISISREFLECTOMETRY_MAP_H
#include <vector>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <boost/optional.hpp>

namespace MantidQt {
namespace CustomInterfaces {

template <typename Container, typename Transform,
          typename Out = typename std::result_of<
              Transform(typename Container::value_type)>::type>
std::vector<Out> map(Container const &in, Transform transform) {
  auto out = std::vector<Out>();
  out.reserve(in.size());
  std::transform(in.cbegin(), in.cend(), std::back_inserter(out), transform);
  return out;
}

template <typename In, typename Transform,
          typename Out = typename std::result_of<Transform(In)>::type>
boost::optional<Out> map(boost::optional<In> const &in, Transform transform) {
  if (in.is_initialized())
    return transform(in.get());
  else
    return boost::none;
}

template <typename T>
std::string optionalToString(boost::optional<T> maybeValue) {
  return map(maybeValue, [](T const &value) -> std::string {
    return std::to_string(value);
  }).get_value_or(std::string());
}
}
}
#endif // MANTID_ISISREFLECTOMETRY_MAP_H