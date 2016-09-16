//CORAL includes
#include "CoralKernel/Context.h"
#include "CoralKernel/IProperty.h"
#include "CoralKernel/IPropertyManager.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IPrimaryKey.h"
#include "RelationalAccess/IForeignKey.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ConnectionService.h"

//STL includes
#include <string>
#include <vector>
#include <iostream>
#include <ctime>

#include <boost/program_options.hpp>

namespace 
{ 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0;
}
namespace po = boost::program_options;

int main(int argc, char** argv){
  //connection string
  std::string m_connectionString;
  bool debug;
  po::options_description opt("Allowed options");
  
  opt.add_options()
    ("help, h", "produce help message")
    ("connection, c", po::value<std::string>(&m_connectionString)->default_value("sqlite_file:db1.db"), "set connection string")
    ("debug, d", po::value<bool>(&debug)->default_value(false), "set debug mode");

  po::variables_map vm;
  try 
  { 
    po::store(po::parse_command_line(argc, argv, opt),  vm);

    po::notify(vm);
 
    if ( vm.count("help")  ) 
    { 
      std::cout << "Databse initialization app" << std::endl << opt << std::endl; 
      return SUCCESS; 
    }

    std::cout << "CONN " << m_connectionString << std::endl;
    std::size_t found = m_connectionString.find("sqlite_file:");
    if (found==std::string::npos){
      std::cout << "The connection string must start with 'sqlite_file:'." << '\n';
      return ERROR_IN_COMMAND_LINE;
    }
  } 
  catch(po::error& e) 
  { 
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    std::cerr << opt << std::endl; 
    return ERROR_IN_COMMAND_LINE; 
  }

  std::cout << "INFO: " <<  "Connnecting to " << m_connectionString << ", in " << __func__ << std::endl;

  coral::ConnectionService m_connectionService;
  std::unique_ptr<coral::ISessionProxy> m_session;

  //Database connection configuration parameters
  coral::MsgLevel level = coral::Error;
  if(debug) level = coral::Debug;
  bool enableConnectionSharing = true;
  int connectionTimeOut = 60;
  bool enableReadOnlySessionOnUpdateConnection = false;
  int connectionRetrialPeriod = 60;
  int connectionRetrialTimeOut = 60;
  bool enablePoolAutomaticCleanUp = false;
  
  //now configure the DB connection
  coral::IConnectionServiceConfiguration& coralConfig = m_connectionService.configuration();

  // message streaming
  coral::MessageStream::setMsgVerbosity( level );
  //connection sharing
  if(enableConnectionSharing) coralConfig.enableConnectionSharing();
  else coralConfig.disableConnectionSharing();
  //connection timeout
  coralConfig.setConnectionTimeOut(connectionTimeOut);
  //read-only session on update connection
  if(enableReadOnlySessionOnUpdateConnection) coralConfig.enableReadOnlySessionOnUpdateConnections();
  else coralConfig.disableReadOnlySessionOnUpdateConnections();
  //connection retrial period
  coralConfig.setConnectionRetrialPeriod( connectionRetrialPeriod );
  //connection retrial timeout
  coralConfig.setConnectionRetrialTimeOut( connectionRetrialTimeOut );
  //pool automatic cleanup
  if(enablePoolAutomaticCleanUp) coralConfig.enablePoolAutomaticCleanUp();
  else coralConfig.disablePoolAutomaticCleanUp();


  //Starting init database

  std::cout << "INFO: " <<  "Creating schema, in " << __func__ << std::endl;

  m_session.reset( m_connectionService.connect( m_connectionString, coral::Update ) );
  //TODO: do not run in production!
  //create the relevant tables
  coral::ISchema& schema = m_session->nominalSchema();
  m_session->transaction().start( false );
  bool dqmTablesExist = schema.existsTable( "HISTOGRAM" );
  if( ! dqmTablesExist )
  {
    int columnSize = 200;

    // Create the first table
    coral::TableDescription table1;
    table1.setName( "HISTOGRAM" );
    table1.insertColumn( "PATH", coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, false );
    table1.insertColumn( "TIMESTAMP", coral::AttributeSpecification::typeNameForType<unsigned int>());
    table1.insertColumn( "TITLE", coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, false );

    table1.setNotNullConstraint( "PATH" );
    table1.setNotNullConstraint( "TIMESTAMP" );
    table1.setNotNullConstraint( "TITLE" );

    std::vector<std::string> columnsForPrimaryKey1;
    columnsForPrimaryKey1.push_back( "PATH" );
    table1.setPrimaryKey( columnsForPrimaryKey1 );

    schema.createTable( table1 );

    // Create the second table
    coral::TableDescription table2;
    table2.setName( "HISTOGRAM_PROPS" );
    table2.insertColumn( "PATH", coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, false );
    table2.insertColumn( "RUN_NUMBER", coral::AttributeSpecification::typeNameForType<unsigned int>() );
    table2.insertColumn( "DIMENSIONS", coral::AttributeSpecification::typeNameForType<int>() );
    table2.insertColumn( "X_AXIS", coral::AttributeSpecification::typeNameForType<std::string>() );
    table2.insertColumn( "Y_AXIS", coral::AttributeSpecification::typeNameForType<std::string>() );
    table2.insertColumn( "Z_AXIS", coral::AttributeSpecification::typeNameForType<std::string>() );

    table2.setNotNullConstraint( "PATH" );
    table2.setNotNullConstraint( "RUN_NUMBER" );
    table2.setNotNullConstraint( "DIMENSIONS" );
    table2.setNotNullConstraint( "X_AXIS" );

    std::vector<std::string> columnsForPrimaryKey2;
    columnsForPrimaryKey2.push_back( "PATH" );
    columnsForPrimaryKey2.push_back( "RUN_NUMBER" );
    table2.setPrimaryKey( columnsForPrimaryKey2 );

    std::vector<std::string> columnsForForeignKey2;

    columnsForForeignKey2.push_back( "PATH" );

    table2.createForeignKey( "HISTOGRAM_PROPS_HISTOGRAM_FK", columnsForForeignKey2, "HISTOGRAM", columnsForPrimaryKey1 );

    schema.createTable( table2 );

    // Create the third table
    coral::TableDescription table3;
    table3.setName( "HISTOGRAM_VALUES" );
    table3.insertColumn( "PATH", coral::AttributeSpecification::typeNameForType<std::string>(), columnSize, false );
    table3.insertColumn( "RUN_NUMBER", coral::AttributeSpecification::typeNameForType<unsigned int>() );
    table3.insertColumn( "LUMISECTION", coral::AttributeSpecification::typeNameForType<unsigned int>() );
    table3.insertColumn( "ENTRIES", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "X_MEAN", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "X_MEAN_ERROR", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "X_RMS", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "X_RMS_ERROR", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "X_UNDERFLOW", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "X_OVERFLOW", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Y_MEAN", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Y_MEAN_ERROR", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Y_RMS", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Y_RMS_ERROR", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Y_UNDERFLOW", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Y_OVERFLOW", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Z_MEAN", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Z_MEAN_ERROR", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Z_RMS", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Z_RMS_ERROR", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Z_UNDERFLOW", coral::AttributeSpecification::typeNameForType<double>() );
    table3.insertColumn( "Z_OVERFLOW", coral::AttributeSpecification::typeNameForType<double>() );

    table3.setNotNullConstraint( "PATH" );
    table3.setNotNullConstraint( "RUN_NUMBER" );
    table3.setNotNullConstraint( "LUMISECTION" );
    table3.setNotNullConstraint( "ENTRIES" );
    table3.setNotNullConstraint( "X_MEAN" );
    table3.setNotNullConstraint( "X_MEAN_ERROR" );
    table3.setNotNullConstraint( "X_RMS" );
    table3.setNotNullConstraint( "X_RMS_ERROR" );
    table3.setNotNullConstraint( "X_UNDERFLOW" );
    table3.setNotNullConstraint( "X_OVERFLOW" );
    table3.setNotNullConstraint( "Y_MEAN" );
    table3.setNotNullConstraint( "Y_MEAN_ERROR" );
    table3.setNotNullConstraint( "Y_RMS" );
    table3.setNotNullConstraint( "Y_RMS_ERROR" );
    table3.setNotNullConstraint( "Y_UNDERFLOW" );
    table3.setNotNullConstraint( "Y_OVERFLOW" );
    table3.setNotNullConstraint( "Z_MEAN" );
    table3.setNotNullConstraint( "Z_MEAN_ERROR" );
    table3.setNotNullConstraint( "Z_RMS" );
    table3.setNotNullConstraint( "Z_RMS_ERROR" );
    table3.setNotNullConstraint( "Z_UNDERFLOW" );
    table3.setNotNullConstraint( "Z_OVERFLOW" );

    std::vector<std::string> columnsForPrimaryKey3;
    columnsForPrimaryKey3.push_back( "PATH" );
    columnsForPrimaryKey3.push_back( "RUN_NUMBER" );
    columnsForPrimaryKey3.push_back( "LUMISECTION" );
    table3.setPrimaryKey( columnsForPrimaryKey3 );

    std::vector<std::string> columnsForForeignKey3;
    columnsForForeignKey3.push_back( "PATH" );
    //columnsForForeignKey3.push_back( "RUN_NUMBER" );

    table3.createForeignKey( "HISTOGRAM_VALUES_HISTOGRAM_FK", columnsForForeignKey3, "HISTOGRAM", columnsForPrimaryKey1 );

    schema.createTable( table3 );
  }
  m_session->transaction().commit();

  std::cout << "INFO: " <<  "Done, in " << __func__ << std::endl;
}
