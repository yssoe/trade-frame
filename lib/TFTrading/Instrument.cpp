/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include <cassert>
#include <stdexcept>

#include "Instrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

const std::string CInstrument::m_sTableName = "instruments";

/*
  if ( NULL != m_pUnderlying.get() ) {
    rtn += sqlite3_bind_text(  
      pStmt, sqlite3_bind_parameter_index( pStmt, ":underlying" ), m_pUnderlying->GetInstrumentName().c_str(), -1, SQLITE_STATIC );
  }
  else {
    rtn += sqlite3_bind_text(   
      pStmt, sqlite3_bind_parameter_index( pStmt, ":underlying" ), "", -1, SQLITE_STATIC );
  }
*/

CInstrument::CInstrument( const TableRowDef& row ) 
  : m_row( row ) 
{
  // other clean up and pointer stuff still yet to be done here
}

// equity / generic creation
CInstrument::CInstrument(
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType,
  const idExchange_t &idExchange
                         )
: m_row( idInstrument, eType, idExchange ),
  m_eUnderlyingStatus( EUnderlyingNotSettable )
{
}

 // future
CInstrument::CInstrument( 
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType, 
  const idExchange_t& idExchange,
  unsigned short year, unsigned short month ) 
: m_row( idInstrument, eType, idExchange, year, month ),
  m_eUnderlyingStatus( EUnderlyingNotSettable )
{
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option yymm
CInstrument::CInstrument( 
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType, 
  const idExchange_t& idExchange,
  unsigned short year, unsigned short month,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide eOptionSide, 
  double dblStrike ) 
  : m_row( idInstrument, eType, idExchange, pUnderlying->GetInstrumentName(), year, month, eOptionSide, dblStrike ),
  m_pUnderlying( pUnderlying ), 
  m_eUnderlyingStatus( EUnderlyingSet )
{
  //assert( 0 < m_sExchange.size() );
}

 // option yymmdd
CInstrument::CInstrument( 
  idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes eType, 
  const idExchange_t& idExchange,
  unsigned short year, unsigned short month, unsigned short day,
  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide eOptionSide, 
  double dblStrike ) 
  : m_row( idInstrument, eType, idExchange, pUnderlying->GetInstrumentName(), year, month, day, eOptionSide, dblStrike ),
  m_pUnderlying( pUnderlying ), 
  m_eUnderlyingStatus( EUnderlyingSet )
{
  //assert( 0 < m_sExchange.size() );
}

// currency
CInstrument::CInstrument(
  const idInstrument_t& idInstrument, const idInstrument_t& idCounterInstrument,
  InstrumentType::enumInstrumentTypes eType, const idExchange_t& idExchange,
  Currency::enumCurrency base, Currency::enumCurrency counter
  ) 
  : m_row( idInstrument, idCounterInstrument, eType, idExchange, base, counter ),
//  m_pUnderlying( pUnderlying ), 
  m_eUnderlyingStatus( EUnderlyingNotSettable )
{
  
}

/*
  m_eUnderlyingStatus = EUnderlyingNotSet;
  if ( InstrumentType::Option == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( InstrumentType::Currency == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( InstrumentType::FuturesOption == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingSet;
  if ( EUnderlyingNotSet == m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::CInstrument: underlying not accepted" );
  }
  if ( NULL == pUnderlying.get() ) {
    throw std::runtime_error( "CInstrument::CInstrument: non null underlying required" );
  }
  if ( m_sUnderlying != pUnderlying->GetInstrumentName() ) {
    throw std::runtime_error( "CInstrument::CInstrument: underlying name does not match expected name" );
  }
}
*/

CInstrument::CInstrument(const CInstrument& instrument) 
:
  m_row( instrument.m_row ),
  m_pUnderlying( instrument.m_pUnderlying ),
  m_eUnderlyingStatus( instrument.m_eUnderlyingStatus )
{
  mapAlternateNames_t::const_iterator iter = instrument.m_mapAlternateNames.begin();
  while ( instrument.m_mapAlternateNames.end() != iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_pair_t( iter->first, iter->second ) );
  }
}

CInstrument::~CInstrument(void) {
}

void CInstrument::SetAlternateName( eidProvider_t id, idInstrument_cref name ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() == iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_pair_t( id, name ) );
    OnAlternateNameAdded( pairNames_t( m_row.idInstrument, name ) );
  }
  else {
    if ( iter->second != name ) {
      idInstrument_t old = iter->second;
      iter->second.assign( name );
      OnAlternateNameChanged( pairNames_t( old, name ) );
    }
  }
}

CInstrument::idInstrument_cref CInstrument::GetInstrumentName( eidProvider_t id ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() != iter ) {
    //throw std::runtime_error( "CInstrument::GetAlternateName no alternate name" );
    return iter->second;
  }
  return m_row.idInstrument;
}

CInstrument::idInstrument_cref CInstrument::GetUnderlyingName( void ) {
  if ( EUnderlyingNotSet != m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::GetUnderlyingName: underlying not set" );
  }
  return m_pUnderlying->GetInstrumentName();
}

CInstrument::idInstrument_cref CInstrument::GetUnderlyingName( eidProvider_t id ) {
  if ( EUnderlyingNotSet != m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::GetUnderlyingName: underlying not set" );
  }
  return m_pUnderlying->GetInstrumentName(id);
}

void CInstrument::SetUnderlying( pInstrument_t pUnderlying ) {
  if ( EUnderlyingNotSettable == m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::SetUnderlying: can not set underlying" );
  }
  if ( EUnderlyingSet == m_eUnderlyingStatus ) {
    throw std::runtime_error( "CInstrument::SetUnderlying: underlying already set" );
  }
  if ( m_row.idUnderlying != pUnderlying->GetInstrumentName() ) {
    throw std::runtime_error( "CInstrument::SetUnderlying: underlying name does not match expected name" );
  }
  m_pUnderlying = pUnderlying;
  m_eUnderlyingStatus = EUnderlyingSet;
}

} // namespace tf
} // namespace ou

