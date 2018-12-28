//
//  JsonReadWrite.hpp
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

#ifndef JsonReadWrite_hpp
#define JsonReadWrite_hpp

#include "Json.hpp"

class JsonReader {
    enum ReaderState {
        READER_STATE_ERROR = -2,
        READER_STATE_DONE = -1,
        READER_STATE_IDLE = 0,
        READER_STATE_NAME,
        READER_STATE_STRING,
        READER_STATE_NUMBER,
        READER_STATE_TRUE,
        READER_STATE_FALSE,
        READER_STATE_NULL,
    };
    enum ReaderSubstate {
        READER_SUBSTATE_IDLE,
        READER_SUBSTATE_NAME,
        READER_SUBSTATE_VALUE,
    };
protected:
    ReaderState    m_state;
    ReaderSubstate m_substate;
    std::string    m_key;
    std::string    m_accumulator;
    std::vector<JsonValue*> m_collections;
    JsonValue  *m_root;
    JsonValue  *m_current;
    bool        m_backslash;
    bool        m_float;

protected:
    bool processBackSlash( const char ch );
    bool processIdle(   const char ch );
    bool processName(   const char ch );
    bool processString( const char ch );
    bool processNumber( const char ch );
    bool processTrue(   const char ch );
    bool processFalse(  const char ch );
    bool processNull(   const char ch );
    bool process(       const char ch );

    JsonValue *clear( void );
    void setState( ReaderState newState, const char ch = 0 );
    void setState( ReaderState newState, ReaderSubstate substate, const char ch = 0 );

    void pushCollection( JsonValue *value );
    JsonValue *popCollection( void );
    void endCollection( void );

    void add( JsonObject *object );
    void add( JsonArray  *array );
    void add( JsonValue  *value );

public:
    JsonReader( void );
    ~JsonReader( void );

    JsonValue *read( std::istream &stream );       // Remember to call delete object to dispose of the returned object.
    JsonValue *read( const std::string &path );
};

class JsonWriter {
protected:
    bool writeName( std::ostream &stream, const JsonValue *jsonValue, const bool inObject );
    bool writeChar( std::ostream &stream, const char ch );
    bool write( std::ostream &stream, const JsonObject  *jsonObject,  const bool inObject );
    bool write( std::ostream &stream, const JsonArray   *jsonArray,   const bool inObject );
    bool write( std::ostream &stream, const JsonString  *jsonString,  const bool inObject );
    bool write( std::ostream &stream, const JsonLong    *jsonLong,    const bool inObject );
    bool write( std::ostream &stream, const JsonDouble  *jsonDouble,  const bool inObject );
    bool write( std::ostream &stream, const JsonBoolean *jsonBoolean, const bool inObject );
    bool write( std::ostream &stream, const JsonNull    *jsonNull,    const bool inObject );
    bool write( std::ostream &stream, const JsonValue   *jsonValue,   const bool inObject );

public:
    JsonWriter( void );
    ~JsonWriter( void );

    bool write(    std::ostream &stream, const JsonValue *value );
    bool write( const std::string &path, const JsonValue *value );

};

#endif /* JsonReadWrite_hpp */
