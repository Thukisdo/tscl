
#include "Logger.hpp"
#include <unordered_map>

#include <fstream>
#include <iostream>
#include <sstream>

namespace tscl {

  std::string const &Log::levelToString(log_level level) {
    static std::unordered_map<log_level, std::string> map = {
            {Trace, "[Trace]"},     {Debug, "[Debug]"}, {Information, "[Information]"},
            {Warning, "[Warning]"}, {Error, "[Error]"}, {Fatal, "[Fatal]"}};

    return map[level];
  }

  Log::Log(log_level level) : m_level(level) {}

  void Log::level(log_level level) { m_level = level; }

  Log::log_level Log::level() const { return m_level; }

  std::string Log::prefix(timestamp_t ts_type) const {
    std::stringstream tmp;

    if (ts_type != timestamp_t::None) tmp << timestamp(ts_type) << " ";

    tmp << levelToString(m_level);

    return tmp.str();
  }

  std::string Log::message() const { return messageImpl(); }

  std::string StringLog::messageImpl() const { return " - " + m_str; }

  StringLog::StringLog(log_level level) : Log(level){};

  StringLog::StringLog(std::string const &message, log_level level) : Log(level), m_str(message) {}

  StringLog::StringLog(std::string &&message, log_level level)
      : Log(level), m_str(std::move(message)) {}

  StringLog::StringLog(StringLog &&other) : m_str(std::move(other.m_str)) {}

  StringLog &StringLog::operator=(StringLog &&other) {
    m_str = std::move(other.m_str);
    return *this;
  }

  std::string ErrorLog::messageImpl() const {
    std::stringstream tmp;

    tmp << " - " << m_str;

    if (m_description != "") {
      tmp << m_description;
    }

    return tmp.str();
  }

  ErrorLog::ErrorLog(std::string const &error, long code, log_level level,
                     std::string const &description)
      : StringLog(error, level), m_error_code(code) {
    m_description = description;
    size_t pos = 0;

    if (m_description == "") return;

    while (pos != std::string::npos) {
      pos = m_description.find('\n', pos);
      if (pos == std::string::npos) break;
      m_description.replace(pos, 1, "\n |\t");
      pos += 1;
    }
    m_description.insert(0, "\n |\t");
  }

  ErrorLog::ErrorLog(ErrorLog &&other) { *this = std::move(other); }

  ErrorLog &ErrorLog::operator=(ErrorLog &&other) {
    m_str = std::move(other.m_str);
    m_error_code = other.m_error_code;
    m_description = std::move(other.m_description);
    return *this;
  }

  std::string ErrorLog::message() const {
    std::stringstream tmp;

    tmp << "[0x" << std::hex << (int) m_error_code << "]" << messageImpl();
    return tmp.str();
  }

  LogHandler::LogHandler(bool enable, Log::log_level min_level)
      : m_enabled(enable), m_min_level(min_level) {}

  std::string StreamLogHandler::colorize(Log::log_level level) {
    static std::unordered_map<Log::log_level, std::string> res = {
            {Log::Trace, "\033[39;90m"},   {Log::Debug, "\033[39;36m"}, {Log::Information, "\033[39;34m"},
            {Log::Warning, "\033[39;33m"}, {Log::Error, "\033[39;31m"}, {Log::Fatal, "\033[39;35m"}};

    return res[level];
  }

  StreamLogHandler::StreamLogHandler(std::ostream &out, bool use_ascii_color)
      : m_use_ascii_color(use_ascii_color) {
    m_out = &out;
    m_stream_owner = false;
  }

  StreamLogHandler::StreamLogHandler(std::string const &path) : m_use_ascii_color(false) {
    m_out = new std::ofstream(path);
    m_stream_owner = true;
  }

  StreamLogHandler::~StreamLogHandler() {
    if (m_stream_owner) delete m_out;
  }

  void StreamLogHandler::log(Log const &log, std::string const &message) {
    std::unique_lock<std::shared_mutex> lock(m_main_mutex);
    auto &out = *m_out;

    if (not enable() or log.level() < minLvl()) return;
    if (m_use_ascii_color) out << colorize(log.level());

    out << log.prefix(tsType()) << log.message() << std::endl;

    if (m_use_ascii_color) out << "\033[0m";
  }

  Logger &Logger::operator()(Log const &log) noexcept {
    std::shared_lock<std::shared_mutex> lock(m_main_mutex);
    std::string msg = log.message();

    for (auto &i : m_loggers) i.second->log(log, msg);

    if (log.level() == Log::Fatal) {
      std::cout << "\n\nThe application has encountered a fatal error and must close.\n";
      exit(1);
    }

    return *this;
  }

  Logger &Logger::operator()(std::string const &msg, Log::log_level level) noexcept {
    StringLog tmp(msg, level);
    operator()(tmp);

    return *this;
  }

  void Logger::removeHandler(std::string name) {
    std::unique_lock<std::shared_mutex> lock(m_main_mutex);

    auto it = m_loggers.find(name);

    if (it != m_loggers.end()) {
      m_loggers.erase(it);
      lock.unlock();
      operator()("Removed log handler \"" + name + '\"');
      return;
    }

    lock.unlock();
    (*this)(ErrorLog("Cannot remove log handler \"" + name + "\", as it does not exist.",
                     errors::ERR_UNKNOWN_HANDLER, Log::Warning));
  }

  Logger &logger = Logger::singleton();

}   // namespace tscl