#include "DQMHistogramDB.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "FWCore/Framework/interface/MakerMacros.h"

namespace dqmservices {

DQMHistogramDB::DQMHistogramDB(edm::ParameterSet const & ps) : DQMHistogramStats(ps){
  dbw_.reset(new DQMDatabaseWriter(ps));
  dbw_->initDatabase();
};

void DQMHistogramDB::dqmEndLuminosityBlock(DQMStore::IBooker &,
                                           DQMStore::IGetter &iGetter,
                                           edm::LuminosityBlock const &iLumi,
                                           edm::EventSetup const &) {
  edm::LogInfo("DQMDatabaseHarvester") << "DQMDatabaseHarvester::dqmEndLuminosityBlock " << std::endl;
  if (dumpOnEndLumi_){
    HistoStats stats = (histogramNamesEndLumi_.size() > 0) ? collect(iGetter, histogramNamesEndLumi_) : collect(iGetter);
    if (checkLumiHistos_) {
      std::cout << "RUN NUMBER OF MAX INT: " << UINT_MAX << std::endl;
      dbw_->dqmPropertiesDbDrop(stats, iLumi.run());
      checkLumiHistos_ = false;
    }
    dbw_->dqmValuesDbDrop(stats, iLumi.run(), iLumi.luminosityBlock());
  }
}

void DQMHistogramDB::dqmEndRun(DQMStore::IBooker &, 
                            DQMStore::IGetter &iGetter,
                            edm::Run const &iRun, 
                            edm::EventSetup const&) {
  if (dumpOnEndRun_){
    edm::LogInfo("DQMDatabaseHarvester") <<  "DQMDatabaseHarvester::endRun" << std::endl;
    HistoStats stats = (histograms_.size() > 0) ? collect(iGetter, histograms_) : collect(iGetter);
    std::cout << "RUN NUMBER OF RUN: " << iRun.run() << std::endl;
    dbw_->dqmPropertiesDbDrop(stats, iRun.run());
    //for run based histograms, we use lumisection value set to 0.
    dbw_->dqmValuesDbDrop(stats, iRun.run(), 0);
  }
}

DEFINE_FWK_MODULE(DQMHistogramDB);
}  // end of namespace


