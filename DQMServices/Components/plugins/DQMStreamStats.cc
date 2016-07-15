#include "DQMStreamStats.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "FWCore/Framework/interface/MakerMacros.h"

namespace dqmservices {

DQMStreamStats::DQMStreamStats(edm::ParameterSet const &) {}

DQMStreamStats::~DQMStreamStats() {}

HistoEntry DQMStreamStats::analyze(MonitorElement *m) {
  HistoEntry e;
  e.path = m->getFullname();
  e.type = "unknown";
  e.bin_count = 0;
  e.bin_size = 0;
  e.extra = 0;
  e.total = 0;

  switch (m->kind()) {
    // one-dim ME
    case MonitorElement::DQM_KIND_TH1F:
      e.type = "TH1F";
      e.bin_size = sizeof(float);
      break;
    case MonitorElement::DQM_KIND_TH1S:
      e.type = "TH1S";
      e.bin_size = sizeof(short);
      break;
    case MonitorElement::DQM_KIND_TH1D:
      e.type = "TH1D";
      e.bin_size = sizeof(double);
      break;
    case MonitorElement::DQM_KIND_TPROFILE:
      e.type = "TProfile";
      e.bin_size = sizeof(double);
      break;

    // two-dim ME
    case MonitorElement::DQM_KIND_TH2F:
      e.type = "TH2F";
      e.bin_size = sizeof(float);
      break;
    case MonitorElement::DQM_KIND_TH2S:
      e.type = "TH2S";
      e.bin_size = sizeof(short);
      break;
    case MonitorElement::DQM_KIND_TH2D:
      e.type = "TH2D";
      e.bin_size = sizeof(double);
      break;
    case MonitorElement::DQM_KIND_TPROFILE2D:
      e.type = "TProfile2D";
      e.bin_size = sizeof(double);
      break;

    // three-dim ME
    case MonitorElement::DQM_KIND_TH3F:
      e.type = "TH3F";
      e.bin_size = sizeof(float);
      break;

    default:
      e.type = "unknown";
      e.bin_size = 0;
  };

  // skip "unknown"
  if (e.bin_size != 0) {
    e.bin_count = m->getTH1()->GetNcells();
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

void DQMStreamStats::writeJson(const std::string &fn, const HistoStats &stats) {
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

    ptree child;
    child.put("path", stat.path);
    child.put("type", stat.type);
    child.put("bin_count", stat.bin_count);
    child.put("bin_size", stat.bin_size);
    child.put("extra", stat.extra);
    child.put("total", stat.total);

    histograms.push_back(std::make_pair("", child));
  }

  info.add_child("path", paths);
  info.add_child("type", types);
  info.add_child("bin_count", bin_counts);
  info.add_child("bin_size", bin_sizes);
  info.add_child("extra", extras);
  info.add_child("total", totals);
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
  writeJson("outputJson", st);
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


