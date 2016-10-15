#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <boost/regex.hpp>
#include <map>

// A class of key/value pairs of strings. 

class Dictionary
{
	public:

	Dictionary() ;
	~Dictionary() {}

	// add a key/val pair 
    void add(const std::string &key, const std::string &val) ;
	// remove entry with given key if exists
    void remove(const std::string &key) ;
    void removeSome(const boost::regex &rx) ;

	// remove all items
	void clear() ;
		
	// get a the value of the given key if exists. Otherwise return defaultValue

    std::string get(const std::string &key, const std::string &defaultVal = std::string()) const ;
    std::string operator[] ( const std::string & key ) const ;
    std::string &operator[] ( const std::string & key ) ;
	
	// check the existance of a key

    bool contains(const std::string &key) const;

	// get a list of the keys in the dictionary

    std::vector<std::string> keys() const ;
    std::vector<std::string> keys(const boost::regex &rx) const ;

	// get values 

    std::vector<std::string> values() const ;
    std::vector<std::string> values(const boost::regex &key) const ;

	// number of entries

	int count() const ;
    int count(const boost::regex &kx) const ;
    int count(const char *) const ;

	bool empty() const ;

    void dump() const ;
	
	private:

	friend class DictionaryIterator ;

    typedef std::map<std::string, std::string> ContainerType ;
	ContainerType container ;
 
} ;

class DictionaryIterator
{
	public:

	DictionaryIterator(const Dictionary &dic): dict(dic), it(dic.container.begin()) {}
	DictionaryIterator(const DictionaryIterator &other): dict(other.dict), it(other.it) {}

	bool operator == (const DictionaryIterator &other) const { return it == other.it ; }
	bool operator != (const DictionaryIterator &other) const { return it != other.it ; }

	operator int () const { return it != dict.container.end() ; }
	
	DictionaryIterator & operator++() { ++it ; return *this ; }
	DictionaryIterator operator++(int) { DictionaryIterator tmp(*this) ; ++it; return tmp ; }

    std::string key() const { return (*it).first ; }
    std::string value() const { return (*it).second ; }

	private:

	const Dictionary &dict ;
	Dictionary::ContainerType::const_iterator it ;
} ;



#endif
