#include <iostream>  
#include <cassert>  

static const char* base64_chars[2] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/",

    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-_"};

// 000001001 01001111
std::string base64_encode(unsigned char const* str_to_encode, size_t len, bool url) {
    const char* base64_char = base64_chars[url];
    std::string res;
    size_t total_long = (len + 2)/3 * 4; // 天才
    unsigned char tail = url ? '.' : '=';
    res.reserve(total_long);
    size_t pos = 0;
    while (pos < len) {
        // pick the first 6 item
        res.push_back(base64_char[(str_to_encode[pos+0] & 0xfc) >> 2]);
        if (pos + 1 < len) {
            // if more than one byte left, first take six from it: 
            res.push_back(base64_char[((str_to_encode[pos+0] & 0x03) << 4) + ((str_to_encode[pos+1] & 0xf0) >> 4)]);
            // wtf ?!!!
            // std::cout<< ((str_to_encode[pos+0] & 0x03) << 4) + ((str_to_encode[pos+1] & 0xf0) >> 4) << std::endl;
            // std::cout<< ((str_to_encode[pos+0] & 0x03) << 4 + (str_to_encode[pos+1] & 0xf0) >> 4) << std::endl;
            if (pos + 2 < len) {
                // three bytes
                res.push_back(base64_char[((str_to_encode[pos+1] & 0x0f) << 2) + ((str_to_encode[pos+2] & 0xc0) >> 6)]);
                res.push_back(base64_char[(str_to_encode[pos+2] & 0x3f)]);
            } else {
                // pos + 2 > len, mean two byte left, we already take 2 * 6, total 2 * 8, remain 4
                res.push_back(base64_char[(str_to_encode[pos+1] & 0x0f) << 2]);
                res.push_back(tail);    
            }
        } else {
            // if pos + 1 > len, mean only one byte left, take the remain two factor:
            res.push_back(base64_char[(str_to_encode[pos] & 0x03) << 4]);
            res.push_back(tail);
            res.push_back(tail);
        }
        pos += 3;
    }
    return res;
}
void test_base64_encode() {  
    // 测试用例 1：简单字符串  
    const char* plain_text = "Man";  
    unsigned char const* input = reinterpret_cast<unsigned char const*>(plain_text);  
    std::string encoded = base64_encode(input, 3, 0);  
    std::cout << "Test 1: 'Man' encoded: " << encoded << std::endl;  
    assert(encoded == "TWFu");  

    // 测试用例 2：两字节输入  
    const char* plain_text2 = "Ma";  
    unsigned char const* input2 = reinterpret_cast<unsigned char const*>(plain_text2);  
    std::string encoded2 = base64_encode(input2, 2, 0);  
    std::cout << "Test 2: 'Ma' encoded: " << encoded2 << std::endl;  
    assert(encoded2 == "TWE=");  

    // 测试用例 3：一字节输入  
    const char* plain_text3 = "M";  
    unsigned char const* input3 = reinterpret_cast<unsigned char const*>(plain_text3);  
    std::string encoded3 = base64_encode(input3, 1, 0);  
    std::cout << "Test 3: 'M' encoded: " << encoded3 << std::endl;  
    assert(encoded3 == "TQ==");  

    // 测试用例 4：特殊字符测试  
    const char* plain_text4 = "+/";  
    unsigned char const* input4 = reinterpret_cast<unsigned char const*>(plain_text4);  
    std::string encoded4 = base64_encode(input4, 2, 0);  // 在标准模式下  
    std::cout << "Test 4: '+/' encoded (std): " << encoded4 << std::endl;  
    assert(encoded4 == "Ky8="); // 正确预期的输出  

    std::cout << "\nAll tests passed!\n";  
}    

int main() {  
    test_base64_encode();  
    return 0;  
}    
// '0' ASCII is : 48/00110000
// 0xfc/11111100
// 001100 00