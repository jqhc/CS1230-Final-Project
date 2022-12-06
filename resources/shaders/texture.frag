#version 330 core

// input UV coordinates from vertex shader
in vec2 uvCoord;

// uniform texture from FBO colour buffer
uniform sampler2D fboTexture;

// uniform bools for each filter type
uniform bool perPixelFilterOn;
uniform bool kernelFilterOn;
uniform bool gammaFilterOn;
uniform float gammaVal;
uniform bool sobelFilterOn;

// uniform arrays representing each kernel
uniform float sharpenKernel[9];
uniform float blurKernel[25];

// uniform variables for image width and height
uniform int imageWidth;
uniform int imageHeight;

out vec4 fragColor;

vec4 getAdjPixel(int deltaX, int deltaY) {
    float deltaU = float(deltaX) / float(imageWidth-1);
    float deltaV = float(deltaY) / float(imageHeight-1);

    return texture(fboTexture, uvCoord + vec2(deltaU, deltaV));
}

float toGrayscale(vec4 colour) {
    return 0.299*colour[0] + 0.587*colour[1] + 0.114*colour[2];
}

void main()
{
    // set fragColor to color sampled from FBO texture at given uv coords
    fragColor = texture(fboTexture, uvCoord);

    if (kernelFilterOn) {
        // SHARPEN
//        fragColor = vec4(0);
//        for (int y = 1; y > -2; y--) {
//            for (int x = -1; x < 2; x++) {
//                fragColor += sharpenKernel[3*(1-y)+x+1]*getAdjPixel(x, y);
//            }
//        }
        fragColor = vec4(0);
        // BLUR
        for (int y = 2; y > -3; y--) {
            for (int x = -2; x < 3; x++) {
                fragColor += blurKernel[5*(2-y)+x+2]*getAdjPixel(x, y);
            }
        }
    }

    // sobel filter (extra credit 2)
    if (sobelFilterOn) {
        // get x and y gradients (in grayscale)
        float gx = -toGrayscale(getAdjPixel(-1,1)) + toGrayscale(getAdjPixel(1,1))
                   - 2.0*toGrayscale(getAdjPixel(-1,0)) + 2.0*toGrayscale(getAdjPixel(1,0))
                   - toGrayscale(getAdjPixel(-1,-1)) + toGrayscale(getAdjPixel(1,-1));
        float gy = -toGrayscale(getAdjPixel(-1,1)) + toGrayscale(getAdjPixel(-1,-1))
                - 2.0*toGrayscale(getAdjPixel(0,1)) + 2.0*toGrayscale(getAdjPixel(0,-1))
                - toGrayscale(getAdjPixel(1,1)) + toGrayscale(getAdjPixel(1,-1));

        // set colour to magnitude
        fragColor = vec4(sqrt(pow(gx,2) + pow(gy,2)));
    }

    // perform per-pixel filtering
    if (perPixelFilterOn) {
        // INVERT
        fragColor = vec4(1) - fragColor;

        // GRAYSCALE
//        fragColor = vec4(toGrayscale(fragColor));
    }

    // gamma filtering (extra credit 1)
    if (gammaFilterOn) {
        fragColor[0] = pow(fragColor[0], gammaVal);
        fragColor[1] = pow(fragColor[1], gammaVal);
        fragColor[2] = pow(fragColor[2], gammaVal);
    }
}
