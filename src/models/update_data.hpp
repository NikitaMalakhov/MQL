#ifndef __UPDATE_DATA_HPP

#define __UPDATE_DATA_HPP
#include <string>
struct UpdateData
{
    std::string before_equal; // expression before = 
    std::string after_equal; // expression after = 
    // example Age = Age - 10*2
    // left_field = "Age"
    // right_field = "Age - 10*2"
};

#endif