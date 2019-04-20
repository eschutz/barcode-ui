#include <stdio.h>

int main() {
    char buf[29];

    snprintf(buf, 29, "-sOutputFile=%s", "barcodeXXXXXX");

    puts(buf);

}
