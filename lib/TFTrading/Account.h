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

#include <string>

#include <boost/shared_ptr.hpp>

#include <OUSQL/Functions.h>

#include "KeyTypes.h"
#include "TradingEnumerations.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// manages margin stuff
// margin adjustments:
//  cash vs cash liens
//  each position has margin requirements
//  each portfolio may have margin requirement adjustments due to combons
//  portfolio is used to adjust cas lien in cash balance
// should match one to one with owner's brokerage account

class Account {
public:

  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idAccountOwner_t idAccountOwner_t;
  //typedef keytypes::idCurrency_t idCurrency_t;
  typedef Currency::type currency_t;
  typedef boost::shared_ptr<Account> pAccount_t;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "accountownerid", idAccountOwner );
      ou::db::Field( a, "accountname", sAccountName );
      ou::db::Field( a, "providerid", idProvider );
      ou::db::Field( a, "brokername", sBrokerName );
      ou::db::Field( a, "brokeraccountid", sBrokerAccountId );
      ou::db::Field( a, "login", sLogin );
      ou::db::Field( a, "password", sPassword );
      ou::db::Field( a, "port", sPort );
      ou::db::Field( a, "basecurrency", idBaseCurrency );
      ou::db::Field( a, "multiplecurrency", bMultipleCurrency );  // multiple currencies allowed if true
      ou::db::Field( a, "margin", dblMargin );
    }

    idAccount_t idAccount;
    idAccountOwner_t idAccountOwner;
    std::string sAccountName;
    keytypes::eidProvider_t idProvider;
    std::string sBrokerName;
    std::string sBrokerAccountId;
    std::string sLogin;
    std::string sPassword;
    std::string sPort;
    currency_t idBaseCurrency;
    bool bMultipleCurrency;
    double dblMargin;  // % from 0 to 100

    TableRowDef( void ) {};
    TableRowDef( 
      const idAccount_t& idAccount_, 
      const idAccountOwner_t& idAccountOwner_, const std::string& sAccountName_, 
      keytypes::eidProvider_t idProvider_, 
      const std::string& sBrokerName_, const std::string& sBrokerAccountId_,
      const std::string& sLogin_, const std::string& sPassword_, const std::string& sPort_,
      const currency_t& idBaseCurrency_, double dblMargin_ ) 
      : idAccount( idAccount_ ), idAccountOwner( idAccountOwner_ ),
        sAccountName( sAccountName_ ), idProvider( idProvider_ ), 
        sBrokerName( sBrokerName_ ), sBrokerAccountId( sBrokerAccountId_ ),
        sLogin( sLogin_ ), sPassword( sPassword_ ), sPort( sPort_ ),
        idBaseCurrency( idBaseCurrency_ ), bMultipleCurrency( true ), dblMargin( dblMargin_ ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "accountid" );
      ou::db::Constraint( a, "accountownerid", tablenames::sAccountOwner, "accountownderid" );
    }
  };

  Account(
    const idAccount_t& idAccount,
    const idAccountOwner_t& idAccountOwner,
    const std::string& sAccountName,
    keytypes::eidProvider_t idProvider,
    const std::string& sBrokerName,
    const std::string& sBrokerAccountId,
    const std::string& sLogin, 
    const std::string& sPassword, 
    const std::string& sPort = "",
    const currency_t& idBaseCurrency = Currency::Name[ Currency::USD ],
    double dblMargin = 100.0
    );
  Account( const TableRowDef& row ) : m_row( row ) {};
  ~Account(void);

  void SetPort( const std::string& sPort ) { m_row.sPort = sPort; };

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:

private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou
