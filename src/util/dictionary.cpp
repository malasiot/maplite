#include "dictionary.hpp"

#include <iostream>

using namespace std ;

Dictionary::Dictionary() {}

void Dictionary::add(const string &key, const string &val)
{
    container_.insert(std::pair<string, string>(key, val)) ;
}

void Dictionary::remove(const string &key)
{
    ContainerType::iterator it = container_.find(key) ;

    if ( it != container_.end() ) container_.erase(it) ;
}

void Dictionary::removeSome(const regex &rx)
{
    ContainerType::iterator it = container_.begin() ;

    for( ; it != container_.end() ;  )
	{
        if ( regex_match((*it).first, rx) )
            container_.erase(it++) ;
        else ++it ;
	}
}

void Dictionary::clear() 
{
    container_.clear() ;
}
		

string Dictionary::get(const string &key, const string &defaultVal) const
{
    ContainerType::const_iterator it = container_.find(key) ;

    if ( it != container_.end() ) return (*it).second ;
	else return defaultVal ;
}

string Dictionary::operator[] ( const string & key ) const
{
	return get(key) ;
}

string &Dictionary::operator[] ( const string & key )
{
    string &r = container_[key] ;

	return r ;
}
	
bool Dictionary::contains(const string &key) const
{
    ContainerType::const_iterator it = container_.find(key) ;

    return ( it != container_.end() ) ;
}

// get a list of the keys in the dictionary

std::vector<string> Dictionary::keys() const
{
    std::vector<string> res ;

    ContainerType::const_iterator it = container_.begin() ;

    for( ; it != container_.end() ; ++it )
		res.push_back((*it).first) ;

	return res ;
}

std::vector<string> Dictionary::keys(const regex &rx) const
{
    std::vector<string> res ;

    ContainerType::const_iterator it = container_.begin() ;

    for( ; it != container_.end() ; ++it )
        if ( regex_match((*it).first, rx) ) res.push_back((*it).first) ;

	return res ;
}

std::vector<string> Dictionary::values() const
{
    std::vector<string> res ;

    ContainerType::const_iterator it = container_.begin() ;

    for( ; it != container_.end() ; ++it )
		res.push_back((*it).second) ;

	return res ;
}

std::vector<string> Dictionary::values(const regex &rx) const
{
    std::vector<string> res ;

    ContainerType::const_iterator it = container_.begin() ;

    for( ; it != container_.end() ; ++it )
        if ( regex_match((*it).first, rx) ) res.push_back((*it).second) ;

	return res ;

}

void Dictionary::dump() const
{
    ContainerType::const_iterator it = container_.begin() ;

    for( ; it != container_.end() ; ++it )
        cout << (*it).first << ':' << (*it).second << endl ;
}

int Dictionary::count() const { return container_.size() ; }

int Dictionary::count(const regex &rx) const
{
	int cc = 0 ;

    ContainerType::const_iterator it = container_.begin() ;

    for( ; it != container_.end() ; ++it )
        if ( regex_match((*it).first, rx ) ) cc++ ;

	return cc ;
}

int Dictionary::count(const string &str) const
{
    int cc = 0 ;

    ContainerType::const_iterator it = container_.begin() ;

    for( ; it != container_.end() ; ++it )
        if ( (*it).first == str ) cc++ ;

    return cc ;
}

bool Dictionary::empty() const { return container_.empty() ; }
