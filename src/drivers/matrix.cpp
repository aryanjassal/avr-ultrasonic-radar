#include "drivers/matrix.hpp"

namespace MatrixDisplay {

// The active matrix display being used as the output
static Matrix* matrix = nullptr;

// Was the framebuffer modified?
static bool dirty = false;

// Using double-buffering to render data
static uint8_t front[HEIGHT] = {};
static uint8_t back[HEIGHT] = {};

// Storing the orientation of the display. The translation is directly applied
// at render-time, and for all other situations, the orientation does not
// matter.
static Orientation orientation = Orientation::Deg0;

// Convenience method to in-place transform x and y coordinates to their rotated
// versions.
static void transform(uint8_t& x, uint8_t& y) {
  uint8_t tx = x;
  uint8_t ty = y;

  switch (orientation) {
    case Orientation::Deg0: break;
    case Orientation::Deg90:
      x = 7 - ty;
      y = tx;
      break;
    case Orientation::Deg180:
      x = 7 - tx;
      y = 7 - ty;
      break;
    case Orientation::Deg270:
      x = ty;
      y = 7 - tx;
      break;
  }
}

void init(Matrix* target) {
  // Set the active panel for the display
  matrix = target;

  // Initialise framebuffers
  for (uint8_t i = 0; i < HEIGHT; i++) {
    front[i] = 0x00;
    back[i] = 0x00;
  }

  // Force-render the framebuffer
  render(true);
}

// Orientation setter
void setOrientation(Orientation value) {
  orientation = value;
  dirty = true;
}

// Orientation getter
Orientation getOrientation() { return orientation; }

// Clear the back buffer and mark dirty to render later
void clearBuffer() {
  for (uint8_t y = 0; y < HEIGHT; y++) {
    if (back[y] != 0x00) {
      back[y] = 0x00;
      dirty = true;
    }
  }
}

// Clear the buffer and render this too.
void clear() {
  clearBuffer();
  for (uint8_t y = 0; y < HEIGHT; y++) { front[y] = 0x00; }
  render(true);
}

// Set a pixel on the display. Does not transform the values. Includes bounds
// checking.
void setPixel(uint8_t x, uint8_t y, bool enabled) {
  if (x >= WIDTH || y >= HEIGHT) return;
  uint8_t mask = (1 << x);
  uint8_t old = back[y];

  // clang-format off
  if (enabled) back[y] |= mask;
  else back[y] &= ~mask;
  if (back[y] != old) dirty = true;
  // clang-format on
}

// Get a pixel state on the display from the back buffer. Does not transform
// values. Includes bound checking.
bool getPixel(uint8_t x, uint8_t y) {
  if (x >= WIDTH || y >= HEIGHT) return false;
  return back[y] & (1 << x);
}

// Toggles the state of a pixel on the back buffer. Does not transofrm values.
// Includes bound checking.
void togglePixel(uint8_t x, uint8_t y) {
  if (x >= WIDTH || y >= HEIGHT) return;
  back[y] ^= (1 << x);
  dirty = true;
}

// Allows direct access to a framebuffer row to fill in the data. The data is
// taken as a bitfield with each bit corresponding to pixel state. Does not
// transform values. Includes bound checking.
void drawRow(uint8_t y, uint8_t value) {
  if (y >= HEIGHT) return;
  if (back[y] != value) {
    back[y] = value;
    dirty = true;
  }
}

// Fills the entire display with either everying being enabled or everything
// being disabled. Only modifies the back buffer.
void fill(bool enabled) {
  uint8_t value = enabled ? 0xFF : 0x00;
  for (uint8_t y = 0; y < HEIGHT; y++) {
    if (back[y] != value) {
      back[y] = value;
      dirty = true;
    }
  }
}

void render(bool force) {
  if (!matrix) return;
  if (!dirty && !force) return;

  uint8_t transformed[8] = {};

  // Diff each position and update only if the contents changed
  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      bool pixel = back[y] & (1 << x);
      if (!pixel) continue;

      // Transform raw coordinates to orientation before rendering
      uint8_t tx = x;
      uint8_t ty = y;
      transform(tx, ty);
      transformed[ty] |= (1 << tx);
    }
  }

  // Apply the transformed framebuffer as the new front buffer
  for (uint8_t y = 0; y < HEIGHT; y++) {
    if (force || front[y] != transformed[y]) {
      matrix->setRow(y, transformed[y]);
      front[y] = transformed[y];
    }
  }

  dirty = false;
}

// Getter for dirty
bool isDirty() { return dirty; }

// Getter for the buffers. While it allows modification due to the passing of
// pointers, nothing can be done about that.
const uint8_t* getFrontBuffer() { return front; }

const uint8_t* getBackBuffer() { return back; }

}  // namespace MatrixDisplay