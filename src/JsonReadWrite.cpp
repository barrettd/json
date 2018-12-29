//
//  JsonReadWrite.cpp
//  https://github.com/barrettd/json
//
//  Created by Barrett Davis on 12/26/18.
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

#include <iostream>
#include <fstream>
#include "JsonReadWrite.hpp"

static const char BACK_SLASH   = '\\';
static const char FORWARD_SLASH= '/';
static const char DOUBLE_QUOTE = '\"';
static const char OBJECT_START = '{';
static const char OBJECT_END   = '}';
static const char ARRAY_START  = '[';
static const char ARRAY_END    = ']';
static const char PLUS         = '+';
static const char MINUS        = '-';
static const char COLON        = ':';
static const char COMMA        = ',';
static const char PERIOD       = '.';

inline bool isSpace( const char ch ) {
    return ch <= 0x20;
}

inline bool isDigit( const char ch ) {
    return ch >= '0' && ch <= '9';
}

inline bool isAlpha( const char ch ) {
    return ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' );
}

inline bool isContainer( JsonValue *value ) {
    if( value == 0 ) {
        return false;
    }
    const JsonValue::JsonType valueType = value->getType();
    return valueType == JsonValue::JSON_OBJECT || valueType == JsonValue::JSON_ARRAY;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonReader
// ------------------------------------------------------------------------------------------------
JsonReader::JsonReader( void ):
m_state( READER_STATE_IDLE ),
m_substate( READER_SUBSTATE_IDLE ),
m_key(),
m_accumulator(),
m_collections(),
m_root( 0 ),
m_current( 0 ),
m_backslash( false ),
m_float( false ) {
}

JsonReader::~JsonReader( void ) {
    m_collections.clear();
    m_root = 0;
    m_current = 0;
}

JsonValue *JsonReader::clear( void ) {
    JsonValue *root = m_root;
    m_state = READER_STATE_IDLE;
    m_substate = READER_SUBSTATE_IDLE;
    m_key.clear();
    m_accumulator.clear();
    m_collections.clear();
    m_root    = 0;
    m_current = 0;
    m_backslash = false;
    m_float     = false;
    return root;
}

void JsonReader::setState( ReaderState newState, ReaderSubstate substate, const char ch ) {
    m_state = newState;
    m_substate = substate;
    m_accumulator.clear();
    m_backslash = false;
    m_float     = false;
    if( ch != 0 ) {
        m_accumulator = ch;
    }
}

void JsonReader::setState( ReaderState newState, const char ch ) {
    setState( newState, READER_SUBSTATE_IDLE, ch );
}

void JsonReader::pushCollection( JsonValue *value ) {
    if( value != 0 ) {
        m_collections.push_back( value );
        if( m_root == 0 ) {
            m_root = value;
        }
        m_current = value;
    }
}

JsonValue* JsonReader::popCollection( void ) {
    if( m_collections.size() < 1 ) {
        return 0;
    }
    m_collections.pop_back();
    if( m_collections.size() > 0 ) {
        m_current = m_collections.back();
    } else {
        m_current = 0;
    }
    return m_current;
}

void JsonReader::endCollection( void ) {
    popCollection();
    if( m_current == 0 ) {
        setState( READER_STATE_DONE );
        return;
    }
    if( m_current != 0 && m_current->getType() == JsonValue::JSON_OBJECT ) {
        setState( READER_STATE_IDLE, READER_SUBSTATE_NAME );
    } else {
        setState( READER_STATE_IDLE );
    }
}

void JsonReader::add( JsonObject *object ) {
    if( object == 0 ) {
        return;
    }
    if( m_current != 0 ) {
        m_current->add( object );
    }
    pushCollection( object );
    setState( READER_STATE_IDLE, READER_SUBSTATE_NAME );
    m_key.clear();
}

void JsonReader::add( JsonArray *array ) {
    if( array == 0 ) {
        return;
    }
    if( m_current != 0 ) {
        m_current->add( array );
    }
    pushCollection( array );
    setState( READER_STATE_IDLE );
    m_key.clear();
}

void JsonReader::add( JsonValue *value ) {
    if( value == 0 ) {
        return;
    }
    if( m_root == 0 ) {     // This is the case where we have just a simple value, not a member of an object nor an array.
        m_root = value;
        setState( READER_STATE_DONE );
        return;
    }
    ReaderSubstate nextSubstate = READER_SUBSTATE_IDLE;
    if( m_current != 0 ) {
        m_current->add( value );
        if( m_current->getType() == JsonArray::JSON_OBJECT ) {
            nextSubstate = READER_SUBSTATE_NAME;
        }
    } else {
        delete value;
    }
    setState( READER_STATE_IDLE, nextSubstate );
    m_key.clear();
}

bool JsonReader::processIdle( const char ch ) {
    if( ch == OBJECT_START ) {
        add( new JsonObject( m_key ));
    } else if( ch == OBJECT_END ) {
        endCollection();
    } else if( ch == ARRAY_START ) {
        add( new JsonArray( m_key ));
    } else if( ch == ARRAY_END ) {
        endCollection();
    } else if( ch == DOUBLE_QUOTE ) {
        if( m_substate == READER_SUBSTATE_NAME ) {
            setState( READER_STATE_NAME );
        } else {
            setState( READER_STATE_STRING );
        }
    } else if( isDigit( ch ) || ch == MINUS ) {
        setState( READER_STATE_NUMBER, ch );
    } else if( ch == PLUS ) {
        setState( READER_STATE_NUMBER );
    } else if( ch == 't' || ch == 'T' ) {
        setState( READER_STATE_TRUE, tolower( ch ));
    } else if( ch == 'f' || ch == 'F' ) {
        setState( READER_STATE_FALSE, tolower( ch ));
    } else if( ch == 'n' || ch == 'N' ) {
        setState( READER_STATE_NULL, tolower( ch ));
    }
    return true;
}

bool JsonReader::processBackSlash( char ch ) {
    char value = 0;
    switch( ch ) {
        case DOUBLE_QUOTE:  value = 0x22; break;    // \" represents the quotation mark character (U+0022).
        case BACK_SLASH:    value = 0x5C; break;    // \\ represents the reverse solidus character (U+005C).
        case FORWARD_SLASH: value = 0x2F; break;    // \/ represents the solidus character (U+002F).
        case 'b': value = 0x08; break;              // \b represents the backspace character (U+0008).
        case 'f': value = 0x0C; break;              // \f represents the form feed character (U+000C).
        case 'n': value = 0x0A; break;              // \n represents the line feed character (U+000A).
        case 'r': value = 0x0D; break;              // \r represents the carriage return character (U+000D).
        case 't': value = 0x09; break;              // \t represents the character tabulation character (U+0009).
    }
    if( value != 0 ) {
        m_accumulator += value;
    }
    m_backslash = false;
    return true;
}

bool JsonReader::processName( const char ch ) {
    if( m_backslash ) {
        return processBackSlash( ch );
    }
    if( ch == BACK_SLASH ) {
        m_backslash = true;
        return true;
    }
    if( ch == DOUBLE_QUOTE ) {
        m_key = m_accumulator;      // Preserve the name.
        setState( READER_STATE_IDLE, READER_SUBSTATE_VALUE );
        return true;
    }
    m_accumulator += ch;
    return true;
}

bool JsonReader::processString( const char ch ) {
    if( m_backslash ) {
        return processBackSlash( ch );
    }
    if( ch == BACK_SLASH ) {
        m_backslash = true;
        return true;
    }
    if( ch == DOUBLE_QUOTE ) {
        JsonString *jsonString = new JsonString( m_key, m_accumulator );
        add( jsonString );
        return true;
    }
    m_accumulator += ch;
    return true;
}

bool JsonReader::processNumber( const char ch ) {
    // 100 or -1.345e-32
    if( isDigit( ch )) {
        m_accumulator += ch;
    } else if( ch == PERIOD || ch == PLUS || ch == MINUS || ch == 'e' || ch == 'E' ) {
        m_accumulator += ch;
        m_float = true;
    } else {
        if( m_float ) {
            const double value = stod( m_accumulator );
            add( new JsonDouble( m_key, value ));
        } else {
            const long value = stol( m_accumulator );
            add( new JsonLong( m_key, value ));
        }
    }
    return true;
}

bool JsonReader::processTrue( const char ch ) {
    // true
    const char sanitized = tolower( ch );
    if( sanitized == 'r' || sanitized == 'u' || sanitized == 'e' ) {
        m_accumulator += ch;
    }
    if( m_accumulator == "true" ) {
        add( new JsonBoolean( m_key, true ));
    }
    return true;
}

bool JsonReader::processFalse( const char ch ) {
    // false
    const char sanitized = tolower( ch );
    if( sanitized == 'a' || sanitized == 'l' || sanitized == 's' || sanitized == 'e' ) {
        m_accumulator += ch;
    }
    if( m_accumulator == "false" ) {
        add( new JsonBoolean( m_key, false ));
    }
    return true;
}

bool JsonReader::processNull( const char ch ) {
    // null
    const char sanitized = tolower( ch );
    if( sanitized == 'u' || sanitized == 'l' ) {
        m_accumulator += ch;
    }
    if( m_accumulator == "null" ) {
        add( new JsonNull( m_key ));
    }
    return true;
}

bool JsonReader::process( const char ch ) {
    if( ch == 0 ) {
        return false;
    }
    switch( m_state ) {
        case READER_STATE_IDLE:   return processIdle(   ch );
        case READER_STATE_NAME:   return processName(   ch );
        case READER_STATE_STRING: return processString( ch );
        case READER_STATE_NUMBER: return processNumber( ch );
        case READER_STATE_TRUE:   return processTrue(   ch );
        case READER_STATE_FALSE:  return processFalse(  ch );
        case READER_STATE_NULL:   return processNull(   ch );
        case READER_STATE_DONE:   return false;
        case READER_STATE_ERROR:  return false;
    }
    return m_state != READER_STATE_DONE && m_state != READER_STATE_ERROR;
}

JsonValue* JsonReader::read( std::istream &stream ) {
    clear();
    if( stream.bad()) {
        return 0;
    }
    char ch = 0;
    bool ok = true;
    while( ok && stream.get(ch)) {
        ok = process( ch );
    }
    if( m_state == READER_STATE_ERROR ) {
        delete m_root;  // Parsing error.
        m_root = 0;     // We could throw an exception, but that is not so different from checking for null, is it?
    }
    return clear();
}

JsonValue* JsonReader::read( const std::string &path ) {
    if( path.empty()) {
        return 0;
    }
    std::ifstream stream( path );
    if( stream.bad()) {
        return 0;
    }
    JsonValue *value = read( stream );
    stream.close();
    return value;
}

// ------------------------------------------------------------------------------------------------
#pragma mark JsonWriter
// ------------------------------------------------------------------------------------------------
JsonWriter::JsonWriter( void ) {
}

JsonWriter::~JsonWriter( void ) {
}

bool JsonWriter::writeName( std::ostream &stream, const JsonValue *jsonValue, const bool inObject ) {
    if( stream.bad() || jsonValue == 0 ) {
        return false;
    }
    if( inObject ) {
        stream << DOUBLE_QUOTE << jsonValue->getName() << DOUBLE_QUOTE << COLON;
    }
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonObject *jsonObject, const bool inObject ) {
    if( !writeName( stream, jsonObject, inObject )) {
        return false;
    }
    stream << OBJECT_START;
    const std::map<std::string,JsonValue*> &values = jsonObject->getValues();
          std::map<std::string,JsonValue*>::const_iterator it  = values.begin();
    const std::map<std::string,JsonValue*>::const_iterator end = values.end();
    bool first = true;
    while( it != end ) {
        JsonValue *value = it->second;
        if( value != 0 ) {
            if( first ) {
                first = false;
            } else {
                stream << COMMA;
            }
            write( stream, value, true );
        }
        it++;
    }
    stream << OBJECT_END;
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonArray *jsonArray, const bool inObject ) {
    if( !writeName( stream, jsonArray, inObject )) {
        return false;
    }
    stream << ARRAY_START;
    const std::vector<JsonValue*> &values = jsonArray->getValues();
          std::vector<JsonValue*>::const_iterator it  = values.begin();
    const std::vector<JsonValue*>::const_iterator end = values.end();
    bool first = true;
    while( it != end ) {
        JsonValue *value = *it++;
        if( value != 0 ) {
            if( first ) {
                first = false;
            } else {
                stream << COMMA;
            }
            write( stream, value, false );
        }
    }
    stream << ARRAY_END;
    return true;
}

bool JsonWriter::writeChar( std::ostream &stream, const char ch ) {
    if( ch == 0 ) {
        return false;
    }
    switch( ch & 0x00FF ) {
        case DOUBLE_QUOTE:  stream << "\\\""; break;    // \" represents the quotation mark character (U+0022).
        case BACK_SLASH:    stream << "\\\\"; break;    // \\ represents the reverse solidus character (U+005C).
        case FORWARD_SLASH: stream << "\\/";  break;    // \/ represents the solidus character (U+002F).
        case 0x08:          stream << "\\b";  break;    // \b represents the backspace character (U+0008).
        case 0x09:          stream << "\\t";  break;    // \t represents the character tabulation character (U+0009).
        case 0x0A:          stream << "\\n";  break;    // \n represents the line feed character (U+000A).
        case 0x0C:          stream << "\\f";  break;    // \f represents the form feed character (U+000C).
        case 0x0D:          stream << "\\r";  break;    // \r represents the carriage return character (U+000D).
        default: stream << ch;
    }
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonString *jsonString, const bool inObject ) {
    if( !writeName( stream, jsonString, inObject )) {
        return false;
    }
    stream << DOUBLE_QUOTE;
    const std::string &value = jsonString->getValue();
    std::string::const_iterator it  = value.begin();
    std::string::const_iterator end = value.end();
    while( it != end ) {
        writeChar( stream, *it++ );
    }
    stream << DOUBLE_QUOTE;
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonLong *jsonLong, const bool inObject ) {
    if( !writeName( stream, jsonLong, inObject )) {
        return false;
    }
    stream << jsonLong->getValue();
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonDouble *jsonDouble, const bool inObject ) {
    if( !writeName( stream, jsonDouble, inObject )) {
        return false;
    }
    stream << jsonDouble->getValue();
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonBoolean *jsonBoolean, const bool inObject ) {
    if( !writeName( stream, jsonBoolean, inObject )) {
        return false;
    }
    if( jsonBoolean->getValue()) {
        stream << "true";
    } else {
        stream << "false";
    }
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonNull *jsonNull, const bool inObject ) {
    if( !writeName( stream, jsonNull, inObject )) {
        return false;
    }
    stream << "null";
    return true;
}

bool JsonWriter::write( std::ostream &stream, const JsonValue *jsonValue, const bool inObject ) {
    if( stream.bad() || jsonValue == 0 ) {
        return false;
    }
    bool result = false;
    switch( jsonValue->getType()) {
        case JsonValue::JSON_OBJECT: {
            const JsonObject *jsonObject = dynamic_cast<const JsonObject*>(jsonValue);
            return write( stream, jsonObject, inObject );
        };
        case JsonValue::JSON_ARRAY: {
            const JsonArray *jsonArray = dynamic_cast<const JsonArray*>(jsonValue);
            return write( stream, jsonArray, inObject );
        }
        case JsonValue::JSON_STRING: {
            const JsonString *jsonString = dynamic_cast<const JsonString*>(jsonValue);
            return write( stream, jsonString, inObject );
        }
        case JsonValue::JSON_LONG: {
            const JsonLong *jsonLong = dynamic_cast<const JsonLong*>(jsonValue);
            return write( stream, jsonLong, inObject );
        }
        case JsonValue::JSON_DOUBLE: {
            const JsonDouble *jsonDouble = dynamic_cast<const JsonDouble*>(jsonValue);
            return write( stream, jsonDouble, inObject );
        }
        case JsonValue::JSON_BOOLEAN: {
            const JsonBoolean *jsonBoolean = dynamic_cast<const JsonBoolean*>(jsonValue);
            return write( stream, jsonBoolean, inObject );
        }
        case JsonValue::JSON_NULL: {
            const JsonNull *jsonNull = dynamic_cast<const JsonNull*>(jsonValue);
            return write( stream, jsonNull, inObject );
        }
    }
    return result;
}

bool JsonWriter::write( std::ostream &stream, const JsonValue *value ) {
    return write( stream, value, false );
}

bool JsonWriter::write( const std::string &path, const JsonValue *value ) {
    if( path.empty() || value == 0 ) {
        return false;
    }
    std::ofstream stream( path );
    if( stream.bad()) {
        return false;
    }
    const bool result = write( stream, value );
    stream.close();
    return result;
}
