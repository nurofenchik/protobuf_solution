#include <protobuf_parser/helpers.hpp>
#include "messages.pb.h"

#include <gtest/gtest.h>

TEST(ParseDelimited, DefaultTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto buffer = serializeDelimited(message);
  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                buffer->data(),
                buffer->size(),
                &bytesConsumed
              );

  ASSERT_FALSE(delimited == nullptr);
  EXPECT_TRUE(delimited->has_request_for_fast_response());
  EXPECT_EQ(bytesConsumed, buffer->size());
}

TEST(ParseDelimited, NullDataTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                nullptr,
                0,
                &bytesConsumed
              );

  ASSERT_TRUE(delimited == nullptr);
  EXPECT_EQ(bytesConsumed, 0);
}

TEST(ParseDelimited, WrongDataTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  std::string buffer = "\x05wrong";
  EXPECT_THROW(
    parseDelimited<TestTask::Messages::WrapperMessage>(buffer.data(), buffer.size()),
    std::runtime_error
  );
}

TEST(ParseDelimited, CorruptedDataTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto buffer = serializeDelimited(message);
  size_t bytesConsumed = 0;

  std::string corrupted = std::string(buffer->begin(), buffer->end());
  corrupted[0] -= 1;

  EXPECT_THROW(
    parseDelimited<TestTask::Messages::WrapperMessage>(corrupted.data(), corrupted.size()),
    std::runtime_error
  );
}

TEST(ParseDelimited, WrongMessageSizeTest)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> delimited;

  TestTask::Messages::WrapperMessage message;
  message.mutable_request_for_fast_response();

  auto buffer = serializeDelimited(message);
  size_t bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                buffer->data(),
                buffer->size() * 2,
                &bytesConsumed
              );

  ASSERT_FALSE(delimited == nullptr);
  EXPECT_TRUE(delimited->has_request_for_fast_response());
  EXPECT_EQ(bytesConsumed, buffer->size());

  bytesConsumed = 0;

  delimited = parseDelimited<TestTask::Messages::WrapperMessage>(
                buffer->data(),
                buffer->size() / 2,
                &bytesConsumed
              );

  ASSERT_TRUE(delimited == nullptr);
  EXPECT_EQ(bytesConsumed, 0);
}
