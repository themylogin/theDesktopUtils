# HSL shift values: https://github.com/chromium/chromium/blob/trunk/chrome/browser/browser_theme_provider.cc
# HSL shift procedure: https://github.com/chromium/chromium/blob/trunk/skia/ext/skia_utils.cc (SkColor HSLShift(HSL hsl, HSL shift))
# http://miguelventura.pt/image-manipulation-with-hsl.html

from colorsys import rgb_to_hls, hls_to_rgb
from PIL import Image, ImageColor

def rgb2hls(t):
    """ convert PIL-like RGB tuple (0 .. 255) to colorsys-like
    HSL tuple (0.0 .. 1.0) """
    r,g,b = t
    r /= 255.0
    g /= 255.0
    b /= 255.0
    return rgb_to_hls(r,g,b)

def hls2rgb(t):
    """ convert a colorsys-like HSL tuple (0.0 .. 1.0) to a
    PIL-like RGB tuple (0 .. 255) """
    r,g,b = hls_to_rgb(*t)
    r *= 255
    g *= 255
    b *= 255
    return (int(r),int(g),int(b))

class HSL(object):
    def __init__(self, h, s, l):
        self.h = h
        self.s = s
        self.l = l

def HSLShift(hsl, shift):
    # Replace the hue with the tint's hue.
    if (shift.h >= 0):
        hsl.h = shift.h

    # Change the saturation.
    if (shift.s >= 0):
        if (shift.s <= 0.5):
            hsl.s *= (shift.s * 2.0)
        else:
            hsl.s = hsl.s + (1.0 - hsl.s) * ((shift.s - 0.5) * 2.0)

    # Change the lightness.
    if (shift.l >= 0):
        if (shift.l <= 0.5):
            hsl.l *= (shift.l * 2.0)
        else:
            hsl.l = hsl.l + (1.0 - hsl.l) * ((shift.l - 0.5) * 2.0)

kDefaultTintFrameInactive = HSL(-1, 0.5, 0.72)
kDefaultTintFrameIncognito = HSL(-1, 0.2, 0.35)
kDefaultTintFrameIncognitoInactive = HSL(-1, 0.3, 0.6)

image = "/home/themylogin/Images/Wallpapers/3200x1200/pure-frame.jpg"
for suffix, shift in [("-inactive", kDefaultTintFrameInactive),
                      ("-incognito", kDefaultTintFrameIncognito),
                      ("-incognito-inactive", kDefaultTintFrameIncognitoInactive)]:
    im = Image.open(image)
    for x in range(0, 1920):
        for y in range(0, 16):
            h, l, s = rgb2hls(im.getpixel((x, y)))
            hsl = HSL(h, s, l)
            HSLShift(hsl, shift)
            im.putpixel((x, y), hls2rgb((hsl.h, hsl.l, hsl.s)))
    im.save(image.replace(".jpg", suffix + ".jpg"), quality=90)
