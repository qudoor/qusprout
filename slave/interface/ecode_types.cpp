/**
 * Autogenerated by Thrift Compiler (0.16.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "ecode_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>



int _kErrCodeValues[] = {
  ErrCode::COM_SUCCESS,
  ErrCode::COM_OTHRE,
  ErrCode::COM_INVALID_PARAM,
  ErrCode::COM_TIMEOUT,
  ErrCode::COM_EXCEPTION,
  ErrCode::QUROOT_MEM_NOT_ENOUGH,
  ErrCode::QUROOT_NOT_INIT,
  ErrCode::QUROOT_NOT_REGISTER,
  ErrCode::QUROOT_NOT_RESOURCE
};
const char* _kErrCodeNames[] = {
  "COM_SUCCESS",
  "COM_OTHRE",
  "COM_INVALID_PARAM",
  "COM_TIMEOUT",
  "COM_EXCEPTION",
  "QUROOT_MEM_NOT_ENOUGH",
  "QUROOT_NOT_INIT",
  "QUROOT_NOT_REGISTER",
  "QUROOT_NOT_RESOURCE"
};
const std::map<int, const char*> _ErrCode_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(9, _kErrCodeValues, _kErrCodeNames), ::apache::thrift::TEnumIterator(-1, nullptr, nullptr));

std::ostream& operator<<(std::ostream& out, const ErrCode::type& val) {
  std::map<int, const char*>::const_iterator it = _ErrCode_VALUES_TO_NAMES.find(val);
  if (it != _ErrCode_VALUES_TO_NAMES.end()) {
    out << it->second;
  } else {
    out << static_cast<int>(val);
  }
  return out;
}

std::string to_string(const ErrCode::type& val) {
  std::map<int, const char*>::const_iterator it = _ErrCode_VALUES_TO_NAMES.find(val);
  if (it != _ErrCode_VALUES_TO_NAMES.end()) {
    return std::string(it->second);
  } else {
    return std::to_string(static_cast<int>(val));
  }
}


BaseCode::~BaseCode() noexcept {
}


void BaseCode::__set_code(const ErrCode::type val) {
  this->code = val;
}

void BaseCode::__set_msg(const std::string& val) {
  this->msg = val;
}
std::ostream& operator<<(std::ostream& out, const BaseCode& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t BaseCode::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  bool isset_code = false;
  bool isset_msg = false;

  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast0;
          xfer += iprot->readI32(ecast0);
          this->code = static_cast<ErrCode::type>(ecast0);
          isset_code = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->msg);
          isset_msg = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  if (!isset_code)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  if (!isset_msg)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  return xfer;
}

uint32_t BaseCode::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("BaseCode");

  xfer += oprot->writeFieldBegin("code", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(static_cast<int32_t>(this->code));
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("msg", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->msg);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(BaseCode &a, BaseCode &b) {
  using ::std::swap;
  swap(a.code, b.code);
  swap(a.msg, b.msg);
}

BaseCode::BaseCode(const BaseCode& other1) {
  code = other1.code;
  msg = other1.msg;
}
BaseCode& BaseCode::operator=(const BaseCode& other2) {
  code = other2.code;
  msg = other2.msg;
  return *this;
}
void BaseCode::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "BaseCode(";
  out << "code=" << to_string(code);
  out << ", " << "msg=" << to_string(msg);
  out << ")";
}

