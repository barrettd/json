//
//  Json.hpp
//  https://github.com/barrettd/json
//
//  Created by Barrett Davis on 12/24/18.
//  Copyright (c) 2018 Tree Frog Software. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//  http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf

#ifndef Json_hpp
#define Json_hpp

#include <iostream>
#include <map>
#include <string>
#include <vector>

class JsonValue {   // A JSON value can be an object, array, number, string, true, false, or null.
public:
    enum JsonType {
        JSON_OBJECT,
        JSON_ARRAY,
        JSON_STRING,
        JSON_LONG,
        JSON_DOUBLE,
        JSON_BOOLEAN,
        JSON_NULL
    };

protected:
    JsonType    m_type;
    std::string m_name;     // This may be empty.

public:
    JsonValue( JsonType jtype );
    JsonValue( JsonType jtype, const std::string &name );
    JsonValue( const JsonValue &other );
    virtual ~JsonValue( void );

    JsonValue& operator= ( const JsonValue &other );

    void setType( JsonType jtype );
    JsonType getType( void ) const;

    void setName( const std::string &name );
    const std::string &getName( void ) const;
    bool hasName( void ) const;

    virtual void add( JsonValue *value );
};

class JsonObject: public JsonValue {
protected:
    std::map<std::string,JsonValue*> m_dictionary;
    
protected:
    JsonObject& copy( const JsonObject &other );

public:
    JsonObject( void );
    JsonObject( const std::string &name );
    JsonObject( const JsonObject &other );
    ~JsonObject( void );

    JsonObject& operator= ( const JsonObject &other );

    void clear( void );

    const std::map<std::string,JsonValue*> &getValues( void ) const;

    int getCount( void ) const;
    const JsonValue *getValue( const std::string &name ) const;
          JsonValue *getValue( const std::string &name );
    virtual void add( JsonValue *value );
};

class JsonArray: public JsonValue {
protected:
    std::vector<JsonValue*> m_values;

protected:
    JsonArray& copy( const JsonArray &other );

public:
    JsonArray( void );
    JsonArray( const std::string &name );
    JsonArray( const JsonArray &other );
    ~JsonArray( void );

    JsonArray& operator= ( const JsonArray &other );

    void clear( void );

    const std::vector<JsonValue*> &getValues( void ) const;

    int getCount( void ) const;
    const JsonValue *getValue( const int index ) const;
          JsonValue *getValue( const int index );
    bool setValue( const int index, JsonValue *value );
    virtual void add( JsonValue *value );
};

class JsonString: public JsonValue {
protected:
    std::string m_value;

public:
    JsonString( void );
    JsonString( const std::string &name );
    JsonString( const std::string &name, const std::string &value );
    JsonString( const JsonString &other );
    ~JsonString( void );

    JsonString& operator= ( const JsonString &other );

    void setValue( const std::string &value );
    const std::string &getValue( void ) const;
          std::string &getValue( void );
};

class JsonLong: public JsonValue {
protected:
    long m_value;
public:
    JsonLong( void );
    JsonLong( const std::string &name );
    JsonLong( const std::string &name, const long value );
    JsonLong( const JsonLong &other );
    ~JsonLong( void );

    JsonLong& operator= ( const JsonLong &other );

    void setValue( const long value );
    long getValue( void ) const;
};

class JsonDouble: public JsonValue {
protected:
    double m_value;
public:
    JsonDouble( void );
    JsonDouble( const std::string &name );
    JsonDouble( const std::string &name, const double value );
    JsonDouble( const JsonDouble &other );
    ~JsonDouble( void );

    JsonDouble& operator= ( const JsonDouble &other );

    void setValue( const double value );
    double getValue( void ) const;
};

class JsonBoolean: public JsonValue {
    bool m_value;
public:
    JsonBoolean( void );
    JsonBoolean( const std::string &name );
    JsonBoolean( const std::string &name, const bool value );
    JsonBoolean( const JsonBoolean &other );
    ~JsonBoolean( void );

    JsonBoolean& operator= ( const JsonBoolean &other );

    void setValue( const bool value );
    bool getValue( void ) const;
};

class JsonNull: public JsonValue {
public:
    JsonNull( void );
    JsonNull( const std::string &name );
    JsonNull( const JsonNull &other );
    ~JsonNull( void );
    
    JsonNull& operator= ( const JsonNull &other );
};


#endif /* Json_hpp */
