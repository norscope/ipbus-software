
namespace uhal
{

	template <class T>
	void NodeTreeBuilder::add ( const std::string& aProtocol )
	{
		try
		{
			std::hash_map<std::string , boost::shared_ptr<CreatorInterface> >::const_iterator lIt = mCreators.find ( aProtocol ) ;

			if ( lIt != mCreators.end() )
			{
				//log ( Error() , "Throwing at " , ThisLocation() );
				//throw ProtocolAlreadyExist();
				log ( Warning() , "Protocol \"" , aProtocol , "\" already exists in map of creators. Continuing for now, but be warned." );
				return;
			}

			mCreators[aProtocol] =  boost::shared_ptr<CreatorInterface> ( new Creator<T>() );
		}
		catch ( const std::exception& aExc )
		{
			log ( Error() , "Exception \"" , aExc.what() , "\" caught at " , ThisLocation() );
			throw uhal::exception ( aExc );
		}
	}


	template <class T>
	boost::shared_ptr< const Node > NodeTreeBuilder::Creator<T>::create ( const pugi::xml_node& aXmlNode , const uint32_t& aParentAddr , const uint32_t& aParentMask )
	{
		try
		{
			return boost::shared_ptr< const Node > ( new T ( aXmlNode , aParentAddr , aParentMask ) );
		}
		catch ( const std::exception& aExc )
		{
			log ( Error() , "Exception \"" , aExc.what() , "\" caught at " , ThisLocation() );
			throw uhal::exception ( aExc );
		}
	}


}