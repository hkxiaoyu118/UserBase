#ifndef _BASE64_2_H									
#define _BASE64_2_H
#include <string>

using namespace std;

 std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
 std::string base64_decode(std::string const& encoded_string);

#endif
