#include <gtest/gtest.h>
#include "messages.pb.h"
#include "delimited_message_parser.h"
#include <string>

typedef TestTask::Messages::WrapperMessage Wrapper;


TEST(ParseDelimitedTest, GivenMessage)
{
    Wrapper msg;
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

TEST(ParseDelimitedTest, IncompleteMessageReturnsNull)
{
    Wrapper msg;
    msg.mutable_fast_response()->set_current_date_time("HELLO");

    auto data = serializeDelimited(msg);

    size_t consumed = 123; // специально мусор
    auto parsed = parseDelimited<Wrapper>(
        data->data(),
        data->size() - 1,   // ОТРЕЗАЛИ 1 БАЙТ
        &consumed
    );

    EXPECT_EQ(parsed, nullptr);
    EXPECT_EQ(consumed, 0); // ничего не съели
}

TEST(StreamParserTest, GarbageReturnsNullptr)
{
    std::string garbage = "GARBAGE"; // несериализованные данные

    size_t bytesConsumed = 0;
    auto msg = parseDelimited<Wrapper>(
        garbage.data(),
        garbage.size(),
        &bytesConsumed
    );

    EXPECT_EQ(msg, nullptr);       // nullptr вернулось
    EXPECT_EQ(bytesConsumed, 0);   // никаких байт не съедено
}

TEST(StreamParserTest, TwoMessagesInOneChunk)
{
    DelimitedMessagesStreamParser<Wrapper> parser;

    Wrapper msg1;
    msg1.mutable_fast_response()->set_current_date_time("HELLO");

    Wrapper msg2;
    msg2.mutable_fast_response()->set_current_date_time("WORLD");

    auto data1 = serializeDelimited(msg1);
    auto data2 = serializeDelimited(msg2);

    std::string stream;
    stream.append(data1->begin(), data1->end());
    stream.append(data2->begin(), data2->end());

    auto messages = parser.parse(stream);

    ASSERT_EQ(messages.size(), 2);

    auto it = messages.begin();
    EXPECT_EQ((*it)->fast_response().current_date_time(), "HELLO");
    ++it;
    EXPECT_EQ((*it)->fast_response().current_date_time(), "WORLD");
}

TEST(StreamParserTest, GarbageBetweenThreeMessagesInOneChunk)
{
    
    DelimitedMessagesStreamParser<Wrapper> parser;

    Wrapper msg1;
    msg1.mutable_fast_response()->set_current_date_time("HELLO");

    Wrapper msg2;
    msg2.mutable_fast_response()->set_current_date_time("WORLD");

    Wrapper msg3;
    msg3.mutable_fast_response()->set_current_date_time("NICETU");


    auto data1 = serializeDelimited(msg1);
    auto data2 = serializeDelimited(msg2);
    auto data3 = serializeDelimited(msg3);
    std::string stream;
    stream.append(data1->begin(), data1->end());
    stream.append(data2->begin(), data2->end());
    stream.append("GARBAGE" , 7);
    stream.append(data3->begin(), data3->end());

    auto messages = parser.parse(stream);

    ASSERT_EQ(messages.size(), 2);

    auto it = messages.begin();
    EXPECT_EQ((*it)->fast_response().current_date_time(), "HELLO");
    ++it;
    EXPECT_EQ((*it)->fast_response().current_date_time(), "WORLD");
}

TEST(StreamParserTest, EmptyBufferReturnsEmptyList)
{
    DelimitedMessagesStreamParser<Wrapper> parser;
    
    std::string empty = "";
    auto messages = parser.parse(empty);
    
    EXPECT_TRUE(messages.empty());
}

TEST(StreamParserTest, PartialMessageWithCompletion)
{
    DelimitedMessagesStreamParser<Wrapper> parser;
    
    Wrapper msg;
    msg.mutable_slow_response()->set_connected_client_count(42);
    
    auto data = serializeDelimited(msg);
    
    // Разделяем на две части
    size_t splitPoint = data->size() / 2;
    std::string part1(data->begin(), data->begin() + splitPoint);
    std::string part2(data->begin() + splitPoint, data->end());
    
    // Первая часть - должна вернуть пустой список (сообщение неполное)
    auto messages1 = parser.parse(part1);
    EXPECT_TRUE(messages1.empty());
    
    // Вторая часть - должна завершить сообщение
    auto messages2 = parser.parse(part2);
    ASSERT_EQ(messages2.size(), 1);
    EXPECT_TRUE((*messages2.begin())->has_slow_response());
    EXPECT_EQ((*messages2.begin())->slow_response().connected_client_count(), 42);
}


