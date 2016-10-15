#ifndef __VALUE_H__
#define __VALUE_H__

#include <string>
#include <vector>

// Lighweight variant type

class Value {

public:
    enum Type { Null, String, Number, Boolean } ;

    Value(): type_(Null) {}
    ~Value() {
        if ( type_ == String ) delete string_ ;
    }

    Value(Type type): type_(type) {}
    Value(const Value &other) {
        type_ = other.type_ ;
        if ( other.type_ == String )
            string_ = new std::string(*other.string_) ;
        else if ( other.type_ == Number )
            number_ = other.number_ ;
        else if ( other.type_ == Boolean )
            boolean_ = other.boolean_ ;
    }

    Value(int i): type_(Number), number_(i) {}
    Value(unsigned int i): type_(Number), number_(i) {}
    Value(float i): type_(Number), number_(i) {}
    Value(double i): type_(Number), number_(i) {}
    Value(bool i): type_(Boolean), boolean_(i) {}
    Value(const std::string &s): type_(String), string_(new std::string(s)) {}

    bool isNumber() const {
        return type_ == Number ;
    }

    bool isString() const {
        return type_ == String ;
    }

    bool isBoolean() const {
        return type_ == Boolean ;
    }

    bool isNull() const {
        return type_ == Null ;
    }

    bool toNumber(double &v) const ;
    bool toColor(unsigned int &color) ;
    bool toString(std::string &s) const ;
    bool toBoolean(bool &v) const ;
    bool toNumberList(std::vector<double> &l) const ;

private:

    Type type_ ;
    union {
        double number_ ;
        std::string *string_ ;
        bool boolean_ ;
    };

};




#endif
