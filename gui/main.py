#!/usr/bin/env python
import tkinter as tk
from tkinter import ttk
from tkinter import font
import serial
import time
import threading
from datetime import datetime
from PIL import Image, ImageTk

# TODO: set the fonts correctly

# Font definitions

JETBRAINS_BOLD = ("JetBrainsMono NF", 11, "bold")


class SerialGUI:
    def __init__(self, master):
        self.master = master
        master.title("Wirelessly-Powered Tire Pressure Monitoring System - GUI")

        # Register keyboard shortcuts
        # TODO: check out what else I can bind
        self.master.bind("<Control-c>", self.on_ctrl_c)
        self.master.bind("<Button-1>", self.on_mouse_motion)

        print(font.families())

        # Main frame
        self.main_frame = ttk.Frame(master, padding="12 12 12 12", relief="ridge")

        # Title label
        self.title_label = ttk.Label(self.main_frame, text="TL-81: Wirelessly-Powered Tire Pressure Monitoring System")

        # Log pane
        self.log_frame = ttk.LabelFrame(self.main_frame, text="Log pane", padding="10 10 10 10")
        self.log_pane = tk.Text(self.log_frame, state='disabled')

        # Main UI panel
        self.ui_frame = ttk.LabelFrame(self.main_frame, text="System view", padding="10 10 10 10")
        self.ui_panel = tk.Canvas(self.ui_frame, bg="white", borderwidth=2, relief="ridge")

        # State panel
        self.state_frame = ttk.LabelFrame(self.main_frame, text="System state", padding="10 10 10 10")
        self.current_time_label = ttk.Label(self.state_frame, padding="10 10 10 10")

        # Footer panel
        self.footer_frame = ttk.Frame(self.main_frame)

        # Quit button
        self.quit_button = ttk.Button(self.footer_frame, text="Quit", command=self.on_closing)

        # --- Main UI canvas configuration ---

        # place background truck image and center it
        self.truck_image = tk.PhotoImage(file="./assets/truck-downsized.png")
        self.ui_panel.create_image(150, 20, image=self.truck_image, anchor="nw")

        # Create all the tire rectangles
        # NOTE: Dimensions of the tire rectangles are: W=43, L=171
        self.right_tire = self.ui_panel.create_rectangle(711, 343, 754, 514, fill="#b5b5b5", tag="right-tire")
        self.left_tire = self.ui_panel.create_rectangle(642, 343, 685, 514, fill="#b5b5b5", tag="left-tire")

        self.inactive_tire_1 = self.ui_panel.create_rectangle(258, 343, 301, 514, fill="gray", tag="inactive-tire-1")
        self.inactive_tire_2 = self.ui_panel.create_rectangle(327, 343, 370, 514, fill="gray", tag="inactive-tire-2")

        # --- Create the tire label boxes ---

        # TODO: put this in a function

        self.tire_label_box = ttk.Frame(self.ui_panel, width=100, height=50, relief="ridge", borderwidth=2, padding="10 10 10 10")

        self.id_label = tk.Label(self.tire_label_box, text="PTN ID", justify="left", font=JETBRAINS_BOLD)
        self.pressure_label = tk.Label(self.tire_label_box, text="Pressure (kPA)", font=JETBRAINS_BOLD)
        self.temp_label = tk.Label(self.tire_label_box, text="Temperature (degC)", font=JETBRAINS_BOLD)
        sn_id_label = tk.Label(self.tire_label_box, text="Sensor ID", font=JETBRAINS_BOLD)
        status_code = tk.Label(self.tire_label_box, text="Status Code", font=JETBRAINS_BOLD)
        pair_status = tk.Label(self.tire_label_box, text="Pair Status", font=JETBRAINS_BOLD)

        self.id_label_text = tk.Label(self.tire_label_box, text="N/A", relief="ridge", borderwidth=2, bg="#ffb8b8", padx=30, pady=5)
        self.pressure_label_text = tk.Label(self.tire_label_box, text="N/A", relief="ridge", borderwidth=2, bg="#ffb8b8", padx=30, pady=5)
        self.temp_label_text = tk.Label(self.tire_label_box, text="N/A", relief="ridge", borderwidth=2, bg="#ffb8b8", padx=30, pady=5)
        sn_id_label_text = tk.Label(self.tire_label_box, text="N/A", relief="ridge", borderwidth=2, bg="#ffb8b8", padx=30, pady=5)
        status_code_text = tk.Label(self.tire_label_box, text="N/A", relief="ridge", borderwidth=2, bg="#ffb8b8", padx=30, pady=5)
        pair_status_text = tk.Label(self.tire_label_box, text="N/A", relief="ridge", borderwidth=2, bg="#ffb8b8", padx=30, pady=5)

        self.window = self.ui_panel.create_window(100, 100, window=self.tire_label_box, anchor="nw")

        self.id_label.grid(column=0, row=0, sticky="w")
        self.pressure_label.grid(column=0, row=1, sticky="w")
        self.temp_label.grid(column=0, row=2, sticky="w")
        sn_id_label.grid(column=0, row=3, sticky="w")
        status_code.grid(column=0, row=4, sticky="w")
        pair_status.grid(column=0, row=5, sticky="w")

        self.id_label_text.grid(column=1, row=0, sticky="w")
        self.pressure_label_text.grid(column=1, row=1, sticky="w")
        self.temp_label_text.grid(column=1, row=2, sticky="w")
        sn_id_label_text.grid(column=1, row=3, sticky="w")
        status_code_text.grid(column=1, row=4, sticky="w")
        pair_status_text.grid(column=1, row=5, sticky="w")

        # --- Grid configuration ---

        master.columnconfigure(0, weight=1)
        # master.columnconfigure(1, weight=1)
        master.rowconfigure(0, weight=1)

        self.main_frame.columnconfigure(0, weight=20)
        self.main_frame.columnconfigure(1, weight=1)

        self.main_frame.rowconfigure(0, weight=1)
        self.main_frame.rowconfigure(1, weight=5)
        self.main_frame.rowconfigure(2, weight=5)

        # seems to make the contents stretch to fill the space of the frame
        self.state_frame.columnconfigure(1, weight=1)
        self.state_frame.rowconfigure(1, weight=1)

        self.ui_frame.columnconfigure(0, weight=10)
        self.ui_frame.rowconfigure(2, weight=1)

        self.log_frame.columnconfigure(1, weight=1)
        self.log_frame.rowconfigure(2, weight=1)

        self.main_frame.grid(column=0, row=0, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.title_label.grid(column=0, row=0, columnspan=2)

        self.ui_frame.grid(column=0, row=1, rowspan=3, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.ui_panel.grid(column=0, row=1, rowspan=3, sticky=(tk.N, tk.W, tk.E, tk.S))

        self.footer_frame.grid(column=0, row=4, columnspan=2, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.quit_button.grid(column=0, row=4, sticky=tk.E, padx=5, pady=5)

        self.log_frame.grid(column=1, row=2, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.log_pane.grid(column=1, row=2, sticky=(tk.N, tk.W, tk.E, tk.S))

        self.state_frame.grid(column=1, row=1, sticky=(tk.N, tk.W, tk.E, tk.S))

        self.current_time_label.grid(column=0, row=0)

        # Serial setup
        # self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)  # Adjust COM port as needed
        self.ser = open("temp_serial_port", "r", buffering=1)
        self.is_running = True

        # Start serial reading thread
        # self.thread = threading.Thread(target=self.read_from_serial)
        self.thread = threading.Thread(target=self.read_from_file)
        self.thread.start()

        # Start the systick thread
        self.systick_thread = threading.Thread(target=self.systick)
        self.systick_thread.start()

    def read_from_file(self):
        while self.is_running:
            line = self.ser.readline().strip()
            # insert a timestamp
            if (len(line) == 0):
                continue
            current_time = datetime.now()
            line = f"[{current_time}] {line}"
            print(line)
            self.process_serial_data(line)

    def systick(self):
        while True:
            # get current time
            now = datetime.now()
            now_str = now.strftime("%a %d %b %Y, %I:%M:%S%p")
            # update the current time label
            self.current_time_label.configure(text=f"{now_str}")
            time.sleep(1.000)

    def read_from_serial(self):
        while self.is_running:
            if self.ser.in_waiting:
                line = self.ser.readline().decode('ascii').strip()
                # insert a timestamp
                if (len(line) == 0):
                    continue
                current_time = datetime.now()
                line = f"[{current_time}] {line}"
                print(line)
                self.process_serial_data(line)

    def process_serial_data(self, data):
        # Update GUI elements based on the received data
        self.master.after(0, self.update_gui, data)

    def on_ctrl_c(self, event):
        self.on_closing()

    def on_mouse_motion(self, event):
        x, y = event.x, event.y
        self.process_serial_data(f"mouse position: {x=}, {y=}")

    def update_gui(self, data):
        # Log the message
        self.log_pane['state'] = 'normal'
        self.log_pane.insert(tk.END, data + "\n")
        self.log_pane.see(tk.END)
        self.log_pane['state'] = 'disable'

        # Parse the data and update UI elements
        # This is a placeholder - adjust based on your actual data format
        if "COLOR" in data:
            color = data.split(":")[1]
            self.ui_panel.itemconfig(self.right_tire, fill=color)
        elif "VALUE" in data:
            value = data.split(":")[1]
            self.ui_panel.itemconfig(self.text, text=f"Value: {value}")

    def on_closing(self):
        self.is_running = False
        self.ser.close()
        self.master.destroy()


root = tk.Tk()
app = SerialGUI(root)
root.protocol("WM_DELETE_WINDOW", app.on_closing)
root.mainloop()
