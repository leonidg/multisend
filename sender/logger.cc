#include "logger.hh"
#include "mysql_credentials.hh"

Logger::Logger() :
  mysql_conn( mysql_init(NULL) ),
  mysql_cmd_buffer( (char*)malloc(10000) )
{
    // TODO: this should probably be moved out to a separate class
    mysql_conn = mysql_init(NULL);
    if (mysql_real_connect(mysql_conn, MYSQL_SERVER, MYSQL_USERNAME,  MYSQL_PASSWORD, MYSQL_DATABASE, 0, NULL, 0) == NULL) {
        printf("Failed to connect to MySQL! error: %s", mysql_error(mysql_conn));
	mysql_conn = NULL;
    }
}

Logger::~Logger() {
    if (mysql_conn != NULL) {
	mysql_close(mysql_conn);
	free(mysql_cmd_buffer);
    }
}

void Logger::log_connection_stats(struct cxn_stat *stat_frame) {
    printf( "%s  start = %s, timestamp = %ld, data_type = %s, mimo or siso = %s, Mbps = %f, lost = %f\n",
	    stat_frame->direction.c_str(),
	    (stat_frame->start_flag ? "true" : "false"),
	    stat_frame->timestamp,
	    stat_frame->data_type.c_str(),
	    stat_frame->mimo_or_siso.c_str(),
	    stat_frame->Mbps,
	    stat_frame->lost);
    if (mysql_conn != NULL) {
      sprintf(mysql_cmd_buffer,
            "INSERT INTO `controlled_delay` (`configuration_id`, `start_test`, `Mbps`, `lossrate`, `timestamp`) VALUES ("
                "(SELECT `id` FROM `configurations` WHERE `data_type`='%s' AND `mimo_or_siso`='%s' AND `up_or_down`='%s' LIMIT 1),"
	         "%d, %f, %f, %ld)",
	      stat_frame->data_type.c_str(),
	      stat_frame->mimo_or_siso.c_str(),
	      stat_frame->direction.c_str(),
	      stat_frame->start_flag,
	      stat_frame->Mbps,
	      stat_frame->lost,
	      stat_frame->timestamp);
      int err = mysql_query(mysql_conn, mysql_cmd_buffer);
      if (err) {
	printf("MySQL error: %s\n", mysql_error(mysql_conn));
	printf("executed query: %s\n", mysql_cmd_buffer);
      }
    }
}

void Logger::mysql_log_basic_bitrates(uint64_t timestamp, double Mbps_down, double Mbps_up,
				      const char *building, const char *note) {
    if (mysql_conn == NULL) {
	return;
    }
    sprintf(mysql_cmd_buffer,
            "INSERT INTO `controlled_delay2` (`timestamp`, `Mbps_down`, `Mbps_up`, `building`, `note`) VALUES (%ld, %f, %f, '%s', '%s')",
	    timestamp,
	    Mbps_down,
	    Mbps_up,
	    building,
	    note);
    int err = mysql_query(mysql_conn, mysql_cmd_buffer);
    if (err) {
      printf("MySQL error: %s\n", mysql_error(mysql_conn));
      printf("executed query: %s\n", mysql_cmd_buffer);
    }
}
