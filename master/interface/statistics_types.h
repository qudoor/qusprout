/**
 * Autogenerated by Thrift Compiler (0.16.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef statistics_TYPES_H
#define statistics_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <functional>
#include <memory>
#include "ecode_types.h"
#include "qusproutdata_types.h"




struct SisGpuType {
  enum type {
    SisGpuType_Default = 0,
    SisGpuType_Nvidia = 1
  };
};

extern const std::map<int, const char*> _SisGpuType_VALUES_TO_NAMES;

std::ostream& operator<<(std::ostream& out, const SisGpuType::type& val);

std::string to_string(const SisGpuType::type& val);

class BaseTag;

class QubitTag;

class InterfaceTag;

class SisAllTaskNum;

class SisCurrTaskNum;

class SisInterfaceCount;

class SisInterfaceTime;

class SisCodeCount;

class MachineFixedInfo;

class MachineDynamicInfo;

class StatisticsInfo;


class BaseTag : public virtual ::apache::thrift::TBase {
 public:

  BaseTag(const BaseTag&);
  BaseTag& operator=(const BaseTag&);
  BaseTag() noexcept
          : hostname(),
            type(static_cast< ::ExecCmdType::type>(0)) {
  }

  virtual ~BaseTag() noexcept;
  std::string hostname;
  /**
   * 
   * @see .ExecCmdType
   */
   ::ExecCmdType::type type;

  void __set_hostname(const std::string& val);

  void __set_type(const  ::ExecCmdType::type val);

  bool operator == (const BaseTag & rhs) const
  {
    if (!(hostname == rhs.hostname))
      return false;
    if (!(type == rhs.type))
      return false;
    return true;
  }
  bool operator != (const BaseTag &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const BaseTag & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(BaseTag &a, BaseTag &b);

std::ostream& operator<<(std::ostream& out, const BaseTag& obj);


class QubitTag : public virtual ::apache::thrift::TBase {
 public:

  QubitTag(const QubitTag&);
  QubitTag& operator=(const QubitTag&);
  QubitTag() noexcept
           : qubits(0) {
  }

  virtual ~QubitTag() noexcept;
  BaseTag base;
  int32_t qubits;

  void __set_base(const BaseTag& val);

  void __set_qubits(const int32_t val);

  bool operator == (const QubitTag & rhs) const
  {
    if (!(base == rhs.base))
      return false;
    if (!(qubits == rhs.qubits))
      return false;
    return true;
  }
  bool operator != (const QubitTag &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const QubitTag & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(QubitTag &a, QubitTag &b);

std::ostream& operator<<(std::ostream& out, const QubitTag& obj);


class InterfaceTag : public virtual ::apache::thrift::TBase {
 public:

  InterfaceTag(const InterfaceTag&);
  InterfaceTag& operator=(const InterfaceTag&);
  InterfaceTag() noexcept
               : interface_name(),
                 qubits(0) {
  }

  virtual ~InterfaceTag() noexcept;
  BaseTag base;
  std::string interface_name;
  int32_t qubits;

  void __set_base(const BaseTag& val);

  void __set_interface_name(const std::string& val);

  void __set_qubits(const int32_t val);

  bool operator == (const InterfaceTag & rhs) const
  {
    if (!(base == rhs.base))
      return false;
    if (!(interface_name == rhs.interface_name))
      return false;
    if (!(qubits == rhs.qubits))
      return false;
    return true;
  }
  bool operator != (const InterfaceTag &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const InterfaceTag & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(InterfaceTag &a, InterfaceTag &b);

std::ostream& operator<<(std::ostream& out, const InterfaceTag& obj);


class SisAllTaskNum : public virtual ::apache::thrift::TBase {
 public:

  SisAllTaskNum(const SisAllTaskNum&);
  SisAllTaskNum& operator=(const SisAllTaskNum&);
  SisAllTaskNum() noexcept
                : num(0) {
  }

  virtual ~SisAllTaskNum() noexcept;
  QubitTag tag;
  int32_t num;

  void __set_tag(const QubitTag& val);

  void __set_num(const int32_t val);

  bool operator == (const SisAllTaskNum & rhs) const
  {
    if (!(tag == rhs.tag))
      return false;
    if (!(num == rhs.num))
      return false;
    return true;
  }
  bool operator != (const SisAllTaskNum &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SisAllTaskNum & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(SisAllTaskNum &a, SisAllTaskNum &b);

std::ostream& operator<<(std::ostream& out, const SisAllTaskNum& obj);


class SisCurrTaskNum : public virtual ::apache::thrift::TBase {
 public:

  SisCurrTaskNum(const SisCurrTaskNum&);
  SisCurrTaskNum& operator=(const SisCurrTaskNum&);
  SisCurrTaskNum() noexcept
                 : num(0),
                   timeout_num(0) {
  }

  virtual ~SisCurrTaskNum() noexcept;
  QubitTag tag;
  int32_t num;
  int32_t timeout_num;

  void __set_tag(const QubitTag& val);

  void __set_num(const int32_t val);

  void __set_timeout_num(const int32_t val);

  bool operator == (const SisCurrTaskNum & rhs) const
  {
    if (!(tag == rhs.tag))
      return false;
    if (!(num == rhs.num))
      return false;
    if (!(timeout_num == rhs.timeout_num))
      return false;
    return true;
  }
  bool operator != (const SisCurrTaskNum &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SisCurrTaskNum & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(SisCurrTaskNum &a, SisCurrTaskNum &b);

std::ostream& operator<<(std::ostream& out, const SisCurrTaskNum& obj);


class SisInterfaceCount : public virtual ::apache::thrift::TBase {
 public:

  SisInterfaceCount(const SisInterfaceCount&);
  SisInterfaceCount& operator=(const SisInterfaceCount&);
  SisInterfaceCount() noexcept
                    : count(0) {
  }

  virtual ~SisInterfaceCount() noexcept;
  InterfaceTag tag;
  int32_t count;

  void __set_tag(const InterfaceTag& val);

  void __set_count(const int32_t val);

  bool operator == (const SisInterfaceCount & rhs) const
  {
    if (!(tag == rhs.tag))
      return false;
    if (!(count == rhs.count))
      return false;
    return true;
  }
  bool operator != (const SisInterfaceCount &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SisInterfaceCount & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(SisInterfaceCount &a, SisInterfaceCount &b);

std::ostream& operator<<(std::ostream& out, const SisInterfaceCount& obj);


class SisInterfaceTime : public virtual ::apache::thrift::TBase {
 public:

  SisInterfaceTime(const SisInterfaceTime&);
  SisInterfaceTime& operator=(const SisInterfaceTime&);
  SisInterfaceTime() noexcept
                   : elapsed(0),
                     count(0) {
  }

  virtual ~SisInterfaceTime() noexcept;
  InterfaceTag tag;
  int32_t elapsed;
  int32_t count;

  void __set_tag(const InterfaceTag& val);

  void __set_elapsed(const int32_t val);

  void __set_count(const int32_t val);

  bool operator == (const SisInterfaceTime & rhs) const
  {
    if (!(tag == rhs.tag))
      return false;
    if (!(elapsed == rhs.elapsed))
      return false;
    if (!(count == rhs.count))
      return false;
    return true;
  }
  bool operator != (const SisInterfaceTime &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SisInterfaceTime & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(SisInterfaceTime &a, SisInterfaceTime &b);

std::ostream& operator<<(std::ostream& out, const SisInterfaceTime& obj);


class SisCodeCount : public virtual ::apache::thrift::TBase {
 public:

  SisCodeCount(const SisCodeCount&);
  SisCodeCount& operator=(const SisCodeCount&);
  SisCodeCount() noexcept
               : code(0),
                 count(0) {
  }

  virtual ~SisCodeCount() noexcept;
  InterfaceTag tag;
  int32_t code;
  int32_t count;

  void __set_tag(const InterfaceTag& val);

  void __set_code(const int32_t val);

  void __set_count(const int32_t val);

  bool operator == (const SisCodeCount & rhs) const
  {
    if (!(tag == rhs.tag))
      return false;
    if (!(code == rhs.code))
      return false;
    if (!(count == rhs.count))
      return false;
    return true;
  }
  bool operator != (const SisCodeCount &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SisCodeCount & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(SisCodeCount &a, SisCodeCount &b);

std::ostream& operator<<(std::ostream& out, const SisCodeCount& obj);

typedef struct _MachineFixedInfo__isset {
  _MachineFixedInfo__isset() : gpu_type(false), gpu_memory(false) {}
  bool gpu_type :1;
  bool gpu_memory :1;
} _MachineFixedInfo__isset;

class MachineFixedInfo : public virtual ::apache::thrift::TBase {
 public:

  MachineFixedInfo(const MachineFixedInfo&) noexcept;
  MachineFixedInfo& operator=(const MachineFixedInfo&) noexcept;
  MachineFixedInfo() noexcept
                   : cpu_num(0),
                     cpu_memory(0),
                     gpu_type(static_cast<SisGpuType::type>(0)),
                     gpu_memory(0) {
  }

  virtual ~MachineFixedInfo() noexcept;
  int32_t cpu_num;
  int64_t cpu_memory;
  /**
   * 
   * @see SisGpuType
   */
  SisGpuType::type gpu_type;
  int64_t gpu_memory;

  _MachineFixedInfo__isset __isset;

  void __set_cpu_num(const int32_t val);

  void __set_cpu_memory(const int64_t val);

  void __set_gpu_type(const SisGpuType::type val);

  void __set_gpu_memory(const int64_t val);

  bool operator == (const MachineFixedInfo & rhs) const
  {
    if (!(cpu_num == rhs.cpu_num))
      return false;
    if (!(cpu_memory == rhs.cpu_memory))
      return false;
    if (__isset.gpu_type != rhs.__isset.gpu_type)
      return false;
    else if (__isset.gpu_type && !(gpu_type == rhs.gpu_type))
      return false;
    if (__isset.gpu_memory != rhs.__isset.gpu_memory)
      return false;
    else if (__isset.gpu_memory && !(gpu_memory == rhs.gpu_memory))
      return false;
    return true;
  }
  bool operator != (const MachineFixedInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MachineFixedInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(MachineFixedInfo &a, MachineFixedInfo &b);

std::ostream& operator<<(std::ostream& out, const MachineFixedInfo& obj);

typedef struct _MachineDynamicInfo__isset {
  _MachineDynamicInfo__isset() : gpu_used_memory(false) {}
  bool gpu_used_memory :1;
} _MachineDynamicInfo__isset;

class MachineDynamicInfo : public virtual ::apache::thrift::TBase {
 public:

  MachineDynamicInfo(const MachineDynamicInfo&) noexcept;
  MachineDynamicInfo& operator=(const MachineDynamicInfo&) noexcept;
  MachineDynamicInfo() noexcept
                     : cpu_used_memory(0),
                       gpu_used_memory(0) {
  }

  virtual ~MachineDynamicInfo() noexcept;
  int64_t cpu_used_memory;
  int64_t gpu_used_memory;

  _MachineDynamicInfo__isset __isset;

  void __set_cpu_used_memory(const int64_t val);

  void __set_gpu_used_memory(const int64_t val);

  bool operator == (const MachineDynamicInfo & rhs) const
  {
    if (!(cpu_used_memory == rhs.cpu_used_memory))
      return false;
    if (__isset.gpu_used_memory != rhs.__isset.gpu_used_memory)
      return false;
    else if (__isset.gpu_used_memory && !(gpu_used_memory == rhs.gpu_used_memory))
      return false;
    return true;
  }
  bool operator != (const MachineDynamicInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MachineDynamicInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(MachineDynamicInfo &a, MachineDynamicInfo &b);

std::ostream& operator<<(std::ostream& out, const MachineDynamicInfo& obj);


class StatisticsInfo : public virtual ::apache::thrift::TBase {
 public:

  StatisticsInfo(const StatisticsInfo&);
  StatisticsInfo& operator=(const StatisticsInfo&);
  StatisticsInfo() noexcept
                 : begin_time(0),
                   end_time(0) {
  }

  virtual ~StatisticsInfo() noexcept;
  MachineFixedInfo fixed_info;
  MachineDynamicInfo dyna_info;
  std::vector<SisCurrTaskNum>  curr_task_num;
  std::map<std::string, SisAllTaskNum>  all_task_num;
  std::map<std::string, SisInterfaceCount>  inter_cout;
  std::map<std::string, SisInterfaceTime>  inter_time;
  std::map<std::string, SisCodeCount>  code_count;
  int64_t begin_time;
  int64_t end_time;

  void __set_fixed_info(const MachineFixedInfo& val);

  void __set_dyna_info(const MachineDynamicInfo& val);

  void __set_curr_task_num(const std::vector<SisCurrTaskNum> & val);

  void __set_all_task_num(const std::map<std::string, SisAllTaskNum> & val);

  void __set_inter_cout(const std::map<std::string, SisInterfaceCount> & val);

  void __set_inter_time(const std::map<std::string, SisInterfaceTime> & val);

  void __set_code_count(const std::map<std::string, SisCodeCount> & val);

  void __set_begin_time(const int64_t val);

  void __set_end_time(const int64_t val);

  bool operator == (const StatisticsInfo & rhs) const
  {
    if (!(fixed_info == rhs.fixed_info))
      return false;
    if (!(dyna_info == rhs.dyna_info))
      return false;
    if (!(curr_task_num == rhs.curr_task_num))
      return false;
    if (!(all_task_num == rhs.all_task_num))
      return false;
    if (!(inter_cout == rhs.inter_cout))
      return false;
    if (!(inter_time == rhs.inter_time))
      return false;
    if (!(code_count == rhs.code_count))
      return false;
    if (!(begin_time == rhs.begin_time))
      return false;
    if (!(end_time == rhs.end_time))
      return false;
    return true;
  }
  bool operator != (const StatisticsInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const StatisticsInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot) override;
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const override;

  virtual void printTo(std::ostream& out) const;
};

void swap(StatisticsInfo &a, StatisticsInfo &b);

std::ostream& operator<<(std::ostream& out, const StatisticsInfo& obj);



#endif
