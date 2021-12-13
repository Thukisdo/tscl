#include <sstream>
#include <Version.hpp>

namespace tscl {

  Version Version::_current = Version(0, 0, 0, "undefined");
  Version const &Version::current = Version::_current;

  bool Version::operator>(const Version &other) const {
    return (major > other.major) or (major == other.major and minor > other.minor) or
           (major == other.major and minor == other.minor and patch > other.patch and
            (tweak.empty() or tweak > other.tweak));
  }

  bool Version::operator<(const tscl::Version &other) const {
    return not(*this == other) and not(*this > other);
  }
  Version::Version(std::string const &version) {
    std::stringstream ss(version);
    if (version.starts_with("v")) ss.ignore(1);

    ss >> major;
    ss.ignore(1);
    ss >> minor;
    ss.ignore(1);
    ss >> patch;
    ss.ignore(1);
    std::getline(ss, tweak);
  }

  Version::Version(size_t major, size_t minor, size_t patch, std::string tweak)
      : major(major), minor(minor), patch(patch), tweak(tweak) {}

}   // namespace tscl