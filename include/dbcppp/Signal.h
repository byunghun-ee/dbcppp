
#pragma once

#include <set>
#include <map>
#include <memory>
#include <string>
#include <cstddef>
#include <boost/optional.hpp>

#include "Export.h"
#include "Node.h"
#include "Attribute.h"


namespace dbcppp
{
    class DBCPPP_API Signal
    {
    public:
        enum class ErrorCode
            : uint64_t
        {
            NoError,
            MaschinesFloatEncodingNotSupported = 1,
            MaschinesDoubleEncodingNotSupported = 2,
            SignalExceedsMessageSize = 4,
            WrongBitSizeForExtendedDataType = 8
        };
        enum class Multiplexer
        {
            NoMux, MuxSwitch, MuxValue
        };
        enum class ByteOrder
        {
            LittleEndian, BigEndian
        };
        enum class ValueType
        {
            Signed, Unsigned
        };
        enum class ExtendedValueType
        {
            Integer, Float, Double
        };
        
        static std::unique_ptr<Signal> create(
              uint64_t message_size
            , std::string&& name
            , Multiplexer multiplexer_indicator
            , uint64_t multiplexer_switch_value
            , uint64_t start_bit
            , uint64_t bit_size
            , ByteOrder byte_order
            , ValueType value_type
            , double factor
            , double offset
            , double minimum
            , double maximum
            , std::string&& unit
            , std::set<std::string>&& receivers
            , std::map<std::string, std::unique_ptr<Attribute>>&& attribute_values
            , std::unordered_map<int64_t, std::string>&& value_descriptions
            , std::string&& comment
            , ExtendedValueType extended_value_type);
            
        virtual std::unique_ptr<Signal> clone() const = 0;

        virtual ~Signal() = default;
        virtual const std::string& getName() const = 0;
        virtual Multiplexer getMultiplexerIndicator() const = 0;
        virtual uint64_t getMultiplexerSwitchValue() const = 0;
        virtual uint64_t getStartBit() const = 0;
        virtual uint64_t getBitSize() const = 0;
        virtual ByteOrder getByteOrder() const = 0;
        virtual ValueType getValueType() const = 0;
        virtual double getFactor() const = 0;
        virtual double getOffset() const = 0;
        virtual double getMinimum() const = 0;
        virtual double getMaximum() const = 0;
        virtual std::string getUnit() const = 0;
        virtual bool hasReceiver(const std::string& name) const = 0;
        virtual void forEachReceiver(std::function<void(const std::string&)>&& cb) const = 0;
        virtual const std::string* getValueDescriptionByValue(int64_t value) const = 0;
        virtual void forEachValueDescription(std::function<void(int64_t, const std::string&)>&& cb) const = 0;
        virtual const Attribute* getAttributeValueByName(const std::string& name) const = 0;
        virtual const Attribute* findAttributeValue(std::function<bool(const Attribute&)>&& pred) const = 0;
        virtual const void forEachAttributeValue(std::function<void(const Attribute&)>&& cb) const = 0;
        virtual const std::string& getComment() const = 0;
        virtual ExtendedValueType getExtendedValueType() const = 0;
        virtual bool getError(ErrorCode code) const = 0;
        
        /// \brief Extracts the raw value from a given n byte array
        ///
        /// This function uses a optimized method of reversing the byte order and extracting
        /// the value from the given data.
        /// !!! Note: This function takes at least 8 bytes and at least as many as the signal needs to be represented,
        ///     if you pass less, the program ends up in undefined behaviour! !!!
        ///
        /// @param nbyte a n byte array (!!! at least 8 bytes !!!) which is representing the can data.
        ///               the data must be in this order:
        ///               bit_0  - bit_7:  bytes[0]
        ///               bit_8  - bit_15: bytes[1]
        ///               ...
        ///               bit_n-7 - bit_n: bytes[n / 8]
        ///               (like the Unix CAN frame does store the data)
        using raw_t = uint64_t;
        inline raw_t decode(const void* bytes) const noexcept { return _decode(this, bytes); }
        inline void encode(raw_t raw, void* buffer) const noexcept { return _encode(this, raw, buffer); }

        inline double rawToPhys(raw_t raw) const { return _raw_to_phys(this, raw); }
        inline raw_t physToRaw(double phys) const { return _phys_to_raw(this, phys); }

    protected:
        // instead of using virtuals dynamic dispatching use function pointers
        raw_t (*_decode)(const Signal* sig, const void* bytes) noexcept {nullptr};
        void (*_encode)(const Signal* sig, raw_t raw, void* buffer) noexcept {nullptr};
        double (*_raw_to_phys)(const Signal* sig, raw_t raw) noexcept {nullptr};
        raw_t (*_phys_to_raw)(const Signal* sig, double phys) noexcept {nullptr};
    };
}
