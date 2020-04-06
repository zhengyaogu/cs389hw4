#include <iostream>
#include <cassert>
#include "cache.hh"
#define CATCH_CONFIG_MAIN 
#include "catch.hpp"
 

using byte_type = char;
using val_type = const byte_type*; 
using size_type = uint32_t; 
using key_type = std::string;


TEST_CASE("LRU", "[lru][extra]")
{
    Cache my_cache("127.0.0.1", "10002");    

    auto first_str = "first_val";
    val_type val_ptr = first_str;
    size_type first_size = 10;
    my_cache.set("first_key", val_ptr, first_size);

    auto second_str = "second_val";
    val_ptr = second_str;
    size_type second_size = 11;
    my_cache.set("second_key", val_ptr, second_size);

    REQUIRE(my_cache.space_used() == first_size + second_size);

    auto modified_first_str = "modified_first_val";
    val_ptr = modified_first_str;
    size_type modified_first_size = 19;
    my_cache.set("first_key", val_ptr, modified_first_size);

    REQUIRE(my_cache.space_used() == modified_first_size + second_size);

    auto third_str = "something_larger_than_30_characters";
    val_ptr = third_str;
    size_type third_size = 36;
    my_cache.set("third_key", val_ptr, third_size);

    auto fourth_str = "fourth_val";
    val_ptr = fourth_str;
    size_type fourth_size = 11;
    my_cache.set("fourth_key", val_ptr, fourth_size);

    //This should behave differently.
    REQUIRE(my_cache.space_used() == modified_first_size + fourth_size);

    auto another_modified_first_str = "1";
    val_ptr = another_modified_first_str;
    size_type another_modified_first_size = 2;
    my_cache.set("first_key", val_ptr, another_modified_first_size);
    REQUIRE(my_cache.space_used() == another_modified_first_size + fourth_size);

    auto fifth_str = "_the_fifth_val_";
    val_ptr = fifth_str;
    size_type fifth_size = 16;
    my_cache.set("fifth_key", val_ptr, fifth_size);
    REQUIRE(my_cache.space_used() == another_modified_first_size + fourth_size + fifth_size);

    // Now fourth should be the middle element in the evictor.
    size_type temp_size = 0;
    REQUIRE(my_cache.get("fourth_key", temp_size) != nullptr);
    REQUIRE(temp_size == fourth_size);
    // Now fourth should be the last element, another_modified_first_str 
    // should be the least recently used element.

    auto sixth_str = "12";
    val_ptr = sixth_str;
    size_type sixth_size = 3;
    my_cache.set("sixth_key", val_ptr, sixth_size);
    REQUIRE(my_cache.space_used() == fourth_size + fifth_size + sixth_size);


    std::cout << "\n";

}
