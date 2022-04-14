//
// Created by thukisdo on 12/12/2021.
//

#pragma once
#include <iostream>
#include <utility>


namespace tscl {

  class Version {
    friend std::ostream &operator<<(std::ostream &os, const Version &version);
    static Version _current;

  public:
    static Version const &current;

    Version() { *this = current; }

    Version(std::string const &version);
    Version(size_t major, size_t minor, size_t patch, size_t tweak = 0);
    Version(size_t major, size_t minor, size_t patch, std::string tweak = "");

    static void setCurrent(Version const &version) { _current = version; }

    bool operator==(const Version &other) const {
      return major_ver == other.major_ver && minor_ver == other.minor_ver && patch_ver == other.patch_ver;
    }

    bool operator!=(const Version &other) const { return not(*this == other); }
    bool operator<(const Version &other) const;
    bool operator>(const Version &other) const;

    [[nodiscard]] size_t getMajor() const { return major_ver; }
    void setMajor(size_t m) { major_ver = m; }

    [[nodiscard]] size_t getMinor() const { return minor_ver; }
    void setMinor(size_t m) { minor_ver = m; }

    [[nodiscard]] size_t getPatch() const { return patch_ver; }
    void setPatch(size_t p) { patch_ver = p; }

    [[nodiscard]] std::string getTweak() const { return tweak; }
    void setTweak(std::string t) { tweak = std::move(t); }


    explicit operator std::string() const { return to_string(); }

    [[nodiscard]] std::string to_string() const {
      return std::to_string(major_ver) + "." + std::to_string(minor_ver) + "." + std::to_string(patch_ver) +
             "." + tweak;
    }

  private:
    size_t major_ver, minor_ver, patch_ver;
    std::string tweak;
  };
}   // namespace tscl