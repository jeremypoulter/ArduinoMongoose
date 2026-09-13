import os

filepath = 'src/MongooseString.h'
with open(filepath, 'r') as f:
    content = f.read()

content = content.replace('class MongooseString\n{', '/**\n * @brief A string wrapper class interoperable with mg_str and Arduino String\n * \n * Simplifies conversions and comparisons between C-strings, Arduino Strings, and Mongoose mg_str.\n */\nclass MongooseString\n{')
content = content.replace('size_t length() const {', '/**\n     * @brief Get the length of the string\n     * @return size_t Length of the string in bytes\n     */\n    size_t length() const {')
content = content.replace('const char *c_str() const', '/**\n     * @brief Get the underlying C-string pointer (may not be null-terminated)\n     * @return const char*\n     */\n    const char *c_str() const')

with open(filepath, 'w') as f:
    f.write(content)

