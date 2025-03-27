#include <iostream>  
#include <cassert>  
#include <fstream>
#include <cstdint>
#include <bitset>
#include <vector>
#include <cstring>

static const char* base64_chars[2] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/",

    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-_"};

char convert(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+' || c == '-') return 62;
    if (c == '/' || c == '_') return 63;
    return 'A' - 'A'; // this will not happen
}

char* convert_group(char* group) {
    // 我这个方法good
    uint32_t value = 0; // exactly four bytes interger, from kk
    for (int i = 0; i < 4; i++) {
        group[i] = convert(group[i]);
        // this print way : std::bitset<8>, print binary directly
        // std::cout << std::bitset<8>((unsigned)group[i]) << std::endl;
        // 0011 1111 3f
        value += (group[i] & 0x3f) << (6 * i);
    }
    char* res = new char[3]{0};
    for (int i = 0; i < 3; i++) {
        // 11111111
        // res[i] = value & (0xFF << i * 8); why this not useful
        res[i] = (value >> (i * 8)) & 0xFF;
        // std::cout << std::bitset<8>(res[i]) << std::endl;
    }
    return res;
}
// In this , I make a mistake, the group[0] is the lowerest in the four item
// such as : 'abc'(binary:011000,01) decode is "YWJj", and the decode function will first read 'a' and get six bit is 'Y'
// when decode , group[0] is 'Y', it should be the higher in the value
char* convert_group(char* group, char* res) {
    // 我这个方法good
    uint32_t value = 0; // exactly four bytes interger, from kk
    int euqal_num = 0;
    for (int i = 0; i < 4; i++) {
        if (group[i] ==  '=') {
            euqal_num ++;
            continue;
        } else {
            group[i] = convert(group[i]);
            value += (group[i] & 0x3f) << (18 - 6 * i);
        }
        // this print way : std::bitset<8>, print binary directly
        // std::cout << std::bitset<8>((unsigned)group[i]) << std::endl;
        // 0011 1111 3f
    }
    // std::cout << std::bitset<32>(value) << std::endl;
    for (int i = 0; i < 3-euqal_num; i++) {
        // 11111111
        // res[i] = value & (0xFF << i * 8); why this not useful
        res[i] = (value >> (16 - i * 8)) & 0xFF;
        // std::cout << std::bitset<8>(res[i]) << std::endl;
    }
    return res;
}
// 000001001 01001111
std::string base64_encode(std::string str_to_encode, size_t len, bool url) {
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
char* base64_decode(std::string& str_to_decode) {
    // 4 * k
    // Mjfd sa+sdafa -> 101111, 10//1010,1100//11, 101001
    // A ASCII 65, A is 0
    size_t len = str_to_decode.size();
    size_t group_num = len / 4; // it must be 4 * k 
    size_t output_num = group_num * 3;
    if (str_to_decode[len-1] == '=') output_num --;
    if (str_to_decode[len-2] == '=') output_num --;
    char* res = new char[output_num+1]; // this must bigger than output_num 1;
    res[output_num] = '\0';
    char* group = new char[5];
    for (size_t i = 0; i < group_num; i++) {
        strcpy(group, (str_to_decode.substr(i*4,4)).c_str());
        convert_group(group, res+i*3);
    }
    delete[] group;
    return res;
}
void test_easy() {
    std::string test_str = "helloworld";
    std::cout << "test_str: " << test_str << std::endl;
    std::string encoded_str = base64_encode(test_str, test_str.size(), false);
    std::cout << "encoded_str: " << encoded_str << std::endl;
    char* decoded_str = base64_decode(encoded_str);
    std::string str_decoded(decoded_str);
    std::cout << "decoded_str: " << str_decoded << std::endl;
}
// void test_base64_encode() {  
//     // 测试用例 1：简单字符串  
//     const char* plain_text = "Man";  
//     unsigned char const* input = reinterpret_cast<unsigned char const*>(plain_text);  
//     std::string encoded = base64_encode(input, 3, 0);  
//     std::cout << "Test 1: 'Man' encoded: " << encoded << std::endl;  
//     assert(encoded == "TWFu");  
//     // 测试用例 2：两字节输入  
//     const char* plain_text2 = "Ma";  
//     unsigned char const* input2 = reinterpret_cast<unsigned char const*>(plain_text2);  
//     std::string encoded2 = base64_encode(input2, 2, 0);  
//     std::cout << "Test 2: 'Ma' encoded: " << encoded2 << std::endl;  
//     assert(encoded2 == "TWE=");  
//     // 测试用例 3：一字节输入  
//     const char* plain_text3 = "M";  
//     unsigned char const* input3 = reinterpret_cast<unsigned char const*>(plain_text3);  
//     std::string encoded3 = base64_encode(input3, 1, 0);  
//     std::cout << "Test 3: 'M' encoded: " << encoded3 << std::endl;  
//     assert(encoded3 == "TQ==");  
//     // 测试用例 4：特殊字符测试  
//     const char* plain_text4 = "+/";  
//     unsigned char const* input4 = reinterpret_cast<unsigned char const*>(plain_text4);  
//     std::string encoded4 = base64_encode(input4, 2, 0);  // 在标准模式下  
//     std::cout << "Test 4: '+/' encoded (std): " << encoded4 << std::endl;  
//     assert(encoded4 == "Ky8="); // 正确预期的输出  
//     std::cout << "\nAll tests passed!\n";  
// }    
int test_all() {
    // 1. 读取当前目录下的二进制文件 "./app"
    std::ifstream infile("./app", std::ios::binary);
    if (!infile) {
        std::cerr << "Error: Cannot open ./app" << std::endl;
        return 1;
    }
    std::vector<char> buffer((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();
    std::string originalData(buffer.begin(), buffer.end());
    
    // 2. Base64 编码
    std::string encodedData = base64_encode(originalData, originalData.size(), false);
    
    // 3. 将编码后的数据写入文件 "./app_e"
    std::ofstream outEnc("./app_e");
    if (!outEnc) {
        std::cerr << "Error: Cannot write to ./app_e" << std::endl;
        return 1;
    }
    outEnc << encodedData;
    outEnc.close();
    
    // 4. 从文件 "./app_e" 读取编码后的数据
    std::ifstream inEnc("./app_e");
    if (!inEnc) {
        std::cerr << "Error: Cannot open ./app_e" << std::endl;
        return 1;
    }
    std::string encodedDataRead((std::istreambuf_iterator<char>(inEnc)), std::istreambuf_iterator<char>());
    inEnc.close();
    
    // 5. Base64 解码
    char* decodedData = base64_decode(encodedDataRead);
    // 根据编码字符串计算解码后数据的长度
    size_t len = encodedDataRead.size();
    size_t group_num = len / 4;
    size_t output_num = group_num * 3;
    if (encodedDataRead[len - 1] == '=') output_num--;
    if (encodedDataRead[len - 2] == '=') output_num--;
    
    // 6. 将解码后的二进制数据写入文件 "./app_d"
    std::ofstream outDec("./app_d", std::ios::binary);
    if (!outDec) {
        std::cerr << "Error: Cannot write to ./app_d" << std::endl;
        delete[] decodedData;
        return 1;
    }
    outDec.write(decodedData, output_num);
    outDec.close();
    delete[] decodedData;
    
    // 可选：赋予 "./app_d" 可执行权限（仅 Linux 下有效）
    // system("chmod +x ./app_d");
    
    std::cout << "Test completed: ./app encoded to ./app_e and decoded to ./app_d" << std::endl;
    return 0;
}
int main() {  
    // test_base64_encode();
    test_all();
    return 0;  
}    
// '0' ASCII is : 48/00110000
// 0xfc/11111100
// 001100 00