#include "delimited_message_parser.h"
#include "messages.pb.h"

#include <iostream>

int main() {
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    std::vector<PointerToConstData> dataset;
    TestTask::Messages::WrapperMessage msg1;
    msg1.mutable_fast_response()->set_current_date_time("HELLO");
    auto first_data_pointer = serializeDelimited(msg1);
    TestTask::Messages::WrapperMessage msg2;
    msg2.mutable_fast_response()->set_current_date_time("WORLD");
    auto second_data_pointer = serializeDelimited("msg2");
    dataset.push_back(first_data_pointer);
    dataset.push_back(second_data_pointer);
    auto messages = parser.parse(std::string(dataset.begin() , dataset.end()));
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
