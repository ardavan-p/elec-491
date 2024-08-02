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

        # Main frame
        self.main_frame = ttk.Frame(master, padding="3 3 12 12", height=1080)
        self.main_frame.grid(column=0, row=0, sticky=(tk.N, tk.W, tk.E, tk.S))
        master.columnconfigure(0, weight=1)
        master.rowconfigure(0, weight=1)

        # Log pane
        self.log_pane = tk.Text(self.main_frame, width=100, height=20, state='disabled')
        self.log_pane.grid(column=0, row=0, sticky=(tk.N, tk.W, tk.E, tk.S))

        # Main UI panel
        self.ui_panel = tk.Canvas(self.main_frame, width=100, height=100, bg="white")
        self.ui_panel.grid(column=0, row=1, sticky=(tk.N, tk.W, tk.E, tk.S))

        # Example rectangle and text
        self.rectangle = self.ui_panel.create_rectangle(50, 50, 150, 150, fill="gray")
        self.text = self.ui_panel.create_text(100, 200, text="Value: N/A")

        # Serial setup
        self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)  # Adjust COM port as needed
        self.is_running = True

        # Quit button
        self.quit_button = ttk.Button(self.main_frame, text="Quit", command=self.on_closing)
        self.quit_button.grid(column=0, row=2, sticky=tk.E, padx=5, pady=5)

        # Start serial reading thread
        self.thread = threading.Thread(target=self.read_from_serial)
        self.thread.start()

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
        print("Updating GUI with data")
        self.master.after(0, self.update_gui, data)

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
