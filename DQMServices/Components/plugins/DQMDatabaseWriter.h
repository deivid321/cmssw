#ifndef DQMSERVICES_DATABASEIO_PLUGINS_DQMDATABASEWRITER_H
#define DQMSERVICES_DATABASEIO_PLUGINS_DQMDATABASEWRITER_H

#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/ServiceRegistry/interface/ModuleCallingContext.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//Framework
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

//DQM
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

//event
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

//CORAL includes
#include "RelationalAccess/ConnectionService.h"

//STL includes
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <sstream>
#include <math.h>
#include <ctime>
#include <cmath>

#include "DQMHistoStats.h"

class DQMDatabaseWriter {

public:

  DQMDatabaseWriter(const edm::ParameterSet& ps);
  virtual ~DQMDatabaseWriter();
  
  void initDatabase();

  void dqmDbDrop(const HistoStats &stats, int luminosity, int run);

protected:
  coral::ConnectionService m_connectionService;
  std::unique_ptr<coral::ISessionProxy> m_session;
  std::string m_connectionString;

};


#endif
