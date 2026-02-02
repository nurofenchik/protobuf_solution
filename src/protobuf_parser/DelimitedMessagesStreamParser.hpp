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

        PointerToConstData m_buffer_ptr = std::make_shared<const Data>(m_buffer);
        size_t bytesRead = 0;
        while (m_buffer.size() > 0) {
            auto parsedMessage = parseDelimited<TestTask::Messages::WrapperMessage>(m_buffer_ptr->data(), m_buffer_ptr->size(), &bytesRead);
            if (parsedMessage)
                result_list.push_back(parsedMessage);
            else
                break;
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + bytesRead);
            m_buffer_ptr = std::make_shared<const Data>(m_buffer);
        }
        return result_list;
    }
private:
    std::vector<char> m_buffer;
};



#endif // DELIMITEDMESSAGESSTREAMPARSER_H
