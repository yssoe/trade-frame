/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

// processes historical data requests against the IQFeed API

// todo:  put parsers in separate compilation units to cut down on compile time

#define FUSION_MAX_VECTOR_SIZE 18

#include <string>
#include <sstream>
#include <vector>
#include <cassert>

#include <boost/config/warning_disable.hpp>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include <boost/fusion/include/adapt_struct.hpp>

#include <LibCommon/ReusableBuffers.h>
#include <LibCommon/Network.h>

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
// custom off

namespace IQFeedHistoryStructs {

  struct structTickDataPoint {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    ptime DateTime;
    double Last;
    long  LastSize;
    long TotalVolume;
    double Bid;
    double Ask;
    long TickID;
    long BidSize;
    long AskSize;
    char BasisForLast;  // 'C' normal, 'E' extended
    
  };

  struct structInterval {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    ptime DateTime;
    double High;
    double Low;
    double Open;
    double Close;
    long TotalVolume;
    long PeriodVolume;
  };

  struct structSummary {
    unsigned short Year;
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    ptime DateTime;
    double High;
    double Low;
    double Open;
    double Close;
    long PeriodVolume;
    long OpenInterest;
  };

}


BOOST_FUSION_ADAPT_STRUCT(
  IQFeedHistoryStructs::structTickDataPoint,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (unsigned short, Hour)
  (unsigned short, Minute)
  (unsigned short, Second)
  (double, Last)
  (long, LastSize)
  (long, TotalVolume)
  (double, Bid)
  (double, Ask)
  (long, TickID)
  (long, BidSize)
  (long, AskSize)
  (char, BasisForLast)
  )

BOOST_FUSION_ADAPT_STRUCT(
  IQFeedHistoryStructs::structInterval,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (unsigned short, Hour)
  (unsigned short, Minute)
  (unsigned short, Second) 
  (double, High)
  (double, Low)
  (double, Open)
  (double, Close)
  (long, TotalVolume)
  (long, PeriodVolume)
  )

BOOST_FUSION_ADAPT_STRUCT(
  IQFeedHistoryStructs::structSummary,
  (unsigned short, Year)
  (unsigned short, Month)
  (unsigned short, Day)
  (unsigned short, Hour)
  (unsigned short, Minute)
  (unsigned short, Second)
  (double, High)
  (double, Low)
  (double, Open)
  (double, Close)
  (long, PeriodVolume)
  (long, OpenInterest)
  )

namespace IQFeedHistoryStructs {

  template <typename Iterator>
  struct DataPointParser: qi::grammar<Iterator, structTickDataPoint()> {
    DataPointParser(): DataPointParser::base_type(start) {
      start %= 
                  qi::ushort_ >> '-' >> qi::ushort_ >> '-' >> qi::ushort_ 
        >> ' ' >> qi::ushort_ >> ':' >> qi::ushort_ >> ':' >> qi::ushort_
        >> ',' >> qi::double_ >> ',' >> qi::long_   >> ',' >> qi::long_
        >> ',' >> qi::double_ >> ',' >> qi::double_ >> ',' >> qi::long_
        >> ',' >> qi::long_   >> ',' >> qi::long_   >> ',' >> ascii::char_ 
        >> ','
        ;
    }
    qi::rule<Iterator, structTickDataPoint()> start;
  };

  template <typename Iterator>
  struct IntervalParser: qi::grammar<Iterator, structInterval()> {
    IntervalParser(): IntervalParser::base_type(start) {
      start %= 
                  qi::ushort_ >> '-' >> qi::ushort_ >> '-' >> qi::ushort_ 
        >> ' ' >> qi::ushort_ >> ':' >> qi::ushort_ >> ':' >> qi::ushort_
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::long_   >> ',' >> qi::long_ 
        >> ','
        ;
    }
    qi::rule<Iterator, structInterval()> start;
  };

  template <typename Iterator>
  struct SummaryParser: qi::grammar<Iterator, structSummary()> {
    SummaryParser(): SummaryParser::base_type(start) {
      start %= 
                  qi::ushort_ >> '-' >> qi::ushort_ >> '-' >> qi::ushort_ 
        >> ' ' >> qi::ushort_ >> ':' >> qi::ushort_ >> ':' >> qi::ushort_
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::double_ >> ',' >> qi::double_ 
        >> ',' >> qi::long_   >> ',' >> qi::long_ 
        >> ','
        ;
    }
    qi::rule<Iterator, structSummary()> start;
  };

}

// T: CRTP inheriting class, U: type passed in for reference by inheriting class
template <typename T, typename U>
class CIQFeedHistoryQuery: public CNetwork<CIQFeedHistoryQuery<T,U> > {
  friend CNetwork<CIQFeedHistoryQuery<T,U> >;
public:

  typedef typename CNetwork<CIQFeedHistoryQuery<T,U> > inherited_t;

  typedef typename IQFeedHistoryStructs::structTickDataPoint structTickDataPoint;
  typedef typename IQFeedHistoryStructs::structInterval structInterval;
  typedef typename IQFeedHistoryStructs::structSummary structSummary;

  CIQFeedHistoryQuery( void );
  ~CIQFeedHistoryQuery( void );

  // start a query with one of these commands
  void RetrieveNDataPoints( const std::string& sSymbol, unsigned int n, U user );  // HTX ticks
  void RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n, U user ); // HTD ticks

  void RetrieveNIntervals( const std::string& sSymbol, unsigned int i, unsigned int n, U user );  // HIX i=interval in seconds  (bars)
  void RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n, U user ); // HID i=interval in seconds (bars)

  void RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n, U user );  // HDX  (bars)

  // once data is extracted, return the buffer for reuse
  void ReQueueTickDataPoint( structTickDataPoint* pDP ) { m_reposTickDataPoint.CheckInL( pDP ); }
  void ReQueueInterval( structInterval* pDP ) { m_reposInterval.CheckInL( pDP ); }
  void ReQueueSummary( structSummary* pDP ) { m_reposSummary.CheckInL( pDP ); }

protected:

  enum enumRetrievalState {  // activity in progress on this port
    RETRIEVE_IDLE = 0,  // no retrievals in progress
    RETRIEVE_HISTORY_DATAPOINTS,  // RequestID='D', data points are arriving
    RETRIEVE_HISTORY_INTERVALS,  // RequestID='I', interval data is arriving
    RETRIEVE_HISTORY_SUMMARY,  // RequestID='E', eod data is arriving
    RETRIEVE_DONE  // end marker arrived and is awaiting processing
  } m_stateRetrieval;

  U m_user; // passed back to caller as reference to data, therefore only one request at a time, based upon m_stateRetrieval

  // called by CNetwork via CRTP
  void OnNetworkConnected(void) {
    if ( &CIQFeedHistoryQuery<T,U>::OnHistoryConnected != &T::OnHistoryConnected ) {
      static_cast<T*>( this )->OnHistoryConnected();
    }
  };
  void OnNetworkDisconnected(void) {
    if ( &CIQFeedHistoryQuery<T,U>::OnHistoryDisconnected != &T::OnHistoryDisconnected ) {
      static_cast<T*>( this )->OnHistoryDisconnected();
    }
  };
  void OnNetworkError( size_t e ) {
    if ( &CIQFeedHistoryQuery<T,U>::OnHistoryError != &T::OnHistoryError ) {
      static_cast<T*>( this )->OnHistoryError(e);
    }
  };
  void OnNetworkSendDone(void) {
    if ( &CIQFeedHistoryQuery<T,U>::OnHistorySendDone != &T::OnHistorySendDone ) {
      static_cast<T*>( this )->OnHistorySendDone();
    }
  };
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  // CRTP based dummy callbacks;
  void OnHistoryConnected( void ) {};
  void OnHistoryDisconnected( void ) {};
  void OnHistoryError( size_t e ) {};
  void OnHistorySendDone( void ) {};
  void OnHistoryTickDataPoint( structTickDataPoint* pDP, U user ) {};
  void OnHistoryIntervalData( structInterval* pDP, U user ) {};
  void OnHistorySummaryData( structSummary* pDP, U user ) {};
  void OnHistoryRequestDone( U user ) {};

private:

  typedef typename inherited_t::linebuffer_t linebuffer_t;
  typedef typename inherited_t::linebuffer_t::const_iterator const_iterator_t;

  // used for containing parsed data and passing it on
  CBufferRepository<structTickDataPoint> m_reposTickDataPoint;
  CBufferRepository<structInterval> m_reposInterval;
  CBufferRepository<structSummary> m_reposSummary;

  IQFeedHistoryStructs::DataPointParser<const_iterator_t> m_grammarDataPoint;
  IQFeedHistoryStructs::IntervalParser<const_iterator_t> m_grammarInterval;
  IQFeedHistoryStructs::SummaryParser<const_iterator_t> m_grammarSummary;

  qi::rule<const_iterator_t> m_ruleEndMsg;

  // Process the line
  void ProcessHistoryRetrieval( linebuffer_t* buf );

};

template <typename T, typename U>
CIQFeedHistoryQuery<T,U>::CIQFeedHistoryQuery( void ) 
: CNetwork<CIQFeedHistoryQuery<T,U> >( "127.0.0.1", 9100 ),
  m_stateRetrieval( RETRIEVE_IDLE )
{
  m_ruleEndMsg = qi::lit("!ENDMSG!," );
}

template <typename T, typename U>
CIQFeedHistoryQuery<T,U>::~CIQFeedHistoryQuery() {
}

template <typename T, typename U>
void CIQFeedHistoryQuery<T,U>::OnNetworkLineBuffer( linebuffer_t* buf ) {

#if defined _DEBUG
  {
    linebuffer_t::const_iterator bgn = (*buf).begin();
    linebuffer_t::const_iterator end = (*buf).end();

    std::string str( bgn, end );
    str += "\n";
    OutputDebugString( str.c_str() );
  }
#endif

  switch ( m_stateRetrieval ) {
    case RETRIEVE_HISTORY_DATAPOINTS:
    case RETRIEVE_HISTORY_INTERVALS:
    case RETRIEVE_HISTORY_SUMMARY:
      ProcessHistoryRetrieval( buf );
      //ReturnLineBuffer( wParam ); 
      break;
    case RETRIEVE_DONE:
      // it is an error to land here
      OutputDebugString( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RETRIEVE_DONE\n" );
      //throw std::logic_error( "RETRIEVE_DONE");
      //ReturnLineBuffer( wParam );
      break;
    case RETRIEVE_IDLE:
      // it is an error to land here
      OutputDebugString( "Unknown CIQFeedNewsQuery<T>::OnConnProcess RETRIEVE_IDLE\n" );
      //throw std::logic_error( "RETRIEVE_IDLE");
      //ReturnLineBuffer( wParam );
      break;
  }

  GiveBackBuffer( buf );
}

template <typename T, typename U>
void CIQFeedHistoryQuery<T,U>::RetrieveNDataPoints( const std::string& sSymbol, unsigned int n, U user ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_DATAPOINTS;
    m_user = user;
    std::stringstream ss;
    ss << "HTX," << sSymbol << "," << n << ",1,D\n";
    Send( ss.str().c_str() );
  }
}

template <typename T, typename U>
void CIQFeedHistoryQuery<T,U>::RetrieveNDaysOfDataPoints( const std::string& sSymbol, unsigned int n, U user) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNDaysOfDataPoints: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_DATAPOINTS;
    m_user = user;
    std::stringstream ss;
    ss << "HTD," << sSymbol << "," << n << ",,,,1,D\n";
    Send( ss.str().c_str() );
  }
}

template <typename T, typename U>
void CIQFeedHistoryQuery<T,U>::RetrieveNIntervals( const std::string& sSymbol, unsigned int i, unsigned int n, U user ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_INTERVALS;
    m_user = user;
    std::stringstream ss;
    ss << "HIX," << sSymbol << "," << i << "," << n << ",1,I\n";
    Send( ss.str().c_str() );
  }
}

template <typename T, typename U>
void CIQFeedHistoryQuery<T,U>::RetrieveNDaysOfIntervals( const std::string& sSymbol, unsigned int i, unsigned int n, U user ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNDaysOfIntervals: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_INTERVALS;
    m_user = user;
    std::stringstream ss;
    ss << "HID," << sSymbol << "," << i << "," << n << ",,,,1,I\n";
    Send( ss.str().c_str() );
  }
}

template <typename T, typename U>
void CIQFeedHistoryQuery<T,U>::RetrieveNEndOfDays( const std::string& sSymbol, unsigned int n, U user ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedHistoryQuery<T>::RetrieveNEndOfDays: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_HISTORY_SUMMARY;
    m_user = user;
    std::stringstream ss;
    ss << "HDX," << sSymbol << "," << n << ",1,E\n";
    Send( ss.str().c_str() );
  }
}

template <typename T, typename U>
void CIQFeedHistoryQuery<T,U>::ProcessHistoryRetrieval( linebuffer_t* buf ) {

  linebuffer_t::const_iterator bgn = (*buf).begin();
  linebuffer_t::const_iterator end = (*buf).end();

  assert( ( end - bgn ) > 2 );
  char chRequestID = *bgn;
  bgn++;
  bgn++;

  bool b = false;
  switch ( chRequestID ) {
    case 'D': {
        assert ( RETRIEVE_HISTORY_DATAPOINTS == m_stateRetrieval );
        structTickDataPoint* pDP = m_reposTickDataPoint.CheckOutL();
        b = parse( bgn, end, m_grammarDataPoint, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = ptime( 
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ), 
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          if ( &CIQFeedHistoryQuery<T,U>::OnHistoryTickDataPoint != &T::OnHistoryTickDataPoint ) {
            static_cast<T*>( this )->OnHistoryTickDataPoint( pDP, m_user );
          }
        }
        else {
          m_reposTickDataPoint.CheckInL( pDP );
        }
      }
      break;
    case 'I': {
        assert ( RETRIEVE_HISTORY_INTERVALS == m_stateRetrieval );
        structInterval* pDP = m_reposInterval.CheckOutL();
        b = parse( bgn, end, m_grammarInterval, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = ptime( 
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ), 
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          if ( &CIQFeedHistoryQuery<T,U>::OnHistoryIntervalData != &T::OnHistoryIntervalData ) {
            static_cast<T*>( this )->OnHistoryIntervalData( pDP, m_user );
          }
        }
        else {
          m_reposInterval.CheckInL( pDP );
        }
      }
      break;
    case 'E': {
        assert ( RETRIEVE_HISTORY_SUMMARY == m_stateRetrieval );
        structSummary* pDP = m_reposSummary.CheckOutL();
        b = parse( bgn, end, m_grammarSummary, *pDP );
        if ( b && ( bgn == end ) ) {
          pDP->DateTime = ptime( 
            boost::gregorian::date( pDP->Year, pDP->Month, pDP->Day ), 
            boost::posix_time::time_duration( pDP->Hour, pDP->Minute, pDP->Second ) );
          if ( &CIQFeedHistoryQuery<T,U>::OnHistorySummaryData != &T::OnHistorySummaryData ) {
            static_cast<T*>( this )->OnHistorySummaryData( pDP, m_user );
          }
        }
        else {
          m_reposSummary.CheckInL( pDP );
        }
      }
      break;
    default:
      throw std::logic_error( "CIQFeedNewsQuery<T>::ProcessHistoryRetrieval unknown record");
  }

  if ( !b ) {
    b = parse( bgn, end, m_ruleEndMsg );
    if ( b && ( bgn ==  end ) ) {
      m_stateRetrieval = RETRIEVE_IDLE;
        if ( &CIQFeedHistoryQuery<T,U>::OnHistoryRequestDone != &T::OnHistoryRequestDone ) {
          static_cast<T*>( this )->OnHistoryRequestDone( m_user );
        }
    }
    else {
      throw std::logic_error( "CIQFeedNewsQuery<T>::ProcessHistoryRetrieval no endmessage");
    }
  }
}

