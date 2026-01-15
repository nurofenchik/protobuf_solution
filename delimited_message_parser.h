
#include <vector>
#include <memory>
#include <list>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

#if GOOGLE_PROTOBUF_VERSION >= 3012004
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSizeLong())
#else
#define PROTOBUF_MESSAGE_BYTE_SIZE(message) ((message).ByteSize())
#endif

typedef std::vector<char> Data;
typedef std::shared_ptr<const Data> PointerToConstData;
typedef std::shared_ptr<Data> PointerToData;

template<typename Message>
std::shared_ptr<Message> parseDelimited(const void* data, size_t size, size_t* bytesConsumed = 0);

/*!
 * \brief Парсер потока delimited-сообщений protobuf
 * \details Класс накапливает данные во внутреннем буфере и извлекает
 * из него последовательность сообщений в формате length-delimited
 */
template<typename MessageType>
class DelimitedMessagesStreamParser
{
public:
  /*!
   * \brief Парсинг входящих данных
   * \param data Строка с данными для парсинга
   * \return Список распарсенных сообщений
   */
  std::list<std::shared_ptr<MessageType>> parse(const std::string& data)
  {
    std::list<std::shared_ptr<MessageType>> result;
    m_buffer.insert(m_buffer.end(), data.begin(), data.end());
    while(!m_buffer.empty())
    {
      size_t bytesConsumed = 0;
      auto message = parseDelimited<MessageType>(m_buffer.data(), m_buffer.size(), &bytesConsumed);

      if(message == nullptr)
      {
        break;
      }
      result.push_back(message);
      m_buffer.erase(m_buffer.begin(), m_buffer.begin() + static_cast<std::ptrdiff_t>(bytesConsumed));
    }
    return result;
  }

private:
  std::vector<char> m_buffer; //!< Буфер накопления данных
};

/*!
 * \brief Парсинг одного delimited-сообщения
 * \param data Указатель на данные
 * \param size Размер данных
 * \param bytesConsumed Количество потребленных байт (выходной параметр)
 * \return Указатель на распарсенное сообщение или nullptr при ошибке
 */
template<typename Message>
std::shared_ptr<Message> parseDelimited(const void* data, size_t size, size_t* bytesConsumed)
{
  if(bytesConsumed != nullptr)
  {
    *bytesConsumed = 0;
  }
  if(!data || !size)
  {
    return nullptr;
  }
  const google::protobuf::uint8* buffer = static_cast<const google::protobuf::uint8*>(data);
  google::protobuf::io::CodedInputStream input(buffer, static_cast<int>(size));
  uint32_t messageSize = 0;
  input.ReadVarint32(&messageSize);
  const int headerSize = input.CurrentPosition();

  if(size < static_cast<size_t>(headerSize + messageSize))
  {
    return nullptr;
  }

  auto message = std::make_shared<Message>();
  if(!message->ParseFromArray(buffer + headerSize, messageSize))
  {
    return nullptr;
  }
  if(bytesConsumed)
  {
    *bytesConsumed = headerSize + messageSize;
  }
  return message;
}

/*!
 * \brief Сериализация сообщения в формат length-delimited
 * \param msg Сообщение для сериализации
 * \return Указатель на буфер с сериализованными данными
 */
template<typename Message>
PointerToConstData serializeDelimited(const Message& msg)
{
  const size_t messageSize = PROTOBUF_MESSAGE_BYTE_SIZE(msg);
  const size_t headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);

  PointerToData result = std::make_shared<Data>(headerSize + messageSize);
  google::protobuf::uint8* buffer = reinterpret_cast<google::protobuf::uint8*>(&*result->begin());

  google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(messageSize, buffer);
  msg.SerializeWithCachedSizesToArray(buffer + headerSize);

  return result;
}
