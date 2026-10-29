#include <impeller.h>

#include <stdio.h>

static int failures;

#define CHECK(expr)                                                        \
    do {                                                                   \
        if (!(expr)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, \
                    #expr);                                                \
            failures++;                                                    \
        }                                                                  \
    } while (0)

static ImpellerColor srgb_color(float red, float green, float blue,
                                float alpha) {
    ImpellerColor color;
    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = alpha;
    color.color_space = kImpellerColorSpaceSRGB;
    return color;
}

static void check_version(void) {
    CHECK(ImpellerGetVersion() == IMPELLER_VERSION);
    CHECK(IMPELLER_VERSION_GET_VARIANT(IMPELLER_VERSION) ==
          IMPELLER_VERSION_VARIANT);
    CHECK(IMPELLER_VERSION_GET_MAJOR(IMPELLER_VERSION) == IMPELLER_VERSION_MAJOR);
    CHECK(IMPELLER_VERSION_GET_MINOR(IMPELLER_VERSION) == IMPELLER_VERSION_MINOR);
    CHECK(IMPELLER_VERSION_GET_PATCH(IMPELLER_VERSION) == IMPELLER_VERSION_PATCH);
}

static void check_paint(void) {
    ImpellerPaint paint = ImpellerPaintNew();
    if (paint == NULL) {
        return;
    }

    ImpellerColor color = srgb_color(1.0f, 0.2f, 0.1f, 1.0f);
    ImpellerPaintSetColor(paint, &color);
    ImpellerPaintSetBlendMode(paint, kImpellerBlendModeSourceOver);
    ImpellerPaintSetDrawStyle(paint, kImpellerDrawStyleFill);
    ImpellerPaintSetStrokeCap(paint, kImpellerStrokeCapButt);
    ImpellerPaintSetStrokeJoin(paint, kImpellerStrokeJoinMiter);
    ImpellerPaintSetStrokeWidth(paint, 2.0f);
    ImpellerPaintSetStrokeMiter(paint, 4.0f);

    ImpellerPaintRetain(paint);
    ImpellerPaintRelease(paint);
    ImpellerPaintRelease(paint);
}

static void check_color_sources(void) {
    ImpellerPaint paint = ImpellerPaintNew();
    if (paint == NULL) {
        CHECK(paint != NULL);
        return;
    }

    ImpellerColor colors[2] = {srgb_color(1.0f, 0.0f, 0.0f, 1.0f),
                               srgb_color(0.0f, 0.0f, 1.0f, 1.0f)};
    float stops[2] = {0.0f, 1.0f};
    ImpellerPoint start = {0.0f, 0.0f};
    ImpellerPoint end = {10.0f, 0.0f};

    ImpellerColorSource gradient = ImpellerColorSourceCreateLinearGradientNew(
        &start, &end, 2, colors, stops, kImpellerTileModeClamp, NULL);
    CHECK(gradient != NULL);
    if (gradient != NULL) {
        ImpellerPaintSetColorSource(paint, gradient);
        ImpellerColorSourceRelease(gradient);
    }

    ImpellerPoint center = {5.0f, 5.0f};
    ImpellerColorSource radial = ImpellerColorSourceCreateRadialGradientNew(
        &center, 5.0f, 2, colors, stops, kImpellerTileModeRepeat, NULL);
    CHECK(radial != NULL);
    if (radial != NULL) {
        ImpellerPaintSetColorSource(paint, radial);
        ImpellerColorSourceRelease(radial);
    }

    ImpellerPaintRelease(paint);
}

static void check_filters(void) {
    ImpellerPaint paint = ImpellerPaintNew();
    if (paint == NULL) {
        CHECK(paint != NULL);
        return;
    }

    ImpellerColor color = srgb_color(0.0f, 1.0f, 0.0f, 1.0f);
    ImpellerColorFilter blend =
        ImpellerColorFilterCreateBlendNew(&color, kImpellerBlendModeSourceOver);
    CHECK(blend != NULL);
    if (blend != NULL) {
        ImpellerPaintSetColorFilter(paint, blend);
        ImpellerColorFilterRelease(blend);
    }

    ImpellerColorMatrix matrix = {
        {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         1.0f, 0.0f},
    };
    ImpellerColorFilter color_matrix =
        ImpellerColorFilterCreateColorMatrixNew(&matrix);
    CHECK(color_matrix != NULL);
    if (color_matrix != NULL) {
        ImpellerPaintSetColorFilter(paint, color_matrix);
        ImpellerColorFilterRelease(color_matrix);
    }

    ImpellerMaskFilter mask =
        ImpellerMaskFilterCreateBlurNew(kImpellerBlurStyleNormal, 2.0f);
    CHECK(mask != NULL);
    if (mask != NULL) {
        ImpellerPaintSetMaskFilter(paint, mask);
        ImpellerMaskFilterRelease(mask);
    }

    ImpellerImageFilter blur =
        ImpellerImageFilterCreateBlurNew(2.0f, 2.0f, kImpellerTileModeClamp);
    CHECK(blur != NULL);
    if (blur != NULL) {
        ImpellerPaintSetImageFilter(paint, blur);
        ImpellerImageFilterRelease(blur);
    }

    ImpellerPaintRelease(paint);
}

static void check_paths(void) {
    ImpellerPathBuilder builder = ImpellerPathBuilderNew();
    CHECK(builder != NULL);
    if (builder == NULL) {
        return;
    }

    ImpellerPoint origin = {0.0f, 0.0f};
    ImpellerPoint line_end = {10.0f, 0.0f};
    ImpellerPoint curve_control = {10.0f, 5.0f};
    ImpellerPoint curve_end = {5.0f, 10.0f};
    ImpellerPoint cubic_control = {2.0f, 8.0f};
    ImpellerRect rect = {0.0f, 0.0f, 10.0f, 20.0f};

    ImpellerPathBuilderMoveTo(builder, &origin);
    ImpellerPathBuilderLineTo(builder, &line_end);
    ImpellerPathBuilderQuadraticCurveTo(builder, &curve_control, &curve_end);
    ImpellerPathBuilderCubicCurveTo(builder, &cubic_control, &curve_end,
                                    &origin);
    ImpellerPathBuilderAddRect(builder, &rect);
    ImpellerPathBuilderAddOval(builder, &rect);
    ImpellerPathBuilderAddArc(builder, &rect, 0.0f, 90.0f);
    ImpellerPathBuilderClose(builder);

    ImpellerPath copied =
        ImpellerPathBuilderCopyPathNew(builder, kImpellerFillTypeNonZero);
    CHECK(copied != NULL);
    if (copied != NULL) {
        ImpellerRect bounds = {0.0f, 0.0f, 0.0f, 0.0f};
        ImpellerPathGetBounds(copied, &bounds);
        CHECK(bounds.width > 0.0f);
        CHECK(bounds.height > 0.0f);
        ImpellerPathRetain(copied);
        ImpellerPathRelease(copied);
        ImpellerPathRelease(copied);
    }

    ImpellerPath taken =
        ImpellerPathBuilderTakePathNew(builder, kImpellerFillTypeOdd);
    CHECK(taken != NULL);
    if (taken != NULL) {
        ImpellerPathRelease(taken);
    }

    ImpellerPathBuilderRelease(builder);
}

static void check_display_lists(void) {
    ImpellerPaint paint = ImpellerPaintNew();
    CHECK(paint != NULL);
    if (paint == NULL) {
        return;
    }

    ImpellerColor color = srgb_color(0.1f, 0.2f, 0.3f, 1.0f);
    ImpellerPaintSetColor(paint, &color);

    ImpellerDisplayListBuilder builder = ImpellerDisplayListBuilderNew(NULL);
    CHECK(builder != NULL);
    if (builder != NULL) {
        ImpellerRect rect = {0.0f, 0.0f, 8.0f, 8.0f};
        ImpellerDisplayListBuilderSave(builder);
        ImpellerDisplayListBuilderDrawRect(builder, &rect, paint);
        ImpellerDisplayListBuilderRestore(builder);

        ImpellerDisplayList list =
            ImpellerDisplayListBuilderCreateDisplayListNew(builder);
        CHECK(list != NULL);
        if (list != NULL) {
            ImpellerDisplayListRetain(list);
            ImpellerDisplayListRelease(list);
            ImpellerDisplayListRelease(list);
        }

        ImpellerDisplayListBuilderRelease(builder);
    }

    ImpellerRect cull_rect = {0.0f, 0.0f, 32.0f, 32.0f};
    ImpellerDisplayListBuilder clipped =
        ImpellerDisplayListBuilderNew(&cull_rect);
    CHECK(clipped != NULL);
    if (clipped != NULL) {
        ImpellerDisplayListBuilderRelease(clipped);
    }

    ImpellerPaintRelease(paint);
}

int main(void) {
    check_version();
    check_paint();
    check_color_sources();
    check_filters();
    check_paths();
    check_display_lists();

    if (failures != 0) {
        fprintf(stderr, "%d check(s) failed\n", failures);
        return 1;
    }

    printf("native C test passed\n");
    return 0;
}
