#include "messages.pb.h"
#include "delimited_message_parser.h"

#include <gtest/gtest.h>

#include <string>

typedef TestTask::Messages::WrapperMessage Wrapper;

//! Тест корректной сериализации и десериализации FastResponse
TEST(ParseDelimitedTest, CorrectFastResponse)
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

//! Тест обработки неполного сообщения
TEST(ParseDelimitedTest, IncorrectFastResponseReturnsNull)
{
  Wrapper msg;
  msg.mutable_fast_response()->set_current_date_time("19851019T050107.333");

  auto data = serializeDelimited(msg);

  size_t consumed = 123;
  auto parsed = parseDelimited<Wrapper>(
    data->data(),
    data->size() - 1,
    &consumed
  );

  EXPECT_EQ(parsed, nullptr);
  EXPECT_EQ(consumed, 0);
}

//! Тест обработки мусорных данных
TEST(ParseDelimitedTest, FastGarbageResponseReturnsNullptr)
{
  std::string garbage = "19851019T050107.333";

  size_t bytesConsumed = 0;
  auto msg = parseDelimited<Wrapper>(
    garbage.data(),
    garbage.size(),
    &bytesConsumed
  );

  EXPECT_EQ(msg, nullptr);
  EXPECT_EQ(bytesConsumed, 0);
}

//! Тест корректной сериализации и десериализации SlowResponse
TEST(ParseDelimitedTest, CorrectSlowResponse)
{
  Wrapper msg;
  msg.mutable_slow_response()->set_connected_client_count(42);

  auto data = serializeDelimited(msg);

  size_t consumed = 0;
  auto parsed = parseDelimited<Wrapper>(
    data->data(),
    data->size(),
    &consumed
  );

  ASSERT_NE(parsed, nullptr);
  EXPECT_TRUE(parsed->has_slow_response());
  EXPECT_EQ(parsed->slow_response().connected_client_count(), 42);
  EXPECT_EQ(consumed, data->size());
}

//! Тест обработки неполного SlowResponse
TEST(ParseDelimitedTest, IncorrectSlowResponseReturnsNull)
{
  Wrapper msg;
  msg.mutable_slow_response()->set_connected_client_count(52);

  auto data = serializeDelimited(msg);

  size_t consumed = 123;
  auto parsed = parseDelimited<Wrapper>(
    data->data(),
    data->size() - 1,
    &consumed
  );

  EXPECT_EQ(parsed, nullptr);
  EXPECT_EQ(consumed, 0);
}

//! Тест мусорных данных для SlowResponse
TEST(ParseDelimitedTest, SlowGarbageResponseReturnsNullptr)
{
  std::string garbage = "2339";

  size_t bytesConsumed = 0;
  auto msg = parseDelimited<Wrapper>(
    garbage.data(),
    garbage.size(),
    &bytesConsumed
  );

  EXPECT_EQ(msg, nullptr);
  EXPECT_EQ(bytesConsumed, 0);
}

//! Тест парсинга двух FastResponse в одном потоке
TEST(StreamParserTest, FastTwoMessagesInOneChunkResponse)
{
  DelimitedMessagesStreamParser<Wrapper> parser;

  Wrapper msg1;
  msg1.mutable_fast_response()->set_current_date_time("19851019T050107.333");

  Wrapper msg2;
  msg2.mutable_fast_response()->set_current_date_time("20260315T234559.120");

  auto data1 = serializeDelimited(msg1);
  auto data2 = serializeDelimited(msg2);

  std::string stream;
  stream.append(data1->begin(), data1->end());
  stream.append(data2->begin(), data2->end());

  auto messages = parser.parse(stream);

  ASSERT_EQ(messages.size(), 2);

  auto it = messages.begin();
  EXPECT_EQ((*it)->fast_response().current_date_time(), "19851019T050107.333");
  ++it;
  EXPECT_EQ((*it)->fast_response().current_date_time(), "20260315T234559.120");
}

//! Тест парсинга двух SlowResponse в одном потоке
TEST(StreamParserTest, SlowTwoMessagesInOneChunkResponse)
{
  DelimitedMessagesStreamParser<Wrapper> parser;

  Wrapper msg1;
  msg1.mutable_slow_response()->set_connected_client_count(42);

  Wrapper msg2;
  msg2.mutable_slow_response()->set_connected_client_count(52);

  auto data1 = serializeDelimited(msg1);
  auto data2 = serializeDelimited(msg2);

  std::string stream;
  stream.append(data1->begin(), data1->end());
  stream.append(data2->begin(), data2->end());

  auto messages = parser.parse(stream);
  auto it = messages.begin();
  EXPECT_EQ((*it)->slow_response().connected_client_count(), 42);
  ++it;
  EXPECT_EQ((*it)->slow_response().connected_client_count(), 52);
}

//! Тест обработки мусора между сообщениями FastResponse
TEST(StreamParserTest, FastGarbageBetweenThreeMessagesInOneChunkResponse)
{
  DelimitedMessagesStreamParser<Wrapper> parser;

  Wrapper msg1;
  msg1.mutable_fast_response()->set_current_date_time("19851019T050107.333");

  Wrapper msg2;
  msg2.mutable_fast_response()->set_current_date_time("20260315T234559.120");

  Wrapper msg3;
  msg3.mutable_fast_response()->set_current_date_time("20251231T235959.999");

  auto data1 = serializeDelimited(msg1);
  auto data2 = serializeDelimited(msg2);
  auto data3 = serializeDelimited(msg3);
  std::string stream;
  stream.append(data1->begin(), data1->end());
  stream.append(data2->begin(), data2->end());
  stream.append("GARBAGE", 7);
  stream.append(data3->begin(), data3->end());

  auto messages = parser.parse(stream);

  ASSERT_EQ(messages.size(), 2);

  auto it = messages.begin();
  EXPECT_EQ((*it)->fast_response().current_date_time(), "19851019T050107.333");
  ++it;
  EXPECT_EQ((*it)->fast_response().current_date_time(), "20260315T234559.120");
}

//! Тест обработки мусора между сообщениями SlowResponse
TEST(StreamParserTest, SlowGarbageBetweenThreeMessagesInOneChunkResponse)
{
  DelimitedMessagesStreamParser<Wrapper> parser;

  Wrapper msg1;
  msg1.mutable_slow_response()->set_connected_client_count(10);

  Wrapper msg2;
  msg2.mutable_slow_response()->set_connected_client_count(20);

  Wrapper msg3;
  msg3.mutable_slow_response()->set_connected_client_count(30);

  auto data1 = serializeDelimited(msg1);
  auto data2 = serializeDelimited(msg2);
  auto data3 = serializeDelimited(msg3);

  std::string stream;
  stream.append(data1->begin(), data1->end());
  stream.append(data2->begin(), data2->end());
  stream.append("GARBAGE", 7);
  stream.append(data3->begin(), data3->end());

  auto messages = parser.parse(stream);

  ASSERT_EQ(messages.size(), 2);

  auto it = messages.begin();
  EXPECT_EQ((*it)->slow_response().connected_client_count(), 10);
  ++it;
  EXPECT_EQ((*it)->slow_response().connected_client_count(), 20);
}

//! Тест обработки пустого буфера
TEST(StreamParserTest, EmptyBufferReturnsEmptyList)
{
  DelimitedMessagesStreamParser<Wrapper> parser;

  std::string empty = "";
  auto messages = parser.parse(empty);

  EXPECT_TRUE(messages.empty());
}

//! Тест докладывания частичного сообщения SlowResponse
TEST(StreamParserTest, PartialMessageWithCompletion)
{
  DelimitedMessagesStreamParser<Wrapper> parser;

  Wrapper msg;
  msg.mutable_slow_response()->set_connected_client_count(42);

  auto data = serializeDelimited(msg);

  size_t splitPoint = data->size() / 2;
  std::string part1(data->begin(), data->begin() + splitPoint);
  std::string part2(data->begin() + splitPoint, data->end());

  auto messages1 = parser.parse(part1);
  EXPECT_TRUE(messages1.empty());

  auto messages2 = parser.parse(part2);
  ASSERT_EQ(messages2.size(), 1);
  EXPECT_TRUE((*messages2.begin())->has_slow_response());
  EXPECT_EQ((*messages2.begin())->slow_response().connected_client_count(), 42);
}

//! Тест докладывания частичного сообщения FastResponse
TEST(StreamParserTest, PartialFastResponseWithCompletion)
{
  DelimitedMessagesStreamParser<Wrapper> parser;

  Wrapper msg;
  msg.mutable_fast_response()->set_current_date_time("19851019T050107.333");

  auto data = serializeDelimited(msg);

  size_t splitPoint = data->size() / 2;
  std::string part1(data->begin(), data->begin() + splitPoint);
  std::string part2(data->begin() + splitPoint, data->end());

  auto messages1 = parser.parse(part1);
  EXPECT_TRUE(messages1.empty());

  auto messages2 = parser.parse(part2);
  ASSERT_EQ(messages2.size(), 1);
  EXPECT_TRUE((*messages2.begin())->has_fast_response());
  EXPECT_EQ(
    (*messages2.begin())->fast_response().current_date_time(),
    "19851019T050107.333"
  );
}
