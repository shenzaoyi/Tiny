# base64

在 Base64 编码中，通常使用两种不同的字符集，这是为了适用于不同的场景需求。

1. **标准 Base64**：
   - 使用 `+` 和 `/` 作为填充字符
   - 适用于大多数通用场景，如在 MIME 协议中发送邮件附件
2. **URL 安全 Base64**：
   - 替换了 `+` 为 `-`，`/` 为 `_`
   - ```.```替换了 ```=```
   - 避免了 URL 中的特殊字符问题，确保字符串可以安全地嵌入到 URL 中

```cpp
static const char* base64_chars[2] = {
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789"
             "+/",

             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789"
             "-_"};
```

```cpp
pos in_len;
pos+1 > len : only one byte left, 
****/****
pick six then : ;
00000011 << 4 = 110000
pos+2 > len : there two bytes left

****/****//****/****//****/****//;
pos
oxfc;
1111/1100;
0x03;
0000/0011;
0xf0;
1111/0000;
0x0f;
0000/1111;
0xc0;
1100/0000;
0x3f;
00111111
```

