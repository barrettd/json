//
//  UnitTestJson.cpp
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
#include <sstream>
#include "Json.hpp"
#include "UnitTest.hpp"
#include "UnitTestJson.hpp"
#include "JsonReadWrite.hpp"

static const std::string TEST_DIRECTORY( "/Users/barrett/tmp/" );   // This will change with user and platform.

// This JSON example from:
// https://developer.mozilla.org/en-US/docs/Learn/JavaScript/Objects/JSON
static const char *json_content_00 = {
    "{"
        "\"squadName\": \"Super hero squad\","
        "\"homeTown\": \"Metro City\","
        "\"formed\": 2016,"
        "\"secretBase\": \"Super tower\","
        "\"active\": true,"
        "\"price\": -123.456,"
        "\"members\": ["
                    "{"
                    "\"name\": \"Molecule Man\","
                    "\"age\": 29,"
                    "\"secretIdentity\": \"Dan Jukes\","
                    "\"powers\": ["
                               "\"Radiation resistance\","
                               "\"Turning tiny\","
                               "\"Radiation blast\""
                               "]"
                    "},{"
                    "\"name\": \"Madame Uppercut\","
                    "\"age\": 39,"
                    "\"secretIdentity\": \"Jane Wilson\","
                    "\"powers\": ["
                               "\"Million tonne punch\","
                               "\"Damage resistance\","
                               "\"Superhuman reflexes\""
                               "]"
                    "},{"
                    "\"name\": \"Eternal Flame\","
                    "\"age\": 1000000,"
                    "\"secretIdentity\": \"Unknown\","
                    "\"powers\": ["
                               "\"Immortality\","
                               "\"Heat Immunity\","
                               "\"Inferno\","
                               "\"Teleportation\","
                               "\"Interdimensional travel\""
                               "]"
                    "}"
                    "]"
    "}"
};

static JsonValue *readJson( const char *json ) {
    if( json == 0 || *json == 0 ) {
        return 0;
    }
    std::istringstream stream( json );
    if( stream.bad()) {
        return 0;
    }
    JsonReader reader;
    return reader.read( stream );
}

static bool unit_test_json_simple_object( void ) {
    JsonValue *root = readJson( "{ \"nn\" : \"vv\" }" );
    bool result = CHECK( root != 0 );
    if( result ) {
        result = CHECK( root->getType() == JsonValue::JSON_OBJECT );
    }
    if( !result ) {
        delete root;
        return false;
    }
    JsonObject *object = (JsonObject*) root;
    JsonValue *value = object->getValue( "nn" );
    result = CHECK( value != 0 );
    if( result ) {
        result = CHECK( value->getType() == JsonValue::JSON_STRING );
    }
    if( !result ) {
        delete object;
        return false;
    }
    JsonString *jsonString = (JsonString*) value;
    const std::string &stringValue = jsonString->getValue();
    result = CHECK( stringValue == "vv" );

    delete object;
    return result;
}

static bool check_member( JsonObject *member, const std::string &name ) {
    if( !CHECK( member != 0 )) {
        return false;
    }
    if( !CHECK( !name.empty())) {
        return false;
    }
    JsonString *jsonString = dynamic_cast<JsonString*>( member->getValue( "name" ));
    if( !CHECK( jsonString != 0 )) {
        return false;
    }
    return CHECK( jsonString->getValue() == name );
}

static bool unit_test_check_array_of_objects( void ) {
    JsonValue *root = readJson( "[{\"name\":\"Lucy\"},{\"name\":\"Emma\"},{\"name\":\"Finn\"},{\"name\":\"Ari\"}]" );
    bool result = CHECK( root != 0 );
    if( result ) {
        result = CHECK( root->getType() == JsonValue::JSON_ARRAY );
    }
    if( !result ) {
        delete root;
        return false;
    }
    JsonArray *jsonArray = dynamic_cast<JsonArray*>( root );
    result = CHECK( jsonArray != 0 );
    if( !result ) {
        delete root;
        return false;
    }
    result = CHECK( jsonArray->getCount() == 4 );
    int index = 0;
    if( result ) {
        JsonObject *member = dynamic_cast<JsonObject*>( jsonArray->getValue( index++ ));
        result = check_member( member, "Lucy" );
    }
    if( result ) {
        JsonObject *member = dynamic_cast<JsonObject*>( jsonArray->getValue( index++ ));
        result = check_member( member, "Emma" );
    }
    if( result ) {
        JsonObject *member = dynamic_cast<JsonObject*>( jsonArray->getValue( index++ ));
        result = check_member( member, "Finn" );
    }
    if( result ) {
        JsonObject *member = dynamic_cast<JsonObject*>( jsonArray->getValue( index++ ));
        result = check_member( member, "Ari" );
    }
    delete root;
    return result;
}

static bool unit_test_json_squad_object( void ) {
    JsonValue *root = readJson( json_content_00 );
    bool result = CHECK( root != 0 );
    if( result ) {
        result = CHECK( root->getType() == JsonValue::JSON_OBJECT );
    }
    if( !result ) {
        delete root;
        return false;
    }
    JsonObject *object = (JsonObject*) root;
    const int count = object->getCount();
    result = CHECK( count == 7 );
    if( result ) {
        JsonDouble *jsonDouble = dynamic_cast<JsonDouble*>( object->getValue( "price" ));
        result = CHECK( jsonDouble != 0 );
        if( result ) {
            result = CHECK( jsonDouble->getValue() == -123.456 );
        }
    }
    if( result ) {
        JsonArray *jsonArray = dynamic_cast<JsonArray*>( object->getValue( "members" ));
        result = CHECK( jsonArray != 0 );
        if( result ) {
            result = CHECK( jsonArray->getCount() == 3 );
        }
        if( result ) {
            JsonObject *member = dynamic_cast<JsonObject*>( jsonArray->getValue( 0 ));
            result = check_member( member, "Molecule Man" );
            if( result ) {
                JsonLong *age = dynamic_cast<JsonLong*>( member->getValue( "age" ));
                result = CHECK( age != 0 );
            }
            if( result ) {
                JsonString *identity = dynamic_cast<JsonString*>( member->getValue( "secretIdentity" ));
                result = CHECK( identity != 0 );
                if( result ) {
                    result = CHECK( identity->getValue() == "Dan Jukes" );
                }
            }
            if( result ) {
                JsonArray *array = dynamic_cast<JsonArray*>( member->getValue("powers"));
                result = CHECK( array != 0 );
                if( result ) {
                    result = CHECK( array->getCount() == 3 );
                }
                if( result ) {
                    int index = 0;
                    JsonString *value0 = dynamic_cast<JsonString*>( array->getValue( index++ ));
                    JsonString *value1 = dynamic_cast<JsonString*>( array->getValue( index++ ));
                    JsonString *value2 = dynamic_cast<JsonString*>( array->getValue( index++ ));
                    result = CHECK( value0 != 0 && value1 != 0 && value2 != 0 );
                    if( result ) {
                        result = CHECK( value0->getValue() == "Radiation resistance" );
                    }
                    if( result ) {
                        result = CHECK( value1->getValue() == "Turning tiny" );
                    }
                    if( result ) {
                        result = CHECK( value2->getValue() == "Radiation blast" );
                    }
                }
            }
        }
        if( result ) {
            JsonObject *member = dynamic_cast<JsonObject*>( jsonArray->getValue( 1 ));
            result = check_member( member, "Madame Uppercut" );
        }
        if( result ) {
            JsonObject *member = dynamic_cast<JsonObject*>( jsonArray->getValue( 2 ));
            result = check_member( member, "Eternal Flame" );
        }

    }
    delete object;
    return result;
}

static bool unit_test_json_string_00( void ) {
    JsonValue *object = readJson( "\"An interesting string!\"" );
    bool result = CHECK( object != 0 );
    if( result ) {
        result = CHECK( object->getType() == JsonValue::JSON_STRING );
        JsonString *jsonString = (JsonString*) object;
        const std::string &name  = jsonString->getName();
        const std::string &value = jsonString->getValue();
        CHECK( name.empty());
        CHECK( value == "An interesting string!" );
        delete object;
    }
    return result;
}

static bool unit_test_json_string_01( void ) {
    JsonValue *object = readJson( "\"Hi There\\n\"" );
    bool result = CHECK( object != 0 );
    if( result ) {
        result = CHECK( object->getType() == JsonValue::JSON_STRING );
        JsonString *jsonString = (JsonString*) object;
        const std::string &name  = jsonString->getName();
        const std::string &value = jsonString->getValue();
        CHECK( name.empty());
        CHECK( value == "Hi There\n" );
        delete object;
    }
    return result;
}

static bool unit_test_write_file( const std::string &filePath ) {
    JsonObject *root = new JsonObject( "root" );
    root->add( new JsonString(  "name", "Gru" ));
    root->add( new JsonLong(   "steps",  1000 ));
    root->add( new JsonDouble( "price",  1.00 ));

    JsonWriter writer;
    const bool result = CHECK( writer.write( filePath, root ));
    delete root;
    return result;
}

static bool unit_test_read_file( const std::string &filePath ) {
    JsonReader reader;
    JsonValue *root = reader.read( filePath );
    if( !CHECK( root != 0 )) {
        return false;
    }
    JsonObject *object = dynamic_cast<JsonObject*>(root);
    if( !CHECK( object != 0 )) {
        delete root;
        return false;
    }
    JsonString  *name = dynamic_cast<JsonString*>(object->getValue("name"));
    JsonLong   *steps = dynamic_cast<JsonLong*>(  object->getValue("steps"));
    JsonDouble *price = dynamic_cast<JsonDouble*>(object->getValue("price"));
    if( !CHECK(  name != 0 ) ||
        !CHECK( steps != 0 ) ||
        !CHECK( price != 0 )) {
        delete root;
        return false;
    }
    if( !CHECK(name->getValue() == "Gru" ) ||
        !CHECK(steps->getValue() == 1000 ) ||
        !CHECK(price->getValue() == 1.00 )) {
        delete root;
        return false;
    }
    delete root;
    return true;
}

static bool unit_test_file( const std::string &filePath ) {
    bool result = true;
    if( !CHECK(unit_test_write_file(filePath))) {
        result = false;
    }
    if( !CHECK(unit_test_read_file(filePath))) {
        result = false;
    }
    return result;
}

bool unit_test_json( void ) {
    bool result = true;
    if( !CHECK(unit_test_json_simple_object())) {
        result = false;
    }
    if( !CHECK(unit_test_check_array_of_objects())) {
        result = false;
    }
    if( !CHECK(unit_test_json_squad_object())) {
        result = false;
    }
    if( !CHECK(unit_test_json_string_00())) {
        result = false;
    }
    if( !CHECK(unit_test_json_string_01())) {
        result = false;
    }
    std::string filePath = TEST_DIRECTORY + "test.json";
    if( !CHECK(unit_test_file(filePath))) {
        result = false;
    }
    return result;
}
