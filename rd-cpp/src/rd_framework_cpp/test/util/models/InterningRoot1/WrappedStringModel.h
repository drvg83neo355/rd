#ifndef WrappedStringModel_H
#define WrappedStringModel_H

#include "Buffer.h"
#include "Identities.h"
#include "MessageBroker.h"
#include "Protocol.h"
#include "RdId.h"
#include "RdList.h"
#include "RdMap.h"
#include "RdProperty.h"
#include "RdSet.h"
#include "RdSignal.h"
#include "RName.h"
#include "ISerializable.h"
#include "Polymorphic.h"
#include "NullableSerializer.h"
#include "ArraySerializer.h"
#include "InternedSerializer.h"
#include "SerializationCtx.h"
#include "Serializers.h"
#include "ISerializersOwner.h"
#include "IUnknownInstance.h"
#include "RdExtBase.h"
#include "RdCall.h"
#include "RdEndpoint.h"
#include "RdTask.h"
#include "gen_util.h"

#include <iostream>
#include <cstring>
#include <cstdint>
#include <vector>
#include <type_traits>
#include <utility>

#include "optional.hpp"


#pragma warning( push )
#pragma warning( disable:4250 )
#pragma warning( disable:4307 )
namespace rd {
    namespace test {
        namespace util {
            
            //data
            class WrappedStringModel : public rd::IPolymorphicSerializable
            {
                
                //companion
                
                //custom serializers
                private:
                
                //fields
                protected:
                rd::Wrapper<std::wstring> text_;
                
                
                //initializer
                private:
                void initialize();
                
                //primary ctor
                public:
                WrappedStringModel(rd::Wrapper<std::wstring> text_);
                
                //secondary constructor
                
                //default ctors and dtors
                
                WrappedStringModel() = delete;
                
                WrappedStringModel(WrappedStringModel const &) = default;
                
                WrappedStringModel& operator=(WrappedStringModel const &) = default;
                
                WrappedStringModel(WrappedStringModel &&) = default;
                
                WrappedStringModel& operator=(WrappedStringModel &&) = default;
                
                virtual ~WrappedStringModel() = default;
                
                //reader
                static WrappedStringModel read(rd::SerializationCtx const& ctx, rd::Buffer const & buffer);
                
                //writer
                void write(rd::SerializationCtx const& ctx, rd::Buffer const& buffer) const override;
                
                //virtual init
                
                //identify
                
                //getters
                std::wstring const & get_text() const;
                
                //intern
                
                //equals trait
                private:
                bool equals(rd::ISerializable const& object) const override;
                
                //equality operators
                public:
                friend bool operator==(const WrappedStringModel &lhs, const WrappedStringModel &rhs);
                friend bool operator!=(const WrappedStringModel &lhs, const WrappedStringModel &rhs);
                
                //hash code trait
                size_t hashCode() const override;
                
                //type name trait
                std::string type_name() const override;
                
                //static type name trait
                static std::string static_type_name();
            };
        };
    };
};

#pragma warning( pop )


//hash code trait
namespace std {
    template <> struct hash<rd::test::util::WrappedStringModel> {
        size_t operator()(const rd::test::util::WrappedStringModel & value) const {
            return value.hashCode();
        }
    };
}

#endif // WrappedStringModel_H