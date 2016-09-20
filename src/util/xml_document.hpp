#ifndef __XML_DOCUMENT_H__
#define __XML_DOCUMENT_H__

#include <string>
#include <deque>
#include <vector>
#include <map>
#include <cassert>


using std::string ;

class XmlElement ;
class XmlDocument ;
class XmlText ;
class XmlReader ;

class XmlNode
{
public:

    enum Type { ElementNode, TextNode, CDATANode, InvalidNode } ;

    virtual ~XmlNode() ;

    XmlNode *parent() const {
        return parent_ ;
    }

    bool hasChildren() const { return front_ != 0 ; }

    XmlNode *firstChild() const {
        return front_ ;
    }

    XmlNode *lastChild() const {
        return back_ ;
    }

    const XmlElement *firstChildElement( const string &value = string() ) const ;

    const XmlNode *previousSibling() const	{
        return prev_ ;
    }

    const XmlElement *previousSiblingElement(const string &name = string() ) const ;

    const XmlNode *nextSibling() const	{
        return next_ ;
    }

    const XmlElement *nextSiblingElement(const string &name = string() ) const ;

    // this is only valid for elements otherwise should be empty
    string nodeName() const {
        return name_ ;
    }

    string nodeValue(bool preserveWhite = false ) const {
        return ( preserveWhite ) ? content_ : stripWhite(content_);
    }

    Type nodeType() const {
        return type_ ;
    }

    const XmlElement *toElement () const {
        assert(type_ == ElementNode ) ;
        return (const XmlElement *)this ;
    }
        ;
    const XmlText *toText() const ;

    bool isText() const {
        return type_ == TextNode ;
    }

    bool isElement () const {
        return type_ == ElementNode ;
    }

    bool isNull () const {
        return type_ == InvalidNode ;
    }

protected:

    friend class XmlDocument ;

    XmlNode ( XmlDocument *, Type t) ;

    void addChild(XmlNode *c) ;
    static std::string stripWhite(const std::string &src) ;

    XmlDocument * document_ ;
    XmlNode * parent_ ;
    XmlNode * prev_;
    XmlNode * next_;
    XmlNode * front_ ;
    XmlNode * back_ ;

    string name_, content_ ;
    Type type_ ;

};

class XmlAttribute {
public:
    XmlAttribute(const string &key, const string &val):
        key_(key), val_(val), prev_(0), next_(0) {}

    const string &key() const { return key_ ; }
    const string &value() const { return val_ ; }

private:
    friend class XmlAttributeIterator ;
    friend class XmlElement ;

    XmlAttribute *prev_ ;
    XmlAttribute *next_ ;
    string key_, val_ ;
};

class XmlAttributeIterator {
    friend class XmlElement ;

    public:

    XmlAttributeIterator(const XmlElement *ele) ;

    const XmlAttribute *next() ;

    private:

    const XmlElement *parent_ ;
    const XmlAttribute *cur_ ;
};

class XmlElement: public XmlNode
{
public:

    virtual ~XmlElement() ;

    string tag() const {
        return name_ ;
    }

    string attribute( const string & name, const string &def = string() ) const ;

    std::vector<const XmlElement *> elementsByTagName(const string & name ) const ;
    std::vector<const XmlElement *> elementsByTagNameNS ( const string & nsURI, const string & localName ) const ;

    bool hasAttribute ( const string & name ) const {
        return attributes_.count(name) ;
    }

    bool hasAttributeNS ( const string & nsURI, const string & localName ) const ;

    // find a namespace definition in this element or its ancestors and return its prefix
    string resolveNamespace(const std::string &nsUri) const ;

    // returns the text enclosed by this element and its descendants

    string text(bool preserveWhite = false) const ;

    const std::map<string, string> &attributes() ;

protected:

    friend class XmlDocument ;
    friend class XmlAttributeIterator ;

    XmlElement(XmlDocument *d): XmlNode(d, ElementNode), aback_(0), afront_(0) {}

    void addAttribute(XmlAttribute *attr) ;

    std::map<string, XmlAttribute *> attributes_ ;
    XmlAttribute *afront_, *aback_ ;

} ;

class XmlText: public XmlNode {


protected:

    friend class XmlDocument ;

    XmlText(XmlDocument *d): XmlNode(d, TextNode) {} ;
};

class XmlElementIterator
{
    friend class XmlElement ;

    public:

    XmlElementIterator(const XmlNode *parent, const string &tagName = string()) ;

    const XmlElement *next() ;

    private:

    const XmlNode *parent_ ;
    const XmlElement *cur_ ;
    string tag_ ;

};

class XmlDocument
{
public:
    XmlDocument() ;
    ~XmlDocument() ;

    bool load(std::istream &strm, std::string *err_msg = 0);
    bool load(const string &fileName, std::string *err_msg = 0) ;

    const XmlNode *root() const {
        return root_ ;
    }

protected:

    XmlNode *root_ ;

private:

    bool load(XmlReader &reader, std::string *err_msg);
};


#define XML_FOREACH_ATTRIBUTE(p, q) XmlAttributeIterator attrs(p) ; while ( const XmlAttribute *q = attrs.next()  )
#define XML_FOREACH_CHILD_ELEMENT(p, q) XmlElementIterator it(p) ; while ( const XmlElement *q = it.next() )
#define XML_FOREACH_CHILD_NODE(p, q) for ( const XmlNode *q = p->firstChild() ; q ; q = q->nextSibling() )


#endif
