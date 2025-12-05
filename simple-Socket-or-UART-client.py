import time
import socket  
import struct
import tkinter as tk
import math
from PIL import Image, ImageTk
import numpy as np
from typing import Tuple
from scipy.optimize import curve_fit

HOST = "192.168.1.1"
PORT = 288

cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cybot_socket.connect((HOST, PORT))
                      
cybot = cybot_socket.makefile("rbw", buffering=0)

def run_auto_mode():
    print("Run Auto mode")

def scan_for_objects():
        cybot.write(b"\x05")
        cybot.write('\n'.encode())
        start_byte = cybot.read(1)
        if start_byte != b"\xF0" and start_byte != b"\xF1":
                print(f"Received unexpected start byte: {start_byte.hex()}")
                raise RuntimeError("Did not receive expected 0xF0 or 0xF1 response")
        if start_byte == b"\xF1":
                print("Hot Object Found")
        elif start_byte == b"\xF0":
                print("No Hot Object Found")
        get_status()

def calibrate_servo():
        cybot.write(b"\x06")
        cybot.write('\n'.encode())

def play_music():
    cybot.write(b"\x07")
    cybot.write('\n'.encode())
    start_byte = cybot.read(1)
    if start_byte != b"\x07":
            print(f"Received unexpected start byte: {start_byte.hex()}")
            raise RuntimeError("Did not receive expected 0x07 response")

def test_1():
    cybot.write(b"\x08")
    cybot.write('\n'.encode())
    start_byte = cybot.read(1)
    if start_byte != b"\x08":
            print(f"Received unexpected start byte: {start_byte.hex()}")
            raise RuntimeError("Did not receive expected 0x08 response")

def test_2():
    print("Test 2")

def ir_calibration():
    cybot.write(b"\x11")
    cybot.write('\n'.encode())
    start_byte = cybot.read(1)
    if start_byte != b"\x11":
        print(f"Received unexpected start byte: {start_byte.hex()}")
        raise RuntimeError("Did not receive expected 0x11 response")
    scan_format = '<hf'
    ir_vals = []
    ping_dists = []
    for i in range(100):
        data = struct.unpack(scan_format, cybot.read(6))
        ir_vals.append(data[0])
        ping_dists.append(data[1])
        print(f"{data[0]},{data[1]}")
    p1, p2, p3 = rational_fitter(ir_vals, ping_dists)

    print(f"p1:{p1}, p2:{p1}, p3:{p3}\n")
    send_format = '<fff'
    cybot.write(f"{struct.pack(send_format, p1, p2, p3)}".encode())

def move_forward():
        cybot.write(b"\x0B")
        cybot.write('\n'.encode())
        start_byte = cybot.read(1)
        if start_byte != b"\x0B":
                print(f"Received unexpected start byte: {start_byte.hex()}")
                raise RuntimeError("Did not receive expected 0x0B response")
        get_status()


def move_left():
        cybot.write(b"\x0C")
        cybot.write('\n'.encode())
        start_byte = cybot.read(1)
        if start_byte != b"\x0C":
                print(f"Received unexpected start byte: {start_byte.hex()}")
                raise RuntimeError("Did not receive expected 0x0C response")
        get_status()

def turn_right():
        cybot.write(b"\x0D")
        cybot.write('\n'.encode())
        start_byte = cybot.read(1)
        if start_byte != b"\x0D":
                print(f"Received unexpected start byte: {start_byte.hex()}")
                raise RuntimeError("Did not receive expected 0x0D response")
        get_status()

def get_status():
        cybot.write(b"\x0E")
        cybot.write('\n'.encode())
        start_byte = cybot.read(1)
        if start_byte != b"\x0E":
                print(f"Received unexpected start byte: {start_byte.hex()}")
                raise RuntimeError("Did not receive expected 0x0E response")

        header_format = '<Bfff'
        header_size = struct.calcsize(header_format)
        header = cybot.read(header_size)
        header = struct.unpack(header_format, header)
        num_objects, self_pos_x, self_pos_y, self_heading = header

        print(f"Start byte: {start_byte.hex()}")
        print(f"Number of objects: {num_objects}")
        print(f"Self Pos X: {self_pos_x}")
        print(f"Self Pos Y: {self_pos_y}")
        print(f"Self Heading: {self_heading}")

        grid_canvas.delete("all")

        for i in range(grid_size + 4):
            grid_canvas.create_line(i * cell_size, 0, i * cell_size, canvas_height)
            grid_canvas.create_line(0, i * cell_size, canvas_width, i * cell_size)

        scale = 5

        self_radius_px = (320 / 2) / scale
        self_x_px = self_pos_x / scale
        self_y_px = self_pos_y / scale
        draw_circle(self_x_px, self_y_px, self_radius_px, "green")
        draw_heading(self_pos_x, self_pos_y, self_heading)

        object_format = '<hhh'
        object_size = struct.calcsize(object_format)
        for i in range(num_objects):
                start = 14 + i * object_size
                end = start + object_size
                obj_data = struct.unpack(object_format, cybot.read(object_size))
                obj_pos_x, obj_pos_y, obj_width = obj_data

                print(f"Object {i+1} Pos X: {obj_pos_x}")
                print(f"Object {i+1} Pos Y: {obj_pos_y}")
                print(f"Object {i+1} Width: {obj_width}")

                obj_radius_px = (obj_width / 2) / scale
                obj_x_px = obj_pos_x / scale
                obj_y_px = obj_pos_y / scale
                draw_circle(obj_x_px, obj_y_px, obj_radius_px, "red")

def run_scan():
    print("Run Scan")

def get_dist_ahead():
    print("Get dist ahead")

def end():
    print("End")


window = tk.Tk()
window.title("Cybot GUI")

button_bg_color = "#3A3A3A"
button_abg_color = "#252525"
button_fg_color = "#FFFFFF"

main_frame = tk.Frame(window)
main_frame.pack(padx=10, pady=10)

button_frame = tk.Frame(main_frame)
button_frame.pack(side=tk.LEFT)

buttons = [
    ("Run Auto mode", run_auto_mode),
    ("Scan for objects", scan_for_objects),
    ("Calibrate servo", calibrate_servo),
    ("Play music", play_music),
    ("Test 1", test_1),
    ("Test 2", test_2),
    ("IR calibration", ir_calibration),
    ("Forward", move_forward),
    ("Turn left", move_left),
    ("Turn right", turn_right),
    ("Get status", get_status),
    ("Run Scan", run_scan),
    ("Get dist ahead", get_dist_ahead),
    ("End", end),
]

for i, (text, command) in enumerate(buttons):
    button = tk.Button(
        button_frame,
        text=text,
        width=15,
        height=2,
        bg=button_bg_color,
        fg=button_fg_color,
        activebackground=button_abg_color,
        activeforeground=button_fg_color,
        command=command,
    )
    button.grid(row=i // 2, column=i % 2, padx=5, pady=5)

canvas_width = 400
canvas_height = 700
grid_size = 4
cell_size = canvas_width // grid_size

grid_canvas = tk.Canvas(main_frame, width=canvas_width, height=canvas_height, bg=button_bg_color)
grid_canvas.pack(side=tk.RIGHT, padx=10)


#image_path = "U:\\CPRE288\\lab_8\\Live_Jones_Reaction.png"
#original_image = Image.open(image_path)
#resized_image = original_image.resize((canvas_width, canvas_height), Image.LANCZOS)
#background_image = ImageTk.PhotoImage(resized_image)
# grid_canvas.create_image(0, 0, anchor=tk.NW, image=background_image)

# Draw grid lines
for i in range(grid_size + 4):
    grid_canvas.create_line(i * cell_size, 0, i * cell_size, canvas_height)
    grid_canvas.create_line(0, i * cell_size, canvas_width, i * cell_size)

def draw_circle(x, y, radius, color):
    x1 = x - radius + 200
    y1 = -y - radius + 200
    x2 = x + radius + 200
    y2 = -y + radius + 200
    grid_canvas.create_oval(x1, y1, x2, y2, fill=color, outline="#FFFFFF")

def draw_heading(x, y, heading):
        x1 = x/5 + 200
        y1 = -y/5 + 200

        rad = heading * (math.pi / 180.0)
        x2 =  32 * math.cos(rad) + x1
        y2 = - 32 * math.sin(rad) + y1
    
        grid_canvas.create_line(x1, y1, x2, y2, fill="#FFFFFF", width=2)

def rational_fitter(x: np.ndarray, y: np.ndarray) -> Tuple[float, float, float]:
        x = np.asarray(x)
        y = np.asarray(y)
        if len(y) == 0:
                print("Empty data, error has occured")
                return 0.0, 0.0
        if np.any(y == 0.0):
                non_zero_indices = np.where(y != 0)
                if len(non_zero_indices[0]) < 2:
                        print("Insufficient valid calibration data")
                        return 0.0, 0.0
                x_filt = x[non_zero_indices]
                y_filt = y[non_zero_indices]
        else:
                x_filt = x
                y_filt = y

        def model(x, p1, p2, p3):
                return p2 * x**p3 + p1

        

        try:
                popt, _ = curve_fit(model, x_filt, y_filt, p0=[90, 689800000000, -3.2])
                p1, p2, p3 = popt
        except Exception as e:
                print(f"Error during curve fit: {e}")
                return 0.0, 0.0, 0.0

        return p1, p2, p3

window.mainloop()