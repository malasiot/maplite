#include "xml_document.hpp"
#include "xml_reader.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using namespace std ;

string XmlNode::stripWhite(const string &src)
{
    string res = boost::regex_replace(src, boost::regex("[\n\r]+"), "") ;
    boost::algorithm::replace_all(res, "\t", " ") ;
    boost::algorithm::trim(res) ;
    res = boost::regex_replace(res, boost::regex("[ ]+"), " ");

    return res ;
}

XmlNode::XmlNode(XmlDocument *doc, Type t): document_(doc), type_(t), parent_(0), front_(0), back_(0), prev_(0), next_(0) {
}

void XmlNode::addChild(XmlNode *node)
{
    node->parent_ = this ;

    if (back_ == 0 )
        front_ = back_ = node ;
    else
    {
        back_->next_ = node ;
        node->prev_ = back_ ;
        back_ = node ;
    }
}


XmlDocument::XmlDocument(): root_(0) {
}


XmlDocument::~XmlDocument()
{
    if ( root_)
        delete root_ ;
}

bool XmlDocument::load(istream &str, string *err_msg) {

    XmlReader reader(str) ;

    return load(reader, err_msg) ;
}

bool XmlDocument::load(const string &fileName, string *err_msg) {

    XmlReader reader(fileName) ;

    return load(reader, err_msg) ;
}

bool XmlDocument::load(XmlReader &reader, string *err_msg)
{
    deque<XmlNode *> nodeStack ;

    while ( reader.read() )
    {
        if ( reader.nodeType() == XmlReader::StartElement )
        {
            XmlElement *node = new XmlElement(this) ;

            node->name_ = reader.nodeName() ;

            const map<string, string> &rattr = reader.attributes() ;

            std::map<string, string>::const_iterator it = rattr.begin() ;

            for( ; it != rattr.end() ; ++it )
            {
                XmlAttribute *attr = new XmlAttribute((*it).first, (*it).second) ;
                node->addAttribute(attr) ;

            }

            if ( nodeStack.empty() )
                root_ = node ;
            else {

                XmlNode *parentNode = nodeStack.back() ;
                parentNode->addChild(node) ;
            }

            nodeStack.push_back(node) ;
        }
        else if ( reader.nodeType() == XmlReader::EndElement )
        {
            if ( nodeStack.empty() ) return false ;
            if ( nodeStack.back()->name_ != reader.nodeName() ) {
                if ( err_msg ) {
                    stringstream strm ;
                    strm << "Found invalid end tag </" << reader.nodeName() << "> at line " << reader.currentLine() << " for start tag <" << nodeStack.back()->name_ << ">" ;
                    *err_msg = strm.str() ;
                }
                return false ;
            }
            nodeStack.pop_back() ;
        }
        else if ( reader.nodeType() == XmlReader::Characters )
        {
            if ( reader.isWhiteSpace() ) continue ;

            XmlText *node = new XmlText(this) ;

            if ( nodeStack.empty() ) {
                return false ;
            }
            else {

                XmlElement *elem = (XmlElement *)nodeStack.back() ;

                node->content_ = reader.nodeName() ;

                elem->addChild(node) ;
            }
        }
    }

   if ( !root_ )
   {
       if ( err_msg ) {
           *err_msg = "Malformed XML file" ;
       }
       return false ;
   }
   else return true ;
}



XmlNode::~XmlNode()
{
    const XmlNode *q = front_ ;

    while (q)
    {
        const XmlNode *next = q->next_ ;
        delete q ;
        q = next ;
    }

}

const XmlElement *XmlNode::firstChildElement(const string &value) const
{
    const XmlNode *p = front_ ;

    while (p)
    {
        if ( p->type_ == ElementNode )  {
            if ( value.empty() || value == p->name_ )
                return (const XmlElement *)p ;
        }

        p = p->next_ ;
    }
}

const XmlElement *XmlNode::previousSiblingElement(const string &name) const
{
    XmlNode *p = prev_ ;

    while (p)
    {
        if ( p->type_ == ElementNode )  {
            if ( name.empty() || name == p->name_ )
                return (const XmlElement *)p ;
        }

        p = p->prev_ ;
    }

}

const XmlElement *XmlNode::nextSiblingElement(const string &name) const
{
    XmlNode *p = next_ ;

    while (p)
    {
        if ( p->type_ == ElementNode )  {
            if ( name.empty() || name == p->name_ )
                return (const XmlElement *)p ;
        }

        p = p->next_ ;
    }
}

string XmlElement::attribute(const string &name, const string &def) const
{
    map<string, XmlAttribute *>::const_iterator it = attributes_.find(name) ;

    if ( it == attributes_.end() ) return def ;
    else return it->second->value() ;
}


class XmlTreeWalker
{
    friend class XmlNode;

private:
    int depth_;

protected:
    // Get current traversal depth
    int depth() const { return depth_ ; }

public:

    XmlTreeWalker();
    virtual ~XmlTreeWalker();

    virtual bool for_each(const XmlNode *node) = 0;

};

void traverseTreeForElements(const XmlNode *parentNode, const string &tagName, vector<const XmlElement *> &elems)
{
    const XmlElement *children = parentNode->firstChildElement() ;

    while ( children )
    {
        if ( children->tag() == tagName )
            elems.push_back(children) ;

        traverseTreeForElements(children, tagName, elems) ;
        children = children->nextSiblingElement() ;
    }
}


std::vector<const XmlElement *> XmlElement::elementsByTagName(const string &name) const
{
    vector<const XmlElement *> res ;
    traverseTreeForElements(this, name, res);
    return res ;
}

void traverseTreeForText(const XmlNode *parentNode, string &txt)
{
    const XmlNode *children = parentNode->firstChild() ;

    while ( children )
    {
        if ( children->isText() )
            txt += ' ' + children->nodeValue() ;
        else if ( children->isElement() )
            traverseTreeForText(children, txt) ;

        children = children->nextSibling() ;
    }
}

string XmlElement::text(bool prw) const
{
    string res ;
    traverseTreeForText(this, res) ;

    return ( prw ) ? res : stripWhite(res) ;
}

XmlElement::~XmlElement()
{
    map<string, XmlAttribute *>::const_iterator it = attributes_.begin() ;

    while ( it != attributes_.end() )
    {
        delete (*it).second ;
        ++it ;
    }

}

void XmlElement::addAttribute(XmlAttribute *attr)
{
    if ( aback_ == 0 )
        afront_ = aback_ = attr ;
    else
    {
        aback_->next_ = attr ;
        attr->prev_ = aback_ ;
        aback_ = attr ;
    }

    attributes_.insert(make_pair(attr->key(), attr)) ;
}

string XmlElement::resolveNamespace(const std::string &nsUri) const
{
    string nsPrefix ;

    XML_FOREACH_ATTRIBUTE(this, attr)
    {
        const string &key = attr->key(), &val = attr->value() ;

        if ( boost::starts_with(key, "xmlns") )
        {
            int pos ;
            if ( ( pos = key.find_first_of(':') ) == string::npos ) continue ;

            string prefix = key.substr(pos+1) ;

            if ( val == nsUri ) {
                nsPrefix = prefix + ':' ;
                break ;
            }
        }
    }

    //We recursively visit ancestors until a namespace definition is found

    if ( nsPrefix.empty() && parent_ ) return parent_->toElement()->resolveNamespace(nsUri) ;
    else return nsPrefix ;
}


XmlElementIterator::XmlElementIterator(const XmlNode *parent, const string &tagName): parent_(parent), cur_(0), tag_(tagName)
{
    assert(parent_) ;
}

const XmlElement *XmlElementIterator::next() {

    if ( cur_ == 0 ) cur_ = parent_->firstChildElement(tag_) ;
    else cur_ = cur_->nextSiblingElement(tag_) ;
    return cur_ ;
}


XmlAttributeIterator::XmlAttributeIterator(const XmlElement *parent): parent_(parent), cur_(0)
{
    assert(parent_) ;
}

const XmlAttribute *XmlAttributeIterator::next() {

    if ( cur_ == 0 ) cur_ = parent_->afront_ ;
    else cur_ = cur_->next_ ;
    return cur_ ;
}
