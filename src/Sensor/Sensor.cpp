#include "Sensor.hpp"

Register::Register(ModuleCtrl &moduleCtrl, std::string name, int address,
                   std::map<int, std::string> &values)
    : name(name), address(address) {
  moduleCtrl.readEEProm(address, &value);
  label = values[value];
}

Register::Register(ModuleCtrl &moduleCtrl, std::string name, int address)
    : name(name), address(address) {
  moduleCtrl.readEEProm(address, &value);
  if(value!=0)
  label = std::string("rev") + std::to_string(value / 256) +
          std::string(".") + std::to_string(value % 256);
    else
    label = std::string("None");
}

std::shared_ptr<Register>
Sensor::Make_Register(std::string name, int address,
                      std::map<int, std::string> values) {
  return std::make_unique<Register>(moduleCtrl, name, address, values);
}

std::shared_ptr<Register> Sensor::Make_Register(std::string name, int address) {
  return std::make_unique<Register>(moduleCtrl, name, address);
}

Sensor::Sensor(ModuleCtrl &moduleCtrl)
    : moduleCtrl(moduleCtrl) { // ALL THIS VALUES ARE IN OPTIMOM USER MANUAL
  std::map<int, std::string> Sensor_family_Content = {{0x0100, "Topaz"},
                                                      {0x0200, "Snappy"}};
  std::map<int, std::string> Sensor_resolution_Content = {{0x0200, "2Mp"},
                                                          {0x0500, "5Mp"}};
  std::map<int, std::string> Sensor_package_Content = {{0x0100, "CSP"},
                                                       {0x0200, "FOGS"}};
  std::map<int, std::string> Sensor_CFA_option_Content = {{0x0100, "BW"},
                                                          {0x1000, "RGB"}};
  std::map<int, std::string> Sensor_CRA_Content = {{0x1400, "CRA14°"},
                                                   {0x2100, "CRA21°"}};
  std::map<int, std::string> Module_option_Content = {
      {0x0000, "no lens module"},
      {0x0100, "Fixed Focus module"},
      {0x0200, "Auto/Multi Focus module"}};
  std::map<int, std::string> Filter_Content = {{0x0100, "IR Filter"},
                                               {0x0200, "AR Filter"}};

  Sensor_family = Make_Register("Sensor family", 0x0686, Sensor_family_Content);
  registers.push_back(Sensor_family);

  Sensor_resolution =
      Make_Register("Sensor resolution", 0x0688, Sensor_resolution_Content);
  registers.push_back(Sensor_resolution);

  Sensor_package =
      Make_Register("Sensor package", 0x068A, Sensor_package_Content);
  registers.push_back(Sensor_package);

  Sensor_CFA_option =
      Make_Register("Sensor CFA option", 0x068C, Sensor_CFA_option_Content);
  registers.push_back(Sensor_CFA_option);

  Sensor_CRA = Make_Register("Sensor CRA", 0x068E, Sensor_CRA_Content);
  registers.push_back(Sensor_CRA);

  Module_option = Make_Register("Module option", 0x0690, Module_option_Content);
  registers.push_back(Module_option);

  Filter = Make_Register("Filter", 0x0692, Filter_Content);
  registers.push_back(Filter);

  Device_version = Make_Register("Device version", 0x069C);
  registers.push_back(Device_version);

  PCB_version = Make_Register("PCB version", 0x069E);
  registers.push_back(PCB_version);

  Tlens_version = Make_Register("T-lens version", 0x06A0);
  registers.push_back(Tlens_version);

  Lens_version = Make_Register("Lens version", 0x06A2);
  registers.push_back(Lens_version);

  Driver_IC_version = Make_Register("Driver IC version", 0x06A4);
  registers.push_back(Driver_IC_version);
}

void Sensor::render() {
  ImGui::Begin("Sensor informations");
  for (std::vector<std::shared_ptr<Register>>::iterator i = registers.begin();
       i != registers.end(); ++i) {
    ImGui::Text("%s : %s", i->get()->getName().c_str(),
                i->get()->getLabel().c_str());
  }
  ImGui::End();
}