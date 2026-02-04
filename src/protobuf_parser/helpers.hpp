#include <fstream>
#include <string>
#include <sstream>
#include "messages.pb.h"
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <memory>
#ifndef HELPERS_H
#define HELPERS_H
typedef std::vector<char> Data;
typedef std::shared_ptr<const Data> PointerToConstData;
typedef std::shared_ptr<Data> PointerToData;

#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSize())
#endif
template <typename Message>
PointerToConstData serializeDelimited(const Message& msg) {
    const size_t messageSize = PROTOBUF_MESSAGE_BYTE_SIZE(msg);
    const size_t headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);
    const PointerToData& result = std::make_shared<Data>(headerSize + messageSize);
    google::protobuf::uint8* buffer = reinterpret_cast<google::protobuf::uint8*>(&*result->begin());
    google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(messageSize, buffer);
    msg.SerializeWithCachedSizesToArray(buffer + headerSize);
    return result;
}

template<typename Message>
std::shared_ptr<Message> parseDelimited(const void* data, size_t size, size_t* bytesConsumed = nullptr) {

    if (data == nullptr) return nullptr;

    google::protobuf::uint32 messageSize;
    google::protobuf::io::CodedInputStream input(reinterpret_cast<const google::protobuf::uint8*>(data), size);
    
    if ( (!input.ReadVarint32(&messageSize)) || (messageSize > size - google::protobuf::io::CodedOutputStream::VarintSize32(messageSize)) ) {
        if (bytesConsumed != nullptr) {
            *bytesConsumed = 0;
        }
        return nullptr;
    }

    input.PushLimit(messageSize);

    std::shared_ptr<Message> result = std::make_shared<Message>();
    
    if (!result->ParseFromCodedStream(&input)) {
        if (bytesConsumed != nullptr) {
            *bytesConsumed = input.CurrentPosition();
        }
        throw std::runtime_error("Failed to parse the message.");
    }

    input.PopLimit(messageSize);
    if (bytesConsumed != nullptr) {
        *bytesConsumed = input.CurrentPosition();
    }
    return result;
}

#endif // HELPERS_H
