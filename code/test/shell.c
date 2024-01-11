#include "syscall.h"

int main() {
    SpaceId newProc;
    OpenFileId input = CONSOLEINPUT;
    OpenFileId output = CONSOLEOUTPUT;
    char prompt[2], ch, buffer[60];
    int i, exitCode;

    prompt[0] = '-';
    prompt[1] = '-';

    while (1) {
        Write(prompt, 2, output);

        i = 0;

        do {
            Read(&buffer[i], 1, input);
        } while (buffer[i++] != '\n');

        buffer[--i] = '\0';

        if (i > 0) {
            newProc = Exec(buffer);
            exitCode = Join(newProc);
            if (exitCode != 0) {
                // Print the exit code if it is not 0
                PrintNum(exitCode);
            }
        }
    }
}