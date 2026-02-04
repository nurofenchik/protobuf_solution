#include "helpers.hpp"
#ifndef DELIMITEDMESSAGESSTREAMPARSER_H
#define DELIMITEDMESSAGESSTREAMPARSER_H

template<typename MessageType>
class DelimitedMessagesStreamParser {
public:
    typedef std::shared_ptr<const MessageType> PointerToConstValue;
    std::list<PointerToConstValue> parse(const std::string& data) {
        std::list<PointerToConstValue> result_list;
        m_buffer.insert(m_buffer.end(), data.begin(), data.end());

        const char* buffer_data = m_buffer.data() + m_buffer_start;
        size_t buffer_size = m_buffer.size() - m_buffer_start;
        size_t bytesRead = 0;
        while (m_buffer.size() > 0) {
            try {
                auto parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(buffer_data, buffer_size, &bytesRead);
                if (parsedMessage){
                    result_list.push_back(parsedMessage);
                    m_buffer_start += bytesRead;
                    buffer_data += bytesRead;
                    buffer_size -= bytesRead;
                }
                else{
                    break;
                }
            } catch (const std::runtime_error& e) {
                // Продвигаем буфер, чтобы не застрять на битых данных
                if (bytesRead > 0) {
                    m_buffer_start += bytesRead;
                    buffer_data += bytesRead;
                    buffer_size -= bytesRead;
                }
                // Пробрасываем исключение дальше
                throw;
            }
        }
        // Периодически чистим буфер, если много обработанных данных
        if (m_buffer_start > m_buffer.size() / 2) {
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_buffer_start);
            m_buffer_start = 0;
        }

        return result_list;
    }
private:
    std::vector<char> m_buffer;
    size_t m_buffer_start = 0;
};



#endif // DELIMITEDMESSAGESSTREAMPARSER_H
