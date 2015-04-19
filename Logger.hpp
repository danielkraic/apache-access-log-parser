#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <mutex>

class Logger
{
public:
  enum class Level {
    ERROR = 0,
    NOTICE,
    DEBUG
  };

  Logger() :
    m_level(Level::DEBUG),
    m_log_stream(std::cout.rdbuf()),
    m_log_stream_mutex()
  {
  }

  void setLevel(Level level) {
    m_level = level;
  }

  template <typename... Ts>
  void Debug(Ts... vs) {
    if (static_cast<int>(m_level) >= static_cast<int>(Level::DEBUG))
      log("DEBUG", std::forward<Ts>(vs)...);
  }

  template <typename... Ts>
  void Notice(Ts... vs) {
    if (static_cast<int>(m_level) >= static_cast<int>(Level::NOTICE))
      log("NOTICE", std::forward<Ts>(vs)...);
  }

  template <typename... Ts>
  void Error(Ts... vs) {
    if (static_cast<int>(m_level) >= static_cast<int>(Level::ERROR))
      log("ERROR", std::forward<Ts>(vs)...);
  }

private:
  template <typename T>
  void log(const char * level, T v) {
    std::lock_guard<std::mutex> l(m_log_stream_mutex);
    m_log_stream << level << ": " << v << std::endl;
  }

  template <typename T1, typename T2>
  void log(const char * level, T1 v1, T2 v2) {
    std::lock_guard<std::mutex> l(m_log_stream_mutex);
    m_log_stream << level << ": " << v1 << ": " << v2 << std::endl;
  }

  template <typename T1, typename T2, typename T3, typename T4>
  void log(const char * level, T1 v1, T2 v2, T3 v3, T4 v4) {
    std::lock_guard<std::mutex> l(m_log_stream_mutex);
    m_log_stream << level << ": " << v1 << ": " << v2 << ", " << v3 << ": " << v4 << std::endl;
  }

  Level m_level;
  std::ostream m_log_stream;
  std::mutex m_log_stream_mutex;
};

#endif // LOGGER_HPP
