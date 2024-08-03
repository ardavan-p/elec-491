#!/usr/bin/env python
import tkinter as tk
from tkinter import ttk
import serial
import threading
from datetime import datetime


class SerialGUI:
    def __init__(self, master):
        self.master = master
        master.title("Wirelessly-Powered Tire Pressure Monitoring System - GUI")

        # Register keyboard shortcuts
        self.master.bind("<Control-c>", self.on_ctrl_c)

        # Main frame
        self.main_frame = ttk.Frame(master, padding="12 12 12 12", relief="ridge")

        # Title label
        self.title_label = ttk.Label(self.main_frame, text="TL-81: Wirelessly-Powered Tire Pressure Monitoring System")

        # Log pane
        self.log_frame = ttk.LabelFrame(self.main_frame, text="Log pane", padding="3 3 12 12")
        self.log_pane = tk.Text(self.log_frame, state='disabled')

        # Main UI panel
        self.ui_frame = ttk.LabelFrame(self.main_frame, text="System view", padding="3 3 12 12")
        self.ui_panel = tk.Canvas(self.ui_frame, bg="lightblue")

        # State panel
        self.state_frame = ttk.LabelFrame(self.main_frame, text="System state", padding="3 3 12 12")
        self.state_pane = tk.Canvas(self.state_frame, bg="lightgreen")

        # Footer panel
        self.footer_frame = ttk.Frame(self.main_frame)

        # Quit button
        self.quit_button = ttk.Button(self.footer_frame, text="Quit", command=self.on_closing)

        # Example rectangle and text
        self.rectangle = self.ui_panel.create_rectangle(50, 50, 150, 150, fill="gray")
        self.text = self.ui_panel.create_text(100, 200, text="Value: N/A")

        # Grid configuration
        master.columnconfigure(0, weight=1)
        master.rowconfigure(0, weight=1)
        self.main_frame.columnconfigure(0, weight=1)
        self.main_frame.columnconfigure(1, weight=1)
        self.main_frame.rowconfigure(0, weight=1)
        self.main_frame.rowconfigure(1, weight=5)

        self.main_frame.grid(column=0, row=0, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.title_label.grid(column=0, row=0, columnspan=2)

        self.ui_frame.grid(column=0, row=1, rowspan=3, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.ui_panel.grid(column=0, row=1, rowspan=3, sticky=(tk.N, tk.W, tk.E, tk.S))

        self.footer_frame.grid(column=0, row=4, columnspan=2, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.quit_button.grid(column=0, row=4, sticky=tk.E, padx=5, pady=5)

        self.log_frame.grid(column=1, row=2, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.log_pane.grid(column=1, row=2, sticky=(tk.N, tk.W, tk.E, tk.S))

        self.state_frame.grid(column=1, row=1, sticky=(tk.N, tk.W, tk.E, tk.S))
        self.state_pane.grid(column=1, row=1, sticky=(tk.N, tk.W, tk.E, tk.S))

        # Serial setup
        # self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)  # Adjust COM port as needed
        self.ser = open("temp_serial_port", "r", buffering=1)
        self.is_running = True

        # Start serial reading thread
        # self.thread = threading.Thread(target=self.read_from_serial)
        self.thread = threading.Thread(target=self.read_from_file)
        self.thread.start()

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
            self.ui_panel.itemconfig(self.rectangle, fill=color)
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
