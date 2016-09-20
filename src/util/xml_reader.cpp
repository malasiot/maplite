#include "xml_reader.hpp"

#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace std ;

#define BUF_SIZE 1024*16

void XmlReader::flush()
{
    cp_ = buf_ ;
    size_t rs ;

    if ( fdesc_ )
        rs = fread(buf_, 1, buf_size_, fdesc_);
    else
    {
        strm_->read(buf_, buf_size_) ;
        rs = strm_->gcount() ;
    }

    be_ = cp_ + rs ;
}

void XmlReader::advance()
{
    if ( cp_ == nullptr ) return ;
    if ( *cp_ == '\n' ) lineno_ ++ ;
    if ( ++cp_ < be_ )  ;
    else {
        flush() ;
        if ( cp_ == be_ ) cp_ = nullptr ;
    }
}

XmlReader::XmlReader(istream &strm): strm_(&strm), fdesc_(nullptr), current_node_type_(None), lineno_(1)
{
    buf_size_ = BUF_SIZE ;
    buf_ = new char [buf_size_] ;
    flush() ;

}

XmlReader::XmlReader(const string &fileName): strm_(nullptr), current_node_type_(None), lineno_(1)
{
    fdesc_ = fopen(fileName.c_str(), "rt") ;

    buf_size_ = BUF_SIZE ;
    buf_ = new char [buf_size_] ;
    flush() ;
}

XmlReader::~XmlReader()
{
    delete [] buf_ ;
    if ( fdesc_ ) fclose(fdesc_) ;
}

bool XmlReader::read()
{
    // if not end reached, parse the node
    if ( !cp_ ) return false ;

    do {
       parseCurrentNode();
    } while ( current_node_type_ == Ignored ) ;

    return ( cp_ && current_node_type_ != Invalid && current_node_type_ != None ) ;

}

// Reads the current xml node
void XmlReader::parseCurrentNode()
{
    if ( current_node_type_ == StartElement && is_empty_elem_ ) {
        current_node_type_ = EndElement ;
        return ;
    }

    current_node_type_ = None ;

    char* start = cp_;

    // more forward until '<' found
    while(cp_ && *cp_ != '<' )
        advance() ;

    if (!cp_) return;

    if (cp_ - start > 0)
    {
        // we found some text, store it
        if (setText(start, cp_)) return;
    }

    advance() ;

    if ( !cp_ ) return ;

    // based on current token, parse and report next element

    switch(*cp_)
    {
        case '/':
            parseClosingXMLElement();
            break;
        case '?':
            ignoreDefinition();
            break;
        case '!':
            if (!parseCDATA()) parseComment();
            break;
        default:
            parseOpeningXMLElement();
            break;
    }
}

//! returns true if a character is whitespace
static bool isWhiteSpaceCharacter(char c)
{
        return ( c == ' ' || c == '\t' || c == '\n' || c == '\r');
}


string replaceSpecialCharacters(const string &orig_str)
{

    string res = boost::replace_all_copy(orig_str, "&amp;", "&") ;
    boost::replace_all(res, "&lt;", "<") ;
    boost::replace_all(res, "&gt;", ">") ;
    boost::replace_all(res, "&quot;", "\"") ;
    boost::replace_all(res, "&apos;", "\'") ;
    return res ;
}

    //! sets the state that text was found. Returns true if set should be set
bool XmlReader::setText(char* start, char* end)
{
    // check if text is more than 2 characters, and if not, check if there is
    // only white space, so that this text won't be reported
    if (end - start < 3)
    {
        char* p = start;
        for(; p != end; ++p)
            if (!isWhiteSpaceCharacter(*p))
                break;

        if (p == end)
            return false;
    }

    // set current text to the parsed text, and replace xml special characters
    std::string s(start, (int)(end - start));
    current_node_name_ = replaceSpecialCharacters(s);

    // current XML node type is text
    current_node_type_ = Characters;

    return true;
}

void XmlReader::ignoreDefinition()
{
    current_node_type_ = Ignored;

    // move until end marked with '>' reached
    while(cp_ && *cp_ != L'>')
        advance() ;

    advance() ;
}

void XmlReader::parseComment()
{
    current_node_type_ = Ignored;
    advance() ;

    int count = 1;

    // move until end of comment reached
    while( count )
    {
        if (*cp_ == '>') --count;
        else if (*cp_ == '<') ++count;

        advance() ;
        if ( !cp_ ) break ;
    }
}


    //! parses an opening xml element and reads attributes
void XmlReader::parseOpeningXMLElement()
{
    current_node_type_ = StartElement;
    is_empty_elem_ = false;
    attrs_.clear();

    current_node_name_.clear() ;

    // find end of element
    while(cp_ && *cp_ != '>' && !isWhiteSpaceCharacter(*cp_)) {
        current_node_name_ += *cp_ ;
        advance() ;
    }

    // find Attributes
    while(*cp_ != '>')
    {
        if (isWhiteSpaceCharacter(*cp_))
            advance() ;
        else
        {
            if (*cp_ != '/')
            {
                // we've got an attribute

                // read the attribute names
                string name ;

                while(!isWhiteSpaceCharacter(*cp_) && *cp_ != '=') {
                    name += *cp_ ;
                    advance() ;
                }

                advance() ;

                // read the attribute value
                while( cp_ && (*cp_ != '\"') && (*cp_ != '\'') )  advance() ;

                if (!cp_) // malformatted xml file
                    return;

                const char attributeQuoteChar = *cp_;

                advance() ;

                string value ;

                while( cp_ && *cp_ != attributeQuoteChar )  {
                    value += *cp_ ;
                    advance() ;
                }

                if (!cp_) // malformatted xml file
                    return;

                advance();

                value = replaceSpecialCharacters(value);

                attrs_[name] = value ;
            }
            else
            {
                // tag is closed directly
                advance() ;
                is_empty_elem_ = true ;

                break;
            }
        }
    }


    int nlen = current_node_name_.size() ;

    if ( current_node_name_[nlen-1] == '/' )
    {
        current_node_name_ = current_node_name_.substr(0, nlen - 1 ) ;
        is_empty_elem_ = true ;
    }



    advance() ;

}

//! parses an closing xml tag
void XmlReader::parseClosingXMLElement()
{
    current_node_type_ = EndElement;

    attrs_.clear() ;
    advance() ;

    current_node_name_.clear() ;

    while(*cp_ != '>')  {
        current_node_name_ += *cp_ ;
        advance() ;
    }

    advance() ;
}


//! parses a possible CDATA section, returns false if begin was not a CDATA section
bool XmlReader::parseCDATA()
{
    advance() ;

    if ( !cp_ ) return false ;

    if ( *cp_ != '[') return false;

    current_node_name_.clear() ;
    current_node_type_ = Characters;

    // skip '<![CDATA['
    int count=0;
    while( *cp_ && count<8 )
    {
        advance() ;
        ++count;
    }

    if (!cp_)  return true;

        // find end of CDATA
    while( cp_ )
    {
        current_node_name_ += *cp_ ;
        advance() ;

        char la1, la2, la3 ;

        if (cp_ && *cp_ == ']' ) {
            la1 = *cp_ ;

            advance() ;

            if ( cp_ && *cp_  == ']' ) {
                 la2 = *cp_ ;
                 advance() ;

                 if ( cp_ && *cp_ == '>' )
                 {
                    advance() ;
                    break ;
                 }
                 else
                     current_node_name_ += *cp_ ;

            }
            else
                current_node_name_ += *cp_ ;

         }


    }

    return true;
}


std::string XmlReader::attribute(const std::string &name, const std::string &def_val) const
{
    map<string, string>::const_iterator it = attrs_.find(name) ;

    if ( it == attrs_.end() ) return def_val ;
    else return (*it).second ;
}

std::string XmlReader::elementText()
{
    string res ;
    //?
    if ( current_node_type_ != StartElement ) return res ;

    string nodeName = current_node_name_ ;

    while ( read() )
    {
        if ( current_node_type_ == EndElement) return res ;
        else if ( current_node_type_ == Characters ) {
            res += current_node_name_ ;
        }
        else return string() ;
    }
}

bool XmlReader::isWhiteSpace() const
{
    if ( current_node_type_ != Characters ) return false ;

    const char *p = current_node_name_.c_str() ;

    while ( *p )
    {
        if ( *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' ) return false ;
        ++p ;
    }

    return true ;
}


void XmlReader::skipElement()
{
    if ( current_node_type_ != StartElement ) return ;

    string nodeName = current_node_name_ ;

    while ( read() )
    {
        if ( current_node_type_ == EndElement && current_node_name_ == nodeName ) return  ;
    }
}

bool XmlReader::isStartElement(const string &name)
{
    return ( current_node_type_ == StartElement && ( name.empty() || current_node_name_ == name ) )   ;
}

bool XmlReader::isEndElement(const string &name)
{
    return ( current_node_type_ == EndElement && ( name.empty() || current_node_name_ == name ) )   ;
}


bool XmlReader::readNextStartElement(const std::string &name)
{
    do
    {
        if ( current_node_type_ == StartElement )
        {
            if ( name.empty() || name == current_node_name_ ) return true ;
        }
    } while ( read() ) ;

    return false ;

}
