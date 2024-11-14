#include "tdot.h"

using namespace std;

static  auto time_start = std::chrono::high_resolution_clock::time_point();

//CDot::CDot()
//{
//    //auto time_in_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(time_start.time_since_epoch()).count();
//
//    m_Id = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-time_start).count();
//
//    //qDebug() << "CDot Id: " << m_Id;
//}
//
//
//
//
