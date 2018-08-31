#ifndef __XML_PULL_PARSER_HPP__
#define __XML_PULL_PARSER_HPP__

#include <istream>
#include <map>
#include <sstream>
#include <memory>

class XmlStreamWrapper ;
// Simple and fast XML pull style reader that follows the Java XmlPullParse API

class XmlPullParser {
public:
    XmlPullParser(std::istream &strm) ;
    ~XmlPullParser() ;

    enum TokenType
    {
        CDSECT,
        COMMENT,
        DOCDECL,
        IGNORABLE_WHITSPACE,
        START_DOCUMENT,
        END_DOCUMENT,
        START_TAG,
        END_TAG,
        TEX
    } ;

    //  Returns the number of attributes on the current element; -1 if the current event is not START_TAG
    int	getAttributeCount() const ;

    // Returns the local name of the specified attribute if namespaces are enabled or just attribute name if namespaces are disabled.
    std::string getAttributeName(int idx) const ;

    // Returns the namespace URI of the specified attribute number index (starts from 0).
    std::string getAttributeNamespace(int idx) const ;

    // Returns the prefix of the specified attribute. Returns empty string if the element has no prefix.
    std::string getAttributePrefix(int idx) const ;

    // Returns the attributes value identified by namespace URI and namespace localName.
    std::string getAttributeValue(const std::string &ns, const std::string &name) const ;

    // Current column: numbering starts from 0 (returned when parser is in START_DOCUMENT state!)
    int getColumnNumber() const ;

    // Returns the current depth of the element.
    int	getDepth() const ;

    // Returns the type of the current event (START_TAG, END_TAG, TEXT, etc.)
    TokenType getEventType() const ;

    // Current line number: numebering starts from 1.
    int	getLineNumber() const ;

    // Returns the (local) name of the current element when namespaces are enabled or raw name when namespaces are disabled.
    std::string getName() const ;

    // Returns the namespace URI of the current element.
    std::string getNamespace() const ;

    // Return uri for the given prefix.
    std::string getNamespace(const std::string &prefix) const ;

    // Return position in stack of first namespace slot for element at passed depth.
    int	getNamespaceCount(int depth) ;

    // Return namespace prefixes for position pos in namespace stack
    std::string getNamespacePrefix(int pos) const ;

    // Return namespace URIs for position pos in namespace stack If pos is out of range it throw exception.
    std::string getNamespaceUri(int pos) const ;

    // Returns the prefix of the current element or empty if elemet has no prefix (is in defualt namespace).
    std::string getPrefix() const ;

    // Look up the value of a property.
    std::string getProperty(const std::string &prop_name) const ;

    // Read text content of the current event as String.
    std::string getText() const ;

   //char[] 	getTextCharacters(int[] holderForStartAndLength)

    //  Returns true if the current event is START_TAG and the tag is degenerated
   bool	isEmptyElementTag() const ;

   // Check if current TEXT event contains only whitespace characters.
   bool isWhitespace() const ;

   // Get next parsing event - element content wil be coalesced and only one TEXT event must be returned for whole element content (comments and processing instructions will be ignored and emtity references must be expanded or exception mus be thrown if entity reerence can not be exapnded).
   TokenType next() ;

   // This method works similarly to next() but will expose additional event types (COMMENT, CDSECT, DOCDECL, ENTITY_REF, PROCESSING_INSTRUCTION, or IGNORABLE_WHITESPACE) if they are available in input.
   TokenType nextToken() ;

   // If the current event is text, the value of getText is returned and next() is called.
   std::string readText() ;

   // test if the current event is of the given type and if the namespace and name do match.
   void require(TokenType type, const std::string &ns, const std::string &name) ;


private:

    bool parseCDATA();
    void parseClosingXMLElement();
    void parseOpeningXMLElement();
    void parseComment();
    void ignoreDefinition();
    bool setText(char *start, char *end);
    void parseCurrentNode();

    void flush();
    void advance();

private:

    std::unique_ptr<XmlStreamWrapper> stream_ ;
    char *cp_, *be_ ;
    std::string current_node_name_ ;
    TokenType current_node_type_ ;
    char * buf_ ;
    size_t buf_size_ ;
    std::istream *strm_ ;
    FILE *fdesc_ ;
    std::map<std::string, std::string> attrs_ ;
    bool is_empty_elem_ ;
    int lineno_, colno_ ;
};









#endif
