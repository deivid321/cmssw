#ifndef DQMStreamStats_H
#define DQMStreamStats_H

/** \class DQMStreamStats
 * *
 *  DQM Store Stats - new version, for multithreaded framework
 *
 *  \author Dmitrijus Bugelskis CERN
 */

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <utility>
#include <fstream>
#include <sstream>

#include "FWCore/Framework/interface/stream/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/MonitorElement.h"

namespace dqmservices {

class HistoEntry {
 public:
  std::string path;

  const char *type;
  size_t bin_count;
  size_t bin_size;
  size_t extra;
  size_t total;

  bool operator<(const HistoEntry &rhs) const { return path < rhs.path; }
};

typedef std::set<HistoEntry> HistoStats;

class DQMStreamStats : public DQMEDHarvester {
 public:
  DQMStreamStats(edm::ParameterSet const &);
  virtual ~DQMStreamStats();

  // static std::unique_ptr<Stats> initializeGlobalCache(edm::ParameterSet
  // const&);
  // static void globalEndJob(Stats const* iStats);
  // virtual void analyze(edm::Event const&, edm::EventSetup const&) override;
  void dqmEndLuminosityBlock(DQMStore::IBooker &, DQMStore::IGetter &,
                             edm::LuminosityBlock const &,
                             edm::EventSetup const &) override;

  void dqmEndJob(DQMStore::IBooker &iBooker,
                 DQMStore::IGetter &iGetter) override{};

  // analyze a single monitor element
  HistoEntry analyze(MonitorElement *m);

  // group summaries per folder
  // void group(HistoStats& st);

 protected:
  HistoStats collect(DQMStore::IGetter &iGetter);
  void writeJson(const std::string &fn, const HistoStats &stats);
};

}  // end of namespace
#endif
