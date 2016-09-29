import FWCore.ParameterSet.Config as cms
import DQMServices.Components.test.checkBooking as booking
import DQMServices.Components.test.createElements as c
import sys

process =cms.Process("TEST")

b = booking.BookingParams(sys.argv)
b.doCheck(testOnly=False)

process.source = cms.Source("EmptySource",
                            numberEventsInRun = cms.untracked.uint32(100),
                            firstLuminosityBlock = cms.untracked.uint32(1),
                            firstEvent = cms.untracked.uint32(1000),
                            firstRun = cms.untracked.uint32(2),
                            numberEventsInLuminosityBlock = cms.untracked.uint32(1))

elements = c.createElements()
readRunElements = c.createReadRunElements()
readLumiElements = c.createReadLumiElements()
eventsToProcess = c.createEventsToProcess()

process.maxEvents = eventsToProcess

dqm_folder = "TestFolder/"
lumi_extension = "_lumi"

histoRunList = []
histoLumiList = []

for x in readRunElements:
    histoRunList.append(dqm_folder + x.name.value())

for x in readLumiElements:
    histoLumiList.append(dqm_folder + x.name.value() + lumi_extension)

process.filler = cms.EDAnalyzer("DummyBookFillDQMStore" + b.mt_postfix(),
                                folder    = cms.untracked.string(dqm_folder),
                                elements  = cms.untracked.VPSet(*elements),
                                fillRuns  = cms.untracked.bool(True),
                                fillLumis = cms.untracked.bool(True),
                                book_at_constructor = cms.untracked.bool(b.getBookLogic('CTOR')),
                                book_at_beginJob = cms.untracked.bool(b.getBookLogic('BJ')),
                                book_at_beginRun = cms.untracked.bool(b.getBookLogic('BR')))

process.reader = cms.EDAnalyzer("DummyTestReadDQMStore",
                                folder = cms.untracked.string(dqm_folder),
                                runElements = cms.untracked.VPSet(*readRunElements),
                                lumiElements = cms.untracked.VPSet(*readLumiElements),
                                runToCheck = cms.untracked.int32(2)
    )

process.out = cms.OutputModule("DQMRootOutputModule",
                               fileName = cms.untracked.string("file_JsonDB.root"))

process.p = cms.Path(process.filler)
process.o = cms.EndPath(process.out+process.reader)

process.add_(cms.Service("DQMStore"))

if b.multithread():
    process.out.enableMultiThread = cms.untracked.bool(True)
    process.DQMStore.enableMultiThread = cms.untracked.bool(True)
    process.o.remove(process.reader)

process.schedule = cms.Schedule(process.p, process.o)

process.load("DQMServices.Components.DQMHistogramDB_cfi")
process.dqmHistogramDB.histogramNamesEndLumi.extend(histoLumiList)
process.dqmHistogramDB.histogramNamesEndRun.extend(histoRunList)
process.dqmHistogramDB.connect = cms.string('sqlite_file:file_JsonDB.db')


# authentication system for the DQM DB writer:
# when using pure CORAL, the auth system is XMLbased and cannot be chosen,
# so we need to customise ESSources with authentication system type 2;
# when using Cond API, you can decide which one to use: default 0 is key based.
#process.dqmHistogramDB.DBParameters.authenticationSystem = cms.untracked.int32(0)
#process.dqmHistogramDB.DBParameters.authenticationPath = cms.untracked.string('')

process.stats = cms.EndPath(process.dqmHistogramDB) 
process.schedule.append(process.stats)

process.load("DQMServices.Components.DQMHistogramJson_cfi")
process.dqmHistogramJson.histogramNamesEndLumi.extend(histoLumiList)
process.dqmHistogramJson.histogramNamesEndRun.extend(histoRunList)


process.json = cms.EndPath(process.dqmHistogramJson) 
process.schedule.append(process.json)


