#!/usr/bin/env python

import serial
import time
import tkinter as tk
import threading

from datetime import datetime
from enum import Enum
from tkinter import ttk
from tkinter import font
from typing import Any, List

from dataclasses import dataclass

# TODO: Switch mismatch colour to orange and timeout color to red
# TODO: Add PTN active processing and colours
# TODO: Add ECU state parsing
# TODO: Maybe add running message statistics
# TODO: Add single click pressure measurement (going to need supporting FW for this)
# TODO: Add a function to hot-connect the serial port when it's connected and disconnected

# Configuration

SERIAL_PORT_PATH = "/dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0"
DEBUG = True

# Font definitions

JETBRAINS_BOLD = ("JetBrainsMono NF", 11, "bold")
JETBRAINS_LARGE_BOLD = ("JetBrainsMono NF", 24, "bold")
JETBRAINS_REGULAR = ("JetBrainsMono NF", 11)

# Color definitions

LIGHT_RED = "#ffb8b8"
LIGHT_ORANGE = "#f7d0a6"
LIGHT_YELLOW = "#f7f0a6"
LIGHT_GREEN = "#a6f7bb"
LIGHT_GREY = "#b5b5b5"

# Constant definitions

LOG_MSG_LEN = 3
ECU_MSG_LEN = 3
PTN_MSG_LEN = 7
RESET_MSG_LEN = 3

# Enum definitions


class PtnId(Enum):
    PTN_ID_ONE = 1
    PTN_ID_TWO = 2


class MessageId(Enum):
    LOG_MSG = "log"
    ECU_MSG = "ecu"
    PTN_MSG = "ptn"
    RESET_MSG = "reset"


class PtnStatusCode(Enum):
    OK = 0
    TIMEOUT = 1
    MISMATCH = 2


class EcuState(Enum):
    RESET = 0
    AUTOLOCALIZATION = 1
    NORMAL_OPERATION = 2

# Datatype definitions


@dataclass
class PtnMessage:
    ptn_id: int
    sensor_id: int
    pressure: int
    temperature: int
    status_code: int
    paired: bool


@dataclass
class EcuMessage:
    ecu_state: str
    log_msg: str


@dataclass
class LogMessage:
    log_level: str
    log_msg: str


@dataclass
class TireLabelBoxFieldRef:
    """
    Stores the `tk.Label` objects for each tire label box.
    This makes it easy to update them when we get a message from the ECU.
    """
    ptn_id: tk.Label
    pressure: tk.Label
    temperature: tk.Label
    sn_id: tk.Label
    status_code: tk.Label
    pair_status: tk.Label
    last_updated: tk.Label


@dataclass
class TireLabelBoxRef:
    frame: tk.Frame
    window: Any
    fields: TireLabelBoxFieldRef


def create_tire_label_box(canvas, x: int, y: int, tag: str) -> TireLabelBoxRef:
    LABELS = ["PTN ID", "Pressure (kPa)", "Temperature (degC)", "Sensor ID", "Status Code", "Pair Status"]

    tlb_frame = tk.Frame(canvas, width=100, height=50, relief="ridge", borderwidth=2)
    tlb_frame.grid(padx=20, pady=20)

    # TODO: consider adding tags so its easy to refer back to the object
    # NOTE: could also iterate over the returned object

    for row_idx, label_str in enumerate(LABELS):
        # create label objects
        label = tk.Label(tlb_frame, text=label_str, justify="left", font=JETBRAINS_BOLD, padx=10)
        # configure the grid locations for them
        label.grid(column=0, row=row_idx, sticky="w")

    # add the last updated label objects

    bfr = TireLabelBoxFieldRef(
        ptn_id=tk.Label(tlb_frame, text="N/A", relief="ridge", borderwidth=2, bg=LIGHT_GREY, padx=30, pady=5, font=JETBRAINS_BOLD),
        pressure=tk.Label(tlb_frame, text="N/A", relief="ridge", borderwidth=2, bg=LIGHT_GREY, padx=30, pady=5, font=JETBRAINS_BOLD),
        temperature=tk.Label(tlb_frame, text="N/A", relief="ridge", borderwidth=2, bg=LIGHT_GREY, padx=30, pady=5, font=JETBRAINS_BOLD),
        sn_id=tk.Label(tlb_frame, text="N/A", relief="ridge", borderwidth=2, bg=LIGHT_GREY, padx=30, pady=5, font=JETBRAINS_BOLD),
        status_code=tk.Label(tlb_frame, text="N/A", relief="ridge", borderwidth=2, bg=LIGHT_RED, padx=10, pady=5, font=JETBRAINS_BOLD),
        pair_status=tk.Label(tlb_frame, text="N/A", relief="ridge", borderwidth=2, bg=LIGHT_RED, padx=30, pady=5, font=JETBRAINS_BOLD),
        last_updated=tk.Label(tlb_frame, text="Last updated: N/A", padx=30, pady=5, fg="blue", font=JETBRAINS_BOLD),
    )

    bfr.ptn_id.grid(column=1, row=0, sticky="ew")
    bfr.pressure.grid(column=1, row=1, sticky="ew")
    bfr.temperature.grid(column=1, row=2, sticky="ew")
    bfr.sn_id.grid(column=1, row=3, sticky="ew")
    bfr.status_code.grid(column=1, row=4, sticky="ew")
    bfr.pair_status.grid(column=1, row=5, sticky="ew")
    bfr.last_updated.grid(column=0, columnspan=2, row=6, sticky="ew")

    window = canvas.create_window(x, y, window=tlb_frame, anchor="nw", tag=tag)

    return TireLabelBoxRef(frame=tlb_frame, fields=bfr, window=window)


class SerialGUI:
    def __init__(self, master):
        self.master = master
        master.title("Wirelessly-Powered Tire Pressure Monitoring System - GUI")

        # Register keyboard shortcuts
        # TODO: check out what else I can bind
        self.master.bind("<Control-c>", self.on_ctrl_c)
        # NOTE: disable for now
        # TODO: make sure to disable
        # self.master.bind("<Button-1>", self.on_mouse_motion)

        # Main frame
        self.main_frame = ttk.Frame(master, padding="12 12 12 12", relief="ridge")

        # Title label
        self.title_label = ttk.Label(self.main_frame, text="TL-81: Wirelessly-Powered Tire Pressure Monitoring System", font=JETBRAINS_LARGE_BOLD)

        # Log pane
        self.log_frame = ttk.LabelFrame(self.main_frame, text="Log pane", padding="10 10 10 10")
        self.log_pane = tk.Text(self.log_frame, state='disabled')

        # Main UI panel
        self.ui_frame = ttk.LabelFrame(self.main_frame, text="System view", padding="10 10 10 10")
        self.ui_panel = tk.Canvas(self.ui_frame, bg="white", borderwidth=2, relief="ridge")

        # State panel
        self.state_frame = ttk.LabelFrame(self.main_frame, text="System state", padding="10 10 10 10")
        self.current_time_label = ttk.Label(self.state_frame, padding="10 10 10 10", justify="right")

        self.current_ecu_state_label = ttk.Label(self.state_frame, text="ECU State", justify="left", font=JETBRAINS_BOLD)
        self.current_ecu_state_text = tk.Label(self.state_frame, text="N/A", relief="ridge", borderwidth=2, bg=LIGHT_GREY, padx=30, pady=5, font=JETBRAINS_REGULAR)

        # Footer panel
        self.footer_frame = ttk.Frame(self.main_frame)

        # Quit button
        self.quit_button = ttk.Button(self.footer_frame, text="Quit", command=self.on_closing)

        # --- Main UI canvas configuration ---

        # place background truck image and center it
        self.truck_image = tk.PhotoImage(file="./assets/gui_visual_v1.png")
        self.ui_panel.create_image(-100, 30, image=self.truck_image, anchor="nw")

        # Create all the tire rectangles
        # NOTE: Dimensions of the tire rectangles are: W=43, L=171
        self.left_tire = self.ui_panel.create_rectangle(532, 230, 578, 408, fill=LIGHT_GREY, tag="left_tire")
        self.right_tire = self.ui_panel.create_rectangle(606, 230, 652, 408, fill=LIGHT_GREY, tag="right_tire")

        # create the other inactive tires
        self.ui_panel.create_rectangle(120, 230, 167, 408, fill="gray")
        self.ui_panel.create_rectangle(195, 230, 243, 408, fill="gray")

        self.ui_panel.create_rectangle(120, 481, 167, 649, fill="gray")
        self.ui_panel.create_rectangle(195, 481, 243, 649, fill="gray")

        self.ui_panel.create_rectangle(532, 481, 578, 649, fill="gray")
        self.ui_panel.create_rectangle(606, 481, 652, 649, fill="gray")

        # --- Create the tire label boxes ---

        self.tlb_right: TireLabelBoxRef = create_tire_label_box(self.ui_panel, x=730, y=500, tag="")
        self.tlb_left: TireLabelBoxRef = create_tire_label_box(self.ui_panel, x=730, y=100, tag="")

        # bind event handlers

        self.tlb_left.frame.bind("<Enter>", self.on_enter_tire_left)
        self.tlb_left.frame.bind("<Leave>", self.on_leave_tire_left)

        self.tlb_right.frame.bind("<Enter>", self.on_enter_tire_right)
        self.tlb_right.frame.bind("<Leave>", self.on_leave_tire_right)

        self.ui_panel.tag_bind("left_tire", "<Enter>", self.on_enter_tire_left_extended)
        self.ui_panel.tag_bind("left_tire", "<Leave>", self.on_leave_tire_left_extended)

        self.ui_panel.tag_bind("right_tire", "<Enter>", self.on_enter_tire_right_extended)
        self.ui_panel.tag_bind("right_tire", "<Leave>", self.on_leave_tire_right_extended)

        # --- Grid configuration ---

        master.columnconfigure(0, weight=1)
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
        self.current_ecu_state_label.grid(column=0, row=1)
        self.current_ecu_state_text.grid(column=1, row=1, sticky="ew")

        # Serial setup
        if (DEBUG):
            self.ser = open("temp_serial_port", "r", buffering=1)
            self.thread = threading.Thread(target=self.read_from_file)
        else:
            self.ser = serial.Serial(SERIAL_PORT_PATH, 115200, timeout=1)  # Adjust COM port as needed
            self.thread = threading.Thread(target=self.read_from_serial)

        self.is_running = True

        # Start serial reading thread
        self.thread.start()

        # Start the systick thread
        self.systick_thread = threading.Thread(target=self.systick)
        self.systick_thread.start()

        # Start the LUT update thread for the left and right tire TLBs
        self.lut_update_left_thread = threading.Thread(target=self.update_last_updated_time, args=[self.tlb_left])
        self.lut_update_left_thread.start()

        self.lut_update_right_thread = threading.Thread(target=self.update_last_updated_time, args=[self.tlb_right])
        self.lut_update_right_thread.start()

    # ---- THREADS ----

    def update_last_updated_time(self, tlb: TireLabelBoxRef):
        while True:
            # check the current value of the last updated label
            if (tlb.fields.last_updated["text"] == "Updated just now!"):
                tlb.fields.last_updated.configure(text="Last updated: 1 second(s) ago", fg="blue")
                time.sleep(1.000)
                continue

            last_updated_time_val: str = tlb.fields.last_updated["text"]
            last_updated_time_val: str = last_updated_time_val.split(" ")[2]

            if (last_updated_time_val == "N/A"):
                time.sleep(1.000)
                continue

            # if there actually is an updated time, then increment it
            last_updated_time_val: int = int(last_updated_time_val)
            new_lut = last_updated_time_val + 1

            tlb.fields.last_updated.configure(text=f"Last updated: {new_lut} second(s) ago", fg="blue")
            time.sleep(1.000)

    def read_from_file(self):
        while self.is_running:
            line = self.ser.readline().strip()
            if (len(line) == 0):
                continue
            self.process_serial_data(line)

    def systick(self):
        while True:
            # get current time
            now = datetime.now()
            now_str = now.strftime("%a %d %b %Y, %I:%M:%S %p")
            # update the current time label
            self.current_time_label.configure(text=f"{now_str}")
            time.sleep(1.000)

    def read_from_serial(self):
        while self.is_running:
            if self.ser.in_waiting:
                try:
                    line = self.ser.readline().decode('ascii').strip()
                except UnicodeDecodeError:
                    pass
                if (len(line) == 0):
                    continue
                print(line)
                self.process_serial_data(line)

    # ---- HELPER FUNCTIONS ----

    def process_serial_data(self, data):
        # Update GUI elements based on the received data
        self.master.after(0, self.update_gui, data)

    def reset_tlb(self, tlb: TireLabelBoxRef, tire_tag: str):
        tlb.fields.ptn_id.configure(text="N/A", bg=LIGHT_GREY)
        tlb.fields.pressure.configure(text="N/A", bg=LIGHT_GREY)
        tlb.fields.temperature.configure(text="N/A", bg=LIGHT_GREY)
        tlb.fields.sn_id.configure(text="N/A", bg=LIGHT_GREY)
        tlb.fields.status_code.configure(text="N/A", bg=LIGHT_RED)
        tlb.fields.pair_status.configure(text="N/A", bg=LIGHT_RED)
        tlb.fields.last_updated.configure(text="Last updated: N/A", fg="blue")
        self.current_ecu_state_text.configure(text="N/A", bg=LIGHT_GREY)
        self.ui_panel.itemconfig(tire_tag, fill=LIGHT_GREY)

    def update_gui(self, data):
        # TODO: maybe also write messages to an unparsed log pane

        split_message: List[str] = data.split(",")

        if len(split_message) == 0:
            return

        msg_id: str = split_message[0]

        match msg_id:
            case MessageId.LOG_MSG.value:
                # check for the length
                if (len(split_message) != LOG_MSG_LEN):
                    print(f"Received `log` message of incorrect length. Expected {LOG_MSG_LEN}, got {len(split_message)}")
                    return

                # parse out the pieces
                log_level = split_message[1]
                log_message = split_message[2]

                # generate a timestamp and append to log line
                current_time = datetime.now()
                line = f"[{current_time}] [{log_level}] :: {log_message}\n"

                # write to the log pane
                self.log_pane['state'] = 'normal'
                self.log_pane.insert(tk.END, line)
                self.log_pane.see(tk.END)
                self.log_pane['state'] = 'disable'

            case MessageId.PTN_MSG.value:
                # check for the length
                if (len(split_message) != PTN_MSG_LEN):
                    print(f"ERROR: Received `ptn` message of incorrect length. Expected {PTN_MSG_LEN}, got {len(split_message)}")
                    return

                # parse out the pieces and pack into a PTN message object
                ptn_id: int = int(split_message[1])
                sensor_id: int = int(split_message[2])
                pressure: int = int(split_message[3])
                temperature: int = int(split_message[4])
                status_code: int = int(split_message[5])
                paired: bool = bool(int(split_message[6]))

                ptn_msg_obj = PtnMessage(
                    ptn_id=ptn_id,
                    sensor_id=sensor_id,
                    pressure=pressure,
                    temperature=temperature,
                    status_code=status_code,
                    paired=paired,
                )

                # depending on the PTN ID write to a different frame
                match ptn_id:
                    case PtnId.PTN_ID_ONE.value:
                        self.update_tire_info(self.tlb_left, ptn_msg_obj, "left_tire")
                    case PtnId.PTN_ID_TWO.value:
                        self.update_tire_info(self.tlb_right, ptn_msg_obj, "right_tire")
                    case _:
                        print(f"ERROR: Received an unexpected PTN ID \"{ptn_id}\".")

            case MessageId.ECU_MSG.value:
                if (len(split_message) != ECU_MSG_LEN):
                    print(f"Received `ecu` message of incorrect length. Expected {ECU_MSG_LEN}, got {len(split_message)}")
                    return

                ecu_state = int(split_message[1])
                match ecu_state:
                    # ECU RESET
                    case 0:
                        self.current_ecu_state_text.configure(text=f"{EcuState(ecu_state).name} (0x{ecu_state})", bg=LIGHT_RED)
                    # ECU AUTOLOCALIZATION
                    case 1:
                        self.current_ecu_state_text.configure(text=f"{EcuState(ecu_state).name} (0x{ecu_state})", bg=LIGHT_ORANGE)
                    # ECU NORMAL OPERATION
                    case 2:
                        self.current_ecu_state_text.configure(text=f"{EcuState(ecu_state).name} (0x{ecu_state})", bg=LIGHT_GREEN)
                    case _:
                        self.current_ecu_state_text.configure(text="UNKNOWN ECU STATE", bg=LIGHT_GREY)

            case MessageId.RESET_MSG.value:
                # check for the length
                if (len(split_message) != LOG_MSG_LEN):
                    print(f"Received `reset` message of incorrect length. Expected {RESET_MSG_LEN}, got {len(split_message)}")
                    return

                if (split_message[1] != "reset" or split_message[2] != "reset"):
                    print(f"Received `reset` message of incorrect format. Expected \"reset,reset,reset\"")
                    return

                # perform GUI reset
                self.reset_tlb(self.tlb_left, "left_tire")
                self.reset_tlb(self.tlb_right, "right_tire")

            case _:
                print(f"ERROR: Received an unexpected message type \"{msg_id}\".")

                # generate a timestamp and append to log line
                current_time = datetime.now()
                line = f"[{current_time}] :: {data}\n"

                self.log_pane['state'] = 'normal'
                self.log_pane.insert(tk.END, line)
                self.log_pane.see(tk.END)
                self.log_pane['state'] = 'disable'

    def update_tire_info(self, tlb: TireLabelBoxRef, ptn_msg: PtnMessage, tire_tag: str):
        # update the fields in the TLB based on the received PTN message
        tlb.fields.ptn_id.configure(text=str(ptn_msg.ptn_id))
        tlb.fields.sn_id.configure(text=str(ptn_msg.sensor_id))
        tlb.fields.pressure.configure(text=str(ptn_msg.pressure))
        tlb.fields.temperature.configure(text=str(ptn_msg.temperature))
        tlb.fields.status_code.configure(text=str(ptn_msg.status_code))

        match ptn_msg.status_code:
            case 0:
                # PTN OK
                tlb.fields.status_code.configure(text=f"{PtnStatusCode(ptn_msg.status_code).name} (0x{ptn_msg.status_code})", bg=LIGHT_GREEN)
                self.ui_panel.itemconfig(tire_tag, fill=LIGHT_GREEN)
            case 1:
                # PTN TIMEOUT
                tlb.fields.status_code.configure(text=f"{PtnStatusCode(ptn_msg.status_code).name} (0x{ptn_msg.status_code})", bg=LIGHT_RED)
                self.ui_panel.itemconfig(tire_tag, fill=LIGHT_RED)
            case 2:
                # PTN MISMATCH
                tlb.fields.status_code.configure(text=f"{PtnStatusCode(ptn_msg.status_code).name} (0x{ptn_msg.status_code})", bg=LIGHT_ORANGE)
                self.ui_panel.itemconfig(tire_tag, fill=LIGHT_ORANGE)
            case _:
                tlb.fields.status_code.configure(text=f"{PtnStatusCode(ptn_msg.status_code).name} (0x{ptn_msg.status_code})", bg=LIGHT_RED)
                self.ui_panel.itemconfig(tire_tag, fill=LIGHT_RED)

        if ptn_msg.paired:
            tlb.fields.pair_status.configure(text="TRUE", bg=LIGHT_GREEN)
        else:
            tlb.fields.pair_status.configure(text="FALSE", bg=LIGHT_RED)

        # update the last updated time value
        tlb.fields.last_updated.configure(text="Updated just now!", fg="green")

    # ---- ASYNC EVENT HANDLERS ----

    def on_ctrl_c(self, event):
        self.on_closing()

    def on_enter_tire_left(self, event):
        self.ui_panel.itemconfig(self.left_tire, outline="black", width=4)

    def on_enter_tire_left_extended(self, event):
        self.ui_panel.itemconfig(self.left_tire, outline="black", width=4)
        self.tlb_left.frame.configure(relief="solid", width=4)

    def on_leave_tire_left_extended(self, event):
        self.ui_panel.itemconfig(self.left_tire, outline="black", width=1)
        self.tlb_left.frame.configure(relief="ridge", width=1)

    def on_enter_tire_right_extended(self, event):
        self.ui_panel.itemconfig(self.right_tire, outline="black", width=4)
        self.tlb_right.frame.configure(relief="solid", width=4)

    def on_leave_tire_right_extended(self, event):
        self.ui_panel.itemconfig(self.right_tire, outline="black", width=1)
        self.tlb_right.frame.configure(relief="ridge", width=1)

    def on_leave_tire_left(self, event):
        self.ui_panel.itemconfig(self.left_tire, outline="black", width=1)

    def on_enter_tire_right(self, event):
        self.ui_panel.itemconfig(self.right_tire, outline="black", width=4)

    def on_leave_tire_right(self, event):
        self.ui_panel.itemconfig(self.right_tire, outline="black", width=1)

    def on_mouse_motion(self, event):
        x, y = event.x, event.y
        self.process_serial_data(f"mouse position: {x=}, {y=}")

    def on_closing(self):
        self.is_running = False
        self.ser.close()
        self.master.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    app = SerialGUI(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()
