
import sys
import sqlite3
from datetime import datetime
from multiprocessing import Pool, Queue, Process
from optparse import OptionParser, OptionGroup
from os import makedirs
from os.path import basename, join, exists
from optparse import OptionParser
import json
from Utilities.RelMon.utils_v2 import ComparisonError, tests, init_database, get_version
from Utilities.RelMon.web.app_utils import get_release, get_stats, get_percentage, get_img_url, get_dataset_name

parser = OptionParser(usage='Usage: %prog <jsonFile> <rootFile> <dbFile> ')

def compare(jsonFile, root, db):
    #Load json file
    with open(jsonFile) as data_file:
        data = json.load(data_file)

    ## Compare to root file
    print "Comparing: json file to root file."
    if not 'TFile' in globals():
        from ROOT import TFile
    f1 = TFile(root)
    dir_DQMData = f1.GetDirectory("DQMData")
    dir_Run = None
    for elem in dir_DQMData.GetListOfKeys():
        elem_name = elem.GetName()
        if elem_name.startswith('Run '):
            dir_Run = dir_DQMData.Get(elem_name)

    diff = walk_through(dir_Run, f1, data["histograms"][0]["path"], data["histograms"][0]["xRms"], [])
    f1.Close()
    print "Difference json file to root: "
    print diff

    # Load database data
    conn = sqlite3.connect(db)
    run = data["histograms"][0]["run_number"][0]
    lumisection = 0 # run based
    c = conn.cursor()
    c.execute("SELECT PATH FROM HISTOGRAM_VALUES WHERE (RUN_NUMBER=? and LUMISECTION=?)", (run, lumisection))
    paths = c.fetchall()
    c.execute("SELECT X_RMS FROM HISTOGRAM_VALUES WHERE (RUN_NUMBER=? and LUMISECTION=?)", (run, lumisection))
    x_rms = c.fetchall()

    # Compare json file to database
    print "Comparing: json file to database."
    isTheSame = True
    rms_db = []
    for x in x_rms:
         rms_db.append(x[0])
    ind = 0
    for json_rms in data["histograms"][0]["xRms"]:
        if json_rms != rms_db[ind]:
            print "UNEQUALS: JSON FILE: path: " + data["histograms"][0]["path"][ind] +" x_rms: "+ json_rms + "Database row: path: " + paths[ind] +" x_rms: "+ rms_db[ind]
            isTheSame = False
        ind+=1
    print "Json and database are the same: " + str(isTheSame)
    print 'Comparison finished'

def walk_through(directory, f1, names, rms, diff, path=''):
    for elem in directory.GetListOfKeys():
        elem_name = elem.GetName()
        subdir = directory.Get(elem_name)
        if subdir:
             if subdir.IsFolder():
                diff = walk_through(subdir, f1, names, rms, diff, path=join(path, elem_name))
             else:
                hist1 = f1.Get(join(directory.GetPath(), elem_name))
                paths = path.split("/")
                try:
                    ind = names.index(join(paths[0], elem_name))
                except ValueError:
                    print "Value was not found in json file. " + join(paths[0], elem_name)
                    continue
                dataStr = "ROOT FILE: " + join(paths[0], elem_name) + " " + str(hist1.GetRMS()) + " JSON FILE: " + names[ind] + " " + str(rms[ind])
                if rms[ind] == hist1.GetRMS():
                    print "EQUALS: " + dataStr
                else:
                    print "UNEQUALS: " + dataStr
                    diff.append("UNEQUALS " + dataStr)
    return diff
                #print "Histogram: %s/%s, Title: %s, Number of Entries: %d" %(directory.GetPath(), hist1.GetName(), hist1.GetTitle(), hist1.GetEntries() )
                #print hist1.GetRMS()


def compare1(filename1):
    if not 'TFile' in globals():
        from ROOT import TFile
    f1 = TFile(filename1)

    ## Create Directory and HistogramComparison structure in the DB
    dir_DQMData = f1.GetDirectory("DQMData")
    dir_Run = None
    for elem in dir_DQMData.GetListOfKeys():
        elem_name = elem.GetName()
        if elem_name.startswith('Run '):
            dir_Run = dir_DQMData.Get(elem_name)

    walk_through(dir_Run, f1)

    f1.Close()
    print "done"


# def loopdir( TDirectory *source ):
#   #source->ls();
#   source->cd();
#   TIter nextkey( source->GetListOfKeys() )
#   TKey *key
#   while( ( key = (TKey*)nextkey() ) )
#     TClass *cl = gROOT->GetClass( key->GetClassName() )
#     if( !cl ) continue
#     if( cl->InheritsFrom( TDirectory::Class() ) )
#       source->cd(key->GetName())
#       TDirectory *subdir = gDirectory
#       print "Dir: " + gDirectory->GetName() + ", Path: " + gDirectory->GetPath() + "\n"
#       loopdir( subdir )
#          else if( cl->InheritsFrom( "TH1" ) ):
#       TH1 *h = (TH1*)key->ReadObj();
#       print "Histogram: " + gDirectory->GetPath() + "/" + h->GetName()
#                 + ", Title: " + h->GetTitle()
#                 + ", Number of Entries: " + h->GetEntries()
#                 + "\n"










if __name__ == '__main__':
    opts, args = parser.parse_args()
    if len(args) != 3:
        parser.error('Specify three files to use for the comparison.')

    #print 'Comparing files:\n%s\n%s\n' % (basename(args[0]), basename(args[1]))
    #res = compare(args[0], args[1])
    print basename(args[0])
    print basename(args[1])
    print basename(args[2])
    compare(basename(args[0]), args[1], args[2])
