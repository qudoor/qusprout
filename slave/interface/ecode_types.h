/**
 * Autogenerated by Thrift Compiler (0.16.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef ecode_TYPES_H
#define ecode_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <functional>
#include <memory>




struct ErrCode {
  enum type {
    COM_SUCCESS = 0,
    COM_OTHRE = 1,
    COM_INVALID_PARAM = 2,
    COM_TIMEOUT = 3,
    COM_EXCEPTION = 4,
    QUROOT_MEM_NOT_ENOUGH = 100,
    QUROOT_NOT_INIT = 101,
    QUROOT_NOT_REGISTER = 102,
    QUROOT_NOT_RESOURCE = 103,
    QUROOT_PRASE_ERROR = 104
  };
};

extern const std::map<int, const char*> _ErrCode_VALUES_TO_NAMES;

std::ostream& operator<<(std::ostream& out, const ErrCode::type& val);

std::string to_string(const ErrCode::type& val);

class BaseCode;


class BaseCode : public virtual ::apache::thrift::TBase {
 public:

  BaseCode(const BaseCode&);
  BaseCode& operator=(const BaseCode&);
  BaseCode() noexcept
           : code(static_cast<ErrCode::type>(0)),
             msg() {
  }

  virtual ~BaseCode() noexcept;
  /**
   * 
   * @see ErrCode
   */
  ErrCode::type code;
  std::string msg;

  void __set_code(const ErrCode::type val);

  void __set_msg(const std::string& val);

  bool operator == (const BaseCode & rhs) const
  {
    if (!(code == rhs.code))
      return false;
    if (!(msg == rhs.msg))
      return false;
    return true;
  }
  bool operator != (const BaseCode &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const BaseCode & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(BaseCode &a, BaseCode &b);

std::ostream& operator<<(std::ostream& out, const BaseCode& obj);



#endif
