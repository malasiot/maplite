#ifndef __XML_PULL_PARSER_HPP__
#define __XML_PULL_PARSER_HPP__

#include <istream>
#include <map>
#include <sstream>
#include <memory>

#include <util/dictionary.hpp>

#include <util/detail/read_ahead_stream_adapter.hpp>

// Simple and fast XML pull style parser that follows the Java XmlPullParse API

class XmlPullParser {
public:
    XmlPullParser(std::istream &strm, bool process_namespace = false) ;

    enum TokenType {
        CDSECT,
        COMMENT,
        DOCDECL,
        IGNORABLE_WHITESPACE,
        PROCESSING_INSTRUCTION,
        START_DOCUMENT,
        END_DOCUMENT,
        START_TAG,
        END_TAG,
        TEXT
    } ;

    const Dictionary &getAttributes() const { return attributes_ ; }

    std::string getAttribute(const std::string &name, const std::string &val = std::string()) const ;

    // Returns the type of the current event (START_TAG, END_TAG, TEXT, etc.)
    TokenType getEventType() const { return event_ ; }

    // Returns the (local) name of the current element when namespaces are enabled or raw name when namespaces are disabled.
    const std::string &getName() const { return local_name_ ;}

    // Returns the namespace URI of the current element.
    std::string getNamespace() const { return ns_ ; }

    // Return uri for the given prefix.
    std::string getNamespace(const std::string &prefix) const ;

    // Returns the prefix of the current element or empty if elemet has no prefix (is in defualt namespace).
    std::string getPrefix() const { return prefix_ ; }

    // Look up the value of a property.
    std::string getProperty(const std::string &prop_name) const ;

    // Read text content of the current event as String.
    const std::string &getText() const { return text_ ; }

    //  Returns true if the current event is START_TAG and the tag is degenerated
    bool isEmptyElementTag() const { return is_empty_element_tag_ ; }

    // Check if current TEXT event contains only whitespace characters.
    bool isWhitespace() const { return is_whitespace_ ; }

    // Get next parsing event - element content wil be coalesced and only one TEXT event must be returned for whole element content (comments and processing instructions will be ignored and emtity references must be expanded or exception mus be thrown if entity reerence can not be exapnded).
    TokenType next() ;

    // This method works similarly to next() but will expose additional event types (COMMENT, CDSECT, DOCDECL, ENTITY_REF, PROCESSING_INSTRUCTION, or IGNORABLE_WHITESPACE) if they are available in input.
    TokenType nextToken() ;

    // If the current event is text, the value of getText is returned and next() is called.
    std::string nextText() ;

    int getColumn() const { return cursor_.column() ; }
    int getLine() const { return cursor_.line() ; }
    int getDepth() const { return depth_ ; }

private:

    bool escapeString(std::string &value) ;
    bool parseBOM() ;

    bool parseXmlDecl() ;
    bool parseDocType() ;
    bool parseCData() ;
    bool parseComment() ;
    bool parsePI() ;
    bool parseName(std::string &name);
    bool parseAttributeValue(std::string &val);
    bool parseStartElement();
    bool parseEndElement() ;
    bool fatal(const std::string &msg) ;
    bool parseAttributeList(Dictionary &attrs);
    bool parseCharacters();
    bool nextEvent(std::string &text) ;
    void parseNameSpaceAttributes() ;
    std::string resolveUri(const std::string ns_prefix) const ;

private:

    ReadAheadStreamAdapter<8> cursor_ ;

    struct Element {
        Element(std::string local_name, std::string &prefix, std::string ns):
            name_(local_name), prefix_(prefix), ns_(ns) {}
        std::string name_, prefix_, ns_ ;
    };

    TokenType token_ = START_DOCUMENT, event_ ;
    Dictionary attributes_ ;
    std::string name_, prefix_, local_name_, text_, ns_ ;
    bool is_empty_element_tag_ = false, is_whitespace_ = true ;
    int depth_ = 0 ;
    std::vector<Dictionary> ns_stack_ ;
    std::vector<Element> element_stack_ ;
    bool process_ns_ = false ;

};

class XmlPullParserException: public std::runtime_error {
public:
    XmlPullParserException(const std::string &msg, int line, int col): msg_(msg), line_(line), col_(col), std::runtime_error(make_message(msg, line, col)) {}
private:
    static std::string make_message(const std::string &msg, int line, int col) ;
    std::string msg_ ;
    int line_,col_ ;
};

#endif
