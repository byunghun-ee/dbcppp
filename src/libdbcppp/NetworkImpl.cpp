
#include <iomanip>
#include "../../include/dbcppp/Network.h"
#include "NetworkImpl.h"
#include "DBC_Grammar.h"

using namespace dbcppp;

std::unique_ptr<Network> Network::create(
      std::string&& version
    , std::set<std::string>&& new_symbols
    , std::unique_ptr<BitTiming>&& bit_timing
    , std::map<std::string, std::unique_ptr<Node>>&& nodes
    , std::map<std::string, std::unique_ptr<ValueTable>>&& value_tables
    , std::unordered_map<uint64_t, std::unique_ptr<Message>>&& messages
    , std::map<std::string, std::unique_ptr<EnvironmentVariable>>&& environment_variables
    , std::map<std::string, std::unique_ptr<AttributeDefinition>>&& attribute_definitions
    , std::map<std::string, std::unique_ptr<Attribute>>&& attribute_defaults
    , std::map<std::string, std::unique_ptr<Attribute>>&& attribute_values
    , std::string&& comment)
{
    BitTimingImpl bt = std::move(static_cast<BitTimingImpl&>(*bit_timing));
    bit_timing.reset(nullptr);
    std::map<std::string, NodeImpl> ns;
    std::map<std::string, ValueTableImpl> vts;
    tsl::robin_map<uint64_t, MessageImpl> ms;
    std::map<std::string, EnvironmentVariableImpl> evs;
    std::map<std::string, AttributeDefinitionImpl> ads;
    std::map<std::string, AttributeImpl> avds;
    std::map<std::string, AttributeImpl> avs;
    for (auto& n : nodes)
    {
        ns.insert(std::make_pair(n.first, std::move(static_cast<NodeImpl&>(*n.second))));
        n.second.reset(nullptr);
    }
    for (auto& vt : value_tables)
    {
        vts.insert(std::make_pair(vt.first, std::move(static_cast<ValueTableImpl&>(*vt.second))));
        vt.second.reset(nullptr);
    }
    for (auto& m : messages)
    {
        ms.insert(std::make_pair(m.first, std::move(static_cast<MessageImpl&>(*m.second))));
        m.second.reset(nullptr);
    }
    for (auto& ev : environment_variables)
    {
        evs.insert(std::make_pair(ev.first, std::move(static_cast<EnvironmentVariableImpl&>(*ev.second))));
        ev.second.reset(nullptr);
    }
    for (auto& ad : attribute_definitions)
    {
        ads.insert(std::make_pair(ad.first, std::move(static_cast<AttributeDefinitionImpl&>(*ad.second))));
        ad.second.reset(nullptr);
    }
    for (auto& ad : attribute_defaults)
    {
        avds.insert(std::make_pair(ad.first, std::move(static_cast<AttributeImpl&>(*ad.second))));
        ad.second.reset(nullptr);
    }
    for (auto& av : attribute_values)
    {
        avs.insert(std::make_pair(av.first, std::move(static_cast<AttributeImpl&>(*av.second))));
        av.second.reset(nullptr);
    }
    return std::make_unique<NetworkImpl>(
          std::move(version)
        , std::move(new_symbols)
        , std::move(bt)
        , std::move(ns)
        , std::move(vts)
        , std::move(ms)
        , std::move(evs)
        , std::move(ads)
        , std::move(avds)
        , std::move(avs)
        , std::move(comment));
}

NetworkImpl::NetworkImpl(
      std::string&& version
    , std::set<std::string>&& new_symbols
    , BitTimingImpl&& bit_timing
    , std::map<std::string, NodeImpl>&& nodes
    , std::map<std::string, ValueTableImpl>&& value_tables
    , tsl::robin_map<uint64_t, MessageImpl>&& messages
    , std::map<std::string, EnvironmentVariableImpl>&& environment_variables
    , std::map<std::string, AttributeDefinitionImpl>&& attribute_definitions
    , std::map<std::string, AttributeImpl>&& attribute_defaults
    , std::map<std::string, AttributeImpl>&& attribute_values
    , std::string&& comment)

    : _version(std::move(version))
    , _new_symbols(std::move(new_symbols))
    , _bit_timing(std::move(bit_timing))
    , _nodes(std::move(nodes))
    , _value_tables(std::move(value_tables))
    , _messages(std::move(messages))
    , _environment_variables(std::move(environment_variables))
    , _attribute_definitions(std::move(attribute_definitions))
    , _attribute_defaults(std::move(attribute_defaults))
    , _attribute_values(std::move(attribute_values))
    , _comment(std::move(comment))
{}
std::unique_ptr<Network> NetworkImpl::clone() const
{
    return std::make_unique<NetworkImpl>(*this);
}
const std::string& NetworkImpl::getVersion() const
{
    return _version;
}
bool NetworkImpl::hasNewSymbol(const std::string& name) const
{
    return std::find(_new_symbols.begin(), _new_symbols.end(), name) != _new_symbols.end();
}
void NetworkImpl::forEachNewSymbol(std::function<void(const std::string&)> cb) const
{
    for (const auto& ns : _new_symbols)
    {
        cb(ns);
    }
}
const BitTiming& NetworkImpl::getBitTiming() const
{
    return _bit_timing;
}
const Node* NetworkImpl::getNodeByName(const std::string& name) const
{
    const Node* result = nullptr;
    auto iter = _nodes.find(name);
    if (iter != _nodes.end())
    {
        result = &iter->second;
    }
    return result;
}
const Node* NetworkImpl::findNode(std::function<bool(const Node&)>&& pred) const
{
    const Node* result = nullptr;
    for (const auto& n : _nodes)
    {
        if (pred(n.second))
        {
            result = &n.second;
            break;
        }
    }
    return result;
}
void NetworkImpl::forEachNode(std::function<void(const Node&)>&& cb) const
{
    for (const auto& n : _nodes)
    {
        cb(n.second);
    }
}
const ValueTable* NetworkImpl::getValueTableByName(const std::string& name) const
{
    const ValueTable* result = nullptr;
    auto iter = _value_tables.find(name);
    if (iter != _value_tables.end())
    {
        result = &iter->second;
    }
    return result;
}
const ValueTable* NetworkImpl::findValueTable(std::function<bool(const ValueTable&)>&& pred) const
{
    const ValueTable* result = nullptr;
    for (const auto& vt : _value_tables)
    {
        if (pred(vt.second))
        {
            result = &vt.second;
            break;
        }
    }
    return result;
}
void NetworkImpl::forEachValueTable(std::function<void(const ValueTable&)>&& cb) const
{
    for (const auto& vt : _value_tables)
    {
        cb(vt.second);
    }
}
const Message* NetworkImpl::getMessageById(uint64_t id) const
{
    const Message* result = nullptr;
    auto iter = _messages.find(id);
    if (iter != _messages.end())
    {
        result = &iter->second;
    }
    return result;
}
const Message* NetworkImpl::findMessage(std::function<bool(const Message&)>&& pred) const
{
    const Message* result = nullptr;
    for (const auto& m : _messages)
    {
        if (pred(m.second))
        {
            result = &m.second;
            break;
        }
    }
    return result;
}
void NetworkImpl::forEachMessage(std::function<void(const Message&)>&& cb) const
{
    for (const auto& m : _messages)
    {
        cb(m.second);
    }
}
const EnvironmentVariable* NetworkImpl::getEnvironmentVariableByName(const std::string& name) const
{
    const EnvironmentVariable* result = nullptr;
    auto iter = _environment_variables.find(name);
    if (iter != _environment_variables.end())
    {
        result = &iter->second;
    }
    return result;
}
const EnvironmentVariable* NetworkImpl::findEnvironmentVariable(std::function<bool(const EnvironmentVariable&)>&& pred) const
{
    const EnvironmentVariable* result = nullptr;
    for (const auto& ev : _environment_variables)
    {
        if (pred(ev.second))
        {
            result = &ev.second;
            break;
        }
    }
    return result;
}
void NetworkImpl::forEachEnvironmentVariable(std::function<void(const EnvironmentVariable&)>&& cb) const
{
    for (const auto& ev : _environment_variables)
    {
        cb(ev.second);
    }
}
const AttributeDefinition* NetworkImpl::getAttributeDefinitionByName(const std::string& name) const
{
    const AttributeDefinition* result = nullptr;
    auto iter = _attribute_definitions.find(name);
    if (iter != _attribute_definitions.end())
    {
        result = &iter->second;
    }
    return result;
}
const AttributeDefinition* NetworkImpl::findAttributeDefinition(std::function<bool(const AttributeDefinition&)>&& pred) const
{
    const AttributeDefinition* result = nullptr;
    for (const auto& ad : _attribute_definitions)
    {
        if (pred(ad.second))
        {
            result = &ad.second;
            break;
        }
    }
    return result;
}
void NetworkImpl::forEachAttributeDefinition(std::function<void(const AttributeDefinition&)>&& cb) const
{
    for (const auto& ad : _attribute_definitions)
    {
        cb(ad.second);
    }
}
const Attribute* NetworkImpl::getAttributeDefaultByName(const std::string& name) const
{
    const Attribute* result = nullptr;
    auto iter = _attribute_defaults.find(name);
    if (iter != _attribute_defaults.end())
    {
        result = &iter->second;
    }
    return result;
}
const Attribute* NetworkImpl::findAttributeDefault(std::function<bool(const Attribute&)>&& pred) const
{
    const Attribute* result = nullptr;
    for (const auto& ad : _attribute_defaults)
    {
        if (pred(ad.second))
        {
            result = &ad.second;
            break;
        }
    }
    return result;
}
void NetworkImpl::forEachAttributeDefault(std::function<void(const Attribute&)>&& cb) const
{
    for (const auto& ad : _attribute_defaults)
    {
        cb(ad.second);
    }
}
const Attribute* NetworkImpl::getAttributeValueByName(const std::string& name) const
{
    const Attribute* result = nullptr;
    auto iter = _attribute_values.find(name);
    if (iter != _attribute_values.end())
    {
        result = &iter->second;
    }
    return result;
}
const Attribute* NetworkImpl::findAttributeValue(std::function<bool(const Attribute&)>&& pred) const
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
void NetworkImpl::forEachAttributeValue(std::function<void(const Attribute&)>&& cb) const
{
    for (const auto& av : _attribute_values)
    {
        cb(av.second);
    }
}
const std::string& NetworkImpl::getComment() const
{
    return _comment;
}
const Message* NetworkImpl::findParentMessage(const Signal* sig) const
{
    const Message* result = nullptr;
    for (const auto& p : _messages)
    {
        const MessageImpl& msg = p.second;
        auto iter = msg.signals().find(sig->getName());
        if (iter != msg.signals().end() && &iter->second == sig)
        {
            result = &msg;
            break;
        }
    }
    return result;
}
std::string& NetworkImpl::version()
{
    return _version;
}
std::set<std::string>& NetworkImpl::newSymbols()
{
    return _new_symbols;
}
BitTimingImpl& NetworkImpl::bitTiming()
{
    return _bit_timing;
}
std::map<std::string, NodeImpl>& NetworkImpl::nodes()
{
    return _nodes;
}
std::map<std::string, ValueTableImpl>& NetworkImpl::valueTables()
{
    return _value_tables;
}
tsl::robin_map<uint64_t, MessageImpl>& NetworkImpl::messages()
{
    return _messages;
}
std::map<std::string, EnvironmentVariableImpl>& NetworkImpl::environmentVariables()
{
    return _environment_variables;
}
std::map<std::string, AttributeDefinitionImpl>& NetworkImpl::attributeDefinitions()
{
    return _attribute_definitions;
}
std::map<std::string, AttributeImpl>& NetworkImpl::attributeDefaults()
{
    return _attribute_defaults;
}
std::map<std::string, AttributeImpl>& NetworkImpl::attributeValues()
{
    return _attribute_values;
}
std::string& NetworkImpl::comment()
{
    return _comment;
}
void Network::merge(std::unique_ptr<Network>&& other)
{
    auto& self = static_cast<NetworkImpl&>(*this);
    auto& o = static_cast<NetworkImpl&>(*other);
    for (auto& ns : o.newSymbols())
    {
        self.newSymbols().insert(std::move(ns));
    }
    for (auto& n : o.nodes())
    {
        self.nodes().insert(std::move(n));
    }
    for (auto& vt : o.valueTables())
    {
        self.valueTables().insert(std::move(vt));
    }
    for (auto& m : o.messages())
    {
        self.messages().insert(std::move(m));
    }
    for (auto& ev : o.environmentVariables())
    {
        self.environmentVariables().insert(std::move(ev));
    }
    for (auto& ad : o.attributeDefinitions())
    {
        self.attributeDefinitions().insert(std::move(ad));
    }
    for (auto& ad : o.attributeDefaults())
    {
        self.attributeDefaults().insert(std::move(ad));
    }
    for (auto& av : o.attributeValues())
    {
        self.attributeValues().insert(std::move(av));
    }
    other.reset(nullptr);
}
std::map<std::string, std::unique_ptr<Network>> Network::fromFile(const std::string& filename)
{
    auto result = std::map<std::string, std::unique_ptr<Network>>();
    auto ending = filename.substr(filename.size() - 3, 3);
    auto is = std::ifstream(filename);
    if (ending == "dbc")
    {
        result.insert(std::make_pair("", fromDBC(is)));
    }
    else if (ending == "kcd")
    {
        result = fromKCD(is);
    }
    return std::move(result);
}
