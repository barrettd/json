//
//  Json.cpp
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
#include "Json.hpp"

// ------------------------------------------------------------------------------------------------
#pragma mark static functions
// ------------------------------------------------------------------------------------------------
static JsonValue* duplicate( JsonValue *ptr ) {
    if( ptr == 0 ) {
        return 0;
    }
    JsonValue *newValue = 0;
    switch( ptr->getType()) {
        case JsonValue::JSON_OBJECT: {
            JsonObject *jsonObject = dynamic_cast<JsonObject*>(ptr);
            if( jsonObject != 0 ) {
                newValue = new JsonObject( *jsonObject );
            }
            break;
        }
        case JsonValue::JSON_ARRAY: {
            JsonArray *jsonArray = dynamic_cast<JsonArray*>(ptr);
            if( jsonArray != 0 ) {
                newValue = new JsonArray( *jsonArray );
            }
            break;
        }
        case JsonValue::JSON_STRING: {
            JsonString *value = dynamic_cast<JsonString*>(ptr);
            if( value != 0 ) {
                newValue = new JsonString( *value );
            }
            break;
        }
        case JsonValue::JSON_LONG: {
            JsonLong *value = dynamic_cast<JsonLong*>(ptr);
            if( value != 0 ) {
                newValue = new JsonLong( *value );
            }
            break;
        }
        case JsonValue::JSON_DOUBLE: {
            JsonDouble *value = dynamic_cast<JsonDouble*>(ptr);
            if( value != 0 ) {
                newValue = new JsonDouble( *value );
            }
            break;
        }
        case JsonValue::JSON_BOOLEAN: {
            JsonBoolean *value = dynamic_cast<JsonBoolean*>(ptr);
            if( value != 0 ) {
                newValue = new JsonBoolean( *value );
            }
            break;
        }
        case JsonValue::JSON_NULL: {
            JsonNull *value = dynamic_cast<JsonNull*>(ptr);
            if( value != 0 ) {
                newValue = new JsonNull( *value );
            }
            break;
        }
    }
    return newValue;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonValue
// ------------------------------------------------------------------------------------------------
JsonValue::JsonValue( JsonType jtype ):
m_type( jtype ),
m_name() {
}

JsonValue::JsonValue( JsonType jtype, const std::string &name ):
m_type( jtype ),
m_name( name ) {
}

JsonValue::JsonValue( const JsonValue &other ):
m_type( other.m_type ),
m_name( other.m_name ) {
}

JsonValue::~JsonValue( void ) {
}

JsonValue& JsonValue::operator= ( const JsonValue &other ) {
    m_type = other.m_type;
    m_name = other.m_name;
    return *this;
}

void JsonValue::setType( JsonType jtype ) {
    m_type = jtype;
}

JsonValue::JsonType JsonValue::getType( void ) const {
    return m_type;
}

void JsonValue::setName( const std::string &name ) {
    m_name = name;
}

const std::string& JsonValue::getName( void ) const {
    return m_name;
}

bool JsonValue::hasName( void ) const {
    return !m_name.empty();
}

void JsonValue::add( JsonValue *value ) {
    delete value;   // This is not a container.
    return;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonObject
// ------------------------------------------------------------------------------------------------
JsonObject::JsonObject( void ):
JsonValue(JSON_OBJECT),
m_dictionary() {
}

JsonObject::JsonObject( const std::string &name ):
JsonValue(JSON_OBJECT, name),
m_dictionary() {
}

JsonObject::JsonObject( const JsonObject &other ):
JsonValue( other ) {
    copy( other );
}

JsonObject::~JsonObject( void ) {
    clear();
}

JsonObject& JsonObject::copy( const JsonObject &other ) {
    m_dictionary.clear();
          std::map<std::string, JsonValue*>::const_iterator it  = other.m_dictionary.begin();
    const std::map<std::string, JsonValue*>::const_iterator end = other.m_dictionary.end();
    while( it != end ) {
        JsonValue *value = duplicate( it->second );
        add( value );
        it++;
    }
    return *this;
}

JsonObject& JsonObject::operator= ( const JsonObject &other ) {
    m_name = other.m_name;
    return copy( other );
}

void JsonObject::clear( void ) {
        std::map<std::string, JsonValue*>::const_iterator it  = m_dictionary.begin();
  const std::map<std::string, JsonValue*>::const_iterator end = m_dictionary.end();
  while( it != end ) {
      JsonValue *value = it->second;
      delete value;
      it++;
  }
  m_dictionary.clear();
}

const std::map<std::string,JsonValue*>& JsonObject::getValues( void ) const {
    return m_dictionary;
}

int JsonObject::getCount( void ) const {
    return (int) m_dictionary.size();
}

const JsonValue* JsonObject::getValue( const std::string &name ) const {
    if( name.empty() || m_dictionary.empty()) {
        return 0;
    }
    std::map<std::string,JsonValue*>::const_iterator it = m_dictionary.find(name);
    if( it == m_dictionary.end()) {
        return 0;
    }
    return m_dictionary.at(name);
}

JsonValue* JsonObject::getValue( const std::string &name ) {
    if( name.empty() || m_dictionary.empty()) {
        return 0;
    }
    std::map<std::string,JsonValue*>::const_iterator it = m_dictionary.find(name);
    if( it == m_dictionary.end()) {
        return 0;
    }
    return m_dictionary[name];
}

void JsonObject::add( JsonValue *value ) {
    if( value == 0 ) {
        return;
    }
    const std::string &name = value->getName();
    if( !name.empty()) {
        std::map<std::string,JsonValue*>::const_iterator it = m_dictionary.find(name);
        if( it != m_dictionary.end()) {
            JsonValue *previous = it->second;
            delete previous;
            m_dictionary.erase(it);
        }
        m_dictionary[name] = value;
    } else {
        delete value;
    }
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonArray
// ------------------------------------------------------------------------------------------------
JsonArray::JsonArray( void ):
JsonValue(JSON_ARRAY) {
}

JsonArray::JsonArray( const std::string &name ):
JsonValue(JSON_ARRAY, name),
m_values() {
}

JsonArray::JsonArray( const JsonArray &other ):
JsonValue( other ) {
    copy( other );
}

JsonArray::~JsonArray( void ) {
    clear();
}

JsonArray& JsonArray::copy( const JsonArray &other ) {
    m_values.clear();
          std::vector<JsonValue*>::const_iterator it  = other.m_values.begin();
    const std::vector<JsonValue*>::const_iterator end = other.m_values.end();
    while( it != end ) {
        JsonValue *value = duplicate( *it++ );
        add( value );
    }
    return *this;
}

JsonArray& JsonArray::operator= ( const JsonArray &other ) {
    m_name = other.m_name;
    return copy( other );
}

void JsonArray::clear( void ) {
        std::vector<JsonValue*>::const_iterator it  = m_values.begin();
  const std::vector<JsonValue*>::const_iterator end = m_values.end();
  while( it != end ) {
      JsonValue *value = *it++;
      delete value;
  }
  m_values.clear();
}

const std::vector<JsonValue*>& JsonArray::getValues( void ) const {
    return m_values;
}

int JsonArray::getCount( void ) const {
    return (int) m_values.size();
}

const JsonValue* JsonArray::getValue( const int index ) const {
    if( index < 0 || index >= m_values.size()) {
        return 0;
    }
    return m_values[index];
}

JsonValue* JsonArray::getValue( const int index ) {
    if( index < 0 || index >= m_values.size()) {
        return 0;
    }
    return m_values[index];
}

bool JsonArray::setValue( const int index, JsonValue *value ) {
    const int count = (int) m_values.size();
    if( index < 0 || index > count || value == 0 ) {
        delete value;
        return false;
    }
    if( index == count ) {
        m_values.push_back( value );
    } else {
        JsonValue *previous = m_values[index];
        m_values[index] = value;
        delete previous;
    }
    return true;
}

void JsonArray::add( JsonValue *value ) {
    if( value != 0 ) {
        m_values.push_back( value );
    }
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonString
// ------------------------------------------------------------------------------------------------
JsonString::JsonString( void ):
JsonValue(JSON_STRING),
m_value() {
}

JsonString::JsonString( const std::string &name ):
JsonValue(JSON_STRING, name),
m_value() {
}

JsonString::JsonString( const std::string &name, const std::string &value ):
JsonValue(JSON_STRING, name),
m_value( value ) {
}

JsonString::JsonString( const JsonString &other ):
JsonValue( other ),
m_value( other.m_value ) {
}

JsonString::~JsonString( void ) {
}

JsonString& JsonString::operator= ( const JsonString &other ) {
    m_name  = other.m_name;
    m_value = other.m_value;
    return *this;
}

void JsonString::setValue( const std::string &value ) {
    m_value = value;
}

const std::string& JsonString::getValue( void ) const {
    return m_value;
}

std::string& JsonString::getValue( void ) {
    return m_value;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonLong
// ------------------------------------------------------------------------------------------------
JsonLong::JsonLong( void ):
JsonValue(JSON_LONG) {
}

JsonLong::JsonLong( const std::string &name ):
JsonValue(JSON_LONG, name) {
}

JsonLong::JsonLong( const std::string &name, long value ):
JsonValue(JSON_LONG, name),
m_value( value ) {
}

JsonLong::JsonLong( const JsonLong &other ):
JsonValue( other ),
m_value( other.m_value ) {
}

JsonLong::~JsonLong( void ) {
}

JsonLong& JsonLong::operator= ( const JsonLong &other ) {
    m_name  = other.m_name;
    m_value = other.m_value;
    return *this;
}

void JsonLong::setValue( const long value ) {
    m_value = value;
}
long JsonLong::getValue( void ) const {
    return m_value;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonDouble
// ------------------------------------------------------------------------------------------------
JsonDouble::JsonDouble( void ):
JsonValue(JSON_DOUBLE) {
}

JsonDouble::JsonDouble( const std::string &name ):
JsonValue(JSON_DOUBLE, name) {
}

JsonDouble::JsonDouble( const std::string &name, const double value ):
JsonValue(JSON_DOUBLE, name),
m_value( value ) {
}

JsonDouble::JsonDouble( const JsonDouble &other ):
JsonValue( other ),
m_value( other.m_value ) {
}

JsonDouble::~JsonDouble( void ) {
}

JsonDouble& JsonDouble::operator= ( const JsonDouble &other ) {
    m_name  = other.m_name;
    m_value = other.m_value;
    return *this;
}

void JsonDouble::setValue( const double value ) {
    m_value = value;
}
double JsonDouble::getValue( void ) const {
    return m_value;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonBoolean
// ------------------------------------------------------------------------------------------------
JsonBoolean::JsonBoolean( void ):
JsonValue(JSON_BOOLEAN) {
}

JsonBoolean::JsonBoolean( const std::string &name ):
JsonValue(JSON_BOOLEAN, name) {
}

JsonBoolean::JsonBoolean( const std::string &name, const bool value ):
JsonValue(JSON_BOOLEAN, name),
m_value( value ) {
}

JsonBoolean::JsonBoolean( const JsonBoolean &other ):
JsonValue( other ),
m_value( other.m_value ) {
}

JsonBoolean::~JsonBoolean( void ) {
}

JsonBoolean& JsonBoolean::operator= ( const JsonBoolean &other ) {
    m_name  = other.m_name;
    m_value = other.m_value;
    return *this;
}

void JsonBoolean::setValue( const bool value ) {
    m_value = value;
}
bool JsonBoolean::getValue( void ) const {
    return m_value;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonNull
// ------------------------------------------------------------------------------------------------
JsonNull::JsonNull( void ):
JsonValue(JSON_NULL) {
}

JsonNull::JsonNull( const std::string &name ):
JsonValue(JSON_NULL, name) {
}

JsonNull::JsonNull( const JsonNull &other ):
JsonValue( other ) {
}

JsonNull::~JsonNull( void ) {
}

JsonNull& JsonNull::operator= ( const JsonNull &other ) {
    m_name = other.m_name;
    return *this;
}

