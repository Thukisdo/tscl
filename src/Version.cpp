#include <sstream>
#include <Version.hpp>

namespace tscl {

  Version Version::_current = Version(0, 0, 0, "undefined");
  Version const &Version::current = Version::_current;


  Version::Version(size_t major, size_t minor, size_t patch, std::string tweak)
          : major_ver(major), minor_ver(minor), patch_ver(patch), tweak(tweak) {}

  Version::Version(size_t major, size_t minor, size_t patch, size_t tweak) :
          major_ver(major), minor_ver(minor), patch_ver(patch), tweak(std::to_string(tweak)) {

  }

  bool Version::operator>(const Version &other) const {
    return (major_ver > other.major_ver) or (major_ver == other.major_ver and minor_ver > other.minor_ver) or
           (major_ver == other.major_ver and minor_ver == other.minor_ver and patch_ver > other.patch_ver and
            (tweak.empty() or tweak > other.tweak));
  }

  bool Version::operator<(const tscl::Version &other) const {
    return not(*this == other) and not(*this > other);
  }

  Version::Version(std::string const &version) {
    std::stringstream ss(version);
    if (version.starts_with("v")) { ss.ignore(1); }

    ss >> major_ver;
    ss.ignore(1);
    ss >> minor_ver;
    ss.ignore(1);
    ss >> patch_ver;
    ss.ignore(1);
    std::getline(ss, tweak);
  }

}   // namespace tscl