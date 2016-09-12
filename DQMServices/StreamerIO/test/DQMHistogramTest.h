#ifndef DQMHistogramTest_H
#define DQMHistogramTest_H

//Framework
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//DQM
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

//Include STL
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <sstream>
#include <math.h>

class DQMHistogramTest: public DQMEDAnalyzer{

public:

  DQMHistogramTest(const edm::ParameterSet& ps);
  virtual ~DQMHistogramTest();

protected:
  void dqmBeginRun(edm::Run const &, edm::EventSetup const &) override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
  void analyze(edm::Event const& e, edm::EventSetup const& eSetup);
  void beginLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& eSetup) ;
  void endLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& eSetup);
  void endRun(edm::Run const& run, edm::EventSetup const& eSetup);

private:
  std::string path_;
  std::vector<std::string> histograms_;
  std::vector<MonitorElement*> mHistograms_;
  //histos booking function
  void bookHistos(DQMStore::IBooker &);

};


#endif
