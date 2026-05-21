#pragma once

#include <stdint.h>

#include "hardware/matrix.hpp"

namespace MatrixDisplay {

// Display resolutoin to build a framebuffer
constexpr uint8_t WIDTH = 8;
constexpr uint8_t HEIGHT = 8;

// The display can be rotated, so the orientation must be corrected in software.
enum class Orientation : uint8_t { Deg0, Deg90, Deg180, Deg270 };

// Initialise the display with the given matrix. Note that the matrix should be
// initialised already.
void init(Matrix* matrix);

// Setter and getter for display orientation. The orientation of the displays
// are stored in a variable and the transformation is only applied when
// rendering. As such, the orientation can be changed arbitrarily before
// rendering. Accessing the framebuffer will return the original coordinates.
void setOrientation(Orientation orientation);
Orientation getOrientation();

// Clears the back buffer. Note that only the buffer is cleared and the dirty
// flag set. As such, the buffer cna be written to, or rendered.
//
// Refer to `MatrixDisplay::clear()` to clear the matrix screen directly.
void clearBuffer();

// Setters and getters for pixel information. Extracts the pixel information
// from the framebuffer to return. The origin of the display, or (0, 0), is the
// top left corner. The pixel operations use the back buffer as the target to
// read pixels from and write pixels to.
void setPixel(uint8_t x, uint8_t y, bool enabled);
bool getPixel(uint8_t x, uint8_t y);
void togglePixel(uint8_t x, uint8_t y);

// Utility function to fill a whole row with a value at once. The value must be
// a bitmap of 8 bits corresponding to each pixel in said row.
void drawRow(uint8_t y, uint8_t value);

// Controls the state of the entire screen at once.
void fill(bool enabled);

// Rendering is only done if the screen is dirty. In other words, if the target
// state of the screen changed from the previous state. If the display is not
// dirty, then regular rendering will be a no-op command. However, the render
// method can be forced to render without respecting the dirty flag. However,
// this should not be used trivially, as this can impact performance heavily.
bool isDirty();

// The front buffer is currently being displayed, and the back buffer is
// currently being rendered to. When rendering, each position in the framebuffer
// is diffed to find the updated section, and the changed section is updated
// while skipping the unchanged section. The back buffer will overwrite the old
// data in the front buffer until both buffers are synchronised, at which point
// the back buffer can be updated with the new content.
//
// This method mutates the matrix state, in contrast to other methods only
// modifying the back buffer for the next render.
void render(bool force = false);

// Clears both the front and back buffer while also clearing the matrix display.
//
// This method mutates the matrix state, in contrast to other methods only
// modifying the back buffer for the next render.
void clear();

// The framebuffers should never be modified directly, as that would break the
// dirty tracking. However, there could be cases when the program needs to read
// the state of the framebuffers directly. For such cases, the framebuffer can
// be accessed in a read-only way.
const uint8_t* getFrontBuffer();
const uint8_t* getBackBuffer();

}  // namespace MatrixDisplay