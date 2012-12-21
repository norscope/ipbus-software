
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

static const std::string gLogLevelsChar[] = { "Fatal" , "Error" , "Warning" , "Notice" , "Info" , "Debug" };
static const uint32_t gNumberEntries ( 6 );

static const std::string gDefaultLevel ( "Info" );

static const std::vector< std::string > gLogLevels ( gLogLevelsChar , gLogLevelsChar+gNumberEntries );



std::string gDivider (	"// " + std::string ( 150,'=' ) + "\n" +
                        "// WARNING! This file is automatically generated! Do not modify it! Any changes will be overwritten!\n" +
                        "// " + std::string ( 150,'=' ) + "\n" );



void fileHeaders ( std::ofstream& aHppFile , std::ofstream& aHxxFile , std::ofstream& aCppFile )
{
  aHppFile	<< "\n"
            << "#ifndef _log_hpp_\n"
            << "#define _log_hpp_\n"
            << "\n"
            << "#include <uhal/log/log_backend.hpp>\n"
            << "#include <uhal/log/log_inserters.hpp>\n"
            << "//#include <boost/thread/thread.hpp>\n"
            << "#include <boost/thread/mutex.hpp>\n"
            << "\n"
            << "#define logging() logger log( ThisLocation() );\n"
            << "\n"
            << "namespace uhal{\n"
            << "\n"
            << gDivider
            << "\n";
  aHxxFile	<< "\n"
            << "namespace uhal{\n"
            << "\n"
            << gDivider
            << "\n";
  aCppFile	<< "\n"
            << "#include <uhal/log/log.hpp>\n"
            << "\n"
            << "namespace uhal{\n"
            << "\n"
            << gDivider
            << "\n";
}


void log_configuration_functions ( std::ofstream& aHppFile , std::ofstream& aHxxFile , std::ofstream& aCppFile )
{
  aHppFile << "/**\n"
           << "\tFunction to specify that the logging level should be retrieved from an environment variable\n"
           << "\t@param aEnvVar the name of the environment variable which is used to specify the logging level\n"
           << "*/\n"
           << "void setLogLevelFromEnvironment ( const char* aEnvVar );\n";
  aCppFile << "void setLogLevelFromEnvironment ( const char* aEnvVar )\n"
           << "{\n"
           << "\tchar * lEnvVar = getenv ( aEnvVar );\n"
           << "\tif( !lEnvVar )\n"
           << "\t{\n"
           << "\t\tlogging();\n"
           << "\t\tlog( Warning() , \"No environment variable \" , Quote( aEnvVar ) , \" set. Using level \" , Quote( \"Info\" ) , \" instead.\" );\n"
           << "\t\tsetLogLevelTo ( " << gDefaultLevel << "() );\n"
           << "\t\treturn;\n"
           << "\t}\n"
           << "\n"
           << "\t//Just comparing the first letter of the environment variable for speed!!!\n"
           << "\tswitch ( lEnvVar[0] )\n"
           << "\t{\n";

  for ( std::vector< std::string >::const_iterator lIt = gLogLevels.begin() ; lIt != gLogLevels.end() ; ++lIt )
  {
    aCppFile << "\t\tcase '" << char ( std::tolower ( lIt->at ( 0 ) ) ) << "' :\n"
             << "\t\tcase '" << char ( std::toupper ( lIt->at ( 0 ) ) ) << "' :\n"
             << "\t\t\tsetLogLevelTo ( " << *lIt << "() );\n"
             << "\t\t\tbreak;\n";
  }

  aCppFile << "\t\tdefault:\n"
           << "\t\t\tlogging();\n"
           << "\t\t\tlog ( Warning() , \"Environment varible has invalid value \" , Quote( lEnvVar ) , \". Using level \" , Quote ( \"Info\" ) , \" instead.\" );\n"
           << "\t\t\tsetLogLevelTo ( Info() );\n"
           << "\t}\n"
           << "}\n"
           << "\n";
  aCppFile	<< gDivider
            << "\n";
  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  aHppFile << "/**\n"
           << "\tFunction to disable all logging levels\n"
           << "*/\n"
           << "void disableLogging();\n";
  aCppFile << "void disableLogging()\n"
           << "{\n";

  for ( std::vector< std::string >::const_iterator lIt = gLogLevels.begin() ; lIt != gLogLevels.end() ; ++lIt )
  {
    aCppFile << "\tlog_configuration::mLoggingIncludes" << *lIt << " = false;\n";
  }

  aCppFile << "}\n"
           << "\n";
  aCppFile	<< gDivider
            << "\n";
  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  aHppFile << "/**\n"
           << "\tFunction to retrieve the mutex lock used by the logger\n"
           << "*/\n"
           << "boost::mutex& GetLoggingMutex();\n";
  aCppFile << "\tboost::mutex& GetLoggingMutex()\n"
           << "\t{\n"
           << "\t\treturn log_configuration::mMutex;\n"
           << "\t}\n";
  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  std::stringstream lIfDefs, lIfDefs2, lEndIfs;

  for ( std::vector< std::string >::const_iterator lIt = gLogLevels.begin() ; lIt != gLogLevels.end() ; ++lIt )
  {
    lIfDefs << "\t#ifndef LOGGING_EXCLUDE_" << boost::to_upper_copy ( *lIt ) << "\n";
    lIfDefs2 << "\t#ifndef LOGGING_EXCLUDE_" << boost::to_upper_copy ( *lIt ) << " // A waste of time to change any level below this if it is going to disabled by compile-time checking anyway... \n"
             << "\t\tlog_configuration::mLoggingIncludes" << *lIt << " = true;\n";
    lEndIfs << "\t#endif\n";
    aHppFile << "//! Helper struct representing the " << *lIt << " log level to allow us to specialize functions according to their log level\n"
             << "struct " << *lIt << " {};\n";
    aHppFile << "/**\n"
             << "\tFunction to specify, at runtime, that only messages with a severity level above " << *lIt << " should be logged\n"
             << "\t@param a" << *lIt << " a dummy parameter to chose the specialization of the function for the " << *lIt << " level\n"
             << "*/\n"
             << "void setLogLevelTo ( const " << *lIt << "& a" << *lIt << " );\n";
    aCppFile << "void setLogLevelTo ( const " << *lIt << "& a" << *lIt << " )\n"
             << "{\n"
             << lIfDefs2.str();

    for ( std::vector< std::string >::const_iterator lIt2 = lIt+1 ; lIt2 != gLogLevels.end() ; ++lIt2 )
    {
      aCppFile << "\t\tlog_configuration::mLoggingIncludes" << *lIt2 << " = false;\n";
    }

    aCppFile << lEndIfs.str()
             << "}\n"
             << "\n";
    aHppFile << "/**\n"
             << "\tFunction to check at runtime whether the level " << *lIt << " is to be included in the log output\n"
             << "\t@param a" << *lIt << " a dummy parameter to chose the specialization of the function for the " << *lIt << " level\n"
             << "\t@return whether the level " << *lIt << " is to be included in the log output\n"
             << "*/\n"
             << "const bool& LoggingIncludes ( const " << *lIt << "& a" << *lIt << " );\n"
             << "\n";
    aCppFile << "const bool& LoggingIncludes ( const " << *lIt << "& a" << *lIt << " )\n"
             << "{\n"
             << lIfDefs.str()
             << "\t\treturn log_configuration::mLoggingIncludes" << *lIt << ";\n"
             << lEndIfs.str()
             << "\treturn log_configuration::mFalse;\n"
             << "}\n"
             << "\n";
  }

  aHppFile << "//! Class to restrict access to the log configuration parameters\n"
           << "class log_configuration\n"
           << "{\n"
           << "\t//! Constructor\n"
           << "\tlog_configuration();\n"
           << "\t//! Destructor\n"
           << "\tvirtual ~log_configuration();\n"
           << "\n"
           << "\tfriend void disableLogging();\n"
           << "\n";
  bool lIncludeLevelByDefault ( true );

  for ( std::vector< std::string >::const_iterator lIt = gLogLevels.begin() ; lIt != gLogLevels.end() ; ++lIt )
  {
    aHppFile << "\t//! static bool storing whether the " << *lIt << " level is to be included in the log output\n"
             << "\tstatic bool mLoggingIncludes" << *lIt << ";\n";

    if ( lIncludeLevelByDefault )
    {
      aCppFile << "bool log_configuration::mLoggingIncludes" << *lIt << " = true; // No #ifdefs required here since they are implemented in all the access functions.\n";
    }
    else
    {
      aCppFile << "bool log_configuration::mLoggingIncludes" << *lIt << " = false; // No #ifdefs required here since they are implemented in all the access functions.\n";
    }

    if ( *lIt == gDefaultLevel )
    {
      lIncludeLevelByDefault = false;
    }

    aHppFile << "\t//!Make setLogLevelTo function a friend so it can access our private members\n"
             << "\tfriend void setLogLevelTo ( const " << *lIt << "& );\n"
             << "\t//!Make LoggingIncludes function a friend so it can access our private members\n"
             << "\tfriend const bool& LoggingIncludes ( const " << *lIt << "& );\n"
             << "\n";
  }

  aHppFile << "\t//!Define a static const member variable to have a value of true so that we can safely return a const reference to true\n"
           << "\tstatic const bool mTrue;\n"
           << "\t//!Define a static const member variable to have a value of false so that we can safely return a const reference to false\n"
           << "\tstatic const bool mFalse;\n"
           << "\n"
           << "\t//!Make GetLoggingMutex function a friend so it can access our private members\n"
           << "\tfriend boost::mutex& GetLoggingMutex();\n"
           << "\t//!Define a static Mutex lock for thread safe logging\n"
           << "\tstatic boost::mutex mMutex;\n"
           << "};\n"
           << "\n";
  aCppFile << "\n"
           << "const bool log_configuration::mTrue = true;\n"
           << "const bool log_configuration::mFalse = false;\n"
           << "\n"
           << "boost::mutex log_configuration::mMutex;\n"
           << "\n";
  aHppFile	<< gDivider
            << "\n";
  aCppFile	<< gDivider
            << "\n";
}


std::string suffix ( uint32_t i )
{
  i = i % 100;

  if ( i > 10 && i < 20 )
  {
    return "th";
  }

  char* lIndices[] = { "th" , "st" , "nd" , "rd" , "th" , "th" , "th" , "th" , "th" , "th" };
  return lIndices[ i%10 ];
}


void log_functions ( std::ofstream& aHppFile , std::ofstream& aHxxFile , std::ofstream& aCppFile )
{
  std::stringstream lIfDefs , lEndIfs;
  aHppFile << "class logger\n"
           << "{\n"
           << "public:\n"
           << "\tlogger( const Location& aLocation );\n"
           << "\tvirtual ~logger();\n"
           << "private:\n"
           << "\tLocation mLocation;\n"
           << "public:\n";
  aCppFile << "logger::logger( const Location& aLocation ) : mLocation( aLocation ) {}\n"
           << "\n"
           << "logger::~logger()\n"
           << "{\n"
           << "\tif (std::uncaught_exception())\n"
           << "\t{\n"
           << "\t\tthis->operator()( Error() , \"Exception spotted at \" , mLocation );\n"
           << "\t}\n"
           << "}\n";

  for ( std::vector< std::string >::const_iterator lIt = gLogLevels.begin() ; lIt != gLogLevels.end() ; ++lIt )
  {
    lIfDefs << "\t#ifndef LOGGING_EXCLUDE_" << boost::to_upper_copy ( *lIt ) << "\n";
    lEndIfs << "\t#endif\n";
    std::stringstream lTemplates;
    std::stringstream lArgs;
    std::stringstream lInstructions;
    std::stringstream lDoxygen;
    lDoxygen << "\t\tFunction to add a log entry at " << *lIt << " level\n"
             << "\t\t@param a" << *lIt << " a dummy parameter to chose the specialization of the function for the " << *lIt << " level\n";

    for ( uint32_t i = 0 ; i!=MAX_NUM_ARGS ; ++i )
    {
      lTemplates << " typename T" << i << " ,";
      std::string lTemplatesStr ( lTemplates.str() );
      lTemplatesStr.resize ( lTemplatesStr.size()-1 );
      lArgs << " const T" << i << "& aArg" << i << " ,";
      std::string lArgsStr ( lArgs.str() );
      lArgsStr.resize ( lArgsStr.size()-1 );
      lInstructions << "\t\t\tlog_inserter( aArg" << i << " );\n";
      lDoxygen << "\t\t@param aArg" << i << " a templated argument to be added to the log " << ( i+1 ) << suffix ( i+1 ) <<"\n";
      aHppFile << "\t/**\n"
               << lDoxygen.str()
               << "\t*/\n"
               << "\ttemplate<" << lTemplatesStr << ">\n"
               << "\tvoid operator() ( const " <<*lIt << "& a" << *lIt << " ," << lArgsStr << ");\n"
               << "\n";
      aHxxFile << "template<" << lTemplatesStr << ">\n"
               << "void logger::operator() ( const " <<*lIt << "& a" << *lIt << " ," << lArgsStr << " )\n"
               << "{\n"
               << lIfDefs.str()
               << "\t\tif( LoggingIncludes( a" << *lIt << " ) ){\n"
               << "\t\t\tboost::lock_guard<boost::mutex> lLock ( GetLoggingMutex() );\n"
               //               << "\t\t\tlog_head< " << *lIt << " >();\n"
               << "\t\t\tlog_head_" << *lIt << "();\n"
               << lInstructions.str()
               //               << "\t\t\tlog_tail< " << *lIt << " >();\n"
               << "\t\t\tlog_tail_" << *lIt << "();\n"
               << "\t\t}\n"
               << lEndIfs.str()
               << "}\n"
               << "\n";
    }

    aHppFile	<< gDivider
              << "\n";
    aHxxFile	<< gDivider
              << "\n";
  }

  aHppFile << "};\n"
           << "\n";
}


void fileFooters ( std::ofstream& aHppFile , std::ofstream& aHxxFile , std::ofstream& aCppFile )
{
  aHppFile	<< "}\n\n"
            << "#include <uhal/log/log.hxx>\n"
            << "#endif\n\n";
  aHxxFile	<< "}\n"
            << "\n";
  aCppFile	<< "}\n"
            << "\n";
}



int main ( int argc , char* argv[] )
{
  try
  {
    std::ofstream lHppFile ( "include/uhal/log/log.hpp" );

    if ( !lHppFile.is_open() )
    {
      std::cout << "Unable to open HPP file" << std::endl;
      return 1;
    }

    std::ofstream lHxxFile ( "include/uhal/log/log.hxx" );

    if ( !lHxxFile.is_open() )
    {
      std::cout << "Unable to open HXX file" << std::endl;
      return 1;
    }

    std::ofstream lCppFile ( "src/common/log.cpp" );

    if ( !lCppFile.is_open() )
    {
      std::cout << "Unable to open CPP file" << std::endl;
      return 1;
    }

    fileHeaders ( lHppFile , lHxxFile , lCppFile );
    log_configuration_functions ( lHppFile , lHxxFile , lCppFile );
    log_functions ( lHppFile , lHxxFile , lCppFile );
    fileFooters ( lHppFile , lHxxFile , lCppFile );
    lHppFile.close();
    lHxxFile.close();
    lCppFile.close();
  }
  catch ( const std::exception& aExc )
  {
    std::cerr << "ERROR: Caught Exception : " << aExc.what() << std::endl;
    exit ( 1 );
  }
}


