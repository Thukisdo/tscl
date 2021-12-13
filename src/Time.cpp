//
// Created by thukisdo on 12/12/2021.
//

#include "Time.hpp"

namespace tscl {

  using namespace std::chrono;

  std::string timestamp(timestamp_t tst) {
    std::stringstream ss;


    if (tst == timestamp_t::None) return "";
    else if (tst == timestamp_t::Delta) {
      high_resolution_clock::time_point current = high_resolution_clock::now();
      std::chrono::duration<double> tmp = current - program_start;

      ss << std::fixed << std::setprecision(5) << tmp.count() << 's';
    } else if (tst == timestamp_t::Partial) {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);

      ss << std::put_time(&tm, "%H:%M:%S");
    } else if (tst == timestamp_t::Full) {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);

      ss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    }

    return ss.str();
  }

  Chrono::Chrono() : m_paused(false), m_current_duration(0) {
    m_begin = high_resolution_clock::now();
  }

  Chrono &Chrono::resume() {
    if (m_paused) {
      m_begin = high_resolution_clock::now();
      m_paused = false;
    }

    return *this;
  }

  Chrono &Chrono::pause() {
    if (m_paused) return *this;

    high_resolution_clock::time_point current = high_resolution_clock::now();

    auto buffer = (current - m_begin);
    m_current_duration += buffer;

    m_paused = true;

    return *this;
  }

  Chrono &Chrono::restart() {
    m_paused = false;

    m_current_duration = 0ns;
    m_begin = high_resolution_clock::now();

    return *this;
  }

  duration<double> Chrono::get() {
    high_resolution_clock::time_point current = high_resolution_clock::now();

    if (not m_paused) {
      auto buffer = (current - m_begin);
      m_current_duration += buffer;
      m_begin = current;
    }

    return m_current_duration;
  }

  std::ostream &operator<<(std::ostream &os, Chrono &val) {
    os << val.get().count();

    return os;
  }

  Chrono::operator std::string() { return std::to_string(get().count()); }

}   // namespace tscl
