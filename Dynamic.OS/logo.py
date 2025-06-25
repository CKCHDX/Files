from PIL import Image, ImageDraw
import math

# Animation parameters
W, H = 480, 480
CENTER = (W // 2, H // 2)
NUM_FRAMES = 120

# Core and node parameters
CORE_RADIUS = 70
NODE_RADII = [18, 14, 18, 14]
NODE_COLORS = ["#00FFA3", "#00E5FF", "#00FFA3", "#00E5FF"]
NODE_BASE_ANGLES = [0, 90, 180, 270]
NODE_ORBIT = 140

def hex_to_rgb(hex_color, alpha=255):
    hex_color = hex_color.lstrip('#')
    if len(hex_color) == 3:
        hex_color = ''.join([c*2 for c in hex_color])
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4)) + (alpha,)

def draw_neural_connections(draw, t):
    draw.line([
        (CENTER[0], CENTER[1] - CORE_RADIUS),
        (CENTER[0] + 65 + 8*math.sin(t), CENTER[1] - 80 + 8*math.cos(t)),
        (CENTER[0] + NODE_ORBIT, CENTER[1])
    ], fill=hex_to_rgb("#00E5FF", 128), width=3)
    draw.line([
        (CENTER[0], CENTER[1] + CORE_RADIUS),
        (CENTER[0] - 65 - 8*math.sin(t), CENTER[1] + 80 - 8*math.cos(t)),
        (CENTER[0] - NODE_ORBIT, CENTER[1])
    ], fill=hex_to_rgb("#00FFA3", 128), width=3)
    draw.line([
        (CENTER[0] + CORE_RADIUS, CENTER[1]),
        (CENTER[0] + NODE_ORBIT, CENTER[1])
    ], fill=hex_to_rgb("#ffffff", 80), width=2)
    draw.line([
        (CENTER[0] - CORE_RADIUS, CENTER[1]),
        (CENTER[0] - NODE_ORBIT, CENTER[1])
    ], fill=hex_to_rgb("#ffffff", 80), width=2)

def draw_D_monogram(draw, alpha):
    x0, y0 = CENTER[0] - 32, CENTER[1] - 48
    y1 = y0 + 96
    arc_box = [x0 - 24, y0, x0 + 72, y0 + 96]
    draw.line([(x0, y0), (x0, y1)], fill=(255,255,255,alpha), width=10)
    draw.arc(arc_box, start=270, end=450, fill=(255,255,255,alpha), width=10)

def draw_logo_frame(frame):
    img = Image.new("RGBA", (W, H), (0,0,0,0))
    draw = ImageDraw.Draw(img, "RGBA")
    t = 2 * math.pi * frame / NUM_FRAMES

    # Glowing core
    for r in range(CORE_RADIUS, 0, -1):
        interp = r / CORE_RADIUS
        color1 = (0, 229, 255)
        color2 = (0, 255, 163)
        r_c = int(color1[0] * interp + color2[0] * (1 - interp))
        g_c = int(color1[1] * interp + color2[1] * (1 - interp))
        b_c = int(color1[2] * interp + color2[2] * (1 - interp))
        alpha = int(180 + 60 * math.sin(t + interp * math.pi))
        draw.ellipse([
            CENTER[0] - r, CENTER[1] - r,
            CENTER[0] + r, CENTER[1] + r
        ], fill=(r_c, g_c, b_c, alpha))

    # Neural connections
    draw_neural_connections(draw, t)

    # Orbiting modular nodes
    for i, (base_angle, r, color) in enumerate(zip(NODE_BASE_ANGLES, NODE_RADII, NODE_COLORS)):
        angle = base_angle + (360 * frame / NUM_FRAMES) + i * 30 * math.sin(t)
        a = math.radians(angle)
        x = CENTER[0] + NODE_ORBIT * math.cos(a)
        y = CENTER[1] + NODE_ORBIT * math.sin(a)
        draw.ellipse([x - r*1.8, y - r*1.8, x + r*1.8, y + r*1.8], fill=hex_to_rgb(color, 80))
        draw.ellipse([x - r, y - r, x + r, y + r], fill=hex_to_rgb(color, 255), outline=hex_to_rgb("#181C20", 255), width=4)

    # "D" monogram
    draw_D_monogram(draw, 255)
    return img

# Generate frames
frames = [draw_logo_frame(i).convert("P", dither=Image.FLOYDSTEINBERG) for i in range(NUM_FRAMES)]

# Save GIF
frames[0].save(
    "dynamic_os_logo.gif",
    save_all=True,
    append_images=frames[1:],
    duration=60,
    loop=0,
    disposal=2,
    optimize=False,
    transparency=0
)

# Save PNG (first frame, full RGBA quality)
first_frame_png = draw_logo_frame(0)
first_frame_png.save("dynamic_os_logo.png", format="PNG")

print("Saved: dynamic_os_logo.gif and dynamic_os_logo.png")
