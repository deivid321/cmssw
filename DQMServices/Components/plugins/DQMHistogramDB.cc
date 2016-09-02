#include "DQMHistogramDB.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <vector>

namespace dqmservices {

DQMHistogramDB::DQMHistogramDB(edm::ParameterSet const & ps) : DQMHistogramStats(ps), dbw_(ps) {}

void DQMHistogramDB::dqmEndLuminosityBlock(DQMStore::IBooker &,
                                           DQMStore::IGetter &iGetter,
                                           edm::LuminosityBlock const &iLumi,
                                           edm::EventSetup const &) {
  edm::LogInfo("DQMDatabaseHarvester") << "DQMDatabaseHarvester::dqmEndLuminosityBlock " << std::endl;
  if (dumpOnEndLumi_){
      HistoStats stats = collect(iGetter, histogramNamesEndLumi_);
      if (stats.size() > 0){
      DQMScopedTransaction scopedTransaction(dbw_);
      scopedTransaction.start();
      if (checkLumiHistos_) {
        dbw_.dqmPropertiesDbDrop(stats, iLumi.run());
        checkLumiHistos_ = false;
      }
      dbw_.dqmValuesDbDrop(stats, iLumi.run(), iLumi.luminosityBlock());
      scopedTransaction.commit();
    }
  }
}

void DQMHistogramDB::dqmEndRun(DQMStore::IBooker &, 
                            DQMStore::IGetter &iGetter,
                            edm::Run const &iRun, 
                            edm::EventSetup const&) {
  if (dumpOnEndRun_){
    edm::LogInfo("DQMDatabaseHarvester") <<  "DQMDatabaseHarvester::endRun" << std::endl;
    HistoStats stats = collect(iGetter, histograms_);
    if (stats.size() > 0){
      DQMScopedTransaction scopedTransaction(dbw_);
      scopedTransaction.start();
      dbw_.dqmPropertiesDbDrop(stats, iRun.run());
      //for run based histograms, we use lumisection value set to 0.
      dbw_.dqmValuesDbDrop(stats, iRun.run(), 0);
      scopedTransaction.commit();
    }
  }
}

DEFINE_FWK_MODULE(DQMHistogramDB);
}  // end of namespace


