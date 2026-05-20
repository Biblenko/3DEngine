#include "pch.h"

#include "DataBaseConnection.h"

#include <thread>
#include <mutex>

void DataBaseConnection::DataBaseLoopCheck()
{
    ///*try {
    //    mysqlx::Session sess("127.0.1.31", 33060, "root", "");
    //    sess.sql("USE test").execute();

    //    while (true) {
    //        std::this_thread::sleep_for(std::chrono::seconds(12));

    //        mysqlx::SqlResult res = sess.sql("SELECT id FROM objects WHERE value = 1").execute();

    //        if (res.count() > 0) {
    //            std::lock_guard<std::mutex> lock(db_mutex);

    //            for (mysqlx::Row row : res) {
    //                objects_to_paint_red.push_back(row[0].get<int>());
    //            }
    //        }
    //    }
    //}
    //catch (const mysqlx::Error& err) {
    //    CString msg = _T("MySQL Error: \n");
    //    msg += CString(CA2W(err.what(), CP_UTF8));

    //    AfxMessageBox(msg);
    //}
    //catch (const std::exception& ex) {
    //    CString msg = _T("Standard Exception: \n");
    //    msg += CString(CA2W(ex.what(), CP_UTF8));

    //    AfxMessageBox(msg);
    //}*/
}
