/*
 * TimeTracker -- A simple header only C++ 11 class for measuring the execution time of
 * your code block using chrono steady_clock.
 *
 * <https://github.com/jacoblica/timertracker>
 *
 * Copyright 2011-2020 Jacob Li
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __TIMETRACKER_H__
#define __TIMETRACKER_H__

#include <bits/stdint-uintn.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <bits/stdint-uintn.h>
#include <iomanip>
#include <iostream>


class TimeTracker 
{
public:
   inline TimeTracker(uint32_t in_reportIntervalInSeconds = 10, uint32_t in_executionCountIntervalInSeconds = 1, const char* in_prefix = "", const char* in_timeUnitName = "ms");

   inline ~TimeTracker() = default;

   inline void set();


   inline bool get(std::stringstream *out_ss = nullptr);

   /**
    * @brief Print the rest of the statistics if there are any
    * 
    */
   inline void stop();

private:
   const std::string m_reportPrefix;
   /// TimeTracker moe
   enum  TIME_TRACK_MODE {
      /// This mode is to meaure the time of a chunk of code
      MODE_PERFORMANCE = 0,  
      /// This mode is to measure the time of a  loop
      MODE_LOOP = 1,         
   };
   /// mode 0: measure using (end - start)
   /// mode 1: measure using (end - end)
   uint32_t m_mode = MODE_PERFORMANCE;
   /// How many seconds to report the measurement
   uint32_t m_reportIntervalInSeconds = 10;
   /// How many seconds to store the execution count,
   uint32_t m_executionCountIntervalInSeconds = 1;
   /// NOTE: m_reportIntervalInSeconds % m_executionCountIntervalInSeconds has to be 0 to generate a regularly report

   /// Execution count all the time
   uint64_t m_executionCountTotalAllTheTime = 0;
   /// Execution count every execution count interval( for example 1 seconds)
   uint64_t m_executionCountTotalInOneSec = 0;
   /// Execution count in total for report interval
   uint64_t m_executionCountTotalInNsec = 0;
   /// Execution time in total for report interval
   double m_executionTimeTotalInMilliseconds = 0;

   /// Time when the one execution begin
   std::chrono::time_point<std::chrono::steady_clock> m_timeStart            = {};
   /// Time when the one execution end
   std::chrono::time_point<std::chrono::steady_clock> m_timeEnd              = {};
   /// Time when an execution count interval begin
   std::chrono::time_point<std::chrono::steady_clock> m_elapseTimeStart      = {};
   /// Time when an execution count interval end
   const std::chrono::time_point<std::chrono::steady_clock> &m_elapseTimeEnd = m_timeEnd;

   /// Interval for one specific execution in milliseconds
   double m_executionTime = 0;
   /// Interval since an execution count begin in milliseconds, for example 1000 milliseconds
   double m_elapseTime    = 0;

   /// Minimal/Average/Maximum execution time of one report
   double m_timeMin = std::numeric_limits<double>::max();
   double m_timeAvg = std::numeric_limits<double>::min();
   double m_timeMax = std::numeric_limits<double>::min();

   /// Array store the execution count of the execution count interval
   std::vector<uint64_t> m_vecExecutionOneSec;

   /// Maximum size of the vector, it will be reserved based on report interval and execution cout interval
   uint32_t m_vecExecutionDepth =  0;

   /// If user does not provide any string stream, this is used to store the execution count of execution count interval
   std::stringstream m_report;


   std::string m_timeUnitName;

   double m_scaleRatio;
   std::map<std::string, double> m_timeUnitScaleRatio = {{"ms", 1}, {"us", 1000}, {"ns", 1000000}};

   uint32_t m_reportPrecision;
   std::map<std::string, uint32_t> m_timeUnitPrecision  = {{"ms", 2}, {"us", 2}, {"ns", 0}};

   const char *TIMETRACKER_PREFIX      = "TimeTracker: ";
   const double ONE_SEC_IN_MILLISECOND = 1000.0;
   const uint32_t REPORT_PRECISION      = 4;
   const uint32_t REPORT_WIDTH          = 5;

   /// Present the report in sting stream format, used by get()
   inline bool getReport(std::stringstream &out_str);
   // Reset internal variables for next report interval
   inline void reset();
};


/**
   * @brief Construct a new Time Tracker object
   * 
   * @param in_reportIntervalInSeconds               report inverval
   * @param in_executionCountIntervalInSeconds       execution interval
   * @param in_prefix                                standard output prefix
   * @param in_timeUnitName                          time unit in ms, us or ns.
   */
TimeTracker::TimeTracker(uint32_t in_reportIntervalInSeconds, uint32_t in_executionCountIntervalInSeconds, const char* in_prefix, const char* in_timeUnitName) 
: m_reportPrefix(in_prefix) 
, m_scaleRatio(1)
, m_reportPrecision(2)
{
   // Initialize the scaleRatio
   if ( m_timeUnitScaleRatio.find(in_timeUnitName) != m_timeUnitScaleRatio.end() ) {
      // found
      m_scaleRatio = m_timeUnitScaleRatio.at(in_timeUnitName);
      m_timeUnitName = in_timeUnitName;
   } else {
      m_timeUnitName = "ms";
   }
   // Initialize the reportPrecision
   if ( m_timeUnitPrecision.find(in_timeUnitName) != m_timeUnitPrecision.end() ) {
      // found
      m_reportPrecision = m_timeUnitPrecision.at(in_timeUnitName);
   }

   std::cout << TIMETRACKER_PREFIX << "INFO: steady_clock steady: " << std::chrono::steady_clock::is_steady
             << " den: " << std::chrono::steady_clock::period::den << " num: " << std::chrono::steady_clock::period::num << " Time unit using: " << m_timeUnitName << " Ratio: " << m_scaleRatio << std::endl;
   if (std::chrono::steady_clock::is_steady != 1) {
      std::cout << TIMETRACKER_PREFIX << "WARNING: steay_lock is NOT steady, timer might not accruate." << std::endl;
   }
   if (in_reportIntervalInSeconds == 0) {
      m_reportIntervalInSeconds = 1;
      // Force m_executionCountIntervalInSeconds to be 1 as well
      m_executionCountIntervalInSeconds = 1;
      std::cout << TIMETRACKER_PREFIX << "WARNING: report interval has to be at least 1 second., using 1 second." << std::endl;
   } else {
      m_reportIntervalInSeconds = in_reportIntervalInSeconds;
      if ((in_executionCountIntervalInSeconds == 0) || (m_reportIntervalInSeconds < in_executionCountIntervalInSeconds) ||
          (in_reportIntervalInSeconds % in_executionCountIntervalInSeconds != 0)) {
         std::cout << TIMETRACKER_PREFIX << "WARNING: illegal report interval " << in_reportIntervalInSeconds << " or execution count interval "
                   << in_executionCountIntervalInSeconds << std::endl;
         m_executionCountIntervalInSeconds = 1;
      } else {
         m_executionCountIntervalInSeconds = in_executionCountIntervalInSeconds;
      }
      std::cout << TIMETRACKER_PREFIX << "INFO: execution statistics will report every " << m_reportIntervalInSeconds << " seconds, exection counting in "
                << m_executionCountIntervalInSeconds << " seconds" << std::endl;
   }
   m_vecExecutionDepth = m_reportIntervalInSeconds / m_executionCountIntervalInSeconds;
   // Reserve the space before hand.
   m_vecExecutionOneSec.reserve(m_vecExecutionDepth);

}

bool TimeTracker::getReport(std::stringstream &out_str) {
   bool ret = true;
   if (m_vecExecutionOneSec.empty()) {
      std::cout << TIMETRACKER_PREFIX << "INFO: Nothing to print." << std::endl;
      ret = false;
   } else {
      out_str.str("");
      out_str.clear();
      out_str << m_reportPrefix;
      out_str << " Exec count " << std::dec ; 
      // return the formatted stringstream
      for (auto itr : m_vecExecutionOneSec) {
         out_str << "[" << itr << "]";
      }
      out_str << "(" << m_executionCountTotalInNsec << ") Exec time(" << m_timeUnitName << ")";
      m_timeAvg = m_executionTimeTotalInMilliseconds / static_cast<double>(m_executionCountTotalInNsec);
      out_str << " (" << std::setw(REPORT_WIDTH) <<  std::setprecision(m_reportPrecision) << std::fixed <<  m_timeMin * m_scaleRatio << "," << m_timeAvg * m_scaleRatio << "," << m_timeMax * m_scaleRatio << ")";

      reset();
   }
   return ret;
}

void TimeTracker::reset() {
   m_executionCountTotalAllTheTime += m_executionCountTotalInNsec;
   m_executionCountTotalInNsec = 0;
   m_executionTimeTotalInMilliseconds = 0;
   m_timeMin = std::numeric_limits<double>::max();
   m_timeMax = std::numeric_limits<double>::min();
   m_timeAvg = std::numeric_limits<double>::min();
   m_vecExecutionOneSec.clear();
}

/**
* @brief Start a timer in performance mode to meaure the exection time of the code block
* Call this before the code to be timed. If you call get() without calling set() first, 
* time track will still stay the loop mode
*/
void TimeTracker::set() {
   if ( m_mode == MODE_PERFORMANCE ) {
      m_timeStart = std::chrono::steady_clock::now();
      // Intialize elapse timer start
      if ( m_elapseTimeStart.time_since_epoch().count() == 0 ) {
         std::cout << TIMETRACKER_PREFIX << "INFO: PERFORMANCE mode has been started to measure the performance of a chunk of code for " << m_reportPrefix << std::endl;
         m_elapseTimeStart = m_timeStart;
      }
   } else {
      std::cerr << TIMETRACKER_PREFIX << "ERROR: Wrong usage of APIs, TimeTracker::get() is called befre TimeTracker::start(), " << std::endl;
   }

}

/**
   * @brief Call this after the code to be timed, so calculation will be done
   * 
   * @param out_ss pass a pointer to store the statistics data, use nullptr if you want TimeTrakcer print to stdout
   * @return statistics data is available
   * @return false no data available
   */
bool TimeTracker::get(std::stringstream *out_ss ) {
   bool ret = false;
   m_timeEnd = std::chrono::steady_clock::now();
   if (m_timeStart.time_since_epoch().count() == 0 ) {
      // This can be used to measure the time it get called(for semphore, condition_wait, etc, measure the frame rate)
      m_mode = MODE_LOOP;
      std::cout << TIMETRACKER_PREFIX << "INFO: LOOP mode has been started to measure the interval between two execution of loop for " << m_reportPrefix << std::endl;
      m_timeStart = m_timeEnd;
      m_elapseTimeStart = m_timeEnd;
      return ret;
   }
   // Calculate execution time in milliseconds
   m_executionTime = std::chrono::duration<double, std::milli>(m_timeEnd - m_timeStart).count();
   m_elapseTime = std::chrono::duration<double, std::milli>(m_elapseTimeEnd - m_elapseTimeStart).count();
   // std::cout << TIMETRACKER_PREFIX << "DEBUG: milliseconds: " << std::setw(5) << std::setprecision(3) << std::chrono::duration<double, std::milli>(m_timeEnd - m_timeStart).count() << " -- " << m_elapseTime << " == " << std::endl;

   if (m_mode == MODE_LOOP ) {
      // end time is the start time for next cycle
      m_timeStart = m_timeEnd;
   }
   // Calulate Min, Max
   if ( m_executionTime < m_timeMin) {
      m_timeMin = m_executionTime;
   }
   if ( m_executionTime > m_timeMax) {
      m_timeMax = m_executionTime;
   }

   // Increase execution counter of current seconds
   ++m_executionCountTotalInOneSec;
   // Increase execution time in total
   m_executionTimeTotalInMilliseconds += m_executionTime;

   // If time elapsed 1 second again, need to store the execution count
   if ( m_elapseTime >= (m_executionCountIntervalInSeconds * ONE_SEC_IN_MILLISECOND )) {
      m_vecExecutionOneSec.push_back(m_executionCountTotalInOneSec);
      m_executionCountTotalInNsec += m_executionCountTotalInOneSec;
      m_executionCountTotalInOneSec = 0;
      if (m_vecExecutionOneSec.size() >= m_vecExecutionDepth ) {
         ret = true;
         if ( out_ss == nullptr ) {
            // it is the time to generate the report.
            if ( getReport(m_report) == true) {
               std::cout << TIMETRACKER_PREFIX << "INFO: "<< m_report.str().c_str() << std::endl;
            }
         } else {
            getReport(*out_ss);
         }
      }
      // std::cout << TIMETRACKER_PREFIX << "VECTOR size: " <<  m_vecExecutionOneSec.size() << std::endl;

      m_elapseTimeStart = m_elapseTimeEnd;
   }
   return ret;
}

void TimeTracker::stop() {
   // If there are left over data, push it into the vector
   if ( m_executionCountTotalInOneSec != 0  ) {
      m_vecExecutionOneSec.push_back(m_executionCountTotalInOneSec);
      m_executionCountTotalInNsec += m_executionCountTotalInOneSec;
      m_executionCountTotalInOneSec = 0;
   }
   if ( getReport(m_report) == true) {
      std::cout << TIMETRACKER_PREFIX<< "INFO: " <<  m_report.str().c_str() <<std::endl;
      std::cout << TIMETRACKER_PREFIX<< "INFO: " <<  "Total execution count " << m_executionCountTotalAllTheTime << std::endl << "The End" << std::endl;
   }
}
#endif // __TIMETRACKER_H__