#include "DQMHistogramJson.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "FWCore/Framework/interface/MakerMacros.h"

namespace dqmservices {

DQMHistogramJson::DQMHistogramJson(edm::ParameterSet const & iConfig) : DQMHistogramStats(iConfig){};

std::string DQMHistogramJson::toString(boost::property_tree::ptree doc)
{

    boost::regex exp("\"([0-9]+(\\.[0-9]+)?)\"");
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, doc);
    std::string rv = boost::regex_replace(ss.str(), exp, "$1");

    return rv;
}

void DQMHistogramJson::writeMemoryJson(const std::string &fn, const HistoStats &stats, const unsigned int run, const unsigned int lumi) {
  using boost::property_tree::ptree;

  ptree doc;

  doc.put("pid", ::getpid());
  doc.put("n_entries", stats.size());
  doc.put("update_timestamp", std::time(NULL));

  ptree histograms;
  
  ptree info;
  ptree paths;
  ptree run_number;
  ptree lumisection;
  ptree dim_number;
  ptree xBins, xLow, xUp, yBins, yLow, yUp, zBins, zLow, zUp;
  ptree entries;
  ptree xMean, xMeanError, xRms, xRmsError, xUnderflow, xOverflow;
  ptree yMean, yMeanError, yRms, yRmsError, yUnderflow, yOverflow;
  ptree zMean, zMeanError, zRms, zRmsError, zUnderflow, zOverflow;
  for (auto &stat : stats) {
    ptree child;
    child.put("", stat.path);
    paths.push_back(std::make_pair("", child));

    child.put("", run);
    run_number.push_back(std::make_pair("", child));

    child.put("", lumi);
    lumisection.push_back(std::make_pair("", child));

    child.put("", stat.dimNumber);
    dim_number.push_back(std::make_pair("", child));

    child.put("", stat.dimX.nBin);
    xBins.push_back(std::make_pair("", child));
    child.put("", stat.dimX.low);
    xLow.push_back(std::make_pair("", child));
    child.put("", stat.dimX.up);
    xUp.push_back(std::make_pair("", child));

    child.put("", stat.dimY.nBin);
    yBins.push_back(std::make_pair("", child));
    child.put("", stat.dimY.low);
    yLow.push_back(std::make_pair("", child));
    child.put("", stat.dimY.up);
    yUp.push_back(std::make_pair("", child));

    child.put("", stat.dimZ.nBin);
    zBins.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.low);
    zLow.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.up);
    zUp.push_back(std::make_pair("", child));

    child.put("", stat.entries);
    entries.push_back(std::make_pair("", child));

    child.put("", stat.dimX.mean);
    xMean.push_back(std::make_pair("", child));
    child.put("", stat.dimX.meanError);
    xMeanError.push_back(std::make_pair("", child));
    child.put("", stat.dimX.rms);
    xRms.push_back(std::make_pair("", child));
    child.put("", stat.dimX.rmsError);
    xRmsError.push_back(std::make_pair("", child));
    child.put("", stat.dimX.underflow);
    xUnderflow.push_back(std::make_pair("", child));
    child.put("", stat.dimX.overflow);
    xOverflow.push_back(std::make_pair("", child));

    child.put("", stat.dimY.mean);
    yMean.push_back(std::make_pair("", child));
    child.put("", stat.dimY.meanError);
    yMeanError.push_back(std::make_pair("", child));
    child.put("", stat.dimY.rms);
    yRms.push_back(std::make_pair("", child));
    child.put("", stat.dimY.rmsError);
    yRmsError.push_back(std::make_pair("", child));
    child.put("", stat.dimY.underflow);
    yUnderflow.push_back(std::make_pair("", child));
    child.put("", stat.dimY.overflow);
    yOverflow.push_back(std::make_pair("", child));

    child.put("", stat.dimZ.mean);
    zMean.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.meanError);
    zMeanError.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.rms);
    zRms.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.rmsError);
    zRmsError.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.underflow);
    zUnderflow.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.overflow);
    zOverflow.push_back(std::make_pair("", child));
  }

  info.add_child("path", paths);
  info.add_child("run_number", run_number);
  info.add_child("lumisection", lumisection);
  info.add_child("dimensions", dim_number);

  info.add_child("xBins", xBins);
  info.add_child("xLow", xLow);
  info.add_child("xUp", xUp);
  info.add_child("yBins", yBins);
  info.add_child("yLow", yLow);
  info.add_child("yUp", yUp);
  info.add_child("zBins", zBins);
  info.add_child("zLow", zLow);
  info.add_child("zUp", zUp);
  info.add_child("entries", entries);

  info.add_child("xMean", xMean);
  info.add_child("xMeanError", xMeanError);
  info.add_child("xRms", xRms);
  info.add_child("xRmsError", xRmsError);
  info.add_child("xUnderflow", xUnderflow);
  info.add_child("xOverflow", xOverflow);

  info.add_child("yMean", yMean);
  info.add_child("yMeanError", yMeanError);
  info.add_child("yRms", yRms);
  info.add_child("yRmsError", yRmsError);
  info.add_child("yUnderflow", yUnderflow);
  info.add_child("yOverflow", yOverflow);

  info.add_child("zMean", zMean);
  info.add_child("zMeanError", zMeanError);
  info.add_child("zRms", zRms);
  info.add_child("zRmsError", zRmsError);
  info.add_child("zUnderflow", zUnderflow);
  info.add_child("zOverflow", zOverflow);

  histograms.push_back(std::make_pair("", info));

  doc.add_child("histograms", histograms);

  std::ofstream file(fn);
  file << toString(doc);
  file.close();
};

void DQMHistogramJson::dqmEndLuminosityBlock(DQMStore::IBooker &,
                                           DQMStore::IGetter &iGetter,
                                           edm::LuminosityBlock const &iLS,
                                           edm::EventSetup const &) {
  if (dumpOnEndLumi_){
    HistoStats stats = collect(iGetter, histogramNamesEndLumi_);
    if (stats.size() > 0){
      unsigned int irun     = iLS.id().run();
      unsigned int ilumi    = iLS.id().luminosityBlock();
      char suffix[64];
      sprintf(suffix, "R%09dLS%09d", irun, ilumi);
      workflow_ = "Default";
      dirName_ = getStepName();
      fileBaseName_ = dirName_ + "_" + producer_;// + version;
      std::string fileName = onlineOfflineFileName(fileBaseName_, std::string(suffix), workflow_, child_);
      writeMemoryJson(fileName, stats, irun, ilumi);
    }
  }
}

void DQMHistogramJson::dqmEndRun(DQMStore::IBooker &, 
                            DQMStore::IGetter &iGetter,
                            edm::Run const &iRun, 
                            edm::EventSetup const&) {
  if (dumpOnEndRun_){
    HistoStats stats = collect(iGetter, histograms_);
    if (stats.size() > 0){
      unsigned int irun = iRun.run();
      char suffix[64];
      sprintf(suffix, "R%09d", irun);
      workflow_ = "Default";
      dirName_ = getStepName();
      fileBaseName_ = dirName_ + "_" + producer_;// + version;
      std::string fileName = onlineOfflineFileName(fileBaseName_, std::string(suffix), workflow_, child_);
      writeMemoryJson(fileName, stats, irun, 0);
    }
  }
}

DEFINE_FWK_MODULE(DQMHistogramJson);

}  // end of namespace


