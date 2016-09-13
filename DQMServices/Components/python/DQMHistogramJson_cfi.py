import FWCore.ParameterSet.Config as cms


dqmHistogramJson = cms.EDAnalyzer("DQMHistogramJson",

    statsDepth = cms.untracked.int32(1),
    pathNameMatch = cms.untracked.string('*'),
    dumpMemoryHistory = cms.untracked.bool(True),                             
    verbose = cms.untracked.int32(0),
    runInEventLoop = cms.untracked.bool(False),
    dumpOnEndLumi = cms.untracked.bool(True),
    dumpOnEndRun = cms.untracked.bool(True),
    runOnEndJob = cms.untracked.bool(False),
    dumpToFWJR = cms.untracked.bool(True),
    histogramNamesEndLumi = cms.untracked.vstring(),
    histogramNamesEndRun = cms.untracked.vstring(),
)
