// Mock class for testing
#ifndef DEVICE_MOCK
#define DEVICE_MOCK

class Device {
  public:
  Device() {};
  ~Device() {};
  void notifyValueChanged(Parameter* pParamWithNewValue) {};
};

#endif
