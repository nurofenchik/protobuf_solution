#include <protobuf_parser/DelimitedMessagesStreamParser.hpp>
#include <protobuf_parser/helpers.hpp>
#include "messages.pb.h"

#include <gtest/gtest.h>

TEST(Parser, OneFastRequest)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto data = serializeDelimited(message);
  messages = parser.parse(std::string(data->begin(), data->end()));
  ASSERT_EQ(1, messages.size());

  auto item = messages.front();
  ASSERT_TRUE(item->has_request_for_fast_response());
}

TEST(Parser, SomeFastRequests)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto data = serializeDelimited(message);

  size_t count = 5;
  std::string stream;
  for (int i = 0; i < count; ++i)
    stream.append(std::string(data->begin(), data->end()));

  messages = parser.parse(stream);
  ASSERT_EQ(count, messages.size());

  for (auto &item : messages)
    ASSERT_TRUE(item->has_request_for_fast_response());
}

TEST(Parser, OneSlowRequest)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(0);

  auto data = serializeDelimited(message);
  messages = parser.parse(std::string(data->begin(), data->end()));
  ASSERT_EQ(1, messages.size());

  auto item = messages.front();
  ASSERT_TRUE(item->has_request_for_slow_response());
  EXPECT_EQ(
    item->request_for_slow_response().time_in_seconds_to_sleep(),
    message.request_for_slow_response().time_in_seconds_to_sleep()
  );
}

TEST(Parser, SomeSlowRequests)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(0);

  auto data = serializeDelimited(message);

  size_t count = 5;
  std::string stream;
  for (int i = 0; i < count; ++i)
    stream.append(std::string(data->begin(), data->end()));

  messages = parser.parse(stream);
  ASSERT_EQ(count, messages.size());

  for (auto &item : messages)
  {
    ASSERT_TRUE(item->has_request_for_slow_response());
    EXPECT_EQ(
      item->request_for_slow_response().time_in_seconds_to_sleep(),
      message.request_for_slow_response().time_in_seconds_to_sleep()
    );
  }
}

TEST(Parser, SomeRequests)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage fastRequest;
  fastRequest.mutable_request_for_fast_response();

  TestTask::Messages::WrapperMessage slowRequest;
  slowRequest.mutable_request_for_slow_response()->set_time_in_seconds_to_sleep(0);

  auto fReqData = serializeDelimited(fastRequest);
  auto sReqData = serializeDelimited(slowRequest);

  size_t count = 5;
  std::string stream;
  for (int i = 0; i < count; ++i)
    stream.append(std::rand() % 2 > 0 ? std::string(fReqData->begin(), fReqData->end()) : std::string(sReqData->begin(), sReqData->end()));

  messages = parser.parse(stream);
  ASSERT_EQ(count, messages.size());

  for (auto &item : messages)
  {
    ASSERT_TRUE(item->has_request_for_fast_response() || item->has_request_for_slow_response());
    if (item->has_request_for_slow_response())
    {
      EXPECT_EQ(
        item->request_for_slow_response().time_in_seconds_to_sleep(),
        slowRequest.request_for_slow_response().time_in_seconds_to_sleep()
      );
    }
  }
}

TEST(Parser, OneFastResponse)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_fast_response()->set_current_date_time("");

  auto data = serializeDelimited(message);
  messages = parser.parse(std::string(data->begin(), data->end()));
  ASSERT_EQ(1, messages.size());

  auto item = messages.front();
  ASSERT_TRUE(item->has_fast_response());
  EXPECT_EQ(
    item->fast_response().current_date_time(),
    message.fast_response().current_date_time()
  );
}

TEST(Parser, SomeFastResponses)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_fast_response()->set_current_date_time("");

  auto data = serializeDelimited(message);

  size_t count = 5;
  std::string stream;
  for (int i = 0; i < count; ++i)
    stream.append(std::string(data->begin(), data->end()));

  messages = parser.parse(stream);
  ASSERT_EQ(count, messages.size());

  for (auto &item : messages)
  {
    ASSERT_TRUE(item->has_fast_response());
    EXPECT_EQ(
      item->fast_response().current_date_time(),
      message.fast_response().current_date_time()
    );
  }
}

TEST(Parser, OneSlowResponse)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_slow_response()->set_connected_client_count(0);

  auto data = serializeDelimited(message);
  messages = parser.parse(std::string(data->begin(), data->end()));
  ASSERT_EQ(1, messages.size());

  auto item = messages.front();
  ASSERT_TRUE(item->has_slow_response());
  EXPECT_EQ(
    item->slow_response().connected_client_count(),
    message.slow_response().connected_client_count()
  );
}

TEST(Parser, SomeSlowResponses)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_slow_response()->set_connected_client_count(0);

  auto data = serializeDelimited(message);

  size_t count = 5;
  std::string stream;
  for (int i = 0; i < count; ++i)
    stream.append(std::string(data->begin(), data->end()));

  messages = parser.parse(stream);
  ASSERT_EQ(count, messages.size());

  for (auto &item : messages)
  {
    ASSERT_TRUE(item->has_slow_response());
    EXPECT_EQ(
      item->slow_response().connected_client_count(),
      message.slow_response().connected_client_count()
    );
  }
}

TEST(Parser, SomeResponses)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage fastResponse;
  fastResponse.mutable_fast_response()->set_current_date_time("");

  TestTask::Messages::WrapperMessage slowResponse;
  slowResponse.mutable_slow_response()->set_connected_client_count(0);

  auto fResData = serializeDelimited(fastResponse);
  auto sResData = serializeDelimited(slowResponse);

  size_t count = 5;
  std::string stream;
  for (int i = 0; i < (count + 1) / 2; ++i)
    stream.append(std::string(fResData->begin(), fResData->end()));

  for (int i = 0; i < count / 2; ++i)
    stream.append(std::string(sResData->begin(), sResData->end()));

  messages = parser.parse(stream);
  ASSERT_EQ(count, messages.size());

  for (auto &item : messages)
  {
    ASSERT_TRUE(item->has_fast_response() || item->has_slow_response());
    if (item->has_fast_response())
    {
      EXPECT_EQ(
        item->fast_response().current_date_time(),
        fastResponse.fast_response().current_date_time()
      );
    }
    else
    {
      EXPECT_EQ(
        item->slow_response().connected_client_count(),
        slowResponse.slow_response().connected_client_count()
      );
    }
  }
}

TEST(Parser, EmptyData)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  messages = parser.parse("");
  EXPECT_EQ(0, messages.size());
}

TEST(Parser, SlicedData)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto data = serializeDelimited(message);
  size_t middle = data->size() / 2;

  messages = parser.parse(std::string(data->begin(), data->begin() + middle));
  EXPECT_EQ(messages.size(), 0);

  messages = parser.parse(std::string(data->begin() + middle, data->end()));
  EXPECT_EQ(messages.size(), 1);

  auto item = messages.front();
  ASSERT_TRUE(item->has_request_for_fast_response());
}

TEST(Parser, WrongData)
{
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
  EXPECT_THROW(parser.parse("\x05wrong"), std::runtime_error);
}

TEST(Parser, CorruptedData)
{
  std::list<typename DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>::PointerToConstValue> messages;
  DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;

  TestTask::Messages::WrapperMessage message;
  message.mutable_fast_response()->set_current_date_time("0");

  auto data = serializeDelimited(message);

  size_t count = 3;
  std::string stream;
  for (int i = 0; i < count; ++i)
    stream.append(std::string(data->begin(), data->end()));

  stream[data->size()] = '\x03';
  EXPECT_THROW(parser.parse(stream), std::runtime_error);
}
