# Project 5

## Design Choices
For this project, I chose to implement all functions which called OpenGL methods under the `Realtime` class, and placed all of these helper functions separately in `realtime_helpers.cpp`. 

For the buffer generation, I implemented it all in the `shapes` folder, one file for each shape and then `Shapes.cpp` which brought them all  together into one function.

Finally, I also created a `Camera` class which stored all the camera data and also calculated the view and projection matrices.

# Project 6

## Design Choices
I implemented point and spot lights in the same way that I did directional lights. Their main functionality is found in `phong.frag`.

I implemented all the camera movement functionality in `Realtime::timerEvent()` and `Realtime::mouseMoveEvent()`. Each one checks for each possible keyboard/mouse input and then modifies the `Realtime` instance's `m_camera` field to match the desired camera parameters.

All my FBO implementation can be found in `realtime_helpers.cpp`. The relevant functions are:
- `Realtime::initializeFullscreenQuad()`, which is called in `initializeGL()` to set up the fullscreen quad in the screen framebuffer.
- `Realtime::makeFBO()`, which is called in `initializeGL()` and `resizeGL()`. It sets up the offscreen framebuffer and its attachments (texture for colour buffer and renderbuffer for depth/stencil).
- `Realtime::paintFBOTextureToScreen()`, which is called in `paintGL()` to sample the offscreen framebuffer's colour buffer onto the fullscreen quad and produce the image.

I created the new shaders `texture.vert` and `texture.frag`, which are used to sample from the offscreen FBO's texture attachment and perform filtering. For the project I chose to use invert and blur filters (although I also implemented grayscale and sharpen, which are commented out and can be found in `texture.frag`).

### Default FBO value
The default FBO value is stored in the field `m_defaultFBO` in the `Realtime` class. It is assigned at the beginning of `initializeGL()` in `realtime.cpp` and can be modified there.

## Extra Credit
I implemented an extra per-pixel filter, the gamma filter, which can be toggled using Extra Credit 1 in the app window. The exact gamma exponent value can be controlled by modifying the `gammaVal` field at the beginning of `initializeGL()`.

I also implemented a Sobel kernel filter, which can be toggled using Extra Credit 2 in the app window. It is implemented in the same way as outlined in Project 2. Note: it's not possible to have both Sobel and blur filters on at the same time.

Both of these extra credits were implemented in `texture.frag`.