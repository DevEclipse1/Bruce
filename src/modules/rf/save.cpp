#include "save.h"

bool rf_raw_save(RawRecording recorded) {
    FS *fs = nullptr;
    if (!getFsStorage(fs) || fs == nullptr) {
        displayError("No space left on device", true);
        return false;
    }

    if (!fs->exists("/BruceRF")) fs->mkdir("/BruceRF");

    String filename;
    int i = 0;
    do {
        filename = "/BruceRF/" + String(i) + ".sub";
        i++;
    } while (fs->exists(filename));

    File file = fs->open(filename, FILE_WRITE);
    if (!file) {
        displayError("Error creating file", true);
        return false;
    }

    file.println("Filetype: Flipper SubGhz RAW File");
    file.println("Version: 1");
    file.printf("Frequency: %d\n", int(recorded.frequency * 1000000));
    file.println("Preset: 0");
    file.println("Protocol: RAW");

    file.print("RAW_Data: ");

    for (size_t i = 0; i < recorded.codes.size(); ++i) {
        for (size_t j = 0; j < recorded.codeLengths[i]; ++j) {
            int d0 = recorded.codes[i][j].duration0;
            int d1 = recorded.codes[i][j].duration1;

            if (d0 < 1000000 && d1 < 1000000 && d0 > -1000000 && d1 > -1000000) {
                if (d0 > 0) {
                    if (recorded.codes[i][j].level0 != 1) file.print("-");
                    file.print(d0);
                    file.print(" ");
                }
                if (d1 > 0) {
                    if (recorded.codes[i][j].level1 != 1) file.print("-");
                    file.print(d1);
                    file.print(" ");
                }
            }
        }

        if (i < recorded.codes.size() - 1) {
            file.print(recorded.gaps[i] * -1000);
            file.print(" ");
        }
    }

    file.println();
    file.close();
    displaySuccess(filename);
    return true;
}
