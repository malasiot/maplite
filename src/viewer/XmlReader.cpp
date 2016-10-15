#include "XmlReader.h"

#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace std ;

#define BUF_SIZE 1024*16

void XmlReader::flush()
{
    cp = buf ;
    size_t rs ;

    if ( fdesc_ )
        rs = fread(buf, 1, buf_size, fdesc_);
    else
    {
        strm_->read(buf, buf_size) ;
        rs = strm_->gcount() ;
    }

    be = cp + rs ;
}

void XmlReader::advance()
{
    if ( cp == 0 ) return ;
    if ( *cp == '\n' ) lineno ++ ;
    if ( ++cp < be )  ;
    else {
        flush() ;
        if ( cp == be ) cp = 0 ;
    }
}

XmlReader::XmlReader(istream &strm): strm_(&strm), fdesc_(0), currentNodeType(None), lineno(1)
{
    buf_size = BUF_SIZE ;
    buf = new char [buf_size] ;
    flush() ;

}

XmlReader::XmlReader(const string &fileName): strm_(0), currentNodeType(None), lineno(1)
{
    fdesc_ = fopen(fileName.c_str(), "rt") ;

    buf_size = BUF_SIZE ;
    buf = new char [buf_size] ;
    flush() ;
}

XmlReader::~XmlReader()
{
    delete [] buf ;
    if ( fdesc_ ) fclose(fdesc_) ;
}

bool XmlReader::read()
{
    // if not end reached, parse the node
    if ( !cp ) return false ;

    do {
       parseCurrentNode();
    } while ( currentNodeType == Ignored ) ;

    return ( cp && currentNodeType != Invalid && currentNodeType != None ) ;

}

// Reads the current xml node
void XmlReader::parseCurrentNode()
{
    if ( currentNodeType == StartElement && is_empty_elem ) {
        currentNodeType = EndElement ;
        return ;
    }

    currentNodeType = None ;

    char* start = cp;

    // more forward until '<' found
    while(cp && *cp != '<' )
        advance() ;

    if (!cp) return;

    if (cp - start > 0)
    {
        // we found some text, store it
        if (setText(start, cp)) return;
    }

    advance() ;

    if ( !cp ) return ;

    // based on current token, parse and report next element

    switch(*cp)
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
    currentNodeName = replaceSpecialCharacters(s);

    // current XML node type is text
    currentNodeType = Characters;

    return true;
}

void XmlReader::ignoreDefinition()
{
    currentNodeType = Ignored;

    // move until end marked with '>' reached
    while(cp && *cp != L'>')
        advance() ;

    advance() ;
}

void XmlReader::parseComment()
{
    currentNodeType = Ignored;
    advance() ;

    int count = 1;

    // move until end of comment reached
    while( count )
    {
        if (*cp == '>') --count;
        else if (*cp == '<') ++count;

        advance() ;
        if ( !cp ) break ;
    }
}


    //! parses an opening xml element and reads attributes
void XmlReader::parseOpeningXMLElement()
{
    currentNodeType = StartElement;
    is_empty_elem = false;
    attrs.clear();

    currentNodeName.clear() ;

    // find end of element
    while(cp && *cp != '>' && !isWhiteSpaceCharacter(*cp)) {
        currentNodeName += *cp ;
        advance() ;
    }

    // find Attributes
    while(*cp != '>')
    {
        if (isWhiteSpaceCharacter(*cp))
            advance() ;
        else
        {
            if (*cp != '/')
            {
                // we've got an attribute

                // read the attribute names
                string name_ ;

                while(!isWhiteSpaceCharacter(*cp) && *cp != '=') {
                    name_ += *cp ;
                    advance() ;
                }

                advance() ;

                // read the attribute value
                while( cp && (*cp != '\"') && (*cp != '\'') )  advance() ;

                if (!cp) // malformatted xml file
                    return;

                const char attributeQuoteChar = *cp;

                advance() ;

                string value_ ;

                while( *cp != attributeQuoteChar && cp )  {
                    value_ += *cp ;
                    advance() ;
                }

                if (!cp) // malformatted xml file
                    return;

                advance();

                value_ = replaceSpecialCharacters(value_);

                attrs[name_] = value_ ;
            }
            else
            {
                // tag is closed directly
                advance() ;
                is_empty_elem = true ;

                break;
            }
        }
    }


    int nlen = currentNodeName.size() ;

    if ( currentNodeName[nlen-1] == '/' )
    {
        currentNodeName = currentNodeName.substr(0, nlen - 1 ) ;
        is_empty_elem = true ;
    }



    advance() ;

}

//! parses an closing xml tag
void XmlReader::parseClosingXMLElement()
{
    currentNodeType = EndElement;

    attrs.clear() ;
    advance() ;

    currentNodeName.clear() ;

    while(*cp != '>')  {
        currentNodeName += *cp ;
        advance() ;
    }

    advance() ;
}


//! parses a possible CDATA section, returns false if begin was not a CDATA section
bool XmlReader::parseCDATA()
{


    advance() ;

    if ( !cp ) return false ;

    if ( *cp != '[') return false;

    currentNodeName.clear() ;
    currentNodeType = Characters;

    // skip '<![CDATA['
    int count=0;
    while( *cp && count<8 )
    {
        advance() ;
        ++count;
    }

    if (!cp)  return true;

        // find end of CDATA
    while( cp )
    {
        currentNodeName += *cp ;
        advance() ;

        char la1, la2, la3 ;

        if (cp && *cp == ']' ) {
            la1 = *cp ;

            advance() ;

            if ( cp && *cp  == ']' ) {
                 la2 = *cp ;
                 advance() ;

                 if ( cp && *cp == '>' )
                 {
                    advance() ;
                    break ;
                 }
                 else
                     currentNodeName += *cp ;

            }
            else
                currentNodeName += *cp ;

         }


    }

    return true;
}


std::string XmlReader::attribute(const std::string &name, const std::string &def_val) const
{
    map<string, string>::const_iterator it = attrs.find(name) ;

    if ( it == attrs.end() ) return def_val ;
    else return (*it).second ;
}

std::string XmlReader::elementText()
{
    string res ;
    //?
    if ( currentNodeType != StartElement ) return res ;

    string nodeName = currentNodeName ;

    while ( read() )
    {
        if ( currentNodeType == EndElement) return res ;
        else if ( currentNodeType == Characters ) {
            res += currentNodeName ;
        }
        else return string() ;
    }
}

bool XmlReader::isWhiteSpace() const
{
    if ( currentNodeType != Characters ) return false ;

    const char *p = currentNodeName.c_str() ;

    while ( *p )
    {
        if ( *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' ) return false ;
        ++p ;
    }

    return true ;
}


void XmlReader::skipElement()
{
    if ( currentNodeType != StartElement ) return ;

    string nodeName = currentNodeName ;

    while ( read() )
    {
        if ( currentNodeType == EndElement && currentNodeName == nodeName ) return  ;
    }
}

bool XmlReader::isStartElement(const string &name)
{
    return ( currentNodeType == StartElement && ( name.empty() || currentNodeName == name ) )   ;
}

bool XmlReader::isEndElement(const string &name)
{
    return ( currentNodeType == EndElement && ( name.empty() || currentNodeName == name ) )   ;
}


bool XmlReader::readNextStartElement(const std::string &name)
{
    do
    {
        if ( currentNodeType == StartElement )
        {
            if ( name.empty() || name == currentNodeName ) return true ;
        }
    } while ( read() ) ;

    return false ;

}
