/***************************************************
#filename      : example.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-23 20:05:21
#last modified : 2023-05-23 20:05:21
#description   : NA
***************************************************/
#include <pybind11/pybind11.h>

// #include <string>

namespace py = pybind11;

using namespace pybind11::literals;
int add(int i, int j) {
  int c = i + j;
  printf("i:%d, j:%d, i+j=%d\n",i,j,c);
  return i + j;
}

int mul(int i, int j) {
  int c = i * j;
  printf("i:%d, j:%d, i*j=%d\n",i,j,c);
  return c;
}

struct pet {
  pet(const std::string &name) : name_(name) {}
  void setName(const std::string &name) {name_ = name;}
  const std::string getName() {return name_;};
  std::string name_;
};

class cet {
 public:
  cet(const std::string &name) : name_(name) {}
  void setName(const std::string &name) {name_ = name;}
  const std::string getName() {return name_;};

 private:
  std::string name_;
};

PYBIND11_MODULE(emp, m) {

  // help(emp).
  m.doc() = "test pybind11 example packag"; // optional module docstring

  // bind function
  m.def("add", &add, "A function that adds two numbers", py::arg("i"), py::arg("j"));
  m.def("mul", &mul, "A function that multiple two numbers", "i"_a, "j"_a);

  // bind attribute
  m.attr("the_answer") = 42;
  py::object world = py::cast("world");
  m.attr("what") = world;

  // bind class
  py::class_<pet>(m, "pet")
    .def(py::init<const std::string&> ())
    .def("setName", &pet::setName)
    .def("getName", &pet::getName)
    // bind lambda functions
    .def("__repr__",
      [](const pet &a) {
          return "<example.Pet named '" + a.name_ + "'>";
      })
    // bind field, python 对象可以自由访问name。
    .def_readwrite("name", &pet::name_);

  py::class_<cet>(m, "cet")
    .def(py::init<const std::string&> ())
    .def("setName", &cet::setName)
    .def("getName", &cet::getName)
    // bind lambda functions
    .def("__repr__",
      [](const cet &a) {
          return "<example.cet named xx>";
      })
    // bind field, python 对象可以自由访问name。
    .def_property("name", &cet::setName, &cet::getName);
}


