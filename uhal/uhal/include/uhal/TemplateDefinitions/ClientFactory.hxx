
namespace uhal
{

	template <class T>
	void ClientFactory::add ( const std::string& aProtocol )
	{
		try
		{
			std::hash_map<std::string , boost::shared_ptr<CreatorInterface> >::const_iterator lIt = mCreators.find ( aProtocol ) ;

			if ( lIt != mCreators.end() )
			{
				//pantheios::log_ERROR ( "Throwing at " , ThisLocation() );
				//throw ProtocolAlreadyExist();
				pantheios::log_WARNING ( "Protocol \"" , aProtocol , "\" already exists in map of creators. Continuing for now, but be warned." );
				return;
			}

			mCreators[aProtocol] =  boost::shared_ptr<CreatorInterface> ( new Creator<T>() );
		}
		catch ( const std::exception& aExc )
		{
			pantheios::log_EXCEPTION ( aExc );
			throw uhal::exception ( aExc );
		}
	}


}