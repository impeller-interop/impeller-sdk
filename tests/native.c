#include <impeller.h>

int main(void) {
    if (ImpellerGetVersion() != IMPELLER_VERSION) {
        return 1;
    }

    ImpellerPaint paint = ImpellerPaintNew();
    if (paint == NULL) {
        return 1;
    }

    ImpellerPaintRelease(paint);
    return 0;
}
