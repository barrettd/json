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

JsonValue::~JsonValue( void ) {
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

JsonObject::~JsonObject( void ) {
    std::map<std::string, JsonValue*>::iterator it  = m_dictionary.begin();
    std::map<std::string, JsonValue*>::iterator end = m_dictionary.end();
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

JsonArray::~JsonArray( void ) {
    std::vector<JsonValue*>::iterator it  = m_values.begin();
    std::vector<JsonValue*>::iterator end = m_values.end();
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

JsonString::~JsonString( void ) {
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

JsonLong::~JsonLong( void ) {
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

JsonDouble::~JsonDouble( void ) {
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

JsonBoolean::~JsonBoolean( void ) {
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

JsonNull::~JsonNull( void ) {
}
