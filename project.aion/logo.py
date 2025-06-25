from PIL import Image, ImageDraw
import math

# Animation parameters
W, H = 480, 480
CENTER = (W // 2, H // 2)
NUM_FRAMES = 120

# Ring and node parameters
CORE_RADIUS = 60
RING_RADII = [90, 120, 150]
RING_WIDTH = 10
NODE_RADIUS = 10
NODE_COLOR = "#00FFA3"
RING_COLORS = ["#00E5FF", "#00FFA3", "#2DF4C1"]

def hex_to_rgb(hex_color, alpha=255):
    hex_color = hex_color.lstrip('#')
    if len(hex_color) == 3:
        hex_color = ''.join([c*2 for c in hex_color])
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4)) + (alpha,)

def draw_ring(draw, radius, width, color, angle, alpha=180):
    box = [
        CENTER[0] - radius, CENTER[1] - radius,
        CENTER[0] + radius, CENTER[1] + radius
    ]
    start = 45 + angle
    end = 315 + angle
    draw.arc(box, start=start, end=end, fill=color[:3]+(alpha,), width=width)

def draw_orbiting_node(draw, radius, angle, color, node_radius=NODE_RADIUS):
    a = math.radians(angle)
    x = CENTER[0] + radius * math.cos(a)
    y = CENTER[1] + radius * math.sin(a)
    draw.ellipse([x - node_radius, y - node_radius, x + node_radius, y + node_radius], fill=color, outline=None)

def draw_A_monogram(draw, alpha):
    pts = [
        (CENTER[0] - 40, CENTER[1] + 80),
        (CENTER[0], CENTER[1] - 120),
        (CENTER[0] + 40, CENTER[1] + 80)
    ]
    draw.line(pts, fill=(255,255,255,alpha), width=12, joint="curve")
    draw.ellipse([
        CENTER[0] - 12, CENTER[1] - 30,
        CENTER[0] + 12, CENTER[1] - 6
    ], fill=hex_to_rgb("#181C20", 255), outline=hex_to_rgb("#00FFA3", 230), width=4)

def draw_logo_frame(frame):
    img = Image.new("RGBA", (W, H), (0,0,0,0))
    draw = ImageDraw.Draw(img, "RGBA")

    # Core pulse
    pulse = 0.85 + 0.15 * math.sin(2 * math.pi * frame / NUM_FRAMES)
    core_r = int(CORE_RADIUS * pulse)
    core_alpha = int(180 + 50 * pulse)
    draw.ellipse([CENTER[0] - core_r, CENTER[1] - core_r, CENTER[0] + core_r, CENTER[1] + core_r],
                 fill=(0,255,163,core_alpha))

    # Rotating rings and orbiting nodes
    for i, (radius, color) in enumerate(zip(RING_RADII, RING_COLORS)):
        direction = 1 if i % 2 == 0 else -1
        angle = direction * (360 * frame / NUM_FRAMES) * (1 + i * 0.2)
        draw_ring(draw, radius, RING_WIDTH, hex_to_rgb(color), angle, alpha=120)
        node_angle = angle + i * 60
        draw_orbiting_node(draw, radius, node_angle, hex_to_rgb(NODE_COLOR))

    # "A" monogram
    draw_A_monogram(draw, 255)
    return img

# Generate frames
frames = [draw_logo_frame(i).convert("P", dither=Image.FLOYDSTEINBERG) for i in range(NUM_FRAMES)]

# Save GIF
frames[0].save(
    "project_aion_logo.gif",
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
first_frame_png.save("project_aion_logo.png", format="PNG")

print("Saved: project_aion_logo.gif and project_aion_logo.png")
