
#pragma once

#include <map>
#include <memory>

#include <robin-map/tsl/robin_map.h>
#include "../../include/dbcppp/ValueTable.h"
#include "SignalTypeImpl.h"

namespace dbcppp
{
    class ValueTableImpl final
        : public ValueTable
    {
    public:
        ValueTableImpl(
              std::string&& name
            , boost::optional<SignalTypeImpl>&& signal_type
            , tsl::robin_map<int64_t, std::string>&& value_encoding_descriptions);

        virtual std::unique_ptr<ValueTable> clone() const override;

        virtual const std::string& getName() const override;
        virtual boost::optional<const SignalType&> getSignalType() const override;
        virtual const std::string* getvalueEncodingDescriptionByValue(int64_t value) const override;
        virtual void forEachValueEncodingDescription(std::function<void(int64_t, const std::string&)>&& cb) const override;

    private:
        std::string _name;
        boost::optional<SignalTypeImpl> _signal_type;
        tsl::robin_map<int64_t, std::string> _value_encoding_descriptions;
    };
}
