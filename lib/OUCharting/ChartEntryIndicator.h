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

#include "ChartEntryBase.h"

namespace ou { // One Unified

class CChartEntryIndicator :
  public CChartEntryBaseWithTime {
public:
  CChartEntryIndicator(void);
  CChartEntryIndicator( unsigned int nSize );
  virtual ~CChartEntryIndicator(void);
  virtual void Reserve( unsigned int );
  virtual void AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes );
protected:
private:
};

} // namespace ou
