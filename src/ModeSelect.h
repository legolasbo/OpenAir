#ifndef ModeSelect_H
#define ModeSelect_H

enum SelectedMode {
  MODE_LOW = 1,
  MODE_MEDIUM = 2,
  MODE_HIGH = 3
};

class ModeSelectSwitch {
    private:
        int sda, scl;

    public:
        ModeSelectSwitch(int sdaPin, int sclPin) {
            sda = sdaPin;
            scl = sclPin;
        }
        SelectedMode read();
};

#endif