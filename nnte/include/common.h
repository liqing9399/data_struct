/***************************************************
#filename      : common.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-26 15:26:26
#last modified : 2023-06-06 10:48:50
#description   : NA
***************************************************/

#ifndef NNTE_INCLUDE_COMMON_H_
#define NNTE_INCLUDE_COMMON_H_

#include <cstdint>
#include <vector>
#include <map>
#include <numeric>
#include <iostream>
#include <cassert>
#include <string>
#include <functional>
#include <set>

#include "./nnte_utils.h"
#include "./logger.h"
#include "./platform.h"

namespace nnte {

typedef enum class Axis {
  DIMN = 0,
  DIMC = 1,
  DIMH = 2,
  DIMW = 3,
} Axis;

typedef enum class Layout {
  LNONE=0,
  NCHW,
  NHWC,
  HW,
} Layout;

typedef enum class Datatype {
  FLOAT8 = 0,
  FLOAT16,
  FLOAT32,
  FLOAT64,
  INT8,
  INT16,
  INT32,
  UINT8,
  UINT16,
  UINT32
} Datatype;

short SizeOf(Datatype dtype) {
  switch (dtype) {
    case Datatype::INT8:
    case Datatype::UINT8:
    case Datatype::FLOAT8:
      return 1;
    case Datatype::INT16:
    case Datatype::UINT16:
    case Datatype::FLOAT16:
      return 2;
    case Datatype::INT32:
    case Datatype::UINT32:
    case Datatype::FLOAT32:
      return 4;
    default:
      return 0;
  }
}

typedef enum class ActivationMode {
  AM_NONE = 0,
  AM_RELU,
  AM_GELU,
} ActiveMode;

#define V2S(val)  #val
std::map<ActivationMode, std::string> act_str = {
  {ActivationMode::AM_NONE, V2S(AM_NONE)},
  {ActivationMode::AM_RELU, V2S(AM_RELU)},
  {ActivationMode::AM_GELU, V2S(AM_GELU)},
};

std::string ActivationMode2String(ActivationMode act) {
  return act_str[act];
};

typedef std::vector<int64_t> Dims;
// typedef std::vector<int64_t> AxisVector;


#define MAX_DIM_NUM (8)

std::map<Layout, std::map<Axis, int>> axis_to_di {
    {Layout::LNONE, {
      {Axis::DIMN, 0},
      {Axis::DIMC, 0},
      {Axis::DIMH, 0},
      {Axis::DIMW, 1},
    }},
    {Layout::NCHW, {
      {Axis::DIMN, 0},
      {Axis::DIMC, 1},
      {Axis::DIMH, 2},
      {Axis::DIMW, 3},
    }},
    {Layout::NHWC, {
      {Axis::DIMN, 0},
      {Axis::DIMH, 1},
      {Axis::DIMW, 2},
      {Axis::DIMC, 3},
    }},
    {Layout::HW, {
      {Axis::DIMH, 0},
      {Axis::DIMW, 1},
    }},
};

class Shape {
 public:
  Shape(Dims ds, Layout layout) : dims_(ds), layout_(layout) {UpdateCounts();}
  Shape(const Shape &rhs) : dims_(rhs.dims_), layout_(rhs.layout_) {
    UpdateCounts();
  }
  Shape &operator=(const Shape &rhs) {
    if (this == &rhs) return *this;
    this->dims_ = rhs.dims_;
    this->layout_ = rhs.layout_;
    UpdateCounts();
    return *this;
  }

  Dims GetDims() {
    return dims_;
  }

  Layout GetLayout() { return layout_; }

  int GetDim(int index) {
    // LOG(I) <<"index: " << index << ", dims.size(): "<< dims_.size();
    if (index > dims_.size()) {
      std::cout << "index out range:size = " <<dims_.size() << ", index: " << index << std::endl;
      assert(0);
    }
    return dims_[index];
  }

  int GetDim(Axis dim) {
    if (layout_ == Layout::LNONE) {
      LOG(W) << "this tensor layout is Layout::LNONE, it's can not be use Axis to get dimension number. Axis: "
             << (int)dim;
    }
    int di = axis_to_di[layout_][dim];
    return GetDim(di);
  }

  size_t GetCount() {
    return elem_cnt_;
  }

  int GetDimNum() { return dims_.size(); }

  int GetLastDimCount() {return dims_[(dims_.size() - 1)];};

  int GetExcludeLastDimCount() {return std::accumulate(dims_.begin(), dims_.end() - 1, 1, std::multiplies<int>());};

  // int GetMacW() {};
  // int GetMacH() {};


 private:
  void UpdateCounts() {
    elem_cnt_ = std::accumulate(dims_.begin(), dims_.end(), 1, std::multiplies<int>());
  }

  Dims dims_;
  Layout layout_;
  int64_t elem_cnt_;
};

static int name_i = 0;
static int dup_i = 0; // duplicate number

class Tensor {
 public:
  ~Tensor() {
    FreePtr(&host_ddr_);
  }

  static Sptr<Tensor> Create(Shape s,
                             Datatype dt = Datatype::FLOAT16,
                             std::string tensor_name = "") {
    static std::set<std::string> tns; // tensor name set.
    if (tensor_name == "") {
      tensor_name = "tensor" + std::to_string(name_i++);
    }
    if (tns.find(tensor_name) != tns.end()) {
      tensor_name += "dup" + std::to_string(dup_i++);
    }
    tns.insert(tensor_name);
    return std::make_shared<Tensor>(s, dt, tensor_name);
  }

  static vSptr<Tensor> CreateVec(int numbers,
                                 Shape s,
                                 Datatype dt = Datatype::FLOAT16,
                                 std::string tensor_name = "") {
    std::vector<Sptr<Tensor>> ts;
    for (int i = 0; i < numbers; ++i) {
      auto t = Tensor::Create(s, dt, tensor_name);
      ts.push_back(t);
    }
    return ts;
  }

  Tensor(Shape &s,
         Datatype dt = Datatype::FLOAT16,
         std::string tensor_name = "")
      : shape_(s), dt_(dt), tensor_name_(tensor_name) {
  }

  // template <typename T>
  // void Save(T *data, size_t num, std::ofstream &os) {
  //   int i = 0;
  //   while (i < num) {
  //     os << (float)(data[i++]) << std::endl;
  //   }
  // }

  void SaveToFile(std::string &tensor_name) {
    // CopyData(stcMemcpyDeviceToHost);
    // std::ofstream os(tensor_name);
    // if (dt_ == Datatype::FLOAT16) {
    //   Save<half_float::half>((half_float::half *)host_ddr_, shape_.GetCount(),
    //                          os);
    // } else if (dt_ == Datatype::FLOAT32) {
    //   Save<float>((float *)host_ddr_, shape_.GetCount(), os);
    // } else {
    //   std::cout << "now not support this datatype data" << std::endl;
    // }
    // os.close();
  }

  Datatype GetDtype() { return dt_; };
  Shape GetShape() { return shape_; };
  void *GetDevDdr() { return dev_ddr_; };
  void *GetHostDdr() { return host_ddr_; };

  size_t GetMemBs() {
    auto sew = SizeOf(dt_);
    size_t memory_size = shape_.GetCount() * sew;
    return memory_size;
  }

 private:
  Shape shape_;
  Datatype dt_;
  std::string tensor_name_;
  void *host_ddr_ = nullptr;
  void *dev_ddr_ = nullptr;
  // bool copied_to_host_ = false;
};



struct CycleCounter {
  CycleCounter() : cycles_mme(0), cycles_vme(0), cycles_gdram2llb(0), cycles_gdram2l1(0),
    cycles_llb2gdram(0), cycles_l12gdram(0), cycles_gdram2gdram(0),
    cycles_llb2l1(0), cycles_llb2l1_b(0), cycles_llb2llb(0),
    cycles_l12llb(0), cycles_l12l1(0),
    cycles_dma(0), cycles_mte(0), cycles_compute(0),
    cycles_bottleneck(0) {};

  void PrintInfo(std::string &op_name) {
    UpdateBottleneck();
    std::stringstream infos;
    infos << op_name << "\n";
    #define PSTR(stream, val) stream << #val << ": " << val  << "\n"
    PSTR(infos, cycles_mme);
    PSTR(infos, cycles_vme);
    PSTR(infos, cycles_gdram2llb);
    PSTR(infos, cycles_gdram2l1);
    PSTR(infos, cycles_llb2gdram);
    PSTR(infos, cycles_l12gdram);
    PSTR(infos, cycles_gdram2gdram);

    PSTR(infos, cycles_llb2l1);
    PSTR(infos, cycles_llb2l1_b);
    PSTR(infos, cycles_llb2llb);

    PSTR(infos, cycles_l12llb);
    PSTR(infos, cycles_l12l1);

    infos << "summary \n";
    PSTR(infos, cycles_dma);
    PSTR(infos, cycles_mte);
    PSTR(infos, cycles_compute);
    PSTR(infos, cycles_bottleneck);
    #undef PSTR
    LOG(I) << infos.str();
  };

  void UpdateBottleneck() {
    cycles_dma = cycles_gdram2llb + cycles_gdram2l1 + cycles_gdram2gdram +
                 cycles_llb2gdram + cycles_l12gdram;
    cycles_mte = cycles_llb2l1 + cycles_llb2l1_b + cycles_llb2llb +
                 cycles_l12llb + cycles_l12l1;
    cycles_compute = std::max(cycles_mme, cycles_vme);
    cycles_bottleneck = std::max(std::max(cycles_dma, cycles_mte), cycles_compute);
  }

  uint64_t GetBottleneck() {
    return cycles_bottleneck;
  };
  uint64_t cycles_mme;
  uint64_t cycles_vme;
  uint64_t cycles_gdram2llb;
  uint64_t cycles_gdram2l1;
  uint64_t cycles_llb2gdram;
  uint64_t cycles_l12gdram;
  uint64_t cycles_gdram2gdram;

  uint64_t cycles_llb2l1;
  uint64_t cycles_llb2l1_b;
  uint64_t cycles_llb2llb;

  uint64_t cycles_l12llb;
  uint64_t cycles_l12l1;

  uint64_t cycles_dma;
  uint64_t cycles_mte;
  uint64_t cycles_compute;
  uint64_t cycles_bottleneck;
};


class BaseParam {
 public:
  BaseParam() = default;
  virtual void PrintInfo() = 0;
};

typedef enum class OperandType {
  INPUT = 0,
  OUTPUT,
} OperandType;

class BaseLayer : public std::enable_shared_from_this<BaseLayer> {
 public:
  BaseLayer(std::vector<Sptr<Tensor>> ins, std::vector<Sptr<Tensor>> outs, std::string &name) :
    /*inputs_(ins), outputs_(outs), */
    opds_({{OperandType::INPUT, ins}, {OperandType::OUTPUT, outs}}), name_(name)/*, workspace_bytes_(0), d_workspace_(nullptr)*/{};
  virtual ~BaseLayer(){};


  void PushInput(Sptr<Tensor> i) { opds_[OperandType::INPUT].push_back(i); };
  void PushOutput(Sptr<Tensor> o) { opds_[OperandType::OUTPUT].push_back(o); };
  std::vector<Sptr<Tensor>> GetInputs() { return GetOperands(OperandType::INPUT); };
  std::vector<Sptr<Tensor>> GetOutputs() { return GetOperands(OperandType::OUTPUT); };
  std::vector<Sptr<Tensor>> GetOperands(OperandType opd_type) { return opds_[opd_type]; };

  const std::string & GetLayerName() {return name_;};

  virtual uint64_t EvaluateCycle() = 0;
  virtual void Check() = 0;

  void CheckOperandNum(OperandType opd_type, int number, std::function<int(int, int)> comp) {
    bool is_valid = comp(GetOperands(opd_type).size(), number);
    if (!is_valid) {
      LOG(E) << typeid(*this).name() << "operand size not match: " << GetOperands(opd_type).size() << " != " << number;
      exit(0);
    }
    return;
  }

 protected:
  std::map<OperandType, std::vector<Sptr<Tensor>> > opds_;
  std::string name_;
  CycleCounter perf_;
  std::stringstream infos_;
  // int workspace_bytes_;
  // void *d_workspace_;
};

} // namespace nnte

#endif // NNTE_INCLUDE_COMMON_H_
