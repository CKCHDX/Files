from PIL import Image, ImageDraw, ImageFont
import math

# UHD dimensions
W, H = 1080, 1080
CENTER = (W // 2, H // 2)
NUM_FRAMES = 80

# Logo parameters
RING_RADIUS = 320
RING_WIDTH = 18
DOT_RADIUS = 38
DOT_COLORS = ["#00E5FF", "#00FFA3", "#00E5FF", "#00FFA3"]
DOT_ANGLES = [0, 90, 180, 270]
MONO_COLOR = "#ffffff"

def hex_to_rgb(hex_color, alpha=255):
    hex_color = hex_color.lstrip('#')
    if len(hex_color) == 3:
        hex_color = ''.join([c*2 for c in hex_color])
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4)) + (alpha,)

def draw_ring(draw, radius, width, color, angle, alpha=90):
    box = [
        CENTER[0] - radius, CENTER[1] - radius,
        CENTER[0] + radius, CENTER[1] + radius
    ]
    draw.arc(box, start=angle, end=angle+360, fill=color[:3]+(alpha,), width=width)

def draw_orbiting_dot(draw, radius, angle, color, dot_radius, pulse):
    a = math.radians(angle)
    x = CENTER[0] + radius * math.cos(a)
    y = CENTER[1] + radius * math.sin(a)
    # Glow
    glow_rad = dot_radius * (1.5 + 0.2 * pulse)
    draw.ellipse([x - glow_rad, y - glow_rad, x + glow_rad, y + glow_rad], fill=hex_to_rgb(color, 50))
    # Main dot
    dot_rad = dot_radius * (1.0 + 0.12 * pulse)
    draw.ellipse([x - dot_rad, y - dot_rad, x + dot_rad, y + dot_rad], fill=hex_to_rgb(color, 255), outline=hex_to_rgb("#181C20", 255), width=6)

def draw_O_monogram(draw, alpha):
    # Stylized "O": thick ring with a diagonal cut (negative space)
    o_r_outer = 120
    o_r_inner = 68
    # Outer ring
    draw.ellipse([
        CENTER[0] - o_r_outer, CENTER[1] - o_r_outer,
        CENTER[0] + o_r_outer, CENTER[1] + o_r_outer
    ], outline=(255,255,255,alpha), width=22)
    # Inner cut (negative space)
    draw.ellipse([
        CENTER[0] - o_r_inner, CENTER[1] - o_r_inner,
        CENTER[0] + o_r_inner, CENTER[1] + o_r_inner
    ], fill=(0,0,0,0))
    # Diagonal slash for a techy "O"
    slash_w = 28
    draw.line([
        (CENTER[0] - 80, CENTER[1] + 100),
        (CENTER[0] + 80, CENTER[1] - 100)
    ], fill=(0,0,0,0), width=slash_w)

def draw_logo_frame(frame):
    img = Image.new("RGBA", (W, H), (0,0,0,0))
    draw = ImageDraw.Draw(img, "RGBA")
    t = 2 * math.pi * frame / NUM_FRAMES

    # Rotating ring for unity/energy
    ring_angle = (360 * frame / NUM_FRAMES)
    draw_ring(draw, RING_RADIUS, RING_WIDTH, hex_to_rgb("#00E5FF"), ring_angle, alpha=60)

    # 4 orbiting dots with subtle pulsing
    for i, color in enumerate(DOT_COLORS):
        angle = DOT_ANGLES[i] + ring_angle
        pulse = 0.5 + 0.5 * math.sin(t + i * math.pi / 2)
        draw_orbiting_dot(draw, RING_RADIUS, angle, color, DOT_RADIUS, pulse)

    # Central "O" monogram
    draw_O_monogram(draw, 255)

    # Company name (optional)
    try:
        font = ImageFont.truetype("arial.ttf", int(H * 0.065))
    except:
        font = ImageFont.load_default()
    text = "Oscyra Solutions"
    # Use font.getbbox (Pillow 10+) for text size
    left, top, right, bottom = font.getbbox(text)
    text_w, text_h = right - left, bottom - top
    draw.text((CENTER[0], H - int(H*0.08)), text, font=font, fill=(0,229,255,255), anchor="mm")

    return img

# Generate frames
frames = [draw_logo_frame(i).convert("P", dither=Image.FLOYDSTEINBERG) for i in range(NUM_FRAMES)]

# Save GIF
frames[0].save(
    "oscyra_solutions_logo.gif",
    save_all=True,
    append_images=frames[1:],
    duration=70,
    loop=0,
    disposal=2,
    optimize=False,
    transparency=0
)

# Save PNG (first frame, full RGBA quality)
first_frame_png = draw_logo_frame(0)
first_frame_png.save("oscyra_solutions_logo.png", format="PNG")

print("Saved: oscyra_solutions_logo.gif and oscyra_solutions_logo.png")
