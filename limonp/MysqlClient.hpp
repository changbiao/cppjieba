#ifndef LIMONP_MYSQLCLIENT_H
#define LIMONP_MYSQLCLIENT_H

#include <mysql/mysql.h>
#include <iostream>
#include <vector>
#include <string>
#include "logger.hpp"
#include "vec_functs.hpp"

namespace Limonp
{
    using namespace std;
    class MysqlClient
    {
        public:
            typedef vector< vector<string> > RowsType;
        private:
            const char * const HOST;
            const unsigned int PORT;
            const char * const USER;
            const char * const PASSWD;
            const char * const DB;
        public:
            MysqlClient(const char* host, uint port, const char* user, const char* passwd, const char* db): HOST(host), PORT(port), USER(user), PASSWD(passwd), DB(db){ _conn = NULL;};
            ~MysqlClient(){dispose();};
        public:
            bool init()
            {
                //cout<<mysql_get_client_info()<<endl;
                if(NULL == (_conn = mysql_init(NULL)))
                {
                    LogError("mysql_init faield. %s", mysql_error(_conn));
                    return false;
                }

                if (mysql_real_connect(_conn, HOST, USER, PASSWD, DB, PORT, NULL, 0) == NULL)
                {
                    LogError("mysql_real_connect failed. %s", mysql_error(_conn));
                    mysql_close(_conn);
                    _conn = NULL;
                    return false;
                }  

                //set reconenct
                char value = 1;
                mysql_options(_conn, MYSQL_OPT_RECONNECT, &value);

                return true;
            }
            bool dispose()
            {
                if(NULL != _conn)
                {
                    mysql_close(_conn);
                }
                _conn = NULL;
                return true;
            }
            bool executeSql(const char* sql)
            {
                if(NULL == _conn)
                {
                    LogError("_conn is NULL");
                    return false;
                }
                if(mysql_query(_conn, sql)) 
                {
                    LogError("mysql_query failed.  %s", mysql_error(_conn));
                    return false;
                }
                return true;
            }
            bool select(const char* sql, RowsType& rows)
            {
                if(!executeSql(sql))
                {
                    LogError("executeSql failed. [%s]", sql);
                    return false;
                }
                MYSQL_RES * result = mysql_store_result(_conn);
                if(NULL == result)
                {
                    LogError("mysql_store_result failed.[%d]", mysql_error(_conn));
                }
                uint num_fields = mysql_num_fields(result);
                MYSQL_ROW row;
                while((row = mysql_fetch_row(result)))
                {
                    vector<string> vec;
                    for(uint i = 0; i < num_fields; i ++)
                    {
                        row[i] ? vec.push_back(row[i]) : vec.push_back("NULL");
                    }
                    rows.push_back(vec);
                }
                mysql_free_result(result);
                return true;
            }

        private:
            MYSQL * _conn;

    };
}

#endif
