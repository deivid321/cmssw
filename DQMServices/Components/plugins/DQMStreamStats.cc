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
  if (!strcmp(e.type,"INT") && !strcmp(e.type,"STRING") && !strcmp(e.type,"REAL") && !strcmp(e.type,"unknown")) {
      e.bin_count = m->getTH1()->GetNcells();
      e.entries = m->getEntries();
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

  for (auto &stat : stats) {
    
    ptree infoChild;
    infoChild.put("", stat.path);
    paths.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.type);
    types.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.bin_count);
    bin_counts.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.bin_size);
    bin_sizes.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.extra);
    extras.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.total);
    totals.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.entries);
    entries.push_back(std::make_pair("", infoChild));

    ptree child;
    child.put("path", stat.path);
    child.put("type", stat.type);
    child.put("bin_count", stat.bin_count);
    child.put("bin_size", stat.bin_size);
    child.put("extra", stat.extra);
    child.put("total", stat.total);
    child.put("entries", stat.entries);

    histograms.push_back(std::make_pair("", child));
  }

  info.add_child("path", paths);
  info.add_child("type", types);
  info.add_child("bin_count", bin_counts);
  info.add_child("bin_size", bin_sizes);
  info.add_child("extra", extras);
  info.add_child("total", totals);
  info.add_child("entries", entries);
  histograms.push_front(std::make_pair("", info));

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
    ptree infoChild;
    infoChild.put("", stat.path);
    paths.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.bin_count);
    bin_counts.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.entries);
    entries.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.name);
    names.push_back(std::make_pair("", infoChild));

    infoChild.put("", stat.dimX.nBin);
    xNBins.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimX.low);
    xLows.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimX.up);
    xUps.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimY.nBin);
    yNBins.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimY.low);
    yLows.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimY.up);
    yUps.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimZ.nBin);
    zNBins.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimZ.low);
    zLows.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimZ.up);
    zUps.push_back(std::make_pair("", infoChild));
    
    infoChild.put("", stat.dimX.mean);
    xMeans.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimX.meanError);
    xMeanErrors.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimX.rms);
    xRms.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimX.rmsError);
    xRmsErrors.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimX.underflow);
    xUnderflows.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimX.overflow);
    xOverflows.push_back(std::make_pair("", infoChild));

    infoChild.put("", stat.dimY.mean);
    yMeans.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimY.meanError);
    yMeanErrors.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimY.rms);
    yRms.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimY.rmsError);
    yRmsErrors.push_back(std::make_pair("", infoChild));

    infoChild.put("", stat.dimZ.mean);
    zMeans.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimZ.meanError);
    zMeanErrors.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimZ.rms);
    zRms.push_back(std::make_pair("", infoChild));
    infoChild.put("", stat.dimZ.rmsError);
    zRmsErrors.push_back(std::make_pair("", infoChild));

    ptree child;
    child.put("path", stat.path);
    child.put("bin_count", stat.bin_count);
    child.put("entries", stat.entries);
    child.put("name", stat.name);

    child.put("x_bins", stat.dimX.nBin);
    child.put("x_low", stat.dimX.low);
    child.put("x_up", stat.dimX.up);
    child.put("y_bins", stat.dimY.nBin);
    child.put("y_low", stat.dimY.low);
    child.put("y_up", stat.dimY.up);
    child.put("z_bins", stat.dimZ.nBin);
    child.put("z_low", stat.dimZ.low);
    child.put("z_up", stat.dimZ.up);
    
    child.put("x_mean", stat.dimX.mean);
    child.put("x_mean_error", stat.dimX.meanError);
    child.put("x_rms", stat.dimX.rms);
    child.put("x_rms_error", stat.dimX.rmsError);
    child.put("x_underflow", stat.dimX.underflow);
    child.put("x_overflow", stat.dimX.overflow);

    child.put("y_mean", stat.dimY.mean);
    child.put("y_mean_error", stat.dimY.meanError);
    child.put("y_rms", stat.dimY.rms);
    child.put("y_rms_error", stat.dimY.rmsError);

    child.put("z_mean", stat.dimZ.mean);
    child.put("z_mean_error", stat.dimZ.meanError);
    child.put("z_rms", stat.dimZ.rms);
    child.put("z_rms_error", stat.dimZ.rmsError);

    histograms.push_back(std::make_pair("", child));
  }

  info.add_child("path", paths);
  info.add_child("bin_count", bin_counts);
  info.add_child("entries", entries);
  info.add_child("name", names);
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
  histograms.push_front(std::make_pair("", info));

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
  writeMemoryJson("outputJson", st);
  writeHistogramJson("outputHistogramJson", st);
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


