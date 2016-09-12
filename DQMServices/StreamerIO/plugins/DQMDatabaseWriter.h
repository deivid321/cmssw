#ifndef DQMSERVICES_STREAMERIO_PLUGINS_DQMDATABASEWRITER_H
#define DQMSERVICES_STREAMERIO_PLUGINS_DQMDATABASEWRITER_H

//local includes
#include "DQMHistoStats.h"

//Framework includes
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//CORAL includes
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"

//boost includes
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//STL includes
#include <string>

class DQMDatabaseWriter {

public:

  DQMDatabaseWriter(const edm::ParameterSet& ps);
  virtual ~DQMDatabaseWriter();
  
  void startTransaction( bool readOnly = false );
  void commitTransaction();
  void rollbackTransaction();
  void dqmPropertiesDbDrop(const HistoStats &stats, unsigned int run);
  void dqmValuesDbDrop(const HistoStats &stats, unsigned int run, unsigned int lumisection);

protected:
  coral::ConnectionService m_connectionService;
  std::unique_ptr<coral::ISessionProxy> m_session;
  std::string m_connectionString;

private:
  std::string toString(boost::property_tree::ptree doc);
  std::string dimensionJson(Dimension &dim);
  void exceptionThrow(std::string quantity, std::string path, unsigned int run);

};

class DQMScopedTransaction {
public:
	DQMScopedTransaction() = delete;
	explicit DQMScopedTransaction( DQMDatabaseWriter& dbw ): m_dbw(dbw), m_committed(false) {}
	~DQMScopedTransaction() {
		if (!m_committed) this->rollback();
	}
	void start(bool readOnly = false) {
		m_dbw.startTransaction(readOnly);
	}
	void commit() {
		m_dbw.commitTransaction();
		m_committed = true;
	}
	void rollback() {
		m_dbw.rollbackTransaction();
		m_committed = false;
	}
private:
	DQMDatabaseWriter& m_dbw;
	bool m_committed;
};

#endif
