#ifndef LOGGER_HH
#define LOGGER_HH


#include <string>
#include <my_global.h>
#undef min
#undef max
#include <mysql.h>

struct cxn_stat {
  cxn_stat(bool _start_flag,
	   uint64_t _timestamp,
	   std::string _data_type,
	   std::string _mimo_or_siso,
	   std::string _direction,
	   double _Mbps,
	   double _lost)
    : start_flag(_start_flag),
      timestamp(_timestamp),
      data_type(_data_type),
      mimo_or_siso(_mimo_or_siso),
      direction(_direction),
      Mbps(_Mbps),
      lost(_lost)
  { }
  bool start_flag;
  uint64_t timestamp;
  std::string data_type;
  std::string mimo_or_siso;
  std::string direction;
  double Mbps;
  double lost;
};

class Logger {

private:
  MYSQL *mysql_conn;
  char *mysql_cmd_buffer;

public:
  Logger();
  ~Logger();
  void init_mysql();
  void log_connection_stats(struct cxn_stat*);
  void mysql_log_basic_bitrates(uint64_t, double, double, const char*, const char*);

  // Effective C++ (enforced by -Weffc++) says that any class with
  // pointer data fields should overload the copy and assignment
  // operators to avoid secretly sharing memory between
  // instances. We're just going to cheat and declare them since we
  // don't actually care to copy Logger objects. This should probably
  // be handled better at some point.
  Logger(const Logger&);
  Logger& operator=(const Logger&);
};

#endif
