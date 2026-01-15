#include <gtest/gtest.h>
#include "messages.pb.h"
#include "delimited_message_parser.h"
#include <vector>
#include <string>
TEST(DummyTest, AlwaysTrue)
{
    EXPECT_TRUE(true);
}



TEST(ParseDelimited, GivenMessage)
{
    TestTask::Messages::WrapperMessage msg;
    msg.mutable_fast_response()->set_current_date_time("19851019T050107.333");

    auto data = serializeDelimited(msg);

    size_t consumed = 0;
    auto parsed = parseDelimited<TestTask::Messages::WrapperMessage>(
        data->data(),
        data->size(),
        &consumed
    );

    ASSERT_NE(parsed, nullptr);
    EXPECT_EQ(consumed, data->size());
    EXPECT_TRUE(parsed->has_fast_response());
    EXPECT_EQ(
        parsed->fast_response().current_date_time(),
        "19851019T050107.333"
    );
}

// TEST(StreamParser, MessageAcrossChunks)
// {
//     DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
//     std::vector<std::string> messages;
//     auto first_message = serializeDelimited("HELLO");
//     auto second_message = serializeDelimited("WORLD");
//     messages.push_back(first_message->data());
//     messages.push_back(second_message->data());
//     for( auto  byte : messages)
//     {
//         std::cout << byte;
//     }
    
// }
