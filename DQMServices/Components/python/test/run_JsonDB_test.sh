#!/bin/bash

function die { echo Failure $1: status $2 ; exit $2 ; }

export LOCAL_TEST_DIR=$CMSSW_BASE/src/DQMServices/Components/python/test/
echo ${LOCAL_TEST_DIR}

COLOR_WARN="\\033[0;31m"
COLOR_NORMAL="\\033[0;39m"
postfix=''

if [ $# -gt 0 ]; then
  postfix=$1;shift
fi

echo -e "   ***   ${COLOR_WARN}RUNNING TEST JsonDB on BeginRun with OPTION $postfix${COLOR_NORMAL}   ***   "
for bookIn in BR
do
  #OLD file4
  rm -f file_JsonDB.db
  rm -f create_file_JsonDB_cfg*.json
  cmsdqm_initDB --c sqlite_file:file_JsonDB.db
  testConfig=create_file_JsonDB_cfg.py
  rm -f file_JsonDB.root
  echo -e "${COLOR_WARN}${testConfig}${COLOR_NORMAL}"
  #deleted ${postfix}
  cmsRun ${LOCAL_TEST_DIR}/${testConfig} $bookIn &> /dev/null || die "cmsRun ${testConfig}" $?

  #check file4
  testConfig=check_file_JsonDB.py
  echo -e "${COLOR_WARN}${testConfig}${COLOR_NORMAL}"
  python ${LOCAL_TEST_DIR}/${testConfig} &> /dev/null || die "cmsRun ${testConfig}" $?

  #HARVEST SINGLE FILES 
  testConfig=harv_file_JsonDB_cfg.py
  rm -f DQM_V0001_R00000000?__Test__FileJsonDB__DQM.root
  echo -e "${COLOR_WARN}${testConfig}${COLOR_NORMAL}"
  cmsRun ${LOCAL_TEST_DIR}/${testConfig} &> /dev/null || die "cmsRun ${testConfig}" $?

  #CHECK HARVESTED FILE
  python compare-dqm_JsonRootDB.py create_file_JsonDB_cfg_DQMR000000002Default.json DQM_V0001_R000000002__Test__FileJsonDB__DQM.root file_JsonDB.db

done

exit 0
