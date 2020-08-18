
#include <boost/move/unique_ptr.hpp>
#include "MessageImpl.h"

using namespace dbcppp;


std::unique_ptr<Message> Message::create(
      uint64_t id
    , std::string&& name
    , uint64_t message_size
    , std::string&& transmitter
    , std::set<std::string>&& message_transmitters
    , std::map<std::string, std::unique_ptr<Signal>>&& signals
    , std::map<std::string, std::unique_ptr<Attribute>>&& attribute_values
    , std::string&& comment)
{
    std::map<std::string, SignalImpl> ss;
    std::map<std::string, AttributeImpl> avs;
    for (auto& s : signals)
    {
        ss.insert(std::make_pair(s.first, std::move(static_cast<SignalImpl&>(*s.second))));
        s.second.reset(nullptr);
    }
    for (auto& av : attribute_values)
    {
        avs.insert(std::make_pair(av.first, std::move(static_cast<AttributeImpl&>(*av.second))));
        av.second.reset(nullptr);
    }
    return std::make_unique<MessageImpl>(
          id
        , std::move(name)
        , message_size
        , std::move(transmitter)
        , std::move(message_transmitters)
        , std::move(ss)
        , std::move(avs)
        , std::move(comment));
}
MessageImpl::MessageImpl(
      uint64_t id
    , std::string&& name
    , uint64_t message_size
    , std::string&& transmitter
    , std::set<std::string>&& message_transmitters
    , std::map<std::string, SignalImpl>&& signals
    , std::map<std::string, AttributeImpl>&& attribute_values
    , std::string&& comment)
    
    : _id(std::move(id))
    , _name(std::move(name))
    , _message_size(std::move(message_size))
    , _transmitter(std::move(transmitter))
    , _message_transmitters(std::move(message_transmitters))
    , _signals(std::move(signals))
    , _attribute_values(std::move(attribute_values))
    , _comment(std::move(comment))
    , _mux_signal(nullptr)
    , _error(ErrorCode::NoError)
{
    bool have_mux_value = false;
    for (const auto& sig : _signals)
    {
        switch (sig.second.getMultiplexerIndicator())
        {
        case Signal::Multiplexer::MuxValue:
            have_mux_value = true;
            break;
        case Signal::Multiplexer::MuxSwitch:
            _mux_signal = &sig.second;
            break;
        }
    }
    if (have_mux_value && _mux_signal == nullptr)
    {
        _error = ErrorCode::MuxValeWithoutMuxSignal;
    }
}
MessageImpl::MessageImpl(const MessageImpl& other)
{
    _id = other._id;
    _name = other._name;
    _message_size = other._message_size;
    _transmitter = other._transmitter;
    _message_transmitters = other._message_transmitters;
    _signals = other._signals;
    _attribute_values = other._attribute_values;
    _comment = other._comment;
    _mux_signal = nullptr;
    for (const auto& sig : _signals)
    {
        switch (sig.second.getMultiplexerIndicator())
        {
        case Signal::Multiplexer::MuxSwitch:
            _mux_signal = &sig.second;
            break;
        }
    }
    _error = other._error;
}
MessageImpl& MessageImpl::operator=(const MessageImpl& other)
{
    _id = other._id;
    _name = other._name;
    _message_size = other._message_size;
    _transmitter = other._transmitter;
    _message_transmitters = other._message_transmitters;
    _signals = other._signals;
    _attribute_values = other._attribute_values;
    _comment = other._comment;
    _mux_signal = nullptr;
    for (const auto& sig : _signals)
    {
        switch (sig.second.getMultiplexerIndicator())
        {
        case Signal::Multiplexer::MuxSwitch:
            _mux_signal = &sig.second;
            break;
        }
    }
    _error = other._error;
    return *this;
}
std::unique_ptr<Message> MessageImpl::clone() const
{
    return std::make_unique<MessageImpl>(*this);
}
uint64_t MessageImpl::getId() const
{
    return _id;
}
const std::string& MessageImpl::getName() const
{
    return _name;
}
uint64_t MessageImpl::getMessageSize() const
{
    return _message_size;
}
const std::string& MessageImpl::getTransmitter() const
{
    return _transmitter;
}
bool MessageImpl::hasMessageTransmitter(const std::string& name) const
{
    return _message_transmitters.find(name) != _message_transmitters.end();
}
void MessageImpl::forEachMessageTransmitter(std::function<void(const std::string&)>&& cb) const
{
    for (const auto& n : _message_transmitters)
    {
        cb(n);
    }
}
const Signal* MessageImpl::getSignalByName(const std::string& name) const
{
    const Signal* result = nullptr;
    auto iter = _signals.find(name);
    if (iter != _signals.end())
    {
        result = &iter->second;
    }
    return result;
}
const Signal* MessageImpl::findSignal(std::function<bool(const Signal&)>&& pred) const
{
    const Signal* result = nullptr;
    for (const auto& s : _signals)
    {
        if (pred(s.second))
        {
            result = &s.second;
            break;
        }
    }
    return result;
}
void MessageImpl::forEachSignal(std::function<void(const Signal&)>&& cb) const
{
    for (const auto& s : _signals)
    {
        cb(s.second);
    }
}
const Attribute* MessageImpl::getAttributeValueByName(const std::string& name) const
{
    const Attribute* result = nullptr;
    auto iter = _attribute_values.find(name);
    if (iter != _attribute_values.end())
    {
        result = &iter->second;
    }
    return result;
}
const Attribute* MessageImpl::findAttributeValue(std::function<bool(const Attribute&)>&& pred) const
{
    const Attribute* result = nullptr;
    for (const auto& av : _attribute_values)
    {
        if (pred(av.second))
        {
            result = &av.second;
            break;
        }
    }
    return result;
}
void MessageImpl::forEachAttributeValue(std::function<void(const Attribute&)>&& cb) const
{
    for (const auto& av : _attribute_values)
    {
        cb(av.second);
    }
}
const std::string& MessageImpl::getComment() const
{
    return _comment;
}
const Signal* MessageImpl::getMuxSignal() const 
{
    return _mux_signal;
}
MessageImpl::ErrorCode MessageImpl::getError() const
{
    return _error;
}

const std::map<std::string, SignalImpl>& MessageImpl::signals() const
{
    return _signals;
}
