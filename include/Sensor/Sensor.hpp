#include "ModuleControl_v100.hpp"
#include "imgui.h"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Register {
private:
  std::string name;
  int address;
  int value;
  std::string label;

public:
  Register(ModuleCtrl &moduleCtrl, std::string name, int address,
           std::map<int, std::string> &values);
  Register(ModuleCtrl &moduleCtrl, std::string name, int address);
  int getAddress() { return this->address; }
  std::string getName() { return this->name; }
  int getValue() { return this->value; }
  std::string getLabel() { return this->label; }
};

class Sensor {

private:
  std::shared_ptr<Register> Sensor_family;
  std::shared_ptr<Register> Sensor_resolution;
  std::shared_ptr<Register> Sensor_package;
  std::shared_ptr<Register> Sensor_CFA_option;
  std::shared_ptr<Register> Sensor_CRA;
  std::shared_ptr<Register> Module_option;
  std::shared_ptr<Register> Filter;
  std::shared_ptr<Register> Device_version;
  std::shared_ptr<Register> PCB_version;
  std::shared_ptr<Register> Tlens_version;
  std::shared_ptr<Register> Lens_version;
  std::shared_ptr<Register> Driver_IC_version;

  std::shared_ptr<Register> Make_Register(std::string name, int address,
                                          std::map<int, std::string> values);
  std::shared_ptr<Register> Make_Register(std::string name, int address);
  ModuleCtrl &moduleCtrl;
  std::vector<std::shared_ptr<Register>> registers;

public:
  bool getColor() {return Sensor_CFA_option->getValue() == 0x1000;};
  bool getMultifocus() {return Module_option->getValue() == 0x0200;};
  Sensor(ModuleCtrl &moduleCtrl);
  void render();
};
