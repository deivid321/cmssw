#include "DQMStreamStats.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "FWCore/Framework/interface/MakerMacros.h"

namespace dqmservices {

DQMStreamStats::DQMStreamStats(edm::ParameterSet const &) {}

DQMStreamStats::~DQMStreamStats() {}

//Set all possible x dimension parameters
void DQMStreamStats::getDimensionX(Dimension &d, MonitorElement *m){
  d.nBin = m->getNbinsX();
  d.low = m->getTH1()->GetXaxis()->GetXmin();
  d.up = m->getTH1()->GetXaxis()->GetXmax();
  d.mean = m->getTH1()->GetMean();
  d.meanError = m->getTH1()->GetMeanError();
  d.rms = m->getTH1()->GetRMS();
  d.rmsError = m->getTH1()->GetRMSError();
  d.underflow = m->getTH1()->GetBinContent(0);
  d.overflow = m->getTH1()->GetBinContent(d.nBin + 1);
}

void DQMStreamStats::getDimensionY(Dimension &d, MonitorElement *m){
  d.nBin = m->getNbinsY();
  d.low = m->getTH1()->GetYaxis()->GetXmin();
  d.up = m->getTH1()->GetYaxis()->GetXmax();
  d.mean = m->getTH1()->GetMean(2);
  d.meanError = m->getTH1()->GetMeanError(2);
  d.rms = m->getTH1()->GetRMS(2);
  d.rmsError = m->getTH1()->GetRMSError(2);
}

void DQMStreamStats::getDimensionZ(Dimension &d, MonitorElement *m){
  d.nBin = m->getNbinsZ();
  d.low = m->getTH1()->GetZaxis()->GetXmin();
  d.up = m->getTH1()->GetZaxis()->GetXmax();
  d.mean = m->getTH1()->GetMean(3);
  d.meanError = m->getTH1()->GetMeanError(3);
  d.rms = m->getTH1()->GetRMS(3);
  d.rmsError = m->getTH1()->GetRMSError(3);
}

HistoEntry DQMStreamStats::analyze(MonitorElement *m) {
  HistoEntry e;
  e.name = m->getName();
  e.path = m->getFullname();
  e.type = "unknown";

  switch (m->kind()) {
    case MonitorElement::DQM_KIND_INVALID:
      e.type = "INVALID";
      e.bin_size = -1;
      break;
    case MonitorElement::DQM_KIND_INT:
      e.type = "INT";
      e.bin_size = sizeof(int);
      break;
    case MonitorElement::DQM_KIND_REAL:
      e.type = "REAL";
      e.bin_size = sizeof(float);
      break;
    case MonitorElement::DQM_KIND_STRING:
      e.type = "STRING";
      e.bin_size = sizeof(char);
      break;

    // one-dim ME
    case MonitorElement::DQM_KIND_TH1F:
      e.type = "TH1F";
      e.bin_size = sizeof(float);
      getDimensionX(e.dimX, m);
      break;
    case MonitorElement::DQM_KIND_TH1S:
      e.type = "TH1S";
      e.bin_size = sizeof(short);
      getDimensionX(e.dimX, m);
      break;
    case MonitorElement::DQM_KIND_TH1D:
      e.type = "TH1D";
      e.bin_size = sizeof(double);
      getDimensionX(e.dimX, m);
      break;
    case MonitorElement::DQM_KIND_TPROFILE:
      e.type = "TProfile";
      e.bin_size = sizeof(double);
      getDimensionX(e.dimX, m);
      break;

    // two-dim ME
    case MonitorElement::DQM_KIND_TH2F:
      e.type = "TH2F";
      e.bin_size = sizeof(float);
      getDimensionX(e.dimX, m);
      getDimensionY(e.dimY, m);
      break;
    case MonitorElement::DQM_KIND_TH2S:
      e.type = "TH2S";
      e.bin_size = sizeof(short);
      getDimensionX(e.dimX, m);
      getDimensionY(e.dimY, m);
      break;
    case MonitorElement::DQM_KIND_TH2D:
      e.type = "TH2D";
      e.bin_size = sizeof(double);
      getDimensionX(e.dimX, m);
      getDimensionY(e.dimY, m);
      break;
    case MonitorElement::DQM_KIND_TPROFILE2D:
      e.type = "TProfile2D";
      e.bin_size = sizeof(double);
      getDimensionX(e.dimX, m);
      getDimensionY(e.dimY, m);
      break;

    // three-dim ME
    case MonitorElement::DQM_KIND_TH3F:
      e.type = "TH3F";
      e.bin_size = sizeof(float);
      getDimensionX(e.dimX, m);
      getDimensionY(e.dimY, m);
      getDimensionZ(e.dimZ, m);
      break;

    default:
      e.type = "unknown";
      e.bin_size = 0;
  };
  
  // skip "INVALID", "INT", "REAL", "STRING", "unknown"
  if (strcmp(e.type,"INVALID") && strcmp(e.type,"INT") && strcmp(e.type,"REAL") && strcmp(e.type,"STRING") && strcmp(e.type,"unknown")) {
      e.bin_count = m->getTH1()->GetNcells();
      e.entries = m->getEntries();
      e.maxBin = m->getTH1()->GetMaximumBin();
      e.minBin = m->getTH1()->GetMinimumBin();
      e.maxValue = m->getTH1()->GetMaximum();
      e.minValue = m->getTH1()->GetMinimum();
  }

  e.total = e.bin_count * e.bin_size + e.extra;

  return e;
}

HistoStats DQMStreamStats::collect(DQMStore::IGetter &iGetter) {
  // new stat frame
  HistoStats st;

  auto collect_f = [&st, this](MonitorElement *m) -> void {
    auto frame = this->analyze(m);
    st.insert(frame);
  };

  // correct stream id don't matter - no booking will be done
  iGetter.getAllContents_(collect_f, "");
  return st;
}

#if 0
void DQMStreamStats::group(HistoStats& stats) {
  HistoStats summaries;

  for (HistoEntry& entry : stats) {
    std::vector<std::string> tokens;
    boost::split(strs, entry.path, boost::is_any_of("/"));

    std::string prefix = entry.path;

    // for every non-base path component
    tokens.pop_back();
    for (std::string& token : tokens) {
      std::string summary_path = "unknown"
      
    }
  }
}
#endif

void DQMStreamStats::writeMemoryJson(const std::string &fn, const HistoStats &stats) {
  using boost::property_tree::ptree;

  ptree doc;

  doc.put("pid", ::getpid());
  doc.put("n_entries", stats.size());
  doc.put("update_timestamp", std::time(NULL));

  ptree histograms;
  
  ptree info;
  ptree paths;
  ptree types;
  ptree bin_counts;
  ptree bin_sizes;
  ptree extras;
  ptree totals;
  ptree entries;
  ptree maxBins, minBins;
  ptree maxValues, minValues;

  for (auto &stat : stats) {
    ptree child;
    child.put("", stat.path);
    paths.push_back(std::make_pair("", child));
    child.put("", stat.type);
    types.push_back(std::make_pair("", child));
    child.put("", stat.bin_count);
    bin_counts.push_back(std::make_pair("", child));
    child.put("", stat.bin_size);
    bin_sizes.push_back(std::make_pair("", child));
    child.put("", stat.extra);
    extras.push_back(std::make_pair("", child));
    child.put("", stat.total);
    totals.push_back(std::make_pair("", child));
    child.put("", stat.entries);
    entries.push_back(std::make_pair("", child));
    child.put("", stat.maxBin);
    maxBins.push_back(std::make_pair("", child));
    child.put("", stat.minBin);
    minBins.push_back(std::make_pair("", child));
    child.put("", stat.maxValue);
    maxValues.push_back(std::make_pair("", child));
    child.put("", stat.minValue);
    minValues.push_back(std::make_pair("", child));
  }

  info.add_child("path", paths);
  info.add_child("type", types);
  info.add_child("bin_count", bin_counts);
  info.add_child("bin_size", bin_sizes);
  info.add_child("extra", extras);
  info.add_child("total", totals);
  info.add_child("entries", entries);
  info.add_child("maxBin", maxBins);
  info.add_child("minBin", minBins);
  info.add_child("maxValues", maxValues);
  info.add_child("minValues", minValues);
  histograms.push_back(std::make_pair("", info));

  doc.add_child("histograms", histograms);

  std::ofstream file(fn);
  write_json(file, doc, true);
  file.close();
};

void DQMStreamStats::writeHistogramJson(const std::string &fn, const HistoStats &stats) {
  using boost::property_tree::ptree;

  ptree doc;

  doc.put("pid", ::getpid());
  doc.put("n_entries", stats.size());
  doc.put("update_timestamp", std::time(NULL));

  ptree histograms;

  ptree info;
  ptree paths;
  ptree bin_counts;
  ptree entries;
  ptree names;
  ptree xNBins, yNBins, zNBins;
  ptree xLows, yLows, zLows;
  ptree xUps, yUps, zUps;
  ptree xMeans, yMeans, zMeans;
  ptree xMeanErrors, yMeanErrors, zMeanErrors;
  ptree xRms, yRms, zRms;
  ptree xRmsErrors, yRmsErrors, zRmsErrors;
  ptree xUnderflows, xOverflows;

  for (auto &stat : stats) {
    ptree child;
    child.put("", stat.path);
    paths.push_back(std::make_pair("", child));
    child.put("", stat.name);
    names.push_back(std::make_pair("", child));
    child.put("", stat.entries);
    entries.push_back(std::make_pair("", child));
    child.put("", stat.bin_count);
    bin_counts.push_back(std::make_pair("", child));

    child.put("", stat.dimX.nBin);
    xNBins.push_back(std::make_pair("", child));
    child.put("", stat.dimX.low);
    xLows.push_back(std::make_pair("", child));
    child.put("", stat.dimX.up);
    xUps.push_back(std::make_pair("", child));
    child.put("", stat.dimY.nBin);
    yNBins.push_back(std::make_pair("", child));
    child.put("", stat.dimY.low);
    yLows.push_back(std::make_pair("", child));
    child.put("", stat.dimY.up);
    yUps.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.nBin);
    zNBins.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.low);
    zLows.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.up);
    zUps.push_back(std::make_pair("", child));
    
    child.put("", stat.dimX.mean);
    xMeans.push_back(std::make_pair("", child));
    child.put("", stat.dimX.meanError);
    xMeanErrors.push_back(std::make_pair("", child));
    child.put("", stat.dimX.rms);
    xRms.push_back(std::make_pair("", child));
    child.put("", stat.dimX.rmsError);
    xRmsErrors.push_back(std::make_pair("", child));
    child.put("", stat.dimX.underflow);
    xUnderflows.push_back(std::make_pair("", child));
    child.put("", stat.dimX.overflow);
    xOverflows.push_back(std::make_pair("", child));

    child.put("", stat.dimY.mean);
    yMeans.push_back(std::make_pair("", child));
    child.put("", stat.dimY.meanError);
    yMeanErrors.push_back(std::make_pair("", child));
    child.put("", stat.dimY.rms);
    yRms.push_back(std::make_pair("", child));
    child.put("", stat.dimY.rmsError);
    yRmsErrors.push_back(std::make_pair("", child));

    child.put("", stat.dimZ.mean);
    zMeans.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.meanError);
    zMeanErrors.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.rms);
    zRms.push_back(std::make_pair("", child));
    child.put("", stat.dimZ.rmsError);
    zRmsErrors.push_back(std::make_pair("", child));
  }

  info.add_child("path", paths);
  info.add_child("name", names);
  info.add_child("entries", entries);
  info.add_child("bin_count", bin_counts);
  info.add_child("x_bins", xNBins);
  info.add_child("x_low", xLows);
  info.add_child("x_up", xUps);
  info.add_child("y_bins", yNBins);
  info.add_child("y_low", yLows);
  info.add_child("y_up", yUps);
  info.add_child("z_bins", zNBins);
  info.add_child("z_low", zLows);
  info.add_child("z_up", zUps);
  info.add_child("x_mean", xMeans);
  info.add_child("x_mean_error", xMeanErrors);
  info.add_child("x_rms", xRms);
  info.add_child("x_rms_error", xRmsErrors);
  info.add_child("x_underflow", xUnderflows);
  info.add_child("x_overflow", xOverflows);
  info.add_child("y_mean", yMeans);
  info.add_child("y_mean_error", yMeanErrors);
  info.add_child("y_rms", yRms);
  info.add_child("y_rms_error", yRmsErrors);
  info.add_child("z_mean", zMeans);
  info.add_child("z_mean_error", zMeanErrors);
  info.add_child("z_rms", zRms);
  info.add_child("z_rms_error", zRmsErrors);
  histograms.push_back(std::make_pair("", info));

  doc.add_child("histograms", histograms);

  std::ofstream file(fn);
  write_json(file, doc, true);
  file.close();
};

void DQMStreamStats::dqmEndLuminosityBlock(DQMStore::IBooker &,
                                           DQMStore::IGetter &iGetter,
                                           edm::LuminosityBlock const &,
                                           edm::EventSetup const &) {
  HistoStats st = collect(iGetter);
  writeMemoryJson("MemoryJson", st);
  writeHistogramJson("HistogramJson", st);
}

#if 0
std::unique_ptr<Stats> DQMStreamStats::initializeGlobalCache(edm::ParameterSet const&) {
  return std::unique_ptr<Stats>(new Stats());
}

void DQMStreamStats::analyze(edm::Event const&, edm::EventSetup const&) {
//This can safely be updated from multiple Streams because we are using an std::atomic
//      ++(globalCache()->value);
}

void DQMStreamStats::globalEndJob(Stats const* iStats) {
  //std::cout <<"Number of events seen "<<iCount->value<<std::endl;
}
#endif

DEFINE_FWK_MODULE(DQMStreamStats);

}  // end of namespace


