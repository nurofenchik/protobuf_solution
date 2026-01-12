#include "delimited_message_parser.h"
#include "messages.pb.h"

#include <iostream>

int main() {
    // Создаём одно сообщение
    TestTask::Messages::WrapperMessage msg;
    msg.mutable_fast_response()->set_current_date_time("19851019T050107.333");

    // Сериализуем в length-prefixed формат
    auto data = serializeDelimited(msg);

    // Парсим поток
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    auto messages = parser.parse(std::string(data->begin(), data->end()));

    // Вывод результата
    for (auto& m : messages) {
        if (m->has_fast_response()) {
            std::cout << "FastResponse: " 
                      << m->fast_response().current_date_time() << std::endl;
        }
        if (m->has_slow_response()) {
            std::cout << "SlowResponse: " 
                      << m->slow_response().connected_client_count() << std::endl;
        }
    }

    return 0;
}
