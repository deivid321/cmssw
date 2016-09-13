import FWCore.ParameterSet.Config as cms


dqmHistogramDB = cms.EDAnalyzer("DQMHistogramDB",

    statsDepth = cms.untracked.int32(1),
    pathNameMatch = cms.untracked.string('*'),
    dumpMemoryHistory = cms.untracked.bool(True),                             
    verbose = cms.untracked.int32(0),
    runInEventLoop = cms.untracked.bool(False),
    dumpOnEndLumi = cms.untracked.bool(True),
    dumpOnEndRun = cms.untracked.bool(True),
    runOnEndJob = cms.untracked.bool(False),
    dumpToFWJR = cms.untracked.bool(True),
    histogramNamesEndLumi = cms.untracked.vstring("AlCaReco\/HcalIsoTrack\/hRatL3"),
    histogramNamesEndRun = cms.untracked.vstring("AlCaReco\/HcalIsoTrack\/hRatL3"),
    
    #database configuration
    DBParameters = cms.PSet(authenticationPath = cms.untracked.string(''),
			messageLevel = cms.untracked.int32(3),
			enableConnectionSharing = cms.untracked.bool(True),
			connectionTimeOut = cms.untracked.int32(60),
			enableReadOnlySessionOnUpdateConnection = cms.untracked.bool(False),
			connectionRetrialTimeOut = cms.untracked.int32(60),
			connectionRetrialPeriod = cms.untracked.int32(10),
			enablePoolAutomaticCleanUp = cms.untracked.bool(False)
			),

    connect = cms.string('oracle://cms_orcoff_prep/CMS_DQM_HISTO'),
    authPath = cms.string('/afs/cern.ch/user/d/dvoronec/private/authentication.xml'),
)
