#ifndef DQMHistogramDB_H
#define DQMHistogramDB_H
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMHistogramStats.h"
#include "DQMDatabaseWriter.h"

namespace dqmservices {

class DQMHistogramDB : public DQMHistogramStats {
 public:
  	DQMHistogramDB(edm::ParameterSet const & iConfig);

  	void dqmEndLuminosityBlock(DQMStore::IBooker &, DQMStore::IGetter &,
                             edm::LuminosityBlock const &,
                             edm::EventSetup const &) override;

  	void dqmEndRun(DQMStore::IBooker &, DQMStore::IGetter &,
              edm::Run const&, 
              edm::EventSetup const&) override;

 private:
  bool checkLumiHistos_ = true;
  DQMDatabaseWriter dbw_;
};

}
#endif